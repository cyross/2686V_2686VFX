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
		static inline const juce::Colour Track = defaultFgColor;
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
