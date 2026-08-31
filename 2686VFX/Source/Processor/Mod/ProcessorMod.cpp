#include "./ProcessorMod.h"

#include "../../Core/Processor/ProcessorNames.h"

namespace
{
	// 画面と DAW に出る名前の頭。チャンネルが無いので短くてよい。
	static inline const juce::String displayName = "MOD";
}

void ModProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
	const juce::String prefix = ModPrKey::prefix;

	// --- AMP ENV ---
	PrHelper::addBool(
		layout,
		prefix + ModPrKey::Env::enable,
		displayName + " Env Enable",
		ModPrValue::Env::enableInitial
	);

	PrHelper::addAdsrBypassParameter(layout, prefix, displayName, CPV::Adsr::Bypass::initial);
	PrHelper::addEnvParameters(layout, prefix, displayName);

	// --- SSG 系エンベロープ ---
	PrHelper::addSsgHwEnvParameters(layout, prefix, displayName);
	PrHelper::addSsgSwEnvParameters(layout, prefix, displayName);
	PrHelper::addSsgSwEnv11Parameters(layout, prefix, displayName);

	// --- LFO ---
	PrHelper::addBool(
		layout,
		prefix + ModPrKey::Lfo::enable,
		displayName + " LFO Enable",
		ModPrValue::Lfo::enableInitial
	);

	PrHelper::addOpzx7LfoParameters(layout, prefix, displayName);
}

void ModProcessor::init(juce::AudioProcessorValueTreeState& apvts)
{
	pEnvEnable = apvts.getRawParameterValue(ModPrKey::prefix + ModPrKey::Env::enable);
	pLfoEnable = apvts.getRawParameterValue(ModPrKey::prefix + ModPrKey::Lfo::enable);

	PrHelper::setupAdsrAmpEnvPtrs(apvts, ModPrKey::prefix, ptAmpEnv);
	PrHelper::setupSsgHwEnv(apvts, ModPrKey::prefix, ptSsgHwEnv);
	PrHelper::setupSsgSwEnvPtrs(apvts, ModPrKey::prefix, ptSsgSwEnv);
	PrHelper::setupSsgSwEnv11Ptrs(apvts, ModPrKey::prefix, ptSsgSwEnv11);
	PrHelper::setupOpzx7LfoPtrs(apvts, ModPrKey::prefix, ptLfo);
}

void ModProcessor::prepare(double sampleRate)
{
	rate = sampleRate;

	ampEnv.prepare(sampleRate);
	ssgHwEnv.prepare(sampleRate);

	// SSG のソフトウェアエンベロープは、どの対象へ掛けるかを番号で受ける。
	// ここでは音量だけなので 0 を渡す。
	ssgSwEnv.prepare(0, sampleRate);
	ssgSwEnv11.prepare(0, sampleRate);

	lfo.prepare(sampleRate);

	ampLevel = 1.0f;
}

void ModProcessor::noteOn()
{
	if (envEnabled)
	{
		ampLevel = ampEnv.noteOn();

		ssgHwEnv.noteOn();
		ssgSwEnv.noteOn();
		ssgSwEnv11.noteOn();
	}

	if (lfoEnabled) lfo.noteOn();
}

void ModProcessor::noteOff()
{
	if (!envEnabled) return;

	ampEnv.noteOff();
	ssgHwEnv.noteOff();
	ssgSwEnv.noteOff();
	ssgSwEnv11.noteOff();
}

bool ModProcessor::isActive() const
{
	return envEnabled && ampEnv.isPlaying();
}

void ModProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& apvts)
{
	juce::ignoreUnused(apvts);

	envEnabled = PrHelper::getBool(pEnvEnable);
	lfoEnabled = PrHelper::getBool(pLfoEnable);

	// どちらも切なら何もしない。素通しにする。
	if (!envEnabled && !lfoEnabled) return;

	if (envEnabled)
	{
		AmpAdsrParams ampParams;
		SsgHwEnvParams hwParams;
		SsgSwEnvParams swParams;
		SsgSwEnv11Params sw11Params;

		PrHelper::applyAdsrAmpEnv(ptAmpEnv, ampParams);
		PrHelper::applySsgHwEnv(ptSsgHwEnv, hwParams);
		PrHelper::applySsgSwEnv(ptSsgSwEnv, swParams);
		PrHelper::applySsgSwEnv11(ptSsgSwEnv11, sw11Params);

		ampEnv.setParameters(ampParams);
		ssgHwEnv.setParameters(hwParams);
		ssgSwEnv.setParameters(swParams);
		ssgSwEnv11.setParameters(sw11Params);
	}

	if (lfoEnabled)
	{
		LfoOpzx7Params lfoParams;

		PrHelper::applyOpzx7Lfo(ptLfo, lfoParams);

		lfo.setParameters(lfoParams);
	}

	int channels = buffer.getNumChannels();
	int samples = buffer.getNumSamples();

	for (int i = 0; i < samples; ++i)
	{
		float gain = 1.0f;

		if (envEnabled)
		{
			// バイパスのものは 1.0 を返すので、掛けても変わらない。
			if (!ampEnv.isBypass()) { ampLevel = ampEnv.process(ampLevel); gain *= ampLevel; }

			gain *= ssgHwEnv.process();

			if (!ssgSwEnv.isBypass()) gain *= ssgSwEnv.process();
			if (!ssgSwEnv11.isBypass()) gain *= ssgSwEnv11.process();
		}

		if (lfoEnabled)
		{
			lfo.getSample();

			// AM は音量側。PM は音程側なので、ここでは使わない。
			gain *= juce::jlimit(0.0f, 2.0f, 1.0f - lfo.value.am);
		}

		for (int ch = 0; ch < channels; ++ch)
		{
			buffer.getWritePointer(ch)[i] *= gain;
		}
	}
}
