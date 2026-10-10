#pragma once

#include <JuceHeader.h>

#include <deque>
#include <functional>
#include <map>
#include <optional>
#include <vector>

#include "Shared/Core/Gui/GuiContext.h"

class GuiCategoryLabel;

// ============================================================================
// パラメータの切り取り・コピー・貼り付け・消去
// ============================================================================
// チャンネル全体 (グローバル部分)、オペレーター / パッド 1 つ、区分 1 つを
// 単位にして、パラメータを写したり初期値へ戻したりする。
//
// 値は APVTS のパラメータ ID から「チャンネルの名前」と「オペレーター・
// パッド・枠の番号」を外した名前 (キー) で持つ。貼るときは貼り先の
// パラメータを同じ規則でキーにし、同じキーのものだけを入れる。音源の
// 種類が違うチャンネルへ貼っても、相手に無いものは読み飛ばされる。
//
//   OPM_LFO_FREQ      → チャンネル: LFO_FREQ
//   OPM_OP2_R_AR      → オペレーター: R_AR  区分: R_AR
//   WTPLUS_SLOT3_SPEED→ チャンネル: SLOT3_SPEED  区分: SPEED
//
// 波形や音声ファイルなど、パラメータの外に持っているもの (素材) も一緒に
// 写す。変調波形 (modWavePaths) はここで扱い、チャンネルごとの音声ファイル
// などは GuiEditorHost の collectClipAssets / applyClipAssets でタブへ頼む。
namespace ParamEdit
{
	// 何を単位にしたか
	enum class Scope
	{
		Channel,   // チャンネルのグローバル部分 (オペレーター・パッドを除く)
		Unit,      // オペレーター / パッド 1 つ
		Category   // 区分 1 つ
	};

	// 写したもの 1 件。履歴にも同じものを並べる。
	struct Snapshot
	{
		int serial = 0;
		Scope scope = Scope::Channel;

		// 一覧に出す名前
		juce::String tab;       // 写したタブ (OPM / CURVE / FX など)
		juce::String unit;      // OP2 / PAD3 など。グローバルなら空
		juce::String category;  // 区分の見出し。区分でなければ空
		juce::Time time;

		std::map<juce::String, float> values;         // キー → 値 (実際の値)
		std::map<juce::String, juce::String> assets;  // 素材のキー → ファイルの場所

		bool isEmpty() const { return values.empty() && assets.empty(); }

		// 一覧に出す「何を写したか」
		juce::String scopeName() const;
	};

	// 写す先・写す元
	struct Target
	{
		Scope scope = Scope::Channel;

		juce::String tab;       // 一覧に出すタブの名前
		juce::String channel;   // APVTS のプレフィックス (OPM など)
		juce::String unit;      // OP2 / PAD3 など (Unit のとき)
		juce::String category;  // 区分の見出し (Category のとき)

		// Category のとき、区分に入っているパラメータの ID
		std::vector<juce::String> ids;

		// Category のとき、区分に入っている変調波形の置き場 (modWavePaths のキー)
		std::vector<juce::String> waveKeys;

		// Category のとき、区分に入っている素材の持ち主 (collectClipAssets の prefix)
		std::vector<juce::String> assetPrefixes;

		juce::String prefix() const { return unit.isEmpty() ? channel : channel + "_" + unit; }
	};

	// ---- ID の分け方
	juce::String channelOf(const juce::String& id);
	bool isUnitToken(const juce::String& token);
	juce::String keyOf(const juce::String& id, Scope scope);

	// ---- 写す・貼る・消す
	Snapshot capture(const GuiContext& ctx, const Target& target);

	// 貼ったパラメータの数を返す。withAssets を切ると、素材 (波形・音声
	// ファイル) には触らず、貼り先の素材をそのまま残す。
	int paste(const GuiContext& ctx, const Target& target, const Snapshot& snapshot, bool withAssets = true);

	void clear(const GuiContext& ctx, const Target& target);

	// オペレーターなど番号付きの単位を、まとめて別の番号へ写す。
	// head は単位の頭 (OP)、from / to は最初の番号、count はいくつ写すか。
	// クリップボードと履歴には入れない。
	void duplicateUnits(const GuiContext& ctx, const juce::String& channel, const juce::String& head,
		int from, int to, int count);

	// 番号付きの単位 1 つを、いくつかの単位へ写す (WT+ / ADPCM+ の TARGET、
	// RHYTHM のパッド)。withParams を切ると素材 (波形・音声ファイル) だけを、
	// withAssets を切るとパラメータだけを写す。
	void duplicateUnitTo(const GuiContext& ctx, const juce::String& channel, const juce::String& head,
		int from, const std::vector<int>& to, bool withParams, bool withAssets = true);

	// 番号付きの単位をいくつか、初期の状態へ戻す (パラメータも素材も)
	void clearUnits(const GuiContext& ctx, const juce::String& channel, const juce::String& head,
		const std::vector<int>& units);

	// ---- 貼る・消すときに読み飛ばすパラメータ
	// RHYTHM の NOTE のように、書き換えると鍵盤の振り分けが変わってしまう
	// ものを、画面のトグル (With Note) で触らないようにする。key は単位の
	// 番号を外したキー (NOTE など)。プラグインの窓 (APVTS) とチャンネルごとに
	// 持つ。貼り付け (区分・単位の複写も) と初期化 (CUT / CLEAR) に効く。
	void setEditSkipped(const GuiContext& ctx, const juce::String& channel, const juce::String& key, bool skipped);
	bool isEditSkipped(const GuiContext& ctx, const juce::String& channel, const juce::String& key);

	// 写して、クリップボードへ入れて履歴へ足す
	void copy(const GuiContext& ctx, const Target& target);
	void cut(const GuiContext& ctx, const Target& target);
	bool pasteClipboard(const GuiContext& ctx, const Target& target);

	// ---- 写している間の覆い
	// 貼るのに少し時間が掛かるので、その間は読み込み中の画面 (GuiLoading) に
	// 「Duplicating…」と出して待たせる。入れ子にしても、外側の 1 つだけが
	// 出し入れする。
	//
	// 処理はその場で (同じ呼び出しの中で) 行う。覆いを先に描かせてから
	// 始めるので、棒は止まって見えるが文字は読める。
	class BusyScope
	{
	public:
		explicit BusyScope(const GuiContext& ctx);
		~BusyScope();

		BusyScope(const BusyScope&) = delete;
		BusyScope& operator=(const BusyScope&) = delete;
	private:
		GuiEditorHost& m_editor;
	};

	// ---- クリップボードと履歴
	// プロセスに 1 つ。同じプラグインを複数立てても、ほかの窓で写したものを貼れる。
	class Clipboard : public juce::ChangeBroadcaster
	{
	public:
		static Clipboard& get();

		const std::optional<Snapshot>& current() const { return m_current; }
		const std::deque<Snapshot>& history() const { return m_history; }

		// 新しく写したもの。履歴の先頭へ足す
		void push(Snapshot snapshot);

		// 履歴から選び直す (再コピー)
		void select(int serial);

		static inline constexpr int maxHistory = 200;
	private:
		std::optional<Snapshot> m_current;
		std::deque<Snapshot> m_history;
		int m_nextSerial = 1;
	};

	// ---- 区分 (見出しの右クリック)
	// 区分に入っているパラメータを集める。閉じている区分は、いったん開いて
	// 並べ直してから調べ、元へ戻す。
	Target categoryTarget(GuiCategoryLabel& label);

	// 見出しを右クリックしたときのメニュー
	void showCategoryMenu(GuiCategoryLabel& label);

	// 枠 (FX の効果など、見出しを持たない区分) の中身と、右クリックのメニュー。
	// 枠の内側に置かれた部品を区分の中身とみなす。
	Target groupTarget(const GuiContext& ctx, juce::GroupComponent& group);
	void showGroupMenu(const GuiContext& ctx, juce::GroupComponent& group);

	// 部品が属しているタブの名前 (タブの見出し)。タブの外なら空
	juce::String tabNameOf(juce::Component& component);

	// ---- 変調波形を貼った・消したことの知らせ
	// 波形は部品の外 (プロセッサ) に持っているので、貼っても部品の絵は
	// 変わらない。部品はこれを持っておき、知らせを受けたら描き直す。
	class WaveWatcher : private juce::ChangeListener
	{
	public:
		explicit WaveWatcher(std::function<void()> onChange);
		~WaveWatcher() override;
	private:
		void changeListenerCallback(juce::ChangeBroadcaster*) override { if (m_onChange) m_onChange(); }

		std::function<void()> m_onChange;
	};

	// 変調波形を入れ替えたら呼ぶ
	void notifyWavesChanged();

	// ---- タブが持つ素材 (collectClipAssets / applyClipAssets の中身)
	// 素材 1 つぶん。set へ空の File を渡すと外す。
	struct AssetSlot
	{
		juce::String key;
		std::function<juce::String()> get;
		std::function<void(const juce::File&)> set;
	};

	void collectAssets(const std::vector<AssetSlot>& slots, std::map<juce::String, juce::String>& out);
	void applyAssets(const std::vector<AssetSlot>& slots, const std::map<juce::String, juce::String>& assets, bool clearOthers);

	// 素材の持ち主 (ADPCMP_SLOT3) から番号を取り出す。channel + "_" + head + 番号
	// の形でなければ -1
	int unitIndexOf(const juce::String& prefix, const juce::String& channel, const juce::String& head);

	// 素材を持つ部品へ付ける印 (区分の右クリックで素材も写すため)
	namespace Property
	{
		// 変調波形の置き場 (modWavePaths のキー)
		inline const juce::Identifier waveKey{ "clipWaveKey" };

		// タブが持つ素材の持ち主 (collectClipAssets の prefix)
		inline const juce::Identifier assetPrefix{ "clipAssetPrefix" };

		// assetPrefix へこれを入れると、区分のパラメータと同じ持ち主
		// (チャンネル、またはオペレーター・パッド・枠) とみなす。TARGET で
		// 指し先が変わる部品でも、印を付け直さずに済む。
		inline const juce::String sameOwner = "@";
	}
}
