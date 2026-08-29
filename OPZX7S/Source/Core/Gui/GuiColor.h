#pragma once

#include <JuceHeader.h>

#include <map>
#include <utility>
#include <vector>

namespace GuiColor {
	// ============================================================================
	// 差し替えられる色
	// ============================================================================
	// 色は SETTINGS から変えられるようにするため、定数ではなく Entry で持つ。
	//
	// juce::Colour への変換演算子があるので、参照する側はこれまでどおり
	// 名前を書くだけでよい。明るさや透明度を変えて使うときだけ get() を挟む。
	class Entry
	{
		juce::Colour m_default;
		juce::Colour m_current;
	public:
		Entry(const char* id, juce::Colour defaultColour);

		operator juce::Colour() const { return m_current; }

		// 明るさや透明度を変えて使いたいときは、いったん色として取り出す
		juce::Colour get() const { return m_current; }

		juce::Colour defaultColour() const { return m_default; }

		void set(juce::Colour colour) { m_current = colour; }
		void reset() { m_current = m_default; }
	};

	// 名前 → 実体。COLORS タブで一覧したり、設定を読み書きしたりするのに使う。
	//
	// 静的初期化の順番に左右されないよう、表は関数の中に置く。Entry は
	// 各翻訳単位で作られる前にこの関数を通るので、必ず出来上がっている。
	std::map<juce::String, Entry*>& registry();

	// 色が変わったことを画面へ知らせる。エディタが受けて全体を描き直す。
	juce::ChangeBroadcaster& changeBroadcaster();

	// 名前で引いて差し替える。無い名前は何もしない。
	void setColour(const juce::String& id, juce::Colour colour);
	void resetColour(const juce::String& id);
	void resetAllColours();

	// 色に付いている名前を返す。名前の付いていない色は #RRGGBBAA で返す。
	// 名前として扱うのは Palette 以下のものだけで、部品ごとの割り当ては含めない。
	juce::String describe(juce::Colour colour);

	// 名前の付いた色の一覧 (表示名 → 色)。COLORS タブの選択肢に使う。
	std::vector<std::pair<juce::String, juce::Colour>> namedColours();

	// ============================================================================
	// 色の名前
	// ============================================================================
	// テーマは「80 年代・90 年代・チップチューンを意識したレトロモダン」。
	//
	// 各部品はここで名前を付けた色だけを参照し、juce::Colours の値を直接
	// 書かないこと。あとから SETTINGS タブで差し替えられるようにするため、
	// 色の実体はこのファイルの外へ散らさない。
	namespace Palette {
		// 真っ白・真っ黒は画面が硬くなるので、少し濁らせた白黒を使う。
		static inline Entry OffWhite{ "Palette.OffWhite", juce::Colour::fromRGB(0xF5, 0xF5, 0xF5) };
		static inline Entry OffBlack{ "Palette.OffBlack", juce::Colour::fromRGB(0x39, 0x42, 0x3F) };

		// 初音ミクの髪留めやイヤホンを参照した差し色と、それに合わせる黒。
		// カテゴリの開閉スイッチで使う。
		static inline Entry MikuPink{ "Palette.MikuPink", juce::Colour::fromRGB(0xFF, 0x33, 0x99) };
		static inline Entry MikuBlack{ "Palette.MikuBlack", juce::Colours::black };

		// 面と背景の境目を締める線と、部品の右下へ落とす影。
		static inline Entry BorderGray{ "Palette.BorderGray", juce::Colours::grey.withAlpha(0.3f) };
		static inline Entry ShadowGray{ "Palette.ShadowGray", juce::Colours::grey.withAlpha(0.5f) };

		// 青を 8 割ぶん白へ寄せた淡い青。スライダーのバーと、
		// ダイアログの OK ボタンで同じ色を共有する。
		static inline Entry SliderBarBlue{ "Palette.SliderBarBlue", juce::Colours::blue.interpolatedWith(juce::Colours::white, 0.8f) };
		static inline Entry DialogOkBackBlue{ "Palette.DialogOkBackBlue", SliderBarBlue };

		// ------------------------------------------------------------------
		// 実機由来の色
		// ------------------------------------------------------------------
		// 当時の機種の筐体やラベルから採った色。UI の既定では使わないが、
		// 色をカスタマイズするときの選択肢として名前を与えておく。
		// 行末のコメントは画面に出す表示名。
		namespace Retro {
			static inline Entry P66mk2Body{ "Palette.Retro.P66mk2Body", juce::Colour::fromRGB(0xB0, 0xB9, 0xBE) }; // P66mk2 Body    PC-6601mk2 の筐体色
			static inline Entry P66mk2Label{ "Palette.Retro.P66mk2Label", juce::Colour::fromRGB(0x61, 0x68, 0x70) }; // P66mk2 Label   PC-6601mk2 のラベル背景色
			static inline Entry P66mk2Key{ "Palette.Retro.P66mk2Key", juce::Colour::fromRGB(0xB6, 0xB1, 0x94) }; // P66mk2 Key     PC-6601mk2 のキー色
			static inline Entry P66mk2FncKey{ "Palette.Retro.P66mk2FncKey", juce::Colour::fromRGB(0x5D, 0x58, 0x52) }; // P66mk2 FncKey  PC-6601mk2 のファンクションキー色
			static inline Entry Pc88Ma2Hd{ "Palette.Retro.Pc88Ma2Hd", juce::Colour::fromRGB(0xF7, 0xA1, 0x0C) }; // PC88MA2 2HD    PC-8801MA2 の「2HD」ラベル背景色
			static inline Entry Pc98FBody2{ "Palette.Retro.Pc98FBody2", juce::Colour::fromRGB(0x47, 0x42, 0x3E) }; // PC98F Body2    PC-9801F の筐体に入っていた灰色
			static inline Entry Pc98RxBody{ "Palette.Retro.Pc98RxBody", juce::Colour::fromRGB(0xB1, 0xB2, 0xAB) }; // PC98RX Body    PC-9801RX21 の筐体色
			static inline Entry Pc98RxLogo{ "Palette.Retro.Pc98RxLogo", juce::Colour::fromRGB(0x36, 0x35, 0x39) }; // PC98RX Logo    PC-9801RX21 のロゴ色
			static inline Entry MdGold{ "Palette.Retro.MdGold", juce::Colour::fromRGB(0xD0, 0xA6, 0x70) }; // MD Gold        メガドライブの「16-BIT」の文字色
			static inline Entry MdBody2{ "Palette.Retro.MdBody2", juce::Colour::fromRGB(0x9E, 0x2B, 0x58) }; // MD Body2       メガドライブの差し色のマゼンタ
			static inline Entry MdSwitch{ "Palette.Retro.MdSwitch", juce::Colour::fromRGB(0x00, 0x68, 0xAB) }; // MD Switch      メガドライブのスイッチの青
			static inline Entry PceLogo{ "Palette.Retro.PceLogo", juce::Colour::fromRGB(0xFF, 0x5C, 0x42) }; // PCE Logo       PC エンジンのロゴのオレンジ
			static inline Entry PceBody2{ "Palette.Retro.PceBody2", juce::Colour::fromRGB(0x65, 0x64, 0x64) }; // PCE Body2      PC エンジンのコントローラーの灰色
			static inline Entry PceBody{ "Palette.Retro.PceBody", juce::Colour::fromRGB(0xE8, 0xE7, 0xE4) }; // PCE Body       初代 PC エンジンの筐体の白
			static inline Entry FcLabel{ "Palette.Retro.FcLabel", juce::Colour::fromRGB(0xCF, 0x29, 0x37) }; // FC Label       初代ファミコンのレバーやカバーの赤
			static inline Entry FcGold{ "Palette.Retro.FcGold", juce::Colour::fromRGB(0xD1, 0xB6, 0x8C) }; // FC Gold        初代ファミコンのロゴの金
			static inline Entry GbScrOn{ "Palette.Retro.GbScrOn", juce::Colour::fromRGB(0x33, 0x52, 0x3A) }; // GB SCR ON      初代ゲームボーイの液晶 (点灯)
			static inline Entry GbScrOff{ "Palette.Retro.GbScrOff", juce::Colour::fromRGB(0x89, 0x97, 0x28) }; // GB SCR OFF     初代ゲームボーイの液晶 (消灯)
			static inline Entry TownsBody{ "Palette.Retro.TownsBody", juce::Colour::fromRGB(0x45, 0x4E, 0x5C) }; // TOWNS Body     初代 FM-TOWNS の筐体色
			static inline Entry TownsLogo{ "Palette.Retro.TownsLogo", juce::Colour::fromRGB(0x34, 0x81, 0x78) }; // TOWNS Logo     初代 FM-TOWNS のロゴ
		};

		// ------------------------------------------------------------------
		// キャラクター由来の色
		// ------------------------------------------------------------------
		// 作者の個人的なイメージで付けた色。公式の設定とは関係が無いので、
		// 画面に出すときは必ずその旨のただし書きを添えること。
		namespace Chara {
			static inline Entry MikuTurquoise{ "Palette.Chara.MikuTurquoise", juce::Colour::fromRGB(0x90, 0xCB, 0xCB) }; // Miku Turquoise 初音ミクの髪色
			static inline Entry RinOrange{ "Palette.Chara.RinOrange", juce::Colour::fromRGB(0xEB, 0xA2, 0x39) }; // Rin Orange     鏡音リンのイメージカラー
			static inline Entry LenYellow{ "Palette.Chara.LenYellow", juce::Colour::fromRGB(0xF6, 0xD9, 0x57) }; // Len Yellow     鏡音レンのイメージカラー
			static inline Entry LukaPink{ "Palette.Chara.LukaPink", juce::Colour::fromRGB(0xC3, 0xA1, 0x9E) }; // Luka Pink      巡音ルカの髪色
			static inline Entry LukaBrown{ "Palette.Chara.LukaBrown", juce::Colour::fromRGB(0x58, 0x38, 0x30) }; // Luka Brown     巡音ルカの腕章の色
			static inline Entry LukaGold{ "Palette.Chara.LukaGold", juce::Colour::fromRGB(0xC1, 0xAC, 0x60) }; // Luka Gold      巡音ルカのブーツの色
			static inline Entry MeikoRed{ "Palette.Chara.MeikoRed", juce::Colour::fromRGB(0xBA, 0x2B, 0x3C) }; // MEIKO Red      MEIKO のイメージカラー
			static inline Entry KaitoBlue{ "Palette.Chara.KaitoBlue", juce::Colour::fromRGB(0x38, 0x3F, 0x8B) }; // KAITO Blue     KAITO のイメージカラー
			static inline Entry ZundaGreen{ "Palette.Chara.ZundaGreen", juce::Colour::fromRGB(0xB4, 0xF7, 0x42) }; // Zunda Green    ずんだもんのイメージカラー
		};
	};

	static inline Entry defaultFgColor{ "defaultFgColor", juce::Colours::white.withAlpha(0.8f) };
	static inline Entry defaultBgColor{ "defaultBgColor", juce::Colours::transparentBlack };
	namespace Editor {
		static inline Entry defaultBg{ "Editor.defaultBg", juce::Colour::fromFloatRGBA(0.15f, 0.15f, 0.15f, 1.0f) };
		static inline Entry wallpaperBg{ "Editor.wallpaperBg", juce::Colours::black.withAlpha(0.2f) };
		static inline Entry blurWallpaperBg{ "Editor.blurWallpaperBg", juce::Colours::black.withAlpha(0.5f) };
	};
	namespace Group {
		static inline Entry Text{ "Group.Text", defaultFgColor };
		static inline Entry Bg{ "Group.Bg", juce::Colours::black.withAlpha(0.4f) };
		static inline Entry Border{ "Group.Border", defaultFgColor };

		// 見出しはカテゴリ見出しと同じく「明るい帯に黒文字」で出す。
		// ただしカテゴリの 3 色とは役割が違うので、色味を持たない明るい灰にして
		// 「区分ではなく入れ物」であることが分かるようにする。
		static inline Entry TitleBg{ "Group.TitleBg", juce::Colours::lightgrey.brighter(0.3f) };
		static inline Entry TitleText{ "Group.TitleText", juce::Colours::black };
	};

	namespace Separator {
		// 白のベタ線は主張が強すぎるので、カテゴリの板と同じくらいまで落とす
		static inline Entry Line{ "Separator.Line", juce::Colours::white.withAlpha(0.45f) };
	};

	namespace EnvelopeGraph {
		// 線の色は波形プレビューと同じ体系。何のエンベロープかで分ける。
		static inline Entry AmpLine{ "EnvelopeGraph.AmpLine", juce::Colours::turquoise.interpolatedWith(juce::Colours::white, 0.6f) };
		static inline Entry PitchLine{ "EnvelopeGraph.PitchLine", juce::Colours::blue.interpolatedWith(juce::Colours::white, 0.6f) };

		// リリースだけは別物として読めるよう、テーマの差し色を当てる。
		static inline Entry ReleaseLine{ "EnvelopeGraph.ReleaseLine", Palette::MikuPink };

		// 区間の切れ目に引く縦線。目盛りとして読めればよいので、
		// 本体より一段引いた白にして色数を増やさない。
		static inline Entry PhaseLine{ "EnvelopeGraph.PhaseLine", juce::Colours::white.withAlpha(0.35f) };
	};

	namespace WavePreview {
		// 何を映しているかが色で分かるようにする。どれも白へ 6 割寄せた
		// 淡い色にして、暗い下地の上で刺さらないようにしてある。
		static inline Entry Lfo{ "WavePreview.Lfo", juce::Colours::green.interpolatedWith(juce::Colours::white, 0.6f) };
		static inline Entry AmpEnv{ "WavePreview.AmpEnv", juce::Colours::turquoise.interpolatedWith(juce::Colours::white, 0.6f) };
		static inline Entry PitchEnv{ "WavePreview.PitchEnv", juce::Colours::blue.interpolatedWith(juce::Colours::white, 0.6f) };
		static inline Entry WaveMemory{ "WavePreview.WaveMemory", juce::Colours::yellow.interpolatedWith(juce::Colours::white, 0.6f) };
		static inline Entry AudioFile{ "WavePreview.AudioFile", juce::Colours::red.interpolatedWith(juce::Colours::white, 0.6f) };
	};
	namespace ScrollBar {
		static inline Entry Thumb{ "ScrollBar.Thumb", juce::Colours::darkgrey };
	};
	namespace Label {
		static inline Entry Text{ "Label.Text", defaultFgColor };
		static inline Entry CategoryText{ "Label.CategoryText", juce::Colours::yellow };

		// スライダーやセレクタの左に出るラベルの地色。
		// 行がひと続きの帯に見えるようにするためのものなので、
		// 文字が読みにくくならない程度にごく薄く敷く。
		static inline Entry RowBg{ "Label.RowBg", juce::Colours::white.withAlpha(0.08f) };
	};
	namespace Category {
		// 80〜90 年代を意識したレトロモダン。文字は黒、背景はカテゴリ色。
		static inline Entry Text{ "Category.Text", juce::Colours::black };

		// 黒い文字を載せるので、素の色より明るくして陰にならないようにする。
		// そのうえで白との中間まで退かせ、彩度を落とす。見出しは画面に多数
		// 並ぶため、鮮やかなままだと中身より目立ってしまう。
		// 色の役割 (ハード / ソフト / その他) は色相で残る。
		static inline Entry HwBg{ "Category.HwBg", juce::Colours::yellow.brighter(0.5f).interpolatedWith(juce::Colours::white, 0.5f) };
		static inline Entry SwBg{ "Category.SwBg", juce::Colours::aqua.brighter(0.5f).interpolatedWith(juce::Colours::white, 0.5f) };
		static inline Entry OtherBg{ "Category.OtherBg", juce::Colours::lime.brighter(0.5f).interpolatedWith(juce::Colours::white, 0.5f) };

		// 音質にかかわるカテゴリ。他の区分と並んだときに一目で分かるよう、
		// ハード / ソフト / その他とは別の色相を当てる。
		static inline Entry QualityBg{ "Category.QualityBg", juce::Colours::red.brighter(0.5f).interpolatedWith(juce::Colours::white, 0.5f) };

		// SETTINGS の見出し。設定画面はカテゴリの色分けを持たないので、
		// 色味のない明るい地に置く。
		static inline Entry SettingsBg{ "Category.SettingsBg", Palette::OffWhite };

		// カテゴリを開いたとき、中身の背後へ敷く色。
		static inline Entry ContentBg{ "Category.ContentBg", juce::Colours::black.withAlpha(0.5f) };

		// 開閉マーカー。閉じているときは黒、開いているときはピンク。
		// ピンクは初音ミクの髪留めやイヤホンの色を参照した差し色。
		static inline Entry MarkerClosed{ "Category.MarkerClosed", Palette::MikuBlack };
		static inline Entry MarkerOpen{ "Category.MarkerOpen", Palette::MikuPink };

		// マーカーの縁。背景が黄でも水色でも見えるよう常に暗い色を敷く。
		static inline Entry MarkerBorder{ "Category.MarkerBorder", juce::Colours::black.withAlpha(0.6f) };
	};

	// 明るい面を持つ部品 (ボタン・コンボボックス) の輪郭。
	static inline Entry Outline{ "Outline", Palette::BorderGray };
	namespace Slider {
		// FF1 のステータスバーを意識した見た目。枠の中を左から塗る。
		static inline Entry Bar{ "Slider.Bar", Palette::SliderBarBlue };
		static inline Entry Frame{ "Slider.Frame", Palette::BorderGray };

		// まだ塗られていない側。暗くしておくと、塗った部分が伸びて見える。
		static inline Entry Trough{ "Slider.Trough", juce::Colours::black.withAlpha(0.35f) };

		// つまみは置かない形になったので Thumb は使っていない。
		// Track は Config の既定値として残してある。
		static inline Entry Track{ "Slider.Track", Bar };
		static inline Entry Thumb{ "Slider.Thumb", juce::Colour::fromRGB(0x8, 0x8, 0xB3) };
		namespace Value {
			static inline Entry Text{ "Slider.Value.Text", Palette::OffWhite };
			static inline Entry Highlight{ "Slider.Value.Highlight", juce::Colour::fromFloatRGBA(0.7f, 0.7f, 0.03f, 0.2f) };
			static inline Entry Border{ "Slider.Value.Border", Palette::BorderGray };
		};
	};
	namespace ComboBox {
		static inline Entry Text{ "ComboBox.Text", Palette::OffBlack };
		static inline Entry Bg{ "ComboBox.Bg", Palette::OffWhite };
		static inline Entry Arrow{ "ComboBox.Arrow", juce::Colours::grey };

		// ドロップダウン。選んでいる行だけ地色を上げて、
		// それ以外は一段落とすことで、どこにいるかが分かるようにする。
		static inline Entry PopupSelectedBg{ "ComboBox.PopupSelectedBg", Palette::OffWhite };
		static inline Entry PopupBg{ "ComboBox.PopupBg", Palette::OffWhite.get().darker(0.2f) };
	};
	namespace ToggleButton {
		static inline Entry Fg{ "ToggleButton.Fg", defaultFgColor };

		// 四角の枠は常に同じ色で、中のランプだけが状態で入れ替わる。
		static inline Entry Box{ "ToggleButton.Box", Palette::OffWhite };
		static inline Entry LampOn{ "ToggleButton.LampOn", Palette::OffWhite };
		static inline Entry LampOff{ "ToggleButton.LampOff", Palette::OffBlack };
	};
	namespace TextButton {
		static inline Entry Text{ "TextButton.Text", Palette::OffBlack };
		static inline Entry TextOn{ "TextButton.TextOn", Palette::OffBlack };
		static inline Entry Bg{ "TextButton.Bg", Palette::OffWhite };
		static inline Entry Border{ "TextButton.Border", Palette::BorderGray };
	};
	namespace HyperLink {
		static inline Entry Fg{ "HyperLink.Fg", juce::Colours::lightblue };
	};
	namespace TableList {
		static inline Entry Text{ "TableList.Text", defaultFgColor };
		static inline Entry SelectedBg{ "TableList.SelectedBg", juce::Colours::lightblue.withAlpha(0.5f) };
		static inline Entry RowBg1{ "TableList.RowBg1", defaultBgColor };
		static inline Entry RowBg2{ "TableList.RowBg2", juce::Colours::white.withAlpha(0.1f) };
	};
	namespace TextEditor {
		static inline Entry Text{ "TextEditor.Text", defaultFgColor };
		static inline Entry Bg{ "TextEditor.Bg", defaultBgColor };
		static inline Entry Border{ "TextEditor.Border", defaultFgColor };
	};
	namespace WaveformContainer {
		static inline Entry Track{ "WaveformContainer.Track", juce::Colours::white.withAlpha(0.5f) };
		static inline Entry Thumb{ "WaveformContainer.Thumb", juce::Colour::fromRGB(0x8, 0x8, 0xB3) };
		namespace ResetBtn {
			static inline Entry P01{ "WaveformContainer.ResetBtn.P01", juce::Colours::white.withAlpha(0.7f) };
			static inline Entry P001{ "WaveformContainer.ResetBtn.P001", juce::Colours::grey.withAlpha(0.7f) };
			static inline Entry To1{ "WaveformContainer.ResetBtn.To1", juce::Colours::red.withAlpha(0.7f) };
			static inline Entry To05{ "WaveformContainer.ResetBtn.To05", juce::Colours::yellow.withAlpha(0.7f) };
			static inline Entry To0{ "WaveformContainer.ResetBtn.To0", juce::Colours::green.withAlpha(0.7f) };
			static inline Entry ToM05{ "WaveformContainer.ResetBtn.ToM05", juce::Colours::cyan.withAlpha(0.7f) };
			static inline Entry ToM1{ "WaveformContainer.ResetBtn.ToM1", juce::Colours::blue.withAlpha(0.7f) };
			static inline Entry M001{ "WaveformContainer.ResetBtn.M001", juce::Colours::grey.withAlpha(0.7f) };
			static inline Entry M01{ "WaveformContainer.ResetBtn.M01", juce::Colours::black.withAlpha(0.7f) };
		}
	};
	namespace Waveform2Container {
		static inline Entry Track{ "Waveform2Container.Track", juce::Colours::white.withAlpha(0.5f) };
		static inline Entry Thumb{ "Waveform2Container.Thumb", juce::Colour::fromRGB(0x8, 0x8, 0xB3) };
		namespace ResetBtn {
			static inline Entry ToCenter{ "Waveform2Container.ResetBtn.ToCenter", juce::Colours::green.withAlpha(0.7f) };
			static inline Entry ToMax{ "Waveform2Container.ResetBtn.ToMax", juce::Colours::red.withAlpha(0.7f) };
			static inline Entry To0{ "Waveform2Container.ResetBtn.To0", juce::Colours::blue.withAlpha(0.7f) };
		}
	};
	namespace Settings {
		static inline Entry SaveAsDefaultBtnBg{ "Settings.SaveAsDefaultBtnBg", juce::Colours::green.withAlpha(0.5f) };
	};
};
