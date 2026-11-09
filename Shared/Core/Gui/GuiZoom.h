#pragma once

#include <array>
#include <cmath>

#include <JuceHeader.h>

// 表示倍率の選択肢 (4.4.1)。生成過程プレビュー・ファイルブラウザーの縦の倍率と、
// パラメータグラフの横の倍率で共通に使う。
//
// x1 より小さい側は、よく使う分数 (1/8・1/6・1/3・2/3 など) を小数 3 桁で並べる。
// x1 より大きい側は、x8 までは 1 刻み、その先は x12・x16・x24・x32。
namespace GuiZoom
{
	inline constexpr std::array<float, 30> choices{
		0.1f, 0.125f, 0.167f, 0.2f, 0.25f, 0.3f, 0.333f, 0.4f, 0.5f, 0.6f,
		0.625f, 0.667f, 0.7f, 0.75f, 0.8f, 0.833f, 0.875f, 0.9f,
		1.0f, 2.0f, 3.0f, 4.0f, 5.0f, 6.0f, 7.0f, 8.0f, 12.0f, 16.0f, 24.0f, 32.0f,
	};

	inline constexpr float minimum = 0.1f;
	inline constexpr float maximum = 32.0f;
	inline constexpr float initial = 1.0f;

	// いちばん近い選択肢の番号。近さは比で測る (x0.1 と x0.125 の差を、x24 と x32 の差と同じ重みで見る)
	inline int nearestIndex(float value)
	{
		if (!std::isfinite(value) || value <= 0.0f) value = initial;

		const float logValue = std::log(value);

		int best = 0;
		float bestDistance = std::abs(std::log(choices[0]) - logValue);

		for (int i = 1; i < (int)choices.size(); ++i)
		{
			const float distance = std::abs(std::log(choices[(size_t)i]) - logValue);

			if (distance < bestDistance)
			{
				best = i;
				bestDistance = distance;
			}
		}

		return best;
	}

	// いちばん近い選択肢へ寄せる
	inline float snap(float value) { return choices[(size_t)nearestIndex(value)]; }

	// 選択肢を 1 つ送る (direction は +1 / -1)。端では止まる
	inline float step(float current, int direction)
	{
		const int index = nearestIndex(current) + (direction > 0 ? 1 : direction < 0 ? -1 : 0);

		return choices[(size_t)juce::jlimit(0, (int)choices.size() - 1, index)];
	}

	// 欄に出す文字 (x0.125・x1・x32)。小数は 3 桁までで、末尾の 0 は落とす
	inline juce::String label(float value)
	{
		if (std::abs(value - std::round(value)) < 1.0e-4f) return "x" + juce::String((int)std::round(value));

		auto text = juce::String(value, 3);

		while (text.endsWithChar('0')) text = text.dropLastCharacters(1);

		return "x" + text;
	}
}
