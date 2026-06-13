#include "./ProcessorWt2.h"

#include "./ProcessorWt2Keys.h"
#include "./ProcessorWt2Values.h"
#include "./ProcessorWt2Names.h"

void Wt2Processor::createCustomWaveLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int size, const juce::String& code, const juce::String& name)
{
    for (int i = 0; i < size; ++i)
    {
        auto paramId = code + juce::String(i);
        auto paramName = name + juce::String(i);
        layout.add(std::make_unique<juce::AudioParameterInt>(paramId, paramName, Wt2PrValue::Custom::min, Wt2PrValue::Custom::max, Wt2PrValue::Custom::initial));
    }
}

template <size_t I>
void Wt2Processor::processCustomWaveBlock(std::array<float, I>& samples, juce::AudioProcessorValueTreeState& apvts, const juce::String& code)
{
    // Custom Wave params reading
    for (int i = 0; i < I; ++i)
    {
        auto paramId = code + juce::String(i);
        samples[i] = (int)*apvts.getRawParameterValue(paramId);
    }

}

void Wt2Processor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = Wt2PrKey::prefix;
    // ==========================================
    // WAVETABLE Parameters
    // ==========================================
    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Wt2PrKey::bit, code + Wt2PrName::bit, Wt2PrValue::Bit::min, Wt2PrValue::Bit::max, Wt2PrValue::Bit::initial));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Wt2PrKey::rate, code + Wt2PrName::rate, Wt2PrValue::Rate::min, Wt2PrValue::Rate::max, Wt2PrValue::Rate::initial)); // Default 6 (16kHz)
    // Size: 0:32, 1:64, 2:128, 3:256
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Wt2PrKey::sampleSize, code + Wt2PrName::sampleSize, Wt2PrValue::SammpleSize::min, Wt2PrValue::SammpleSize::max, Wt2PrValue::SammpleSize::initial));
    // Resolution : 0:16, 1:32, 2:64, 3:128, 4:256
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Wt2PrKey::resolution, code + Wt2PrName::resolution, Wt2PrValue::Resolution::min, Wt2PrValue::Resolution::max, Wt2PrValue::Resolution::initial));
    // Waveform WtPreset : 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Wt2PrKey::wave, code + Wt2PrName::waveform, Wt2PrValue::WaveForm::min, Wt2PrValue::WaveForm::max, Wt2PrValue::WaveForm::initial));

    // Fix
    layout.add(std::make_unique<juce::AudioParameterBool>(code + Wt2PrKey::fix, code + Wt2PrName::fix, Wt2PrValue::Fix::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Wt2PrKey::fixFreq, code + Wt2PrName::fixFreq, Wt2PrValue::FixFreq::min, Wt2PrValue::FixFreq::max, Wt2PrValue::FixFreq::initial));

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Wt2PrKey::Unison::voices, code + Wt2PrName::Unison::voices, Wt2PrValue::Unison::Voices::min, Wt2PrValue::Unison::Voices::max, Wt2PrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Wt2PrKey::Unison::detune, code + Wt2PrName::Unison::detune, Wt2PrValue::Unison::Detune::min, Wt2PrValue::Unison::Detune::max, Wt2PrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Wt2PrKey::Unison::spread, code + Wt2PrName::Unison::spread, Wt2PrValue::Unison::Spread::min, Wt2PrValue::Unison::Spread::max, Wt2PrValue::Unison::Spread::initial));

    createCustomWaveLayout(layout, Wt2PrValue::customSize1, code + Wt2PrKey::custom32, code + Wt2PrName::custom32);
    createCustomWaveLayout(layout, Wt2PrValue::customSize2, code + Wt2PrKey::custom64, code + Wt2PrName::custom64);
    createCustomWaveLayout(layout, Wt2PrValue::customSize3, code + Wt2PrKey::custom128, code + Wt2PrName::custom128);
    createCustomWaveLayout(layout, Wt2PrValue::customSize4, code + Wt2PrKey::custom256, code + Wt2PrName::custom256);

    // Modulation
    layout.add(std::make_unique<juce::AudioParameterBool>(code + Wt2PrKey::Mod::enable, code + Wt2PrName::Mod::enable, Wt2PrValue::Mod::Enable::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Wt2PrKey::Mod::depth, code + Wt2PrName::Mod::depth, Wt2PrValue::Mod::Depth::min, Wt2PrValue::Mod::Depth::max, Wt2PrValue::Mod::Depth::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Wt2PrKey::Mod::speed, code + Wt2PrName::Mod::speed, Wt2PrValue::Mod::Speed::min, Wt2PrValue::Mod::Speed::max, Wt2PrValue::Mod::Speed::initial));

    // Common
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Wt2PrKey::level, code + Wt2PrName::level, Wt2PrValue::Level::min, Wt2PrValue::Level::max, Wt2PrValue::Level::initial));

    // Detune
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Wt2PrKey::mul, code + Wt2PrName::mul, Wt2PrValue::Mul::min, Wt2PrValue::Mul::max, Wt2PrValue::Mul::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + Wt2PrKey::mulRatio, code + Wt2PrName::mulRatio, Wt2PrValue::MulRatio::min, Wt2PrValue::MulRatio::max, Wt2PrValue::MulRatio::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Wt2PrKey::dt, code + Wt2PrName::dt1, Wt2PrValue::Dt1::min, Wt2PrValue::Dt1::max, Wt2PrValue::Dt1::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + Wt2PrKey::dt2, code + Wt2PrName::dt2, Wt2PrValue::Dt2::min, Wt2PrValue::Dt2::max, Wt2PrValue::Dt2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt> (code + Wt2PrKey::dt3, code + Wt2PrName::dt3, Wt2PrValue::Dt3::min, Wt2PrValue::Dt3::max, Wt2PrValue::Dt3::initial));

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + Wt2PrKey::adsr + Wt2PrKey::bypass, code + Wt2PrName::Adsr::bypass, Wt2PrValue::Adsr::Bypass::initial));

    // PitchEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + Wt2PrKey::pitchAdsr + Wt2PrKey::bypass, code + Wt2PrName::PitchAdsr::bypass, Wt2PrValue::PitchAdsr::Bypass::initial));

    // SSG SwEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + Wt2PrKey::ssgSwEnv + Wt2PrKey::bypass, code + Wt2PrName::SsgSwEnv::bypass, Wt2PrValue::SsgSwEnv::Bypass::initial));

    addEnvParameters(layout, code);
	addPitchEnvParameters(layout, code);
    addSsgSwEnvParameters(layout, code);
    addOpzx7LfoParameters(layout, code);
}

void Wt2Processor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String code = Wt2PrKey::prefix;

    pLevel = apvts.getRawParameterValue(code + Wt2PrKey::level);
    pSampleSize = apvts.getRawParameterValue(code + Wt2PrKey::sampleSize);
    pResolution = apvts.getRawParameterValue(code + Wt2PrKey::resolution);
    pWave = apvts.getRawParameterValue(code + Wt2PrKey::wave);
    pBit = apvts.getRawParameterValue(code + Wt2PrKey::bit);
    pRate = apvts.getRawParameterValue(code + Wt2PrKey::rate);

    for (int i = 0; i < 32; ++i)
    {
        pCustom32[i] = apvts.getRawParameterValue(code + Wt2PrKey::custom32 + juce::String(i));
    }

    for (int i = 0; i < 64; ++i)
    {
        pCustom64[i] = apvts.getRawParameterValue(code + Wt2PrKey::custom64 + juce::String(i));
    }

    for (int i = 0; i < 128; ++i)
    {
        pCustom128[i] = apvts.getRawParameterValue(code + Wt2PrKey::custom128 + juce::String(i));
    }

    for (int i = 0; i < 256; ++i)
    {
        pCustom256[i] = apvts.getRawParameterValue(code + Wt2PrKey::custom256 + juce::String(i));
    }

    pModEnable = apvts.getRawParameterValue(code + Wt2PrKey::Mod::enable);
    pModDepth = apvts.getRawParameterValue(code + Wt2PrKey::Mod::depth);
    pModSpeed = apvts.getRawParameterValue(code + Wt2PrKey::Mod::speed);

    pFixMode = apvts.getRawParameterValue(code + Wt2PrKey::fix);
    pFixFreq = apvts.getRawParameterValue(code + Wt2PrKey::fixFreq);

    pUnisonVoices = apvts.getRawParameterValue(code + Wt2PrKey::Unison::voices);
    pUnisonDetuneCents = apvts.getRawParameterValue(code + Wt2PrKey::Unison::detune);
    pUnisonSpread = apvts.getRawParameterValue(code + Wt2PrKey::Unison::spread);

    pAdsrBypass = apvts.getRawParameterValue(code + Wt2PrKey::adsr + Wt2PrKey::bypass);
    pAdsrStl = apvts.getRawParameterValue(code + Wt2PrKey::Adsr::stl);
    pAdsrAr = apvts.getRawParameterValue(code + Wt2PrKey::Adsr::ar);
    pAdsrDr = apvts.getRawParameterValue(code + Wt2PrKey::Adsr::dr);
    pAdsrSl = apvts.getRawParameterValue(code + Wt2PrKey::Adsr::sl);
    pAdsrRr = apvts.getRawParameterValue(code + Wt2PrKey::Adsr::rr);
    pAdsrKor = apvts.getRawParameterValue(code + Wt2PrKey::Adsr::kor);

    pPitchAdsrBypass = apvts.getRawParameterValue(code + Wt2PrKey::pitchAdsr + Wt2PrKey::bypass);
    pPitchAdsrAr = apvts.getRawParameterValue(code + Wt2PrKey::PitchAdsr::ar);
    pPitchAdsrDr = apvts.getRawParameterValue(code + Wt2PrKey::PitchAdsr::dr);
    pPitchAdsrRr = apvts.getRawParameterValue(code + Wt2PrKey::PitchAdsr::rr);
    pPitchAdsrStl = apvts.getRawParameterValue(code + Wt2PrKey::PitchAdsr::stl);
    pPitchAdsrAtl = apvts.getRawParameterValue(code + Wt2PrKey::PitchAdsr::atl);
    pPitchAdsrSsl = apvts.getRawParameterValue(code + Wt2PrKey::PitchAdsr::ssl);
    pPitchAdsrRll = apvts.getRawParameterValue(code + Wt2PrKey::PitchAdsr::rll);

    pSsgSwEnvBypass = apvts.getRawParameterValue(code + Wt2PrKey::ssgSwEnv + Wt2PrKey::bypass);
    pSsgSwEnvSteps = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::steps);
    pSsgSwEnvLoop = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::loop);
    pSsgSwEnvLoopTo = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::loopTo);
    pSsgSwEnvLoopCount = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::loopCount);
    pSsgSwEnvR1 = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::r1);
    pSsgSwEnvR2 = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::r2);
    pSsgSwEnvR3 = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::r3);
    pSsgSwEnvR4 = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::r4);
    pSsgSwEnvR5 = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::r5);
    pSsgSwEnvR6 = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::r6);
    pSsgSwEnvStl = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::stl);
    pSsgSwEnvL1 = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::l1);
    pSsgSwEnvL2 = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::l2);
    pSsgSwEnvL3 = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::l3);
    pSsgSwEnvL4 = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::l4);
    pSsgSwEnvL5 = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::l5);
    pSsgSwEnvL6 = apvts.getRawParameterValue(code + Wt2PrKey::SsgSwEnv::l6);

    pMultiple = apvts.getRawParameterValue(code + Wt2PrKey::mul);
    pMultipleRatio = apvts.getRawParameterValue(code + Wt2PrKey::mulRatio);
    pDetune = apvts.getRawParameterValue(code + Wt2PrKey::dt);
    pDetune2 = apvts.getRawParameterValue(code + Wt2PrKey::dt2);
    pDetune3 = apvts.getRawParameterValue(code + Wt2PrKey::dt3);

    pLfoPmSyncDelay = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::pmSyncDelay);
    pLfoAmSyncDelay = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::amSyncDelay);
    pLfoAmSmoothRatio = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::amSmoothRatio);
    pLfoPmFreq = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::pmFreq);
    pLfoAmFreq = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::amFreq);
    pLfoPmShape = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::pgShape);
    pLfoAmShape = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::egShape);
    pLfoPm = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::pm);
    pLfoAm = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::am);
    pLfoPmd = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::pmd);
    pLfoPms = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::pms);
    pLfoAmd = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::amd);
    pLfoAms = apvts.getRawParameterValue(code + CorePrKey::Post::Lfo::ams);
}

void Wt2Processor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.wt2.bitDepth = (int)pBit->load(std::memory_order_relaxed);
    params.wt2.rateIndex = (int)pRate->load(std::memory_order_relaxed);
    params.wt2.tableSize = (int)pSampleSize->load(std::memory_order_relaxed);
    params.wt2.customWaveResolution = (int)pResolution->load(std::memory_order_relaxed);
    params.wt2.waveform = (int)pWave->load(std::memory_order_relaxed);

    params.wt2.fixedMode = (pFixMode->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.fixedFreq = pFixFreq->load(std::memory_order_relaxed);

    // ユニゾン・ハーモニー用
    params.wt2.unisonVoices = (int)pUnisonVoices->load(std::memory_order_relaxed);
    params.wt2.unisonDetuneCents = (int)pUnisonDetuneCents->load(std::memory_order_relaxed);
    params.wt2.unisonSpread = pUnisonSpread->load(std::memory_order_relaxed);

    for (int i = 0; i < 32; ++i)
    {
        params.wt2.customWave32[i] = pCustom32[i]->load(std::memory_order_relaxed);
    }

    for (int i = 0; i < 64; ++i)
    {
        params.wt2.customWave64[i] = pCustom64[i]->load(std::memory_order_relaxed);
    }

    for (int i = 0; i < 128; ++i)
    {
        params.wt2.customWave128[i] = pCustom128[i]->load(std::memory_order_relaxed);
    }

    for (int i = 0; i < 256; ++i)
    {
        params.wt2.customWave256[i] = pCustom256[i]->load(std::memory_order_relaxed);
    }

    params.wt2.modEnable = (pModEnable->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.modDepth = pModDepth->load(std::memory_order_relaxed);
    params.wt2.modSpeed = pModSpeed->load(std::memory_order_relaxed);
    params.wt2.level = pLevel->load(std::memory_order_relaxed);

    params.wt2.adsr.bypass = (pAdsrBypass->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.adsr.stl = pAdsrStl->load(std::memory_order_relaxed);
    params.wt2.adsr.ar = pAdsrAr->load(std::memory_order_relaxed);
    params.wt2.adsr.dr = pAdsrDr->load(std::memory_order_relaxed);
    params.wt2.adsr.sl = pAdsrSl->load(std::memory_order_relaxed);
    params.wt2.adsr.rr = pAdsrRr->load(std::memory_order_relaxed);
    params.wt2.adsr.kor = pAdsrKor->load(std::memory_order_relaxed) > Wt2PrValue::boolThread;

    params.wt2.pitchAdsr.bypass = (pPitchAdsrBypass->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.pitchAdsr.ar = pPitchAdsrAr->load(std::memory_order_relaxed);
    params.wt2.pitchAdsr.dr = pPitchAdsrDr->load(std::memory_order_relaxed);
    params.wt2.pitchAdsr.rr = pPitchAdsrRr->load(std::memory_order_relaxed);
    params.wt2.pitchAdsr.stl = (int)pPitchAdsrStl->load(std::memory_order_relaxed);
    params.wt2.pitchAdsr.atl = (int)pPitchAdsrAtl->load(std::memory_order_relaxed);
    params.wt2.pitchAdsr.ssl = (int)pPitchAdsrSsl->load(std::memory_order_relaxed);
    params.wt2.pitchAdsr.rll = (int)pPitchAdsrRll->load(std::memory_order_relaxed);

    params.wt2.ssgSwEnv.bypass = (pSsgSwEnvBypass->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.ssgSwEnv.steps = (int)pSsgSwEnvSteps->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.loop = (pSsgSwEnvLoop->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.ssgSwEnv.loopTo = (int)pSsgSwEnvLoopTo->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.loopCount = (int)pSsgSwEnvLoopCount->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.stl = pSsgSwEnvStl->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.r1 = pSsgSwEnvR1->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.l1 = pSsgSwEnvL1->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.r2 = pSsgSwEnvR2->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.l2 = pSsgSwEnvL2->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.r3 = pSsgSwEnvR3->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.l3 = pSsgSwEnvL3->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.r4 = pSsgSwEnvR4->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.l4 = pSsgSwEnvL4->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.r5 = pSsgSwEnvR5->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.l5 = pSsgSwEnvL5->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.r6 = pSsgSwEnvR6->load(std::memory_order_relaxed);
    params.wt2.ssgSwEnv.l6 = pSsgSwEnvL6->load(std::memory_order_relaxed);

    params.wt2.multiple = (int)pMultiple->load(std::memory_order_relaxed);
    params.wt2.multipleRatio = pMultipleRatio->load(std::memory_order_relaxed);
    params.wt2.detune = (int)pDetune->load(std::memory_order_relaxed);
    params.wt2.detune2 = (int)pDetune2->load(std::memory_order_relaxed);
    params.wt2.detune3 = (int)pDetune3->load(std::memory_order_relaxed);

    params.wt2.lfoPmFreq = pLfoPmFreq->load(std::memory_order_relaxed);
    params.wt2.lfoAmFreq = pLfoAmFreq->load(std::memory_order_relaxed);
    params.wt2.lfoPmWave = (int)pLfoPmShape->load(std::memory_order_relaxed);
    params.wt2.lfoAmWave = (int)pLfoAmShape->load(std::memory_order_relaxed);
    params.wt2.lfoAmSmRt = pLfoAmSmoothRatio->load(std::memory_order_relaxed);
    params.wt2.lfoAmEnable = (pLfoAm->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.lfoPmEnable = (pLfoPm->load(std::memory_order_relaxed) > Wt2PrValue::boolThread);
    params.wt2.lfoPms = pLfoPms->load(std::memory_order_relaxed);
    params.wt2.lfoAms = pLfoAms->load(std::memory_order_relaxed);
    params.wt2.lfoPmd = pLfoPmd->load(std::memory_order_relaxed);
    params.wt2.lfoAmd = pLfoAmd->load(std::memory_order_relaxed);
    params.wt2.lfoPmSyncDelay = pLfoPmSyncDelay->load(std::memory_order_relaxed);
    params.wt2.lfoAmSyncDelay = pLfoAmSyncDelay->load(std::memory_order_relaxed);
}
