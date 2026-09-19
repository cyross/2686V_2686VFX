// ============================================================================
// このプラグインだけの定数
// ============================================================================
// 名前と版は、JUCE がプラグインごとに書き出す ProjectInfo から取る。12 本で
// 共有する Shared/Core/Const/ConstGlobal.h には置けないので、ここに持つ。
// Global::Plugin のほかの定数 (author など) は ConstGlobal.h にある。
#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Const/ConstGlobal.h"

namespace Global
{
	namespace Plugin
	{
		static inline const juce::String name = ProjectInfo::projectName;
		static inline const juce::String version = ProjectInfo::versionString;
	};
};
