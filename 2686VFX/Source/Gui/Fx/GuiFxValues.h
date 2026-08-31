// UI で使用するデータを管理

#pragma once

namespace FxGuiValue
{

	namespace PaddingTop
	{
		static inline constexpr int mask = 16;
		static inline constexpr int mmlBtn = 20;
	}

	namespace PaddingBottom
	{
		static inline constexpr int block = 6;
	}

	namespace Padding
	{
		static inline constexpr int space = 8;
	}

	namespace Group
	{
		static inline constexpr int TitlePaddingTop = 10;

		namespace Padding
		{
			static inline constexpr int width = 10;
			static inline constexpr int height = 10;
		}
	}

	namespace Fx
	{
		static inline constexpr int Width = 1000;
		static inline constexpr int Height = 720;
		// 画面全体を同じ幅の列で組む。メインも FX も変調も同じ幅。
		// 左右の余白を引いた 1740px を、メイン 1 列＋残り 5 列で割る。
		static inline constexpr int SideMargin = 20;
		static inline constexpr int ColWidth = 281;
		static inline constexpr int ColGap = 10;
		static inline constexpr int SectionGap = 8;

		// メインの右に並ぶ列の数。FX も変調もこの数で折り返す。
		static inline constexpr int Cols = 5;

		// 出力へ掛ける変調を置く領域。FX は左 400px しか使っていないので、
		// その右の空きへ詰める。タブは増やさない。
		static inline constexpr int ModLeft = 20;
		static inline constexpr int ModRight = 20;
		static inline constexpr int ModBottom = 40;

		// 入り切りの札を並べる帯。列の見出しの上に置く。
		static inline constexpr int ModHeaderHeight = 30;
		static inline constexpr int ModHeaderGap = 6;
		static inline constexpr int ModToggleWidth = 220;

		// 変調の枠の数。効果と一緒に横一列へ並べる。
		static inline constexpr int ModPanels = 9;

		// WT PITCH MOD の基準周波数。実機には無い、こちらで足したもの。
		static inline constexpr int ModBaseFreqHeight = 40;
		static inline constexpr int MainHeight = 120;
		static inline constexpr int MainHeightRoute = 320;
		static inline constexpr int AreaWidth = 200;
		static inline constexpr int AreaHeightBig = 166;
		static inline constexpr int AreaHeightMini = 136;
		static inline constexpr int AreaHeight = 146;
		// FX の段は 2 つ。1 段目は背の低いもの、2 段目は背の高いもの。
		static inline constexpr int AreaHeightRow1 = 170;
		static inline constexpr int HeightFilter = 160;
		static inline constexpr int HeightEq3b = 170;
		static inline constexpr int AreaHeightRow2 = 280;
		static inline constexpr int HeightTremoro = 140;
		static inline constexpr int HeightVibrato = 140;
		static inline constexpr int AreaHeightRow3 = 140;
		static inline constexpr int HeightMbc = 140;
		static inline constexpr int HeightDelay = 140;
		static inline constexpr int AreaHeightRow4 = 280;
		static inline constexpr int HeightReverb = 166;
		static inline constexpr int HeightSfcEcho = 280;
		static inline constexpr int HeightPcm = 260;
		static inline constexpr int AreaLabelWidth = 40;
		static inline constexpr int MixBtnWidth = 40;
		static inline constexpr int GlobalBypassHeight = 120;
		static inline constexpr int BypassWidth = 120;
		static inline constexpr int GroupHeight = 200;
		static inline constexpr int KnobAreaWidth = 200;
		static inline constexpr int KnobWidth = 120;
		static inline constexpr int KnobHeight = 60;
		static inline constexpr int ButtonWidth = 240;
		static inline constexpr int SeparatorHeight = 16;
	}
}
