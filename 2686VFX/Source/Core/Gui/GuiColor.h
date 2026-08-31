#pragma once

#include <JuceHeader.h>

#include <map>
#include <functional>
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
		// 既定値は色そのものではなく「作り方」で持つ。
		//
		// juce::Colour の引数付きコンストラクタが constexpr ではないため、
		// juce::Colours の中身はプログラムが始まってから順に作られる。
		// 既定値をここで値として書くと、まだ作られていない色や、まだ
		// 作られていない別の Entry を写してしまうことがある。写した先は
		// 黒なので、色を変えても黒いままという形で表に出る。
		//
		// 作るのを最初に使うときまで遅らせれば、この順番に左右されない。
		// 触るのは画面を描くときだけなので、遅らせた初期化が重なる心配は
		// 無い。
		using Maker = juce::Colour (*)();

		Maker m_make;

		mutable juce::Colour m_default;
		mutable juce::Colour m_current;
		mutable bool m_ready = false;

		void ensure() const
		{
			if (m_ready) return;

			// 先に立てておく。別の Entry を参照している場合に、
			// 万一の循環でここへ戻ってきても止まらなくなるのを防ぐ。
			m_ready = true;

			m_default = m_make();
			m_current = m_default;
		}
	public:
		Entry(const char* id, Maker make);

		operator juce::Colour() const { return get(); }

		// 明るさや透明度を変えて使いたいときは、いったん色として取り出す
		juce::Colour get() const { ensure(); return m_current; }

		juce::Colour defaultColour() const { ensure(); return m_default; }

		void set(juce::Colour colour) { ensure(); m_current = colour; }
		void reset() { ensure(); m_current = m_default; }
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

	// 色を部品へ焼き付けている場所のための道具。
	//
	// paint の中で色を読んでいる場所は、描き直すだけで新しい色になる。
	// けれど setColour で JUCE 側の色 ID へ写している場所は、写した
	// ときの値が残るので描き直しても変わらない。そういう場所は
	// これを持たせて、色が変わったら写し直す。
	class Refresher : private juce::ChangeListener
	{
		std::function<void()> m_apply;
	public:
		explicit Refresher(std::function<void()> apply);
		~Refresher() override;
	private:
		void changeListenerCallback(juce::ChangeBroadcaster*) override
		{
			if (m_apply) m_apply();
		}
	};

	// 色に付いている名前を返す。名前の付いていない色は #RRGGBBAA で返す。
	// 名前として扱うのは Palette 以下のものだけで、部品ごとの割り当ては含めない。
	juce::String describe(juce::Colour colour);

	// 名前の付いた色の一覧 (表示名 → 色)。COLORS タブの選択肢に使う。
	std::vector<std::pair<juce::String, juce::Colour>> namedColours();

	// JUCE が持っている色名。こちらで付けた名前とは分けて扱う。
	const std::vector<std::pair<juce::String, juce::Colour>>& juceColours();

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
		inline Entry OffWhite{ "Palette.OffWhite", []() -> juce::Colour { return juce::Colour::fromRGB(0xF5, 0xF5, 0xF5); } };
		inline Entry OffBlack{ "Palette.OffBlack", []() -> juce::Colour { return juce::Colour::fromRGB(0x39, 0x42, 0x3F); } };

		// 初音ミクの髪留めやイヤホンを参照した差し色と、それに合わせる黒。
		// カテゴリの開閉スイッチで使う。
		inline Entry MikuPink{ "Palette.MikuPink", []() -> juce::Colour { return juce::Colour::fromRGB(0xFF, 0x33, 0x99); } };
		inline Entry MikuBlack{ "Palette.MikuBlack", []() -> juce::Colour { return juce::Colours::black; } };

		// 面と背景の境目を締める線と、部品の右下へ落とす影。
		inline Entry BorderGray{ "Palette.BorderGray", []() -> juce::Colour { return juce::Colours::grey.withAlpha(0.3f); } };
		inline Entry ShadowGray{ "Palette.ShadowGray", []() -> juce::Colour { return juce::Colours::grey.withAlpha(0.5f); } };

		// 青を 8 割ぶん白へ寄せた淡い青。スライダーのバーと、
		// ダイアログの OK ボタンで同じ色を共有する。
		inline Entry SliderBarBlue{ "Palette.SliderBarBlue", []() -> juce::Colour { return juce::Colours::blue.interpolatedWith(juce::Colours::white, 0.8f); } };
		inline Entry DialogOkBackBlue{ "Palette.DialogOkBackBlue", []() -> juce::Colour { return SliderBarBlue; } };

		// ------------------------------------------------------------------
		// 実機由来の色
		// ------------------------------------------------------------------
		// 当時の機種の筐体やラベルから採った色。UI の既定では使わないが、
		// 色をカスタマイズするときの選択肢として名前を与えておく。
		// 行末のコメントは画面に出す表示名。
		namespace Retro {
			inline Entry P66mk2Body{ "Palette.Retro.P66mk2Body", []() -> juce::Colour { return juce::Colour::fromRGB(0xB0, 0xB9, 0xBE); } }; // P66mk2 Body    PC-6601mk2 の筐体色
			inline Entry P66mk2Label{ "Palette.Retro.P66mk2Label", []() -> juce::Colour { return juce::Colour::fromRGB(0x61, 0x68, 0x70); } }; // P66mk2 Label   PC-6601mk2 のラベル背景色
			inline Entry P66mk2Key{ "Palette.Retro.P66mk2Key", []() -> juce::Colour { return juce::Colour::fromRGB(0xB6, 0xB1, 0x94); } }; // P66mk2 Key     PC-6601mk2 のキー色
			inline Entry P66mk2FncKey{ "Palette.Retro.P66mk2FncKey", []() -> juce::Colour { return juce::Colour::fromRGB(0x5D, 0x58, 0x52); } }; // P66mk2 FncKey  PC-6601mk2 のファンクションキー色
			inline Entry Pc88Ma2Hd{ "Palette.Retro.Pc88Ma2Hd", []() -> juce::Colour { return juce::Colour::fromRGB(0xF7, 0xA1, 0x0C); } }; // PC88MA2 2HD    PC-8801MA2 の「2HD」ラベル背景色
			inline Entry Pc98FBody2{ "Palette.Retro.Pc98FBody2", []() -> juce::Colour { return juce::Colour::fromRGB(0x47, 0x42, 0x3E); } }; // PC98F Body2    PC-9801F の筐体に入っていた灰色
			inline Entry Pc98RxBody{ "Palette.Retro.Pc98RxBody", []() -> juce::Colour { return juce::Colour::fromRGB(0xB1, 0xB2, 0xAB); } }; // PC98RX Body    PC-9801RX21 の筐体色
			inline Entry Pc98RxLogo{ "Palette.Retro.Pc98RxLogo", []() -> juce::Colour { return juce::Colour::fromRGB(0x36, 0x35, 0x39); } }; // PC98RX Logo    PC-9801RX21 のロゴ色
			inline Entry MdGold{ "Palette.Retro.MdGold", []() -> juce::Colour { return juce::Colour::fromRGB(0xD0, 0xA6, 0x70); } }; // MD Gold        メガドライブの「16-BIT」の文字色
			inline Entry MdBody2{ "Palette.Retro.MdBody2", []() -> juce::Colour { return juce::Colour::fromRGB(0x9E, 0x2B, 0x58); } }; // MD Body2       メガドライブの差し色のマゼンタ
			inline Entry MdSwitch{ "Palette.Retro.MdSwitch", []() -> juce::Colour { return juce::Colour::fromRGB(0x00, 0x68, 0xAB); } }; // MD Switch      メガドライブのスイッチの青
			inline Entry PceLogo{ "Palette.Retro.PceLogo", []() -> juce::Colour { return juce::Colour::fromRGB(0xFF, 0x5C, 0x42); } }; // PCE Logo       PC エンジンのロゴのオレンジ
			inline Entry PceBody2{ "Palette.Retro.PceBody2", []() -> juce::Colour { return juce::Colour::fromRGB(0x65, 0x64, 0x64); } }; // PCE Body2      PC エンジンのコントローラーの灰色
			inline Entry PceBody{ "Palette.Retro.PceBody", []() -> juce::Colour { return juce::Colour::fromRGB(0xE8, 0xE7, 0xE4); } }; // PCE Body       初代 PC エンジンの筐体の白
			inline Entry FcLabel{ "Palette.Retro.FcLabel", []() -> juce::Colour { return juce::Colour::fromRGB(0xCF, 0x29, 0x37); } }; // FC Label       初代ファミコンのレバーやカバーの赤
			inline Entry FcGold{ "Palette.Retro.FcGold", []() -> juce::Colour { return juce::Colour::fromRGB(0xD1, 0xB6, 0x8C); } }; // FC Gold        初代ファミコンのロゴの金
			inline Entry GbScrOn{ "Palette.Retro.GbScrOn", []() -> juce::Colour { return juce::Colour::fromRGB(0x33, 0x52, 0x3A); } }; // GB SCR ON      初代ゲームボーイの液晶 (点灯)
			inline Entry GbScrOff{ "Palette.Retro.GbScrOff", []() -> juce::Colour { return juce::Colour::fromRGB(0x89, 0x97, 0x28); } }; // GB SCR OFF     初代ゲームボーイの液晶 (消灯)
			inline Entry TownsBody{ "Palette.Retro.TownsBody", []() -> juce::Colour { return juce::Colour::fromRGB(0x45, 0x4E, 0x5C); } }; // TOWNS Body     初代 FM-TOWNS の筐体色
			inline Entry TownsLogo{ "Palette.Retro.TownsLogo", []() -> juce::Colour { return juce::Colour::fromRGB(0x34, 0x81, 0x78); } }; // TOWNS Logo     初代 FM-TOWNS のロゴ
		};

		// ------------------------------------------------------------------
		// キャラクター由来の色
		// ------------------------------------------------------------------
		// 作者の個人的なイメージで付けた色。公式の設定とは関係が無いので、
		// 画面に出すときは必ずその旨のただし書きを添えること。
		namespace Chara {
			inline Entry MikuTurquoise{ "Palette.Chara.MikuTurquoise", []() -> juce::Colour { return juce::Colour::fromRGB(0x90, 0xCB, 0xCB); } }; // Miku Turquoise 初音ミクの髪色
			inline Entry RinOrange{ "Palette.Chara.RinOrange", []() -> juce::Colour { return juce::Colour::fromRGB(0xEB, 0xA2, 0x39); } }; // Rin Orange     鏡音リンのイメージカラー
			inline Entry LenYellow{ "Palette.Chara.LenYellow", []() -> juce::Colour { return juce::Colour::fromRGB(0xF6, 0xD9, 0x57); } }; // Len Yellow     鏡音レンのイメージカラー
			inline Entry LukaPink{ "Palette.Chara.LukaPink", []() -> juce::Colour { return juce::Colour::fromRGB(0xC3, 0xA1, 0x9E); } }; // Luka Pink      巡音ルカの髪色
			inline Entry LukaBrown{ "Palette.Chara.LukaBrown", []() -> juce::Colour { return juce::Colour::fromRGB(0x58, 0x38, 0x30); } }; // Luka Brown     巡音ルカの腕章の色
			inline Entry LukaGold{ "Palette.Chara.LukaGold", []() -> juce::Colour { return juce::Colour::fromRGB(0xC1, 0xAC, 0x60); } }; // Luka Gold      巡音ルカのブーツの色
			inline Entry MeikoRed{ "Palette.Chara.MeikoRed", []() -> juce::Colour { return juce::Colour::fromRGB(0xBA, 0x2B, 0x3C); } }; // MEIKO Red      MEIKO のイメージカラー
			inline Entry KaitoBlue{ "Palette.Chara.KaitoBlue", []() -> juce::Colour { return juce::Colour::fromRGB(0x38, 0x3F, 0x8B); } }; // KAITO Blue     KAITO のイメージカラー
			inline Entry ZundaGreen{ "Palette.Chara.ZundaGreen", []() -> juce::Colour { return juce::Colour::fromRGB(0xB4, 0xF7, 0x42); } }; // Zunda Green    ずんだもんのイメージカラー
		};
	};

	inline Entry defaultFgColor{ "defaultFgColor", []() -> juce::Colour { return juce::Colours::white.withAlpha(0.8f); } };
	inline Entry defaultBgColor{ "defaultBgColor", []() -> juce::Colour { return juce::Colours::transparentBlack; } };
	namespace Editor {
		inline Entry defaultBg{ "Editor.defaultBg", []() -> juce::Colour { return juce::Colour::fromFloatRGBA(0.15f, 0.15f, 0.15f, 1.0f); } };
		inline Entry wallpaperBg{ "Editor.wallpaperBg", []() -> juce::Colour { return juce::Colours::black.withAlpha(0.2f); } };
		inline Entry blurWallpaperBg{ "Editor.blurWallpaperBg", []() -> juce::Colour { return juce::Colours::black.withAlpha(0.5f); } };
	};
	namespace Group {
		inline Entry Text{ "Group.Text", []() -> juce::Colour { return defaultFgColor; } };
		inline Entry Bg{ "Group.Bg", []() -> juce::Colour { return juce::Colours::black.withAlpha(0.4f); } };
		inline Entry Border{ "Group.Border", []() -> juce::Colour { return defaultFgColor; } };

		// 見出しはカテゴリ見出しと同じく「明るい帯に黒文字」で出す。
		// ただしカテゴリの 3 色とは役割が違うので、色味を持たない明るい灰にして
		// 「区分ではなく入れ物」であることが分かるようにする。
		inline Entry TitleBg{ "Group.TitleBg", []() -> juce::Colour { return juce::Colours::lightgrey.brighter(0.3f); } };
		inline Entry TitleText{ "Group.TitleText", []() -> juce::Colour { return juce::Colours::black; } };
	};

	namespace Separator {
		// 白のベタ線は主張が強すぎるので、カテゴリの板と同じくらいまで落とす
		inline Entry Line{ "Separator.Line", []() -> juce::Colour { return juce::Colours::white.withAlpha(0.45f); } };
	};

	namespace EnvelopeGraph {
		// 線の色は波形プレビューと同じ体系。何のエンベロープかで分ける。
		inline Entry AmpLine{ "EnvelopeGraph.AmpLine", []() -> juce::Colour { return juce::Colours::turquoise.interpolatedWith(juce::Colours::white, 0.6f); } };
		inline Entry PitchLine{ "EnvelopeGraph.PitchLine", []() -> juce::Colour { return juce::Colours::blue.interpolatedWith(juce::Colours::white, 0.6f); } };

		// リリースだけは別物として読めるよう、テーマの差し色を当てる。
		inline Entry ReleaseLine{ "EnvelopeGraph.ReleaseLine", []() -> juce::Colour { return Palette::MikuPink; } };

		// 区間の切れ目に引く縦線。目盛りとして読めればよいので、
		// 本体より一段引いた白にして色数を増やさない。
		inline Entry PhaseLine{ "EnvelopeGraph.PhaseLine", []() -> juce::Colour { return juce::Colours::white.withAlpha(0.35f); } };
	};

	namespace WavePreview {
		// 何を映しているかが色で分かるようにする。どれも白へ 6 割寄せた
		// 淡い色にして、暗い下地の上で刺さらないようにしてある。
		inline Entry Lfo{ "WavePreview.Lfo", []() -> juce::Colour { return juce::Colours::green.interpolatedWith(juce::Colours::white, 0.6f); } };
		inline Entry AmpEnv{ "WavePreview.AmpEnv", []() -> juce::Colour { return juce::Colours::turquoise.interpolatedWith(juce::Colours::white, 0.6f); } };
		inline Entry PitchEnv{ "WavePreview.PitchEnv", []() -> juce::Colour { return juce::Colours::blue.interpolatedWith(juce::Colours::white, 0.6f); } };
		inline Entry WaveMemory{ "WavePreview.WaveMemory", []() -> juce::Colour { return juce::Colours::yellow.interpolatedWith(juce::Colours::white, 0.6f); } };
		inline Entry AudioFile{ "WavePreview.AudioFile", []() -> juce::Colour { return juce::Colours::red.interpolatedWith(juce::Colours::white, 0.6f); } };
	};
	namespace ScrollBar {
		inline Entry Thumb{ "ScrollBar.Thumb", []() -> juce::Colour { return juce::Colours::darkgrey; } };
	};
	namespace Label {
		inline Entry Text{ "Label.Text", []() -> juce::Colour { return defaultFgColor; } };
		inline Entry CategoryText{ "Label.CategoryText", []() -> juce::Colour { return juce::Colours::yellow; } };

		// スライダーやセレクタの左に出るラベルの地色。
		// 行がひと続きの帯に見えるようにするためのものなので、
		// 文字が読みにくくならない程度にごく薄く敷く。
		inline Entry RowBg{ "Label.RowBg", []() -> juce::Colour { return juce::Colours::white.withAlpha(0.08f); } };
	};
	// ============================================================================
	// タブの色
	// ============================================================================
	// 音源の系統ごとに色を分ける。どのタブがどれを使うかは各プラグインの
	// GuiLF が決める。タブの並びはプラグインごとに違うため。
	//
	// 値は juce::Colours の定数と同じものを数値で書いてある。名前で
	// 書くと、まだ作られていない色を読む危険があるため。
	namespace Tab
	{
		inline Entry Fm{ "Tab.Fm", []() -> juce::Colour { return juce::Colour(0xff006400); } };            // darkgreen
		inline Entry Ssg{ "Tab.Ssg", []() -> juce::Colour { return juce::Colour(0xff00008b); } };          // darkblue
		inline Entry Wt{ "Tab.Wt", []() -> juce::Colour { return juce::Colour(0xff00ced1); } };            // darkturquoise
		inline Entry Pcm{ "Tab.Pcm", []() -> juce::Colour { return juce::Colour(0xff663399); } };          // rebeccapurple
		inline Entry Beep{ "Tab.Beep", []() -> juce::Colour { return juce::Colour(0xff00008b); } };        // darkblue
		inline Entry Advanced{ "Tab.Advanced", []() -> juce::Colour { return juce::Colour(0xff8b0000); } };// darkred
		inline Entry Utility{ "Tab.Utility", []() -> juce::Colour { return juce::Colour(0xffb8860b); } };  // darkgoldenrod
		inline Entry Other{ "Tab.Other", []() -> juce::Colour { return juce::Colour(0xff555555); } };      // darkgrey
	}

	namespace Category {
		// 80〜90 年代を意識したレトロモダン。文字は黒、背景はカテゴリ色。
		inline Entry Text{ "Category.Text", []() -> juce::Colour { return juce::Colours::black; } };

		// 黒い文字を載せるので、素の色より明るくして陰にならないようにする。
		// そのうえで白との中間まで退かせ、彩度を落とす。見出しは画面に多数
		// 並ぶため、鮮やかなままだと中身より目立ってしまう。
		// 色の役割 (ハード / ソフト / その他) は色相で残る。
		inline Entry HwBg{ "Category.HwBg", []() -> juce::Colour { return juce::Colours::yellow.brighter(0.5f).interpolatedWith(juce::Colours::white, 0.5f); } };
		inline Entry SwBg{ "Category.SwBg", []() -> juce::Colour { return juce::Colours::aqua.brighter(0.5f).interpolatedWith(juce::Colours::white, 0.5f); } };
		inline Entry OtherBg{ "Category.OtherBg", []() -> juce::Colour { return juce::Colours::lime.brighter(0.5f).interpolatedWith(juce::Colours::white, 0.5f); } };

		// 音質にかかわるカテゴリ。他の区分と並んだときに一目で分かるよう、
		// ハード / ソフト / その他とは別の色相を当てる。
		inline Entry QualityBg{ "Category.QualityBg", []() -> juce::Colour { return juce::Colours::red.brighter(0.5f).interpolatedWith(juce::Colours::white, 0.5f); } };

		// SETTINGS の見出し。設定画面はカテゴリの色分けを持たないので、
		// 色味のない明るい地に置く。
		inline Entry SettingsBg{ "Category.SettingsBg", []() -> juce::Colour { return Palette::OffWhite; } };

		// カテゴリを開いたとき、中身の背後へ敷く色。
		inline Entry ContentBg{ "Category.ContentBg", []() -> juce::Colour { return juce::Colours::black.withAlpha(0.5f); } };

		// 開閉マーカー。閉じているときは黒、開いているときはピンク。
		// ピンクは初音ミクの髪留めやイヤホンの色を参照した差し色。
		inline Entry MarkerClosed{ "Category.MarkerClosed", []() -> juce::Colour { return Palette::MikuBlack; } };
		inline Entry MarkerOpen{ "Category.MarkerOpen", []() -> juce::Colour { return Palette::MikuPink; } };

		// マーカーの縁。背景が黄でも水色でも見えるよう常に暗い色を敷く。
		inline Entry MarkerBorder{ "Category.MarkerBorder", []() -> juce::Colour { return juce::Colours::black.withAlpha(0.6f); } };
	};

	// 明るい面を持つ部品 (ボタン・コンボボックス) の輪郭。
	inline Entry Outline{ "Outline", []() -> juce::Colour { return Palette::BorderGray; } };
	namespace Slider {
		// FF1 のステータスバーを意識した見た目。枠の中を左から塗る。
		inline Entry Bar{ "Slider.Bar", []() -> juce::Colour { return Palette::SliderBarBlue; } };
		inline Entry Frame{ "Slider.Frame", []() -> juce::Colour { return Palette::BorderGray; } };

		// まだ塗られていない側。暗くしておくと、塗った部分が伸びて見える。
		inline Entry Trough{ "Slider.Trough", []() -> juce::Colour { return juce::Colours::black.withAlpha(0.35f); } };

		// つまみは置かない形になったので Thumb は使っていない。
		// Track は Config の既定値として残してある。
		inline Entry Track{ "Slider.Track", []() -> juce::Colour { return Bar; } };
		inline Entry Thumb{ "Slider.Thumb", []() -> juce::Colour { return juce::Colour::fromRGB(0x8, 0x8, 0xB3); } };
		namespace Value {
			inline Entry Text{ "Slider.Value.Text", []() -> juce::Colour { return Palette::OffWhite; } };
			inline Entry Highlight{ "Slider.Value.Highlight", []() -> juce::Colour { return juce::Colour::fromFloatRGBA(0.7f, 0.7f, 0.03f, 0.2f); } };
			inline Entry Border{ "Slider.Value.Border", []() -> juce::Colour { return Palette::BorderGray; } };
		};
	};
	namespace ComboBox {
		inline Entry Text{ "ComboBox.Text", []() -> juce::Colour { return Palette::OffBlack; } };
		inline Entry Bg{ "ComboBox.Bg", []() -> juce::Colour { return Palette::OffWhite; } };
		inline Entry Arrow{ "ComboBox.Arrow", []() -> juce::Colour { return juce::Colours::grey; } };

		// ドロップダウン。選んでいる行だけ地色を上げて、
		// それ以外は一段落とすことで、どこにいるかが分かるようにする。
		inline Entry PopupSelectedBg{ "ComboBox.PopupSelectedBg", []() -> juce::Colour { return Palette::OffWhite; } };
		inline Entry PopupBg{ "ComboBox.PopupBg", []() -> juce::Colour { return Palette::OffWhite.get().darker(0.2f); } };
	};
	namespace ToggleButton {
		inline Entry Fg{ "ToggleButton.Fg", []() -> juce::Colour { return defaultFgColor; } };

		// 四角の枠は常に同じ色で、中のランプだけが状態で入れ替わる。
		inline Entry Box{ "ToggleButton.Box", []() -> juce::Colour { return Palette::OffWhite; } };
		inline Entry LampOn{ "ToggleButton.LampOn", []() -> juce::Colour { return Palette::OffWhite; } };
		inline Entry LampOff{ "ToggleButton.LampOff", []() -> juce::Colour { return Palette::OffBlack; } };
	};
	namespace TextButton {
		inline Entry Text{ "TextButton.Text", []() -> juce::Colour { return Palette::OffBlack; } };
		inline Entry TextOn{ "TextButton.TextOn", []() -> juce::Colour { return Palette::OffBlack; } };
		inline Entry Bg{ "TextButton.Bg", []() -> juce::Colour { return Palette::OffWhite; } };
		inline Entry Border{ "TextButton.Border", []() -> juce::Colour { return Palette::BorderGray; } };
	};
	namespace HyperLink {
		inline Entry Fg{ "HyperLink.Fg", []() -> juce::Colour { return juce::Colours::lightblue; } };
	};
	namespace TableList {
		inline Entry Text{ "TableList.Text", []() -> juce::Colour { return defaultFgColor; } };
		inline Entry SelectedBg{ "TableList.SelectedBg", []() -> juce::Colour { return juce::Colours::lightblue.withAlpha(0.5f); } };
		inline Entry RowBg1{ "TableList.RowBg1", []() -> juce::Colour { return defaultBgColor; } };
		inline Entry RowBg2{ "TableList.RowBg2", []() -> juce::Colour { return juce::Colours::white.withAlpha(0.1f); } };
	};
	namespace TextEditor {
		inline Entry Text{ "TextEditor.Text", []() -> juce::Colour { return defaultFgColor; } };
		inline Entry Bg{ "TextEditor.Bg", []() -> juce::Colour { return defaultBgColor; } };
		inline Entry Border{ "TextEditor.Border", []() -> juce::Colour { return defaultFgColor; } };
	};
	namespace WaveformContainer {
		inline Entry Track{ "WaveformContainer.Track", []() -> juce::Colour { return juce::Colours::white.withAlpha(0.5f); } };
		inline Entry Thumb{ "WaveformContainer.Thumb", []() -> juce::Colour { return juce::Colour::fromRGB(0x8, 0x8, 0xB3); } };
		namespace ResetBtn {
			inline Entry P01{ "WaveformContainer.ResetBtn.P01", []() -> juce::Colour { return juce::Colours::white.withAlpha(0.7f); } };
			inline Entry P001{ "WaveformContainer.ResetBtn.P001", []() -> juce::Colour { return juce::Colours::grey.withAlpha(0.7f); } };
			inline Entry To1{ "WaveformContainer.ResetBtn.To1", []() -> juce::Colour { return juce::Colours::red.withAlpha(0.7f); } };
			inline Entry To05{ "WaveformContainer.ResetBtn.To05", []() -> juce::Colour { return juce::Colours::yellow.withAlpha(0.7f); } };
			inline Entry To0{ "WaveformContainer.ResetBtn.To0", []() -> juce::Colour { return juce::Colours::green.withAlpha(0.7f); } };
			inline Entry ToM05{ "WaveformContainer.ResetBtn.ToM05", []() -> juce::Colour { return juce::Colours::cyan.withAlpha(0.7f); } };
			inline Entry ToM1{ "WaveformContainer.ResetBtn.ToM1", []() -> juce::Colour { return juce::Colours::blue.withAlpha(0.7f); } };
			inline Entry M001{ "WaveformContainer.ResetBtn.M001", []() -> juce::Colour { return juce::Colours::grey.withAlpha(0.7f); } };
			inline Entry M01{ "WaveformContainer.ResetBtn.M01", []() -> juce::Colour { return juce::Colours::black.withAlpha(0.7f); } };
		}
	};
	namespace Waveform2Container {
		inline Entry Track{ "Waveform2Container.Track", []() -> juce::Colour { return juce::Colours::white.withAlpha(0.5f); } };
		inline Entry Thumb{ "Waveform2Container.Thumb", []() -> juce::Colour { return juce::Colour::fromRGB(0x8, 0x8, 0xB3); } };
		namespace ResetBtn {
			inline Entry ToCenter{ "Waveform2Container.ResetBtn.ToCenter", []() -> juce::Colour { return juce::Colours::green.withAlpha(0.7f); } };
			inline Entry ToMax{ "Waveform2Container.ResetBtn.ToMax", []() -> juce::Colour { return juce::Colours::red.withAlpha(0.7f); } };
			inline Entry To0{ "Waveform2Container.ResetBtn.To0", []() -> juce::Colour { return juce::Colours::blue.withAlpha(0.7f); } };
		}
	};
	namespace Settings {
		inline Entry SaveAsDefaultBtnBg{ "Settings.SaveAsDefaultBtnBg", []() -> juce::Colour { return juce::Colours::green.withAlpha(0.5f); } };
	};
};
