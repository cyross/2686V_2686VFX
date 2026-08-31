#pragma once

#include <JuceHeader.h>

#include <array>

#include "../../Core/Processor/ProcessorHelper.h"
#include "../../Core/Processor/ProcessorStructs.h"

#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7.h"
#include "../../Effect/Detune/Opzx7/DetuneOpzx7.h"
#include "../../Generator/WtMod/GenWtModulator.h"

#include "../../Core/Synth/UnisonParams.h"
#include "../../Core/Synth/UnisonState.h"

#include "./ModPitchShifter.h"
#include "./ProcessorModKeys.h"
#include "./ProcessorModValues.h"

// ============================================================================
// 出力へ掛ける変調
// ============================================================================
// 音源のプラグインでは、エンベロープや LFO はチャンネルごとに持っていて、
// オペレータの音量や音程を動かしていた。エフェクトにはチャンネルが無いので、
// 出力に対して 1 組だけ持つ。
//
// 動かすきっかけは MIDI。鍵盤を押すとエンベロープが始まり、離すと戻る。
// 鍵盤を触らなければ何も掛からず、そのまま素通しになる。
//
// 音量側はそのまま掛ければよいが、音程側は作りが違う。音源には発振器が
// あって位相の進み方を変えれば済んだのに対し、エフェクトは入ってきた音を
// 溜めてから読み出す速さを変えるしかない。そこは ModPitchShifter が担う。
class ModProcessor
{
	// --- 音量側 ---
	PrPtrsAdsrAmpEnv ptAmpEnv;
	PrPtrsSsgHwEnv ptSsgHwEnv;
	PrPtrsSsgSwEnv ptSsgSwEnv;
	PrPtrsSsgSwEnv11 ptSsgSwEnv11;
	PrPtrsOpzx7Lfo ptLfo;

	// --- 音程側 ---
	PrPtrsPitchEnv ptPitchEnv;
	PrPtrsSsgSwPEnv11 ptSsgSwPEnv11;
	PrPtrsWtMod ptWtMod;

	// --- 音程を一定量ずらすもの ---
	PrPtrsOpzx7Detune ptDetune;
	PrPtrsUnison ptUnison;

	std::atomic<float>* pEnvEnable = nullptr;
	std::atomic<float>* pLfoEnable = nullptr;
	std::atomic<float>* pPitchEnable = nullptr;
	std::atomic<float>* pWtModBaseFreq = nullptr;
	std::atomic<float>* pShiftEnable = nullptr;

	AmpAdsrEnv ampEnv;
	SsgHwEnv ssgHwEnv;
	SsgSwEnv ssgSwEnv;
	SsgSwEnv11 ssgSwEnv11;
	Opzx7LfoCore lfo;

	PitchAdsrEnv pitchEnv;
	SsgSwPEnv11 ssgSwPEnv11;
	WtModulator wtMod;

	Opzx7Detune detune;
	UnisonParams unisonParams;

	// ボイスごと・左右ごとに持つ。ユニゾンはボイスで音程が違い、
	// 音程が違えば溜めた音の読み口も別々になる。共用はできない。
	std::array<std::array<ModPitchShifter, 2>, Global::unisonVoices> shifters;

	// 疑似高速アルペジオ。今どのボイスを鳴らしているかと、
	// 切り替わり目でクリック音を出さないための渡り具合。
	int arpVoice = 0;
	double arpPhase = 0.0;
	std::array<float, Global::unisonVoices> arpGains{};

	// 今の音量。滑らかに動かすため、毎サンプル更新する。
	float ampLevel = 1.0f;

	bool envEnabled = false;
	bool lfoEnabled = false;
	bool pitchEnabled = false;
	bool shiftEnabled = false;

	// 前の塊で音程を動かしていたか。切り替わり目で溜めた音を捨てる。
	bool wasShifting = false;

	double rate = 44100.0;
public:
	void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);

	// WT PITCH MOD は波形の置き場を見に行くので、一緒に受け取る。
	void init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& store);

	void prepare(double sampleRate);

	// 鍵盤の押し離し。音を鳴らすためではないので、どの音程かは見ない。
	void noteOn();
	void noteOff();

	// 出力へ掛ける。何も有効になっていなければ触らない。
	void processBlock(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& apvts);

	// 画面の表示に使う。鳴っている間だけ真になる。
	bool isActive() const;
};
