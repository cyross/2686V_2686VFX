#pragma once

#include <cstdint>
#include <random>
#include <vector>

// --- 圧縮系のモード番号 (Quality の BIT リストの並びと対応) ---
// 13: YM2608 ADPCM / 14: 1bit DPCM / 15: SNES BRR / 16: PS1 VAG
// 17: IMA ADPCM   / 18: CD-ROM XA / 19: YMZ280B  / 20: K053260 / 21: K054539
namespace PcmCodecMode
{
	inline constexpr int ym2608Adpcm = 13;
	inline constexpr int dpcm        = 14;
	inline constexpr int snesBrr     = 15;
	inline constexpr int psxVag      = 16;
	inline constexpr int imaAdpcm    = 17;
	inline constexpr int cdromXa     = 18;
	inline constexpr int ymz280b     = 19;
	inline constexpr int k053260     = 20;
	inline constexpr int k054539     = 21;

	inline constexpr int first = ym2608Adpcm;
	inline constexpr int last  = k054539;
}

namespace GenPcmHelper
{
	static std::random_device pcmRd;
	static std::mt19937 pcmGen(pcmRd());
	static std::uniform_real_distribution<float> pcmDis(0.0f, 1.0f);

	void lowPassFilter(std::vector<int16_t>& buffer);
	float bitReduction(float input, int qIndex);

	// エンコード済みバッファを使うモードかどうか
	bool isEncodedMode(int qIndex);

	// float のソースを step 間隔で間引きつつ、qIndex のコーデックで
	// エンコード → デコードした int16 列を dest に作る。
	// 圧縮系モードの追加はこの 1 箇所だけで完結する。
	void encodeBuffer(
		const std::vector<float>& source,
		double step,
		int qIndex,
		std::vector<int16_t>& dest
	);
}
