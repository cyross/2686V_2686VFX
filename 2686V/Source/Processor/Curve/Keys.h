// オートメーションで使用するパラメータキー(パラメータ名)を構成する文字列を管理

#pragma once

#include <JuceHeader.h>

#include <array>

#include "../../Core/Processor/Keys.h"

namespace CurvePrKey
{
	// 共通プレフィックス
	static inline const juce::String prefix = "ADV_CURVE";

	// Enable
	static inline const juce::String enable = "_ENABLE";

	// Position
	static const std::array<juce::String, 5> position{ "_C", "_OP1", "_OP2", "_OP3", "_OP4" };

	// Target
	static const std::array<juce::String, 4> target{ "_RVAL", "_AENV", "_PENV", "_SSE" };

	// パラメータリスト
	static const std::array<juce::String, 8> paramList{ "_P1", "_P2", "_P3", "_P4", "_P5", "_P6", "_P7", "_P8" };

	// カーブロジック
	static inline const juce::String logic = "_LOGIC";

	// 倍率
	static inline const juce::String k = "_K";

	// バリューリスト
	static const std::array<juce::String, 8> valueList{ "_V1", "_V2", "_V3", "_V4", "_V5", "_V6", "_V7", "_V8" };
};

