#pragma once

#include <JuceHeader.h>

#include <cmath>
#include <vector>

// ============================================================================
// 音程を動かすための簡易ピッチシフター
// ============================================================================
// 音源では、音程の変調は発振器の位相の進み方を変えるだけで済んだ。作る音の
// 元がこちらにあるので、進みを速くすれば音が高くなる。
//
// エフェクトには発振器が無い。入ってきた音の音程を変えるには、いったん溜めて
// から読み出す速さを変えるしかない。読み出しは書き込みより速かったり遅かったり
// するので、そのままでは追い越すか離される。そこで読み口を 2 つ持ち、
// 片方が端に来る前にもう片方へ渡す。渡すときは音が途切れないよう重ねて混ぜる。
//
// 実機の再現ではなく、エフェクトとして音程を動かすための独自の作り。
class ModPitchShifter
{
	// 読み口を渡し終えるまでの長さ。長いと音の揺れが目立ち、短いと
	// 渡す回数が増えてざらつく。この辺りが聞きやすい。
	static inline constexpr float windowSec = 0.050f;

	// 書き口を追い越さないための余裕。
	static inline constexpr int headroom = 4;

	std::vector<float> buffer;

	int size = 0;
	int writePos = 0;
	int window = 1;

	// 2 つの読み口のうち、片方の位置。もう片方は必ず半周ぶん離れている。
	float phase = 0.0f;

	float readAt(float delay) const
	{
		float pos = (float)writePos - delay;

		while (pos < 0.0f) pos += (float)size;

		int i0 = (int)pos;
		int i1 = (i0 + 1 >= size) ? 0 : i0 + 1;
		float frac = pos - (float)i0;

		return buffer[i0] + (buffer[i1] - buffer[i0]) * frac;
	}
public:
	void prepare(double sampleRate)
	{
		window = juce::jmax(1, (int)(windowSec * sampleRate));
		size = window + headroom + 2;

		buffer.assign((size_t)size, 0.0f);

		writePos = 0;
		phase = 0.0f;
	}

	void reset()
	{
		std::fill(buffer.begin(), buffer.end(), 0.0f);

		writePos = 0;
		phase = 0.0f;
	}

	// ratio は音程の倍率。1.0 でそのまま、2.0 で 1 オクターブ上。
	float process(float input, float ratio)
	{
		buffer[(size_t)writePos] = input;

		// 読み口の遅れは、書き口との速さの差ぶんだけ増えていく。
		// 高くするときは差が負になり、遅れは減っていく。
		float delayA = headroom + phase * (float)window;
		float delayB = headroom + std::fmod(phase + 0.5f, 1.0f) * (float)window;

		// 混ぜ方は sin と cos の二乗。足すと必ず 1 になるので、
		// 渡している最中も音量が変わらない。
		float s = std::sin(juce::MathConstants<float>::pi * phase);
		float gainA = s * s;
		float gainB = 1.0f - gainA;

		float out = readAt(delayA) * gainA + readAt(delayB) * gainB;

		phase += (1.0f - ratio) / (float)window;

		while (phase >= 1.0f) phase -= 1.0f;
		while (phase < 0.0f) phase += 1.0f;

		if (++writePos >= size) writePos = 0;

		return out;
	}
};
