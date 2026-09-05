#pragma once

#include <cstdint>

#include "./EnvSsgHwParams.h"

// SSG HW PITCH ENV。
//
// 形の作り方は SSG HW AMP ENV (Effect/Envelope/Amp/SsgHw) と同じで、
// 出力の当て先だけが違う。あちらは音量の倍率を返すが、こちらはセント値を
// 求めて位相増分へ掛ける比に直す。
class SsgHwPEnv {
public:
	SsgHwPEnv();

	// スムース処理の時定数(秒)。
	// 音量版と同じ値。ピッチの場合は段差がそのまま音程の飛びになるので、
	// 効き方は音量版より分かりやすい。
	static inline constexpr float smoothTimeSec = 0.002f;

	// HW Pitch Env Params
	bool m_useHwEnv = false;
	int m_envShape = 0;
	float m_envFreq = 1.0f;
	double m_hwEnvPhase = 0.0;
	float m_min = 0.0f;
	float m_max = 1200.0f;

	// スムース処理
	bool m_smooth = false;
	float m_smoothedCent = 0.0f;
	float m_smoothCoeff = 1.0f;

	double sampleRate = 44100.0; // DAW Host Sample Rate

	// いま出しているセント値。グラフやデバッグから覗けるように持っておく。
	float currentCent = 0.0f;

	void prepare(double sampleRate);
	void updateSampleRate(double newSampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(const SsgHwPEnvParams& params);
	void noteOn();
	void noteOff();

	// 1 サンプルぶん進めて、位相増分へ掛ける比を返す。
	float process(float phaseDelta);

	// 位相増分を触らずに状態だけ進める。バイパス中でも形が流れ続けるように。
	float processCent();

private:
	// サンプリングレートが変わったら 1 次ローパスの係数を計算し直す
	void updateSmoothCoeff();

	// Sample & Hold / Random 用の乱数。
	// ノイズ用の LFSR は 1 サンプルにつき 1 ビットしか進まず、
	// 周期単位で拾うと値がかたよるので独立した xorshift を持たせる。
	float nextRandom();

	uint32_t m_rngState = 0x2686FDu;

	// Sample & Hold 用のサイクル管理
	int m_prevCycle = 0;
	uint32_t m_cycleCount = 0;
	float m_holdLevel = 1.0f;
};
