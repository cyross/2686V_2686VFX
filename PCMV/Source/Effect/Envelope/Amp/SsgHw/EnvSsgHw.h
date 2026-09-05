#pragma once

#include <cstdint>

#include "./EnvSsgHwParams.h"

class SsgHwEnv {
public:
	SsgHwEnv();

	// スムース処理の時定数(秒)。
	// 波形の段差を鈍らせてブツブツ音を抑えるのが目的なので、
	// 音色そのものが変わらない程度に短くしてある。
	static inline constexpr float smoothTimeSec = 0.002f;

	// HW Env Params
	bool m_useHwEnv = false;
	int m_envShape = 0;
	float m_envFreq = 1.0f;
	double m_hwEnvPhase = 0.0;
	float m_min = 0.0f;
	float m_max = 1.0f;

	// スムース処理
	bool m_smooth = false;
	float m_smoothedGain = 1.0f;
	float m_smoothCoeff = 1.0f;

	float m_currentRate = 0.0f;
	double sampleRate = 44100.0; // DAW Host Sample Rate
	float currentLevel = 0.0f;

	void prepare(double sampleRate);
	void updateSampleRate(double newSampleRate);
	void updateTargetSampleRate(double newSampleRate);
	void setParameters(const SsgHwEnvParams& params);
	void noteOn();
	void noteOff();
	float process();

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
