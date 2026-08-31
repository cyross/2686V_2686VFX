#include "./ProcessorMod.h"

#include "../../Core/Processor/ProcessorNames.h"

namespace
{
	// 画面と DAW に出る名前の頭。チャンネルが無いので短くてよい。
	static inline const juce::String displayName = "MOD";

	// 音程側の変調へ渡す「搬送波の位相増分」。
	//
	// 音源では、音程の変調は発振器の位相の進み方に掛かっていた。エフェクトには
	// 発振器が無いので、代わりの基準を決めて同じ形で通す。PITCH ENV と
	// SSG SW PITCH ENV[11] は進みに倍率を掛けるだけなので、基準が何であっても
	// 出てくる倍率は変わらない。基準が効くのは、速さを搬送波との比で持つ
	// WT PITCH MOD だけで、そちらは基準の周波数を画面から決められる。
	static inline constexpr float nominalDelta = 0.01f;
}

void ModProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
	const juce::String prefix = ModPrKey::prefix;

	// ------------------------------------------------------------------
	// 音量側
	// ------------------------------------------------------------------
	PrHelper::addBool(
		layout,
		prefix + ModPrKey::Env::enable,
		displayName + " Env Enable",
		ModPrValue::Env::enableInitial
	);

	PrHelper::addAdsrBypassParameter(layout, prefix, displayName, CPV::Adsr::Bypass::initial);
	PrHelper::addEnvParameters(layout, prefix, displayName);

	PrHelper::addSsgHwEnvParameters(layout, prefix, displayName);

	// SSG のソフトウェアエンベロープは、入り切りの札だけが別の関数で
	// まとめて登録される作りになっている。ここは必要なぶんだけ登録する。
	PrHelper::addSsgSwEnvParameters(layout, prefix, displayName);
	PrHelper::addBool(
		layout,
		prefix + CPK::ssgSwEnv + CPK::bypass,
		displayName + CPN::SsgSwEnv::bypass,
		CPV::SsgSwEnv::Bypass::initial
	);

	PrHelper::addSsgSwEnv11Parameters(layout, prefix, displayName);
	PrHelper::addBool(
		layout,
		prefix + CPK::ssgSwEnv11 + CPK::bypass,
		displayName + CPN::SsgSwEnv11::bypass,
		CPV::SsgSwEnv11::Bypass::initial
	);

	// ------------------------------------------------------------------
	// LFO
	// ------------------------------------------------------------------
	PrHelper::addBool(
		layout,
		prefix + ModPrKey::Lfo::enable,
		displayName + " LFO Enable",
		ModPrValue::Lfo::enableInitial
	);

	PrHelper::addOpzx7LfoParameters(layout, prefix, displayName);

	// ------------------------------------------------------------------
	// 音程側
	// ------------------------------------------------------------------
	PrHelper::addBool(
		layout,
		prefix + ModPrKey::Pitch::enable,
		displayName + " Pitch Enable",
		ModPrValue::Pitch::enableInitial
	);

	PrHelper::addPitchEnvParameters(layout, prefix, displayName);
	PrHelper::addBool(
		layout,
		prefix + CPK::pitchAdsr + CPK::bypass,
		displayName + CPN::PitchAdsr::bypass,
		CPV::PitchAdsr::Bypass::initial
	);

	PrHelper::addSsgSwPEnv11Parameters(layout, prefix, displayName);
	PrHelper::addSsgSwPEnv11BypassParameter(layout, prefix, displayName, CPV::SsgSwPEnv11::Bypass::initial);

	PrHelper::addWtModParameters(layout, prefix, displayName);

	// WT PITCH MOD の速さは、実機では搬送波の周波数に対する比で決まる。
	// エフェクトには搬送波が無いので、その代わりになる周波数をここで決める。
	// 実機には無い、こちらで足したもの。
	PrHelper::addFloat(
		layout,
		prefix + ModPrKey::WtMod::baseFreq,
		displayName + " WT Mod Base Freq",
		ModPrValue::WtMod::baseFreqMin,
		ModPrValue::WtMod::baseFreqMax,
		ModPrValue::WtMod::baseFreqInitial
	);
}

void ModProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& store)
{
	pEnvEnable = apvts.getRawParameterValue(ModPrKey::prefix + ModPrKey::Env::enable);
	pLfoEnable = apvts.getRawParameterValue(ModPrKey::prefix + ModPrKey::Lfo::enable);
	pPitchEnable = apvts.getRawParameterValue(ModPrKey::prefix + ModPrKey::Pitch::enable);
	pWtModBaseFreq = apvts.getRawParameterValue(ModPrKey::prefix + ModPrKey::WtMod::baseFreq);

	PrHelper::setupAdsrAmpEnvPtrs(apvts, ModPrKey::prefix, ptAmpEnv);
	PrHelper::setupSsgHwEnv(apvts, ModPrKey::prefix, ptSsgHwEnv);
	PrHelper::setupSsgSwEnvPtrs(apvts, ModPrKey::prefix, ptSsgSwEnv);
	PrHelper::setupSsgSwEnv11Ptrs(apvts, ModPrKey::prefix, ptSsgSwEnv11);
	PrHelper::setupOpzx7LfoPtrs(apvts, ModPrKey::prefix, ptLfo);

	PrHelper::setupPitchEnvPtrs(apvts, ModPrKey::prefix, ptPitchEnv);
	PrHelper::setupSsgSwPEnv11Ptrs(apvts, ModPrKey::prefix, ptSsgSwPEnv11);
	PrHelper::setupWtMod(apvts, ModPrKey::prefix, ptWtMod, store);
}

void ModProcessor::prepare(double sampleRate)
{
	rate = sampleRate;

	ampEnv.prepare(sampleRate);
	ssgHwEnv.prepare(sampleRate);

	// SSG のソフトウェアエンベロープは、どの対象へ掛けるかを番号で受ける。
	// ここでは 1 組しか持たないので 0 を渡す。
	ssgSwEnv.prepare(0, sampleRate);
	ssgSwEnv11.prepare(0, sampleRate);

	lfo.prepare(sampleRate);

	pitchEnv.prepare(0, sampleRate);
	ssgSwPEnv11.prepare(0, sampleRate);
	wtMod.reset();

	for (auto& shifter : shifters) shifter.prepare(sampleRate);

	ampLevel = 1.0f;
	wasShifting = false;
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

	if (pitchEnabled)
	{
		pitchEnv.noteOn();
		ssgSwPEnv11.noteOn();
		wtMod.reset();
	}
}

void ModProcessor::noteOff()
{
	if (envEnabled)
	{
		ampEnv.noteOff();
		ssgHwEnv.noteOff();
		ssgSwEnv.noteOff();
		ssgSwEnv11.noteOff();
	}

	if (pitchEnabled)
	{
		pitchEnv.noteOff();
		ssgSwPEnv11.noteOff();
	}
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
	pitchEnabled = PrHelper::getBool(pPitchEnable);

	// どれも切なら何もしない。素通しにする。
	if (!envEnabled && !lfoEnabled && !pitchEnabled)
	{
		wasShifting = false;

		return;
	}

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

	float wtModDelta = nominalDelta;

	if (pitchEnabled)
	{
		PitchAdsrParams pitchParams;
		SsgSwPEnv11Params pEnv11Params;
		WtModParams wtModParams;

		PrHelper::applyPitchEnv(ptPitchEnv, pitchParams);
		PrHelper::applySsgSwPEnv11(ptSsgSwPEnv11, pEnv11Params);
		PrHelper::applyWtMod(ptWtMod, wtModParams);

		pitchEnv.setParameters(pitchParams);
		ssgSwPEnv11.setParameters(pEnv11Params);
		wtMod.setParameters(wtModParams);

		// WT PITCH MOD の速さだけは搬送波との比なので、基準の周波数から
		// 位相増分を作って渡す。他は倍率なので基準に左右されない。
		wtModDelta = (float)(PrHelper::getFloat(pWtModBaseFreq) / rate);
	}

	// 音程を動かすかどうかは、この塊のあいだ変えない。1 サンプルごとに
	// 出し入れすると、溜めてある音との継ぎ目で音が飛ぶ。
	bool shifting = pitchEnabled || (lfoEnabled && lfo.pm.enable);

	// 使い始めるときは溜めてある古い音を捨てる。前に鳴っていたものが
	// 尾を引いて出てきてしまう。
	if (shifting && !wasShifting)
	{
		for (auto& shifter : shifters) shifter.reset();
	}

	wasShifting = shifting;

	int channels = buffer.getNumChannels();
	int samples = buffer.getNumSamples();

	for (int i = 0; i < samples; ++i)
	{
		float gain = 1.0f;

		if (envEnabled)
		{
			// バイパスのものは元の値をそのまま返すので、掛けても変わらない。
			if (!ampEnv.isBypass())
			{
				ampLevel = ampEnv.process(ampLevel);
				gain *= ampLevel;
			}

			gain *= ssgHwEnv.process();

			if (!ssgSwEnv.isBypass()) gain *= ssgSwEnv.process();
			if (!ssgSwEnv11.isBypass()) gain *= ssgSwEnv11.process();
		}

		float ratio = 1.0f;

		if (lfoEnabled)
		{
			lfo.getSample();

			// AM は波の形そのもので、深さは別に持っている。音源と同じく
			// dB の減衰として掛ける。
			if (lfo.am.enable)
			{
				float attenDb = lfo.value.am * lfo.am.depthDb;

				gain *= std::pow(10.0f, -attenDb / 20.0f);
			}

			// PM も同じ。こちらの深さはセント。
			if (lfo.pm.enable)
			{
				ratio *= std::pow(2.0f, (lfo.value.pm * lfo.pm.depthCent) / 1200.0f);
			}
		}

		if (pitchEnabled)
		{
			// 音源と同じ順で位相の進みへ掛けてから、元の進みとの比を採る。
			float delta = nominalDelta;

			delta = pitchEnv.process(delta);
			delta = ssgSwPEnv11.process(delta);

			ratio *= delta / nominalDelta;
			ratio *= wtMod.process(wtModDelta);
		}

		for (int ch = 0; ch < channels; ++ch)
		{
			auto* data = buffer.getWritePointer(ch);

			float sample = data[i] * gain;

			if (shifting && ch < (int)shifters.size())
			{
				sample = shifters[(size_t)ch].process(sample, ratio);
			}

			data[i] = sample;
		}
	}
}
