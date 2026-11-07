#pragma once

#include <algorithm>
#include <array>
#include <atomic>
#include <cmath>

// ============================================================================
// レベルメーターへ渡す音 (4.4.0)
// ============================================================================
// 音の側は、出し終えた音 (FX の後) をここへ書くだけにする。スペクトラムの
// FFT は画面の側でやり、音の側には重い計算を置かない。
//
//   L / R の大きさ : 前に読まれてからいちばん大きかった値 (ピーク)。読む側が
//                    取り出すと 0 に戻る。
//   スペクトラム   : 左右を混ぜたものを輪のバッファへ。読む側が最新の
//                    fftSize 点を写し取る。
//
// 画面を出していないあいだは書かない (active)。書く側は 1 本 (オーディオ
// スレッド)、読む側も 1 本 (メッセージスレッド) を前提にしている。読む途中で
// 書き進められて頭が少し新しくなることはあるが、メーターの見た目には効かない。
class LevelMeterFeed
{
public:
	// 2 の冪にしておく。FFT の点数 (画面の側) より大きければよい
	static constexpr int ringSize = 8192;

	// 画面がメーターを出しているか。切っているあいだは書かない
	std::atomic<bool> active{ false };

	// 書いた音のサンプリングレート (スペクトラムの周波数を決めるのに使う)
	std::atomic<double> sampleRate{ 44100.0 };

	// 出し終えた音を書く。right が無ければ (モノラル) left を両方に使う
	void push(const float* left, const float* right, int numSamples, double rate)
	{
		if (!active.load(std::memory_order_relaxed) || left == nullptr || numSamples <= 0) return;

		if (right == nullptr) right = left;

		sampleRate.store(rate, std::memory_order_relaxed);

		float maxL = 0.0f;
		float maxR = 0.0f;

		int pos = writePos.load(std::memory_order_relaxed);

		for (int i = 0; i < numSamples; ++i) {
			const float l = left[i];
			const float r = right[i];

			maxL = std::max(maxL, std::abs(l));
			maxR = std::max(maxR, std::abs(r));

			ring[(size_t)pos] = (l + r) * 0.5f;
			pos = (pos + 1) & (ringSize - 1);
		}

		writePos.store(pos, std::memory_order_release);

		raise(peakL, maxL);
		raise(peakR, maxR);
	}

	// 前に読んでからいちばん大きかった値を取り出す (0 に戻す)
	float takePeakL() { return peakL.exchange(0.0f, std::memory_order_relaxed); }
	float takePeakR() { return peakR.exchange(0.0f, std::memory_order_relaxed); }

	// 最新の count 点を古い順に dest へ写す。count は ringSize 以下
	void copyLatest(float* dest, int count) const
	{
		count = std::clamp(count, 0, ringSize);

		int pos = writePos.load(std::memory_order_acquire) - count;

		if (pos < 0) pos += ringSize;

		for (int i = 0; i < count; ++i) {
			dest[i] = ring[(size_t)pos];
			pos = (pos + 1) & (ringSize - 1);
		}
	}

	// 出すのをやめたときに、古い音が残らないよう空にする (メッセージスレッドから)
	void clear()
	{
		ring.fill(0.0f);
		peakL.store(0.0f, std::memory_order_relaxed);
		peakR.store(0.0f, std::memory_order_relaxed);
	}

private:
	std::array<float, ringSize> ring{};
	std::atomic<int> writePos{ 0 };

	std::atomic<float> peakL{ 0.0f };
	std::atomic<float> peakR{ 0.0f };

	// 読む側が 0 に戻すことがあるので、比べてから書き換える
	static void raise(std::atomic<float>& peak, float value)
	{
		float current = peak.load(std::memory_order_relaxed);

		while (value > current && !peak.compare_exchange_weak(current, value, std::memory_order_relaxed)) {}
	}
};
