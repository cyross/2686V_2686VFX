#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/ProcessorHelper.h"
#include "../../Core/Processor/ProcessorStructs.h"

#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"
#include "../../Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7.h"

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
// 音量側だけを扱う。音程側は入力音のピッチを変えることになり、作りが
// まったく違うので別に用意する。
class ModProcessor
{
	PrPtrsAdsrAmpEnv ptAmpEnv;
	PrPtrsSsgHwEnv ptSsgHwEnv;
	PrPtrsSsgSwEnv ptSsgSwEnv;
	PrPtrsSsgSwEnv11 ptSsgSwEnv11;
	PrPtrsOpzx7Lfo ptLfo;

	std::atomic<float>* pEnvEnable = nullptr;
	std::atomic<float>* pLfoEnable = nullptr;

	AmpAdsrEnv ampEnv;
	SsgHwEnv ssgHwEnv;
	SsgSwEnv ssgSwEnv;
	SsgSwEnv11 ssgSwEnv11;
	Opzx7LfoCore lfo;

	// 今の音量。滑らかに動かすため、毎サンプル更新する。
	float ampLevel = 1.0f;

	bool envEnabled = false;
	bool lfoEnabled = false;

	double rate = 44100.0;
public:
	void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
	void init(juce::AudioProcessorValueTreeState& apvts);
	void prepare(double sampleRate);

	// 鍵盤の押し離し。音を鳴らすためではないので、どの音程かは見ない。
	void noteOn();
	void noteOff();

	// 出力へ掛ける。何も有効になっていなければ触らない。
	void processBlock(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& apvts);

	// 画面の表示に使う。鳴っている間だけ真になる。
	bool isActive() const;
};
