#pragma once

#include "../../Core/Processor/ProcessorValues.h"

namespace CurvePrValue
{
	inline constexpr float boolThread = CPV::boolThread;

	inline constexpr int positions = 9;
	inline constexpr int targets = 6;
	inline constexpr int params = 16;
	inline constexpr int logics = 31;
	inline constexpr int values = 16;

	inline constexpr int regValParams = 6;
	inline constexpr int regAmpEnvParams = 4;
	inline constexpr int regPitchEnvParams = 3;
	inline constexpr int regSsgSwEnvParams = 7;
	inline constexpr int regSsgSwEnv11Params = 12;
	inline constexpr int regSsgSwPEnv11Params = 12;

	inline constexpr float posMin = 0.0f;
	inline constexpr float posMax = 1.0f;
	inline constexpr float valueMin = -3.0f;
	inline constexpr float valueMax = 3.0f;

	namespace Enable
	{
		inline constexpr bool initial = false;
	}

	namespace Logic
	{
		inline constexpr int min = 0;
		inline constexpr int max = CurvePrValue::logics - 1;
		inline constexpr int initial = 0;
	}

	namespace K
	{
		inline constexpr float min = -50.0f;
		inline constexpr float max = 50.0f;
		inline constexpr float initial = 1.0f;
	}

	namespace Value
	{
		inline constexpr float min = CurvePrValue::valueMin;
		inline constexpr float max = CurvePrValue::valueMax;
		inline constexpr float initial = 0.0f;
	}
}
