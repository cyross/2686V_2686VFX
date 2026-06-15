#pragma once

#include<JuceHeader.h>

#include "../../Core/Processor/ProcessorNames.h"

// UI上に表示するタイトル等を管理
namespace CurvePrName
{
	// 共通プレフィックス
	static inline const juce::String prefix = "Advance Mode (Curve)";

	// Enable
	static inline const juce::String enable = " Enable";

	// Position
	static const std::array<juce::String, 7> position{ " Common", " Op1", " Op2", " Op3", " Op4", " Op5", " Op6" };

	// Target
	static const std::array<juce::String, 4> target{ " Register Value", " Amp Env", " Pitch Env", " SSG Sw Env" };

	// パラメータリスト
	static const std::array<juce::String, 8> paramList{ " Param[1]", " Param[2]", " Param[3]", " Param[4]", " Param[5]", " Param[6]", " Param[7]", " Param[8]" };

	// カーブロジック
	static inline const juce::String logic = " Logic";

	// 倍率
	static inline const juce::String k = " K";

	// バリューリスト
	static const std::array<juce::String, 8> valueList{ " Value[1]", " Value[2]", " Value[3]", " Value[4]", " Value[5]", " Value[6]", " Value[7]", " Value[8]" };
}
