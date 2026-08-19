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
	static const std::array<juce::String, 9> position{ " Common", " Op1", " Op2", " Op3", " Op4", " Op5", " Op6", " Op7", " Op8" };

	// Target
	static const std::array<juce::String, 6> target{ " Register Value", " Amp Env", " Pitch Env", " SSG Sw Env", " SSG Sw Env11", " SSG Sw PEnv11" };

	// パラメータリスト
	static const std::array<juce::String, 16> paramList{ " Param[1]", " Param[2]", " Param[3]", " Param[4]", " Param[5]", " Param[6]", " Param[7]", " Param[8]", " Param[9]", " Param[10]", " Param[11]", " Param[12]", " Param[13]", " Param[14]", " Param[15]", " Param[16]" };

	// カーブロジック
	static inline const juce::String logic = " Logic";

	// 倍率
	static inline const juce::String k = " K";

	// バリューリスト
	static const std::array<juce::String, 16> valueList{ " Value[1]", " Value[2]", " Value[3]", " Value[4]", " Value[5]", " Value[6]", " Value[7]", " Value[8]", " Value[9]", " Value[10]", " Value[11]", " Value[12]", " Value[13]", " Value[14]", " Value[15]", " Value[16]" };
}
