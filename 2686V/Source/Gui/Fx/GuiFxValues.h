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
		static inline constexpr int MainWidth = 300;
		static inline constexpr int MainHeight = 120;
		static inline constexpr int MainHeightRoute = 320;
		static inline constexpr int AreaWidth = 150;
		static inline constexpr int AreaHeightBig = 166;
		static inline constexpr int AreaHeightMini = 136;
		static inline constexpr int AreaHeight = 146;
		static inline constexpr int AreaHeightRow1 = 170;
		static inline constexpr int HeightFilter = 160;
		static inline constexpr int HeightEq3b = 170;
		static inline constexpr int AreaHeightRow2 = 140;
		static inline constexpr int HeightTremoro = 140;
		static inline constexpr int HeightVibrato = 140;
		static inline constexpr int AreaHeightRow3 = 140;
		static inline constexpr int HeightMbc = 140;
		static inline constexpr int HeightDelay = 140;
		static inline constexpr int AreaHeightRow4 = 280;
		static inline constexpr int HeightReverb = 166;
		static inline constexpr int HeightSfcEcho = 280;
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
