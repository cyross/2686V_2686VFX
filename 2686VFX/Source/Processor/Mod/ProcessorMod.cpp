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
		prefix + ModPrKey::Env::bypass,
		displayName + " Env Bypass",
		ModPrValue::Env::bypassInitial
	);

	PrHelper::addAdsrBypassParameter(layout, prefix, displayName, CPV::Adsr::Bypass::initial);
	PrHelper::addEnvParameters(layout, prefix, displayName);

	PrHelper::addSsgHwEnvParameters(layout, prefix, displayName);
	PrHelper::addWtAmpModParameters(layout, prefix, displayName);

	// SSG のソフトウェアエンベロープは、入り切りの札だけが別の関数で
	// まとめて登録される作りになっている。ここは必要なぶんだけ登録する。
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
		prefix + ModPrKey::Lfo::bypass,
		displayName + " LFO Bypass",
		ModPrValue::Lfo::bypassInitial
	);

	PrHelper::addOpzx7LfoParameters(layout, prefix, displayName);

	// ------------------------------------------------------------------
	// 音程側
	// ------------------------------------------------------------------
	PrHelper::addBool(
		layout,
		prefix + ModPrKey::Pitch::bypass,
		displayName + " Pitch Bypass",
		ModPrValue::Pitch::bypassInitial
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
	PrHelper::addSsgHwPEnvParameters(layout, prefix, displayName);

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

	// ------------------------------------------------------------------
	// 音程を一定量ずらすもの
	// ------------------------------------------------------------------
	PrHelper::addBool(
		layout,
		prefix + ModPrKey::Shift::bypass,
		displayName + " Shift Bypass",
		ModPrValue::Shift::bypassInitial
	);

	PrHelper::addOpzx7DetuneParameters(layout, prefix, displayName);
	PrHelper::addUnisonParameters(layout, prefix, displayName);
}

void ModProcessor::init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& store)
{
	pEnvBypass = apvts.getRawParameterValue(ModPrKey::prefix + ModPrKey::Env::bypass);
	pLfoBypass = apvts.getRawParameterValue(ModPrKey::prefix + ModPrKey::Lfo::bypass);
	pPitchBypass = apvts.getRawParameterValue(ModPrKey::prefix + ModPrKey::Pitch::bypass);
	pWtModBaseFreq = apvts.getRawParameterValue(ModPrKey::prefix + ModPrKey::WtMod::baseFreq);

	PrHelper::setupAdsrAmpEnvPtrs(apvts, ModPrKey::prefix, ptAmpEnv);
	PrHelper::setupSsgHwEnv(apvts, ModPrKey::prefix, ptSsgHwEnv);
	PrHelper::setupWtAmpMod(apvts, ModPrKey::prefix, ptWtAmpMod, store);
	PrHelper::setupSsgSwEnv11Ptrs(apvts, ModPrKey::prefix, ptSsgSwEnv11);
	PrHelper::setupOpzx7LfoPtrs(apvts, ModPrKey::prefix, ptLfo);

	PrHelper::setupPitchEnvPtrs(apvts, ModPrKey::prefix, ptPitchEnv);
	PrHelper::setupSsgSwPEnv11Ptrs(apvts, ModPrKey::prefix, ptSsgSwPEnv11);
	PrHelper::setupWtMod(apvts, ModPrKey::prefix, ptWtMod, store);
	PrHelper::setupSsgHwPEnv(apvts, ModPrKey::prefix, ptSsgHwPEnv);

	pShiftBypass = apvts.getRawParameterValue(ModPrKey::prefix + ModPrKey::Shift::bypass);

	PrHelper::setupOpzx7DetunePtrs(apvts, ModPrKey::prefix, ptDetune);
	PrHelper::setupUnisonPtrs(apvts, ModPrKey::prefix, ptUnison);
}

void ModProcessor::prepare(double sampleRate)
{
	rate = sampleRate;

	ampEnv.prepare(sampleRate);
	ssgHwEnv.prepare(sampleRate);
	wtAmpMod.reset();

	// SSG のソフトウェアエンベロープは、どの対象へ掛けるかを番号で受ける。
	// ここでは 1 組しか持たないので 0 を渡す。
	ssgSwEnv11.prepare(0, sampleRate);

	lfo.prepare(sampleRate);

	pitchEnv.prepare(0, sampleRate);
	ssgSwPEnv11.prepare(0, sampleRate);
	wtMod.reset();
	ssgHwPEnv.prepare(sampleRate);

	for (auto& voice : shifters)
	{
		for (auto& shifter : voice) shifter.prepare(sampleRate);
	}

	arpVoice = 0;
	arpPhase = 0.0;
	arpGains.fill(0.0f);
	arpGains[0] = 1.0f;

	ampLevel = 1.0f;
	wasShifting = false;
}

// 入り切りの札を読み直す。
//
// 鍵盤の押し離しは、この塊の音を作るより先に届く。札の読み取りを
// 音を作るところに任せると、札を入れた直後の 1 音目だけが取りこぼされる。
void ModProcessor::refreshSwitches()
{
	envEnabled = !PrHelper::getBool(pEnvBypass);
	lfoEnabled = !PrHelper::getBool(pLfoBypass);
	pitchEnabled = !PrHelper::getBool(pPitchBypass);
	shiftEnabled = !PrHelper::getBool(pShiftBypass);
}

void ModProcessor::noteOn()
{
	refreshSwitches();

	if (envEnabled)
	{
		ampLevel = ampEnv.noteOn();

		ssgHwEnv.noteOn();
		wtAmpMod.reset();
		ssgSwEnv11.noteOn();
	}

	if (lfoEnabled) lfo.noteOn();

	if (pitchEnabled)
	{
		pitchEnv.noteOn();
		ssgSwPEnv11.noteOn();
		wtMod.reset();
		ssgHwPEnv.noteOn();
	}
}

void ModProcessor::noteOff()
{
	refreshSwitches();

	if (envEnabled)
	{
		ampEnv.noteOff();
		ssgHwEnv.noteOff();
		ssgHwPEnv.noteOff();
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

	refreshSwitches();

	// どれも切なら何もしない。素通しにする。
	if (!envEnabled && !lfoEnabled && !pitchEnabled && !shiftEnabled)
	{
		wasShifting = false;

		return;
	}

	if (envEnabled)
	{
		AmpAdsrParams ampParams;
		SsgHwEnvParams hwParams;
		SsgSwEnv11Params sw11Params;

		PrHelper::applyAdsrAmpEnv(ptAmpEnv, ampParams);
		PrHelper::applySsgHwEnv(ptSsgHwEnv, hwParams);

		WtAmpModParams ampModParams;

		PrHelper::applyWtAmpMod(ptWtAmpMod, ampModParams);
		wtAmpMod.setParameters(ampModParams);
		PrHelper::applySsgSwEnv11(ptSsgSwEnv11, sw11Params);

		ampEnv.setParameters(ampParams);
		ssgHwEnv.setParameters(hwParams);
		ssgSwEnv11.setParameters(sw11Params);
	}

	if (lfoEnabled)
	{
		LfoOpzx7Params lfoParams;

		PrHelper::applyOpzx7Lfo(ptLfo, lfoParams);

		lfo.setParameters(lfoParams);
	}

	float wtModDelta = nominalDelta;

	// WT AMP MOD も速さは搬送波との比なので、同じ基準周波数から作る。
	const float ampModDelta = (float)(PrHelper::getFloat(pWtModBaseFreq) / rate);

	if (pitchEnabled)
	{
		PitchAdsrParams pitchParams;
		SsgSwPEnv11Params pEnv11Params;
		WtModParams wtModParams;

		PrHelper::applyPitchEnv(ptPitchEnv, pitchParams);
		PrHelper::applySsgSwPEnv11(ptSsgSwPEnv11, pEnv11Params);
		PrHelper::applyWtMod(ptWtMod, wtModParams);

		SsgHwPEnvParams hwPEnvParams;

		PrHelper::applySsgHwPEnv(ptSsgHwPEnv, hwPEnvParams);
		ssgHwPEnv.setParameters(hwPEnvParams);

		pitchEnv.setParameters(pitchParams);
		ssgSwPEnv11.setParameters(pEnv11Params);
		wtMod.setParameters(wtModParams);

		// WT PITCH MOD の速さだけは搬送波との比なので、基準の周波数から
		// 位相増分を作って渡す。他は倍率なので基準に左右されない。
		wtModDelta = (float)(PrHelper::getFloat(pWtModBaseFreq) / rate);
	}

	// MUL・DET は音程を一定量ずらすだけなので、倍率を 1 度求めれば足りる。
	// 基準に 1.0 を渡すと、返ってくる値がそのまま倍率になる。
	float shiftRatio = 1.0f;
	int voices = 1;

	if (shiftEnabled)
	{
		Opzx7DetuneParams detuneParams;

		PrHelper::applyOpzx7Detune(ptDetune, detuneParams);

		detune.setParameters(detuneParams);

		shiftRatio = detune.noteOn(1.0f);

		PrHelper::applyUnison(ptUnison, unisonParams);

		voices = juce::jlimit(1, Global::unisonVoices, unisonParams.voices);
	}

	// 音程を動かすかどうかは、この塊のあいだ変えない。1 サンプルごとに
	// 出し入れすると、溜めてある音との継ぎ目で音が飛ぶ。
	bool shifting = pitchEnabled
		|| (lfoEnabled && lfo.pm.enable)
		|| (shiftEnabled && (voices > 1 || shiftRatio < 0.9999f || shiftRatio > 1.0001f));

	// 使い始めるときは溜めてある古い音を捨てる。前に鳴っていたものが
	// 尾を引いて出てきてしまう。
	if (shifting && !wasShifting)
	{
		for (auto& voice : shifters)
		{
			for (auto& shifter : voice) shifter.reset();
		}
	}

	wasShifting = shifting;

	// ボイスごとの音程・左右の振り分け・音量を先に出しておく。
	// 1 サンプルごとに求め直しても結果は変わらない。
	std::array<float, Global::unisonVoices> voiceRatio{};
	std::array<float, Global::unisonVoices> voicePanL{};
	std::array<float, Global::unisonVoices> voicePanR{};

	float voiceGain = 1.0f;

	for (int v = 0; v < voices; ++v)
	{
		voiceRatio[(size_t)v] = shiftRatio;
		voicePanL[(size_t)v] = 1.0f;
		voicePanR[(size_t)v] = 1.0f;
	}

	if (shiftEnabled && voices > 1)
	{
		for (int v = 0; v < voices; ++v)
		{
			UnisonState state;

			// 添字 0 はメインなので、ボイス単位の上乗せを持たない。
			float paraDetune = (v > 0) ? (float)unisonParams.paraDetune[(size_t)(v - 1)] : 0.0f;
			float paraDistance = (v > 0) ? unisonParams.paraDistance[(size_t)(v - 1)] : 0.0f;

			state.setParams(v, voices, (float)unisonParams.detuneCents, unisonParams.spread,
				paraDetune, paraDistance);

			// 基準に 1.0 を渡して、返ってきた値をそのまま倍率として使う。
			voiceRatio[(size_t)v] = shiftRatio * state.applyDetune(1.0f);

			float panL = 1.0f;
			float panR = 1.0f;

			state.applyPan(panL, panR);

			voicePanL[(size_t)v] = panL;
			voicePanR[(size_t)v] = panR;

			voiceGain = state.getGainComp();
		}
	}

	// 疑似高速アルペジオ。全部を重ねずに 1 つずつ切り替えて鳴らす。
	bool arpeggio = shiftEnabled && voices > 1 && unisonParams.arpEnable;

	double arpStep = arpeggio ? ((double)juce::jmax(1, unisonParams.arpFreq) / rate) : 0.0;

	// 切り替わり目のクリック音対策。1 ミリ秒ほどかけて渡す。
	float arpRamp = (float)(1.0 / juce::jmax(1.0, rate * 0.001));

	if (!arpeggio)
	{
		arpGains.fill(1.0f);
	}

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
			gain *= wtAmpMod.process(ampModDelta);

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
			delta = ssgHwPEnv.process(delta);

			ratio *= delta / nominalDelta;
			ratio *= wtMod.process(wtModDelta);
		}

		if (arpeggio)
		{
			arpPhase += arpStep;

			while (arpPhase >= 1.0)
			{
				arpPhase -= 1.0;

				if (++arpVoice >= voices) arpVoice = 0;
			}

			// 鳴らすボイスへ寄せ、他は下げる。滑らかさを切ってあるときは
			// 一気に切り替える。
			for (int v = 0; v < voices; ++v)
			{
				float target = (v == arpVoice) ? 1.0f : 0.0f;
				float& g = arpGains[(size_t)v];

				if (!unisonParams.arpSmooth) g = target;
				else if (g < target) g = juce::jmin(target, g + arpRamp);
				else if (g > target) g = juce::jmax(target, g - arpRamp);
			}
		}

		// 元の音をいったん控える。ボイスごとに読み直すため。
		float dryL = buffer.getWritePointer(0)[i] * gain;
		float dryR = (channels > 1) ? buffer.getWritePointer(1)[i] * gain : dryL;

		float outL = 0.0f;
		float outR = 0.0f;

		for (int v = 0; v < voices; ++v)
		{
			float g = arpGains[(size_t)v] * voiceGain;

			if (g <= 0.0f) continue;

			float voiceRate = ratio * voiceRatio[(size_t)v];

			// 変調の元がおかしな値を返すことがあるので、渡す前に音程として
			// ありえる幅へ収める。下は 5 オクターブ下、上は 5 オクターブ上。
			if (!std::isfinite(voiceRate)) voiceRate = 1.0f;

			voiceRate = juce::jlimit(0.03125f, 32.0f, voiceRate);

			float sL = dryL;
			float sR = dryR;

			if (shifting)
			{
				sL = shifters[(size_t)v][0].process(dryL, voiceRate);
				sR = shifters[(size_t)v][1].process(dryR, voiceRate);
			}

			outL += sL * voicePanL[(size_t)v] * g;
			outR += sR * voicePanR[(size_t)v] * g;
		}

		buffer.getWritePointer(0)[i] = outL;

		if (channels > 1) buffer.getWritePointer(1)[i] = outR;

		// 3 本以上あるときは、余りへ左右を混ぜたものを入れる。
		for (int ch = 2; ch < channels; ++ch)
		{
			buffer.getWritePointer(ch)[i] = (outL + outR) * 0.5f;
		}
	}
}
