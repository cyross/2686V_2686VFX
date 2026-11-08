#pragma once

#include <JuceHeader.h>

#include <functional>

#include "Shared/Core/Gui/GuiHost.h"
#include "Shared/Core/Synth/ChipDac.h"
#include "Shared/Core/Synth/SynthCore.h"
#include "Shared/Synth/Rhythm/SynthRhythm.h"

// ============================================================================
// 生成中の波形の窓 (4.4.0) の音源一式
// ============================================================================
// 各プラグインの createStageRig が、自分のシンセ・ボイス・FX の型で組み立てる。
// 中身は鳴らしている音源と同じ並び (シンセ → CHIP DAC → ヘッドルーム → FX) で、
// 違うのは、ボイスのコアに StageProbe を持たせて途中の音を写すことだけ。
template <typename Synth, typename Voice, typename Sound, typename Fx, typename Params>
class StageRig : public GuiStageRig
{
public:
	Synth synth;
	Params params;
	Fx fx;

	// CHIP DAC。通さないなら nullptr
	const ChipDac::Spec* dac = nullptr;

	// ヘッドルーム (SETTINGS)。使わないなら 1
	float headroom = 1.0f;

	juce::AudioProcessorValueTreeState* apvts = nullptr;

	// ボイスを作り、コアへ写す口を持たせる。prepareVoice は鳴らすボイスと同じ参照を与える
	// (各プラグインの prepareRenderVoice)。params は先に入れておくこと
	void prepareSynth(int voices, double sampleRate, bool noModulation, const std::function<void(Voice&)>& prepareVoice)
	{
		synth.clearVoices();
		synth.clearSounds();
		synth.addSound(new Sound());

		m_probe.resize(maxBlock);

		for (int i = 0; i < juce::jmax(1, voices); ++i)
		{
			auto* voice = new Voice();

			synth.addVoice(voice);

			prepareVoice(*voice);

			voice->setParameters(params);

			for (auto* core : voice->coreMap) {
				if (core != nullptr) core->setStageProbe(&m_probe, noModulation);
			}
		}

		synth.setCurrentPlaybackSampleRate(sampleRate);

		// 本体と同じ鳴らし方にするため、シンセ側の設定も写す
		synth.currentParams = &params;
		synth.isMonoMode = params.monoMode;
		synth.useVelocity = params.useVelocity;
		synth.pitchResetOnLegato = params.pitchResetOnLegato;
		synth.fixedVelocity = params.fixedVelocity;
	}

	// 一度に回せる長さ
	static constexpr int maxBlock = 512;

	void render(const juce::MidiBuffer& midi, int numSamples,
		juce::AudioBuffer<float>& synthOut, juce::AudioBuffer<float>& dacOut, juce::AudioBuffer<float>& fxOut) override
	{
		numSamples = juce::jlimit(0, maxBlock, numSamples);

		m_probe.clear();

		for (auto* b : { &synthOut, &dacOut, &fxOut }) b->clear(0, numSamples);

		synth.renderNextBlock(synthOut, midi, 0, numSamples);

		const int channels = juce::jmin(synthOut.getNumChannels(), dacOut.getNumChannels(), fxOut.getNumChannels());

		for (int ch = 0; ch < channels; ++ch) {
			dacOut.copyFrom(ch, 0, synthOut, ch, 0, numSamples);

			if (dac != nullptr) ChipDac::process(dacOut.getWritePointer(ch), numSamples, *dac);

			fxOut.copyFrom(ch, 0, dacOut, ch, 0, numSamples);
		}

		// FX はバッファの長さぶん回すので、回す長さだけを見せる
		juce::AudioBuffer<float> view(fxOut.getArrayOfWritePointers(), channels, numSamples);

		if (headroom != 1.0f) view.applyGain(headroom);

		if (apvts != nullptr) fx.processBlock(view, params, *apvts);
	}

	void stopNote(int midiNote) override
	{
		for (int i = 0; i < synth.getNumVoices(); ++i)
		{
			auto* voice = synth.getVoice(i);

			if (voice != nullptr && voice->isVoiceActive() && voice->getCurrentlyPlayingNote() == midiNote) {
				voice->stopNote(0.0f, false);
			}
		}
	}

	StageProbe& probe() override { return m_probe; }

	void setHoldWhileKeyDown(bool hold) override
	{
		for (int i = 0; i < synth.getNumVoices(); ++i)
		{
			if (auto* voice = dynamic_cast<Voice*>(synth.getVoice(i))) {
				for (auto* core : voice->coreMap) {
					if (core != nullptr) core->m_probeHold = hold;
				}
			}
		}
	}

	GuiStageInfo info() const override
	{
		GuiStageInfo out;

		out.chipDac = (dac != nullptr);

		if (synth.getNumVoices() == 0) return out;

		const auto* voice = dynamic_cast<const Voice*>(synth.getVoice(0));

		if (voice == nullptr) return out;

		const SynthCore* core = voice->activeCore();

		if (core == nullptr) return out;

		out.kind = core->stageKind();
		out.opCount = juce::jlimit(0, StageProbe::maxOps, core->stageOpCount());
		out.modules = core->stageModules();

		if (const auto* rhythm = dynamic_cast<const RhythmCore*>(core)) {
			for (int i = 0; i < out.opCount; ++i) out.padNotes[(size_t)i] = rhythm->pads[(size_t)i].m_noteNumber;
		}

		return out;
	}

private:
	StageProbe m_probe;
};
