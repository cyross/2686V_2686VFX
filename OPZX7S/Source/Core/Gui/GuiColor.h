#pragma once

#include <JuceHeader.h>

namespace GuiColor {
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
		static inline const juce::Colour OffWhite = juce::Colour::fromRGB(0xF5, 0xF5, 0xF5);
		static inline const juce::Colour OffBlack = juce::Colour::fromRGB(0x39, 0x42, 0x3F);

		// 初音ミクの髪留めやイヤホンを参照した差し色と、それに合わせる黒。
		// カテゴリの開閉スイッチで使う。
		static inline const juce::Colour MikuPink = juce::Colour::fromRGB(0xFF, 0x33, 0x99);
		static inline const juce::Colour MikuBlack = juce::Colours::black;

		// 面と背景の境目を締める線と、部品の右下へ落とす影。
		static inline const juce::Colour BorderGray = juce::Colours::grey.withAlpha(0.3f);
		static inline const juce::Colour ShadowGray = juce::Colours::grey.withAlpha(0.5f);

		// 青を 8 割ぶん白へ寄せた淡い青。スライダーのバーと、
		// ダイアログの OK ボタンで同じ色を共有する。
		static inline const juce::Colour SliderBarBlue = juce::Colours::blue.interpolatedWith(juce::Colours::white, 0.8f);
		static inline const juce::Colour DialogOkBackBlue = SliderBarBlue;

		// ------------------------------------------------------------------
		// 実機由来の色
		// ------------------------------------------------------------------
		// 当時の機種の筐体やラベルから採った色。UI の既定では使わないが、
		// 色をカスタマイズするときの選択肢として名前を与えておく。
		// 行末のコメントは画面に出す表示名。
		namespace Retro {
			static inline const juce::Colour P66mk2Body   = juce::Colour::fromRGB(0xB0, 0xB9, 0xBE); // P66mk2 Body    PC-6601mk2 の筐体色
			static inline const juce::Colour P66mk2Label  = juce::Colour::fromRGB(0x61, 0x68, 0x70); // P66mk2 Label   PC-6601mk2 のラベル背景色
			static inline const juce::Colour P66mk2Key    = juce::Colour::fromRGB(0xB6, 0xB1, 0x94); // P66mk2 Key     PC-6601mk2 のキー色
			static inline const juce::Colour P66mk2FncKey = juce::Colour::fromRGB(0x5D, 0x58, 0x52); // P66mk2 FncKey  PC-6601mk2 のファンクションキー色
			static inline const juce::Colour Pc88Ma2Hd    = juce::Colour::fromRGB(0xF7, 0xA1, 0x0C); // PC88MA2 2HD    PC-8801MA2 の「2HD」ラベル背景色
			static inline const juce::Colour Pc98FBody2   = juce::Colour::fromRGB(0x47, 0x42, 0x3E); // PC98F Body2    PC-9801F の筐体に入っていた灰色
			static inline const juce::Colour Pc98RxBody   = juce::Colour::fromRGB(0xB1, 0xB2, 0xAB); // PC98RX Body    PC-9801RX21 の筐体色
			static inline const juce::Colour Pc98RxLogo   = juce::Colour::fromRGB(0x36, 0x35, 0x39); // PC98RX Logo    PC-9801RX21 のロゴ色
			static inline const juce::Colour MdGold       = juce::Colour::fromRGB(0xD0, 0xA6, 0x70); // MD Gold        メガドライブの「16-BIT」の文字色
			static inline const juce::Colour MdBody2      = juce::Colour::fromRGB(0x9E, 0x2B, 0x58); // MD Body2       メガドライブの差し色のマゼンタ
			static inline const juce::Colour MdSwitch     = juce::Colour::fromRGB(0x00, 0x68, 0xAB); // MD Switch      メガドライブのスイッチの青
			static inline const juce::Colour PceLogo      = juce::Colour::fromRGB(0xFF, 0x5C, 0x42); // PCE Logo       PC エンジンのロゴのオレンジ
			static inline const juce::Colour PceBody2     = juce::Colour::fromRGB(0x65, 0x64, 0x64); // PCE Body2      PC エンジンのコントローラーの灰色
			static inline const juce::Colour PceBody      = juce::Colour::fromRGB(0xE8, 0xE7, 0xE4); // PCE Body       初代 PC エンジンの筐体の白
			static inline const juce::Colour FcLabel      = juce::Colour::fromRGB(0xCF, 0x29, 0x37); // FC Label       初代ファミコンのレバーやカバーの赤
			static inline const juce::Colour FcGold       = juce::Colour::fromRGB(0xD1, 0xB6, 0x8C); // FC Gold        初代ファミコンのロゴの金
			static inline const juce::Colour GbScrOn      = juce::Colour::fromRGB(0x33, 0x52, 0x3A); // GB SCR ON      初代ゲームボーイの液晶 (点灯)
			static inline const juce::Colour GbScrOff     = juce::Colour::fromRGB(0x89, 0x97, 0x28); // GB SCR OFF     初代ゲームボーイの液晶 (消灯)
		};
	};

	static inline const juce::Colour defaultFgColor = juce::Colours::white.withAlpha(0.8f);
	static inline const juce::Colour defaultBgColor = juce::Colours::transparentBlack;
	namespace Editor {
		static inline const juce::Colour defaultBg = juce::Colour::fromFloatRGBA(0.15f, 0.15f, 0.15f, 1.0f);
		static inline const juce::Colour wallpaperBg = juce::Colours::black.withAlpha(0.2f);
		static inline const juce::Colour blurWallpaperBg = juce::Colours::black.withAlpha(0.5f);
	};
	namespace Group {
		static inline const juce::Colour Text = defaultFgColor;
		static inline const juce::Colour Bg = juce::Colours::black.withAlpha(0.4f);
		static inline const juce::Colour Border = defaultFgColor;

		// 見出しはカテゴリ見出しと同じく「明るい帯に黒文字」で出す。
		// ただしカテゴリの 3 色とは役割が違うので、色味を持たない明るい灰にして
		// 「区分ではなく入れ物」であることが分かるようにする。
		static inline const juce::Colour TitleBg = juce::Colours::lightgrey.brighter(0.3f);
		static inline const juce::Colour TitleText = juce::Colours::black;
	};

	namespace Separator {
		// 白のベタ線は主張が強すぎるので、カテゴリの板と同じくらいまで落とす
		static inline const juce::Colour Line = juce::Colours::white.withAlpha(0.45f);
	};

	namespace EnvelopeGraph {
		// 線の色は波形プレビューと同じ体系。何のエンベロープかで分ける。
		static inline const juce::Colour AmpLine = juce::Colours::turquoise.interpolatedWith(juce::Colours::white, 0.6f);
		static inline const juce::Colour PitchLine = juce::Colours::blue.interpolatedWith(juce::Colours::white, 0.6f);

		// リリースだけは別物として読めるよう、テーマの差し色を当てる。
		static inline const juce::Colour ReleaseLine = Palette::MikuPink;

		// 区間の切れ目に引く縦線。目盛りとして読めればよいので、
		// 本体より一段引いた白にして色数を増やさない。
		static inline const juce::Colour PhaseLine = juce::Colours::white.withAlpha(0.35f);
	};

	namespace WavePreview {
		// 何を映しているかが色で分かるようにする。どれも白へ 6 割寄せた
		// 淡い色にして、暗い下地の上で刺さらないようにしてある。
		static inline const juce::Colour Lfo = juce::Colours::green.interpolatedWith(juce::Colours::white, 0.6f);
		static inline const juce::Colour AmpEnv = juce::Colours::turquoise.interpolatedWith(juce::Colours::white, 0.6f);
		static inline const juce::Colour PitchEnv = juce::Colours::blue.interpolatedWith(juce::Colours::white, 0.6f);
		static inline const juce::Colour WaveMemory = juce::Colours::yellow.interpolatedWith(juce::Colours::white, 0.6f);
		static inline const juce::Colour AudioFile = juce::Colours::red.interpolatedWith(juce::Colours::white, 0.6f);
	};
	namespace ScrollBar {
		static inline const juce::Colour Thumb = juce::Colours::darkgrey;
	};
	namespace Label {
		static inline const juce::Colour Text = defaultFgColor;
		static inline const juce::Colour CategoryText = juce::Colours::yellow;

		// スライダーやセレクタの左に出るラベルの地色。
		// 行がひと続きの帯に見えるようにするためのものなので、
		// 文字が読みにくくならない程度にごく薄く敷く。
		static inline const juce::Colour RowBg = juce::Colours::white.withAlpha(0.08f);
	};
	namespace Category {
		// 80〜90 年代を意識したレトロモダン。文字は黒、背景はカテゴリ色。
		static inline const juce::Colour Text = juce::Colours::black;

		// 黒い文字を載せるので、素の色より明るくして陰にならないようにする。
		// そのうえで白との中間まで退かせ、彩度を落とす。見出しは画面に多数
		// 並ぶため、鮮やかなままだと中身より目立ってしまう。
		// 色の役割 (ハード / ソフト / その他) は色相で残る。
		static inline const juce::Colour HwBg = juce::Colours::yellow.brighter(0.5f).interpolatedWith(juce::Colours::white, 0.5f);
		static inline const juce::Colour SwBg = juce::Colours::aqua.brighter(0.5f).interpolatedWith(juce::Colours::white, 0.5f);
		static inline const juce::Colour OtherBg = juce::Colours::lime.brighter(0.5f).interpolatedWith(juce::Colours::white, 0.5f);

		// 音質にかかわるカテゴリ。他の区分と並んだときに一目で分かるよう、
		// ハード / ソフト / その他とは別の色相を当てる。
		static inline const juce::Colour QualityBg = juce::Colours::red.brighter(0.5f).interpolatedWith(juce::Colours::white, 0.5f);

		// SETTINGS の見出し。設定画面はカテゴリの色分けを持たないので、
		// 色味のない明るい地に置く。
		static inline const juce::Colour SettingsBg = Palette::OffWhite;

		// カテゴリを開いたとき、中身の背後へ敷く色。
		static inline const juce::Colour ContentBg = juce::Colours::black.withAlpha(0.5f);

		// 開閉マーカー。閉じているときは黒、開いているときはピンク。
		// ピンクは初音ミクの髪留めやイヤホンの色を参照した差し色。
		static inline const juce::Colour MarkerClosed = Palette::MikuBlack;
		static inline const juce::Colour MarkerOpen = Palette::MikuPink;

		// マーカーの縁。背景が黄でも水色でも見えるよう常に暗い色を敷く。
		static inline const juce::Colour MarkerBorder = juce::Colours::black.withAlpha(0.6f);
	};

	// 明るい面を持つ部品 (ボタン・コンボボックス) の輪郭。
	static inline const juce::Colour Outline = Palette::BorderGray;
	namespace Slider {
		// FF1 のステータスバーを意識した見た目。枠の中を左から塗る。
		static inline const juce::Colour Bar = Palette::SliderBarBlue;
		static inline const juce::Colour Frame = Palette::BorderGray;

		// まだ塗られていない側。暗くしておくと、塗った部分が伸びて見える。
		static inline const juce::Colour Trough = juce::Colours::black.withAlpha(0.35f);

		// つまみは置かない形になったので Thumb は使っていない。
		// Track は Config の既定値として残してある。
		static inline const juce::Colour Track = Bar;
		static inline const juce::Colour Thumb = juce::Colour::fromRGB(0x8, 0x8, 0xB3);
		namespace Value {
			static inline const juce::Colour Text = Palette::OffWhite;
			static inline const juce::Colour Highlight = juce::Colour::fromFloatRGBA(0.7f, 0.7f, 0.03f, 0.2f);
			static inline const juce::Colour Border = Palette::BorderGray;
		};
	};
	namespace ComboBox {
		static inline const juce::Colour Text = Palette::OffBlack;
		static inline const juce::Colour Bg = Palette::OffWhite;
		static inline const juce::Colour Arrow = juce::Colours::grey;

		// ドロップダウン。選んでいる行だけ地色を上げて、
		// それ以外は一段落とすことで、どこにいるかが分かるようにする。
		static inline const juce::Colour PopupSelectedBg = Palette::OffWhite;
		static inline const juce::Colour PopupBg = Palette::OffWhite.darker(0.2f);
	};
	namespace ToggleButton {
		static inline const juce::Colour Fg = defaultFgColor;

		// 四角の枠は常に同じ色で、中のランプだけが状態で入れ替わる。
		static inline const juce::Colour Box = Palette::OffWhite;
		static inline const juce::Colour LampOn = Palette::OffWhite;
		static inline const juce::Colour LampOff = Palette::OffBlack;
	};
	namespace TextButton {
		static inline const juce::Colour Text = Palette::OffBlack;
		static inline const juce::Colour TextOn = Palette::OffBlack;
		static inline const juce::Colour Bg = Palette::OffWhite;
		static inline const juce::Colour Border = Palette::BorderGray;
	};
	namespace HyperLink {
		static inline const juce::Colour Fg = juce::Colours::lightblue;
	};
	namespace TableList {
		static inline const juce::Colour Text = defaultFgColor;
		static inline const juce::Colour SelectedBg = juce::Colours::lightblue.withAlpha(0.5f);
		static inline const juce::Colour RowBg1 = defaultBgColor;
		static inline const juce::Colour RowBg2 = juce::Colours::white.withAlpha(0.1f);
	};
	namespace TextEditor {
		static inline const juce::Colour Text = defaultFgColor;
		static inline const juce::Colour Bg = defaultBgColor;
		static inline const juce::Colour Border = defaultFgColor;
	};
	namespace WaveformContainer {
		static inline const juce::Colour Track = juce::Colours::white.withAlpha(0.5f);
		static inline const juce::Colour Thumb = juce::Colour::fromRGB(0x8, 0x8, 0xB3);
		namespace ResetBtn {
			static inline const juce::Colour P01 = juce::Colours::white.withAlpha(0.7f);
			static inline const juce::Colour P001 = juce::Colours::grey.withAlpha(0.7f);
			static inline const juce::Colour To1 = juce::Colours::red.withAlpha(0.7f);
			static inline const juce::Colour To05 = juce::Colours::yellow.withAlpha(0.7f);
			static inline const juce::Colour To0 = juce::Colours::green.withAlpha(0.7f);
			static inline const juce::Colour ToM05 = juce::Colours::cyan.withAlpha(0.7f);
			static inline const juce::Colour ToM1 = juce::Colours::blue.withAlpha(0.7f);
			static inline const juce::Colour M001 = juce::Colours::grey.withAlpha(0.7f);
			static inline const juce::Colour M01 = juce::Colours::black.withAlpha(0.7f);
		}
	};
	namespace Waveform2Container {
		static inline const juce::Colour Track = juce::Colours::white.withAlpha(0.5f);
		static inline const juce::Colour Thumb = juce::Colour::fromRGB(0x8, 0x8, 0xB3);
		namespace ResetBtn {
			static inline const juce::Colour ToCenter = juce::Colours::green.withAlpha(0.7f);
			static inline const juce::Colour ToMax = juce::Colours::red.withAlpha(0.7f);
			static inline const juce::Colour To0 = juce::Colours::blue.withAlpha(0.7f);
		}
	};
	namespace Settings {
		static inline const juce::Colour SaveAsDefaultBtnBg = juce::Colours::green.withAlpha(0.5f);
	};
};
