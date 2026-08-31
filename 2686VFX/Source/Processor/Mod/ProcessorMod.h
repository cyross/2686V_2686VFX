#pragma once

#include <JuceHeader.h>

#include "../../Core/Processor/ProcessorHelper.h"
#include "../../Core/Processor/ProcessorStructs.h"
#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"

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
class ModProcessor
{
	PrPtrsAdsrAmpEnv ptAmpEnv;

	std::atomic<float>* pAmpEnvEnable = nullptr;

	AmpAdsrEnv ampEnv;

	// 今の音量。滑らかに動かすため、毎サンプル更新する。
	float level = 1.0f;

	bool ampEnvEnabled = false;
public:
	void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);
	void init(juce::AudioProcessorValueTreeState& apvts);
	void prepare(double sampleRate);

	// 鍵盤の押し離し。処理の前に、そのブロックぶんをまとめて受け取る。
	void noteOn();
	void noteOff();

	// 出力へ掛ける。何も有効になっていなければ触らない。
	void processBlock(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& apvts);

	// 画面の表示に使う。鳴っている間だけ真になる。
	bool isActive() const;
};
