#pragma once

#include "../../Core/Processor/Values.h"

namespace CurvePrValue
{
	inline constexpr float boolThread = CorePrValue::boolThread;

	inline constexpr int positions = 5;
	inline constexpr int targets = 4;
	inline constexpr int params = 8;
	inline constexpr int logics = 25;
	inline constexpr int values = 8;

	inline constexpr int regValParams = 6;
	inline constexpr int regAmpEnvParams = 4;
	inline constexpr int regPitchEnvParams = 3;
	inline constexpr int regSsgSwEnvParams = 7;

	namespace Enable
	{
		inline constexpr bool initial = false;
	}

	namespace Logic
	{
		inline constexpr int min = 0;
		inline constexpr int max = CurvePrValue::logics;
		inline constexpr int initial = 0;
	}

	namespace K
	{
		inline constexpr float min = 0.0f;
		inline constexpr float max = 20.0f;
		inline constexpr float initial = 1.0f;
	}

	namespace Value
	{
		inline constexpr float min = 0.0f;
		inline constexpr float max = 1.0f;
		inline constexpr float initial = 0.0f;
	}
}
