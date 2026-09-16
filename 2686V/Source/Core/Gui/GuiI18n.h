#pragma once

#include <JuceHeader.h>

// ============================================================================
// 画面に出す文字列の言語
// ============================================================================
// 日本語と英語を切り替える。実機の用語 (AR / TL / ALG や区分の名前) は、
// どちらの言語でも英語のまま出す。実機の資料やマニュアルと字面を揃える
// ためで、訳し分けるのは説明の文とボタンの名前だけにしてある。
//
// DAW へ出すパラメータの名前 (ProcessorNames.h) はここに含めない。言語で
// 変えるとオートメーションの名前が変わり、前に作った曲のつながりが切れる。
namespace I18n
{
	enum class Lang { ja, en };

	// 設定ファイルへ書く名前。
	//
	// 番号で持つと、あとから言語を足したときに並びがずれ、前に保存した
	// 設定が別の言語を指してしまう。名前で持てばずれない。
	static inline const char* const jaCode = "ja";
	static inline const char* const enCode = "en";

	// いまの言語。
	//
	// 色 (GuiColor) と同じく、プラグインの実体ごとではなく全体で 1 つ持つ。
	// 同じ DAW に複数立てたときは、あとから設定したほうに揃う。
	Lang current();
	void setCurrent(Lang lang);

	inline bool isJa() { return current() == Lang::ja; }

	juce::String toCode(Lang lang);
	Lang fromCode(const juce::String& code);

	// 設定ファイルに言語が書かれていないとき (初めて立ち上げたとき) の
	// 見立て。OS の言語が日本語なら日本語、それ以外は英語にする。
	Lang detect();

	// 言語が変わったことを画面へ知らせる。エディタが受けてタブを作り直す。
	juce::ChangeBroadcaster& changeBroadcaster();

	// ========================================================================
	// 訳を組にして持つ文字列
	// ========================================================================
	// 参照する側はこれまでどおり名前を書くだけでよいように、juce::String
	// への変換演算子を持たせる。setup へ渡した時点で juce::String になる
	// ので、言語を変えたときは画面を作り直す (PluginEditor を参照)。
	//
	// 中身は juce::String ではなく文字の並びのまま持つ。juce::String を
	// 静的な置き場で作ると、プログラムが始まる前に順番の定まらない初期化
	// が走る。色で同じことを踏んでいる (GuiColor.h の Entry を参照)。
	// 1 か所きりの文字列。名前を付けて Text.h へ置くまでもないものは、
	// 使うところで訳を並べる。ダイアログの本文などがこれにあたる。
	juce::String pick(const char8_t* ja, const char8_t* en);

	class Text
	{
		const char8_t* m_ja;
		const char8_t* m_en;

	public:
		constexpr Text(const char8_t* ja, const char8_t* en) : m_ja(ja), m_en(en) {}

		// 訳し分けないもの。実機の用語はこちらで書く。
		constexpr explicit Text(const char8_t* both) : m_ja(both), m_en(both) {}

		operator juce::String() const { return get(); }

		juce::String get() const { return get(current()); }

		juce::String get(Lang lang) const
		{
			return juce::String::fromUTF8(lang == Lang::ja ? m_ja : m_en);
		}
	};
}
