#pragma once

#include <JuceHeader.h>

// ============================================================================
// このプラグインのプリセットを置くフォルダの名前
// ============================================================================
// プラグインごとに違うので、12 本で共有する ConstFileValues.h には置けない。
namespace Io
{
	namespace Folder
	{
		static inline const juce::String preset = "2686VLPresets";
	}
}
