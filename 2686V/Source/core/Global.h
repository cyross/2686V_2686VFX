// カテゴライズできない定数を管理

#pragma once

#include <JuceHeader.h>

namespace Global
{
	static inline const int voices = 32; // 最大同時発音数

	namespace Plugin
	{
		static inline const juce::String name = ProjectInfo::projectName;
		static inline const juce::String version = ProjectInfo::versionString;
		static inline const juce::String author = "Copyright (C) 2026 CYROSS";
		static inline const juce::String parameters = "Parameters";
		static inline const juce::String allowedControlChars = "\r\n\t";
	};

	namespace Audio
	{
		static inline const juce::String input = "Input";
		static inline const juce::String output = "Output";
	};

	namespace Fm
	{
		static const int Op4 = 4;
		static const int Op2 = 2;
	}

	namespace About
	{
		namespace Gpl
		{
			static inline const juce::String notice = "This software is released under the GNU General Public License v3.0 (GPLv3).";
			static inline const juce::String navigate = "View Full License (GNU.org)";
			static inline const juce::String linkUrl = "https://www.gnu.org/licenses/gpl-3.0.en.html";
		};

		namespace Vst
		{
			static inline const juce::String notice = "VST is a registered trademark of Steinberg Media Technologies GmbH.";
		}
	};
};
