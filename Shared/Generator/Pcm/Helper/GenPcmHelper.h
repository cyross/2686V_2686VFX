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
	//
	// cleanResample を立てると、間引きを resampleClean で行う
	// (QUALITY のノイズリダクション設定)。既定は立てない。
	void encodeBuffer(
		const std::vector<float>& source,
		double step,
		int qIndex,
		bool cleanResample,
		std::vector<int16_t>& dest
	);

	// 折り返しを防ぎながら、step 間隔の位置の値を補間して拾う。
	//
	// ふだんの間引きは、割り切れない位置を切り捨てて手前の 1 点を拾う。
	// 拾う位置が揺れてノイズになり (44.1kHz → 16kHz で約 28dB)、目的の
	// レートのナイキストを超える成分もそのまま折り返す。符号化そのもの
	// より、こちらのほうがノイズが大きい。
	//
	// 窓付き sinc で帯域を目的のナイキストの手前まで絞りつつ、ちょうど
	// その位置の値を求める。
	void resampleClean(const std::vector<float>& source, double step, std::vector<int16_t>& dest);
}
