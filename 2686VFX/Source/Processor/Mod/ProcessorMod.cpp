#include "./ProcessorMod.h"

#include "../../Core/Processor/ProcessorNames.h"

void ModProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
	const juce::String prefix = ModPrKey::prefix;
	const juce::String name = "MOD";

	PrHelper::addBool(
		layout,
		prefix + ModPrKey::AmpEnv::enable,
		name + " Amp Env Enable",
		ModPrValue::AmpEnv::enableInitial
	);

	PrHelper::addAdsrBypassParameter(layout, prefix, name, CPV::Adsr::Bypass::initial);
	PrHelper::addEnvParameters(layout, prefix, name);
}

void ModProcessor::init(juce::AudioProcessorValueTreeState& apvts)
{
	pAmpEnvEnable = apvts.getRawParameterValue(ModPrKey::prefix + ModPrKey::AmpEnv::enable);

	PrHelper::setupAdsrAmpEnvPtrs(apvts, ModPrKey::prefix, ptAmpEnv);
}

void ModProcessor::prepare(double sampleRate)
{
	ampEnv.prepare(sampleRate);

	level = 1.0f;
}

void ModProcessor::noteOn()
{
	if (!ampEnvEnabled) return;

	level = ampEnv.noteOn();
}

void ModProcessor::noteOff()
{
	if (!ampEnvEnabled) return;

	ampEnv.noteOff();
}

bool ModProcessor::isActive() const
{
	return ampEnvEnabled && ampEnv.isPlaying();
}

void ModProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& apvts)
{
	juce::ignoreUnused(apvts);

	ampEnvEnabled = PrHelper::getBool(pAmpEnvEnable);

	if (!ampEnvEnabled) return;

	AmpAdsrParams params;

	PrHelper::applyAdsrAmpEnv(ptAmpEnv, params);

	ampEnv.setParameters(params);

	// 素通しにするのはバイパスのときだけ。鍵盤を押していないときは
	// エンベロープが 0 を返すので、無音になるのが正しい。
	if (ampEnv.isBypass()) return;

	int channels = buffer.getNumChannels();
	int samples = buffer.getNumSamples();

	for (int i = 0; i < samples; ++i)
	{
		level = ampEnv.process(level);

		for (int ch = 0; ch < channels; ++ch)
		{
			buffer.getWritePointer(ch)[i] *= level;
		}
	}
}
