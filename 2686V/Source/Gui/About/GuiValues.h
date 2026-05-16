// UI で使用するデータを管理

#pragma once

#include <JuceHeader.h>

namespace AboutGuiValue
{
	namespace LogoImage
	{
		static inline constexpr int areaHeight = 280;
		static inline constexpr int imageSize = 256;
	}

	namespace PluginName
	{
		static inline constexpr int areaHeight = 60;
		static inline const juce::String fontFamily = "Times New Roman";
		static inline constexpr float fontSize = 32.0f;
	}

	namespace PluginVerion
	{
		static inline constexpr int areaHeight = 40;
		static inline constexpr float fontSize = 20.0f;
	}

	namespace PluginCioyright
	{
		static inline constexpr int areaHeight = 30;
		static inline constexpr float fontSize = 16.0f;
	}

	namespace VstLogo
	{

		static inline constexpr int areaHeight = 96;
		static inline constexpr int width = 150;
		static inline constexpr int height = 60;
	}

	namespace VstNotice
	{
		static inline constexpr int areaHeight = 20;
		static inline constexpr float fontSize = 14.0f;
	}

	namespace GplNotice
	{
		static inline constexpr int areaHeight = 40;
		static inline constexpr float fontSize = 14.0f;
	}

	namespace GplLink
	{
		static inline constexpr int areaHeight = 40;
		static inline constexpr float fontSize = 24.0f;
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
}
