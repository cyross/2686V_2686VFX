#pragma once

#include <JuceHeader.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cstdint>
#include <map>
#include <memory>
#include <mutex>
#include <vector>

#include "Shared/Core/Synth/StepTable.h"
#include "Shared/Core/Synth/StepTableIo.h"

// ============================================================================
// エンベロープ・シーケンサーのパターン (4.5.0)
// ============================================================================
// SSG HW AMP / PITCH ENV、SSG SW AMP / PITCH ENV[256]、TAP AMP / PITCH SEQUENCER、
// UNISON / HARMONY の区分は、区分の設定をまるごと「パターン」として最大 32 組持てる。
// 区分の一番下に PATTERNS (組の数)・PATTERN (使う組)・RESET TO CHANGE を置く。
//
// ---- 値の置き場
// 区分の頭 (STEPS・Shape・VOICES など) は APVTS のパラメータで、それとは別に組ごとの
// 写し (slots) をここに持つ。APVTS はいつも「画面で選んでいる組 (editing)」を映す。
// 音の側はパラメータを直に読まず、ここの live を読む (PrHelper の setup が live の
// ポインタを渡す)。ブロックの頭 (process) で
//   APVTS → slots[editing] → (使う組 PATTERN の) slots → live
// と写すので、組が 1 つのとき (既定) はこれまでどおり APVTS の値がそのまま効く。
//
// PATTERN が変わると、音の側はその場で新しい組の slots を読む (オートメーションでも
// 書き出しでも、ブロックの頭で確実に切り替わる)。APVTS へ新しい組の値を戻すのは
// メッセージスレッド (タイマー) で、戻し終えるまでは editing を 0 にして、古い組の
// 写しを新しい組へ書かないようにする。
//
// 段の表 (StepTable) は組ごとに別の表を持つ。1 組目はこれまでの鍵、2 組目からは鍵の
// 後ろに "#P<組>" を足したもの (EnvPattern::tableKey)。表は組を増やしたときに作る
// (メッセージスレッド)。まだ無い組を使うあいだは 1 組目の表で鳴らす。
//
// 区分の札 (Enable / Bypass) と、TAP SEQUENCER の段を動かす札 (INCREMENT など) は
// 組に入れない (区分そのものの入り切り・操作なので、組をまたいで 1 つ)。
//
// ---- RESET TO CHANGE
// 入れていると、組を切り替えたときに鳴っている音のエンベロープを頭から、TAP
// SEQUENCER を開始前へ、UNISON の ARP を頭から始め直す。切っていると、位置を
// 保ったまま新しい組の値で続ける (TAP SEQUENCER は段の位置を新しい組へ引き継ぐ)。
// 始め直しは restart (数) を進めて伝え、音の側は前に見た数と違えば始め直す。
namespace EnvPattern
{
	inline constexpr int maxPatterns = 32;

	// 区分の頭 (base) の後ろに付ける鍵
	inline const juce::String patternsKey = "_PATTERNS";
	inline const juce::String patternKey = "_PATTERN";
	inline const juce::String resetKey = "_PATRESET";

	// パラメータの表示名の後ろ
	inline const juce::String patternsName = " Patterns";
	inline const juce::String patternName = " Pattern";
	inline const juce::String resetName = " Reset To Change";

	// 区分の頭 (code の後ろ)。区分のパラメータはどれも base + "_" で始まる
	inline const juce::String ssgHwEnv = "_SSGHWENV";
	inline const juce::String ssgHwPEnv = "_SSGHWPENV";
	inline const juce::String ssgSwEnv256 = "_SSGSW256";
	inline const juce::String ssgSwPEnv256 = "_SSGSWP256";
	inline const juce::String unison = "_UNI";
	inline const juce::String tapAmp = "_TAPAMP";
	inline const juce::String tapPitch = "_TAPPITCH";

	// 組に入れないパラメータの尻尾 (区分の札と、TAP SEQUENCER の段を動かす札)
	inline bool isExcludedTail(const juce::String& tail)
	{
		return tail == "_ENABLE" || tail == "_BYPASS" || tail == "_INC" || tail == "_DEC" || tail == "_RESET"
			|| tail == patternsKey || tail == patternKey || tail == resetKey;
	}

	// 組の表の鍵。1 組目はこれまでの鍵のまま (state をそのまま読める)
	inline juce::String tableKey(const juce::String& key, int pattern)
	{
		return pattern <= 1 ? key : key + "#P" + juce::String(pattern);
	}

	// 区分の 3 つのパラメータを登録する。PATTERNS は既定 1 (これまでどおり)
	inline void addParameters(juce::AudioProcessorValueTreeState::ParameterLayout& layout, const juce::String& base, const juce::String& baseName)
	{
		layout.add(std::make_unique<juce::AudioParameterInt>(base + patternsKey, baseName + patternsName, 1, maxPatterns, 1));
		layout.add(std::make_unique<juce::AudioParameterInt>(base + patternKey, baseName + patternName, 1, maxPatterns, 1));
		layout.add(std::make_unique<juce::AudioParameterBool>(base + resetKey, baseName + resetName, false));
	}

	// 使う組。PATTERN を PATTERNS で抑える
	inline int clampPattern(float patterns, float pattern)
	{
		const int n = std::clamp((int)patterns, 1, maxPatterns);

		return std::clamp((int)pattern, 1, n);
	}
}

class PatternStore : private juce::Timer
{
public:
	PatternStore() = default;
	~PatternStore() override;

	PatternStore(const PatternStore&) = delete;
	PatternStore& operator=(const PatternStore&) = delete;

	// プロセッサの構築で、音源の init (PrHelper の setup) より前に呼ぶ。APVTS の
	// パラメータを見て区分を集め、setup から引けるように登録する
	void attach(juce::AudioProcessorValueTreeState& apvts, StepTableStore& tables);

	// setup から引く。attach していなければ nullptr (テストや FX だけのプラグイン)
	static PatternStore* find(const juce::AudioProcessorValueTreeState& apvts);

	// 音の側が読むポインタ。区分の組に入るパラメータなら live、そうでなければ APVTS
	static std::atomic<float>* raw(juce::AudioProcessorValueTreeState& apvts, const juce::String& id);

	// 区分の表を引く。組ごとの表に切り替わるよう、slot (Ptrs の table) を覚えておく
	template <typename T>
	static T* bindTable(juce::AudioProcessorValueTreeState& apvts, StepTableStore& tables, const juce::String& key, float defaultR, float defaultL, T** slot)
	{
		auto* table = &tables.get(key, defaultR, defaultL);

		if (auto* store = find(apvts)) store->registerTable(key, defaultR, defaultL, table, (void**)slot);

		return table;
	}

	// 始め直しの数。区分の頭 (code + EnvPattern::ssgHwEnv など) で引く。無ければ nullptr
	static const std::atomic<uint32_t>* restartOf(juce::AudioProcessorValueTreeState& apvts, const juce::String& base);

	// ブロックの頭で呼ぶ (音のスレッド)
	void process();

	// 区分の頭から、いま画面で選んでいる組 (PATTERN を PATTERNS で抑えたもの)
	int editingPattern(const juce::String& base) const;

	// 画面が引く表の鍵。key (code + 表の頭) の区分で選んでいる組の表があればその鍵、
	// 無ければ (組を持たない区分・まだ作っていない組) key のまま
	juce::String tableKeyFor(const juce::String& key) const;

	// state への出し入れ。
	//   書く   : write (表を書いたあと)
	//   読む   : prepareTables → (表を読む) → read → (APVTS を戻す) → settle
	// prepareTables は組の表を作る (表を読むのは作ってある表だけなので)。read は組の
	// 写しを読み、settle は戻した APVTS (いま選んでいる組) を写しへ入れて音の側を合わせる
	void write(juce::XmlElement& parent) const;
	void prepareTables(const juce::XmlElement& state);
	void read(const juce::XmlElement& state);
	void settle();

	// 区分の頭が token で終わる区分の、始め直しの数の和 (UNISON の ARP を始め直すのに使う)
	uint32_t restartSum(const juce::String& token) const;

	// 初期化。code が空ならすべての区分、そうでなければ code で始まる区分の組を初期値へ戻す
	void reset(const juce::String& code = {});

	// 組の表を、組の数 (PATTERNS) まで作る。メッセージスレッドから呼ぶ
	void ensureTables();

	// 組の表を作り、音の側が切り替えた組の値を APVTS へ戻す。メッセージスレッドのタイマーが
	// 呼ぶ (テストからも直に呼ぶ)
	void update();

	static inline const juce::String xmlTag = "PATTERNS";
	static inline const juce::String xmlSection = "SECTION";
	static inline const juce::String xmlPattern = "P";

private:
	struct TableBind
	{
		juce::String key;
		float defaultR = 0.0f;
		float defaultL = 0.0f;

		// [1〜32]。無い組は nullptr
		std::array<std::atomic<StepTable*>, EnvPattern::maxPatterns + 1> tables{};

		// 表を指す Ptrs の table (const StepTable* か StepTable*)
		std::vector<void**> slots;
	};

	struct Section
	{
		juce::String base;

		std::vector<juce::RangedAudioParameter*> params;
		std::vector<std::atomic<float>*> apvtsRaw;
		std::vector<float> defaults;

		// 音の側が読む値
		std::unique_ptr<std::atomic<float>[]> live;

		// [1〜32] の組の写し
		std::array<std::unique_ptr<std::atomic<float>[]>, EnvPattern::maxPatterns + 1> slots;

		std::atomic<float>* patterns = nullptr;
		std::atomic<float>* pattern = nullptr;
		std::atomic<float>* resetFlag = nullptr;

		// 音の側が使っている組
		std::atomic<int> active{ 1 };

		// APVTS が映している組。0 はメッセージスレッドが入れ替えている途中
		std::atomic<int> editing{ 1 };

		std::atomic<uint32_t> restart{ 0 };

		std::vector<std::unique_ptr<TableBind>> tables;

		int indexOf(const juce::String& id) const;
	};

	void registerTable(const juce::String& key, float defaultR, float defaultL, StepTable* table, void** slot);
	Section* sectionOfTable(const juce::String& key) const;
	Section* sectionOf(const juce::String& base) const;

	// 組の値を APVTS へ戻す (メッセージスレッド)
	void load(Section& s, int pattern);

	void timerCallback() override;

	juce::AudioProcessorValueTreeState* apvts = nullptr;
	StepTableStore* tableStore = nullptr;

	std::vector<std::unique_ptr<Section>> sections;

	// 最後に process を呼んだ時刻 (ミリ秒)。音が止まっているあいだは、タイマーが代わりに
	// 写す (画面のスレッドで作る波形プレビューが、音の側と同じ値を読むため)
	std::atomic<uint32_t> lastProcess{ 0 };

	// パラメータの ID から (区分, 組の中の番号)
	std::map<juce::String, std::pair<Section*, int>> members;

	// 区分の頭から区分、表の鍵から (区分, 表)。画面が頻繁に引くので表で持つ
	std::map<juce::String, Section*> byBase;
	std::map<juce::String, std::pair<Section*, TableBind*>> byTable;

	static std::mutex& registryLock();
	static std::map<const juce::AudioProcessorValueTreeState*, PatternStore*>& registry();
};
