#include "./ProcessorWt.h"

#include "./ProcessorWtKeys.h"
#include "./ProcessorWtValues.h"
#include "./ProcessorWtNames.h"

void WtProcessor::createCustomWaveLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout, int size, const juce::String& code, const juce::String& name)
{
    for (int i = 0; i < size; ++i)
    {
        auto paramId = code + juce::String(i);
        auto paramName = name + juce::String(i);
        layout.add(std::make_unique<juce::AudioParameterFloat>(paramId, paramName, WtPrValue::Custom::min, WtPrValue::Custom::max, WtPrValue::Custom::initial));
    }
}

template <size_t I>
void WtProcessor::processCustomWaveBlock(std::array<float, I>& samples, juce::AudioProcessorValueTreeState& apvts, const juce::String& code)
{
    // Custom Wave params reading
    for (int i = 0; i < I; ++i)
    {
        auto paramId = code + juce::String(i);
        samples[i] = *apvts.getRawParameterValue(paramId);
    }

}

void WtProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = WtPrKey::prefix;
    // ==========================================
    // WAVETABLE Parameters
    // ==========================================
    // Bit Depth: 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::bit, code + WtPrName::bit, WtPrValue::Bit::min, WtPrValue::Bit::max, WtPrValue::Bit::initial));
    // ADD: Sample Rate Index
    // 1:96k, 2:55.5k, 3:48k, 4:44.1k, 5:22.05k, 6:16k, 7:8k
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::rate, code + WtPrName::rate, WtPrValue::Rate::min, WtPrValue::Rate::max, WtPrValue::Rate::initial)); // Default 6 (16kHz)
    // Size: 0:32, 1:64, 2:128, 3:256
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::sampleSize, code + WtPrName::sampleSize, WtPrValue::SammpleSize::min, WtPrValue::SammpleSize::max, WtPrValue::SammpleSize::initial));
    // Steps : 0:Free, 1:16(+), 2:32(+), 3:64(+), 4:128(+), 5:256(+), 6:16(-), 7:32(-), 8:64(-), 9:128(-), 10:256(-)
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::steps, code + WtPrName::steps, WtPrValue::Steps::min, WtPrValue::Steps::max, WtPrValue::Steps::initial));
    // Waveform WtPreset : 0:Sine, 1:Tri, 2:SawUp, 3:SawDown, 4:Square, 5:Pulse25, 6:Pulse12, 7:Noise, 8:Custom
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::wave, code + WtPrName::waveform, WtPrValue::WaveForm::min, WtPrValue::WaveForm::max, WtPrValue::WaveForm::initial));

    // Fix
    layout.add(std::make_unique<juce::AudioParameterBool>(code + WtPrKey::fix, code + WtPrName::fix, WtPrValue::Fix::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::fixFreq, code + WtPrName::fixFreq, WtPrValue::FixFreq::min, WtPrValue::FixFreq::max, WtPrValue::FixFreq::initial));

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::Unison::voices, code + WtPrName::Unison::voices, WtPrValue::Unison::Voices::min, WtPrValue::Unison::Voices::max, WtPrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::Unison::detune, code + WtPrName::Unison::detune, WtPrValue::Unison::Detune::min, WtPrValue::Unison::Detune::max, WtPrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Unison::spread, code + WtPrName::Unison::spread, WtPrValue::Unison::Spread::min, WtPrValue::Unison::Spread::max, WtPrValue::Unison::Spread::initial));

    createCustomWaveLayout(layout, WtPrValue::customSize1, code + WtPrKey::custom32, code + WtPrName::custom32);
    createCustomWaveLayout(layout, WtPrValue::customSize2, code + WtPrKey::custom64, code + WtPrName::custom64);
    createCustomWaveLayout(layout, WtPrValue::customSize3, code + WtPrKey::custom128, code + WtPrName::custom128);
    createCustomWaveLayout(layout, WtPrValue::customSize4, code + WtPrKey::custom256, code + WtPrName::custom256);

    // Modulation
    layout.add(std::make_unique<juce::AudioParameterBool>(code + WtPrKey::Mod::enable, code + WtPrName::Mod::enable, WtPrValue::Mod::Enable::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Mod::depth, code + WtPrName::Mod::depth, WtPrValue::Mod::Depth::min, WtPrValue::Mod::Depth::max, WtPrValue::Mod::Depth::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Mod::speed, code + WtPrName::Mod::speed, WtPrValue::Mod::Speed::min, WtPrValue::Mod::Speed::max, WtPrValue::Mod::Speed::initial));

    // Common
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::level, code + WtPrName::level, WtPrValue::Level::min, WtPrValue::Level::max, WtPrValue::Level::initial));

    // Detune
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::mul, code + WtPrName::mul, WtPrValue::Mul::min, WtPrValue::Mul::max, WtPrValue::Mul::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::mulRatio, code + WtPrName::mulRatio, WtPrValue::MulRatio::min, WtPrValue::MulRatio::max, WtPrValue::MulRatio::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::dt, code + WtPrName::dt1, WtPrValue::Dt1::min, WtPrValue::Dt1::max, WtPrValue::Dt1::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::dt2, code + WtPrName::dt2, WtPrValue::Dt2::min, WtPrValue::Dt2::max, WtPrValue::Dt2::initial));
    layout.add(std::make_unique<juce::AudioParameterInt> (code + WtPrKey::dt3, code + WtPrName::dt3, WtPrValue::Dt3::min, WtPrValue::Dt3::max, WtPrValue::Dt3::initial));

    // ADSR Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + WtPrKey::adsr + WtPrKey::bypass, code + WtPrName::Adsr::bypass, WtPrValue::Adsr::Bypass::initial));

    // PitchEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + WtPrKey::pitchAdsr + WtPrKey::bypass, code + WtPrName::PitchAdsr::bypass, WtPrValue::PitchAdsr::Bypass::initial));

    // SSG SwEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + WtPrKey::ssgSwEnv + WtPrKey::bypass, code + WtPrName::SsgSwEnv::bypass, WtPrValue::SsgSwEnv::Bypass::initial));

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Lfo::pmFreq, code + WtPrName::Lfo::pmFreq, WtPrValue::Lfo::PmFreq::min, WtPrValue::Lfo::PmFreq::max, WtPrValue::Lfo::PmFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Lfo::amFreq, code + WtPrName::Lfo::amFreq, WtPrValue::Lfo::AmFreq::min, WtPrValue::Lfo::AmFreq::max, WtPrValue::Lfo::AmFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::Lfo::syncDelay, code + WtPrName::Lfo::syncDelay, WtPrValue::Lfo::SyncDelay::min, WtPrValue::Lfo::SyncDelay::max, WtPrValue::Lfo::SyncDelay::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Lfo::amSmoothRatio, code + WtPrName::Lfo::amSmoothRatio, WtPrValue::Lfo::AmSmRt::min, WtPrValue::Lfo::AmSmRt::max, WtPrValue::Lfo::AmSmRt::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::Lfo::pmShape, code + WtPrName::Lfo::pmShape, WtPrValue::Lfo::PmShape::min, WtPrValue::Lfo::PmShape::max, WtPrValue::Lfo::PmShape::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + WtPrKey::Lfo::amShape, code + WtPrName::Lfo::amShape, WtPrValue::Lfo::AmShape::min, WtPrValue::Lfo::AmShape::max, WtPrValue::Lfo::AmShape::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + WtPrKey::Lfo::am, code + WtPrName::Lfo::am, WtPrValue::Lfo::Am::initial)); // AM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterBool>(code + WtPrKey::Lfo::pm, code + WtPrName::Lfo::pm, WtPrValue::Lfo::Pm::initial)); // PM Enable (Switch)
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Lfo::pms, code + WtPrName::Lfo::pms, WtPrValue::Lfo::Pms::min, WtPrValue::Lfo::Pms::max, WtPrValue::Lfo::Pms::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Lfo::ams, code + WtPrName::Lfo::ams, WtPrValue::Lfo::Ams::min, WtPrValue::Lfo::Ams::max, WtPrValue::Lfo::Ams::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Lfo::pmd, code + WtPrName::Lfo::pmd, WtPrValue::Lfo::Pmd::min, WtPrValue::Lfo::Pmd::max, WtPrValue::Lfo::Pmd::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + WtPrKey::Lfo::amd, code + WtPrName::Lfo::amd, WtPrValue::Lfo::Amd::min, WtPrValue::Lfo::Amd::max, WtPrValue::Lfo::Amd::initial));

    addEnvParameters(layout, code);
	addPitchEnvParameters(layout, code);
    addSsgSwEnvParameters(layout, code);
}

void WtProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String code = WtPrKey::prefix;

    pLevel = apvts.getRawParameterValue(code + WtPrKey::level);
    pSampleSize = apvts.getRawParameterValue(code + WtPrKey::sampleSize);
    pStep = apvts.getRawParameterValue(code + WtPrKey::steps);
    pWave = apvts.getRawParameterValue(code + WtPrKey::wave);
    pBit = apvts.getRawParameterValue(code + WtPrKey::bit);
    pRate = apvts.getRawParameterValue(code + WtPrKey::rate);

    for (int i = 0; i < 32; ++i)
    {
        pCustom32[i] = apvts.getRawParameterValue(code + WtPrKey::custom32 + juce::String(i));
    }

    for (int i = 0; i < 64; ++i)
    {
        pCustom64[i] = apvts.getRawParameterValue(code + WtPrKey::custom64 + juce::String(i));
    }

    for (int i = 0; i < 128; ++i)
    {
        pCustom128[i] = apvts.getRawParameterValue(code + WtPrKey::custom128 + juce::String(i));
    }

    for (int i = 0; i < 256; ++i)
    {
        pCustom256[i] = apvts.getRawParameterValue(code + WtPrKey::custom256 + juce::String(i));
    }

    pModEnable = apvts.getRawParameterValue(code + WtPrKey::Mod::enable);
    pModDepth = apvts.getRawParameterValue(code + WtPrKey::Mod::depth);
    pModSpeed = apvts.getRawParameterValue(code + WtPrKey::Mod::speed);

    pFixMode = apvts.getRawParameterValue(code + WtPrKey::fix);
    pFixFreq = apvts.getRawParameterValue(code + WtPrKey::fixFreq);

    pUnisonVoices = apvts.getRawParameterValue(code + WtPrKey::Unison::voices);
    pUnisonDetuneCents = apvts.getRawParameterValue(code + WtPrKey::Unison::detune);
    pUnisonSpread = apvts.getRawParameterValue(code + WtPrKey::Unison::spread);

    pAdsrBypass = apvts.getRawParameterValue(code + WtPrKey::adsr + WtPrKey::bypass);
    pAdsrStl = apvts.getRawParameterValue(code + WtPrKey::Adsr::stl);
    pAdsrAr = apvts.getRawParameterValue(code + WtPrKey::Adsr::ar);
    pAdsrDr = apvts.getRawParameterValue(code + WtPrKey::Adsr::dr);
    pAdsrSl = apvts.getRawParameterValue(code + WtPrKey::Adsr::sl);
    pAdsrRr = apvts.getRawParameterValue(code + WtPrKey::Adsr::rr);
    pAdsrKor = apvts.getRawParameterValue(code + WtPrKey::Adsr::kor);

    pPitchAdsrBypass = apvts.getRawParameterValue(code + WtPrKey::pitchAdsr + WtPrKey::bypass);
    pPitchAdsrAr = apvts.getRawParameterValue(code + WtPrKey::PitchAdsr::ar);
    pPitchAdsrDr = apvts.getRawParameterValue(code + WtPrKey::PitchAdsr::dr);
    pPitchAdsrRr = apvts.getRawParameterValue(code + WtPrKey::PitchAdsr::rr);
    pPitchAdsrStl = apvts.getRawParameterValue(code + WtPrKey::PitchAdsr::stl);
    pPitchAdsrAtl = apvts.getRawParameterValue(code + WtPrKey::PitchAdsr::atl);
    pPitchAdsrSsl = apvts.getRawParameterValue(code + WtPrKey::PitchAdsr::ssl);
    pPitchAdsrRll = apvts.getRawParameterValue(code + WtPrKey::PitchAdsr::rll);

    pSsgSwEnvBypass = apvts.getRawParameterValue(code + WtPrKey::ssgSwEnv + WtPrKey::bypass);
    pSsgSwEnvSteps = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::steps);
    pSsgSwEnvLoop = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::loop);
    pSsgSwEnvLoopTo = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::loopTo);
    pSsgSwEnvLoopCount = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::loopCount);
    pSsgSwEnvR1 = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::r1);
    pSsgSwEnvR2 = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::r2);
    pSsgSwEnvR3 = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::r3);
    pSsgSwEnvR4 = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::r4);
    pSsgSwEnvR5 = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::r5);
    pSsgSwEnvR6 = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::r6);
    pSsgSwEnvStl = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::stl);
    pSsgSwEnvL1 = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::l1);
    pSsgSwEnvL2 = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::l2);
    pSsgSwEnvL3 = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::l3);
    pSsgSwEnvL4 = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::l4);
    pSsgSwEnvL5 = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::l5);
    pSsgSwEnvL6 = apvts.getRawParameterValue(code + WtPrKey::SsgSwEnv::l6);

    pMultiple = apvts.getRawParameterValue(code + WtPrKey::mul);
    pMultipleRatio = apvts.getRawParameterValue(code + WtPrKey::mulRatio);
    pDetune = apvts.getRawParameterValue(code + WtPrKey::dt);
    pDetune2 = apvts.getRawParameterValue(code + WtPrKey::dt2);
    pDetune3 = apvts.getRawParameterValue(code + WtPrKey::dt3);

    pLfoSyncDelay = apvts.getRawParameterValue(code + WtPrKey::Lfo::syncDelay);
    pLfoAmSmoothRatio = apvts.getRawParameterValue(code + WtPrKey::Lfo::amSmoothRatio);
    pLfoPmFreq = apvts.getRawParameterValue(code + WtPrKey::Lfo::pmFreq);
    pLfoAmFreq = apvts.getRawParameterValue(code + WtPrKey::Lfo::amFreq);
    pLfoPmShape = apvts.getRawParameterValue(code + WtPrKey::Lfo::pmShape);
    pLfoAmShape = apvts.getRawParameterValue(code + WtPrKey::Lfo::amShape);
    pLfoPm = apvts.getRawParameterValue(code + WtPrKey::Lfo::pm);
    pLfoAm = apvts.getRawParameterValue(code + WtPrKey::Lfo::am);
    pLfoPmd = apvts.getRawParameterValue(code + WtPrKey::Lfo::pmd);
    pLfoPms = apvts.getRawParameterValue(code + WtPrKey::Lfo::pms);
    pLfoAmd = apvts.getRawParameterValue(code + WtPrKey::Lfo::amd);
    pLfoAms = apvts.getRawParameterValue(code + WtPrKey::Lfo::ams);
}

void WtProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    params.wt.bitDepth = (int)pBit->load(std::memory_order_relaxed);
    params.wt.rateIndex = (int)pRate->load(std::memory_order_relaxed);
    params.wt.tableSize = (int)pSampleSize->load(std::memory_order_relaxed);
    params.wt.steps = (int)pStep->load(std::memory_order_relaxed);
    params.wt.waveform = (int)pWave->load(std::memory_order_relaxed);

    params.wt.fixedMode = (pFixMode->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.fixedFreq = pFixFreq->load(std::memory_order_relaxed);

    // ユニゾン・ハーモニー用
    params.wt.unisonVoices = (int)pUnisonVoices->load(std::memory_order_relaxed);
    params.wt.unisonDetuneCents = (int)pUnisonDetuneCents->load(std::memory_order_relaxed);
    params.wt.unisonSpread = pUnisonSpread->load(std::memory_order_relaxed);

    for (int i = 0; i < 32; ++i)
    {
        params.wt.customWave32[i] = pCustom32[i]->load(std::memory_order_relaxed);
    }

    for (int i = 0; i < 64; ++i)
    {
        params.wt.customWave64[i] = pCustom64[i]->load(std::memory_order_relaxed);
    }

    for (int i = 0; i < 128; ++i)
    {
        params.wt.customWave128[i] = pCustom128[i]->load(std::memory_order_relaxed);
    }

    for (int i = 0; i < 256; ++i)
    {
        params.wt.customWave256[i] = pCustom256[i]->load(std::memory_order_relaxed);
    }

    params.wt.modEnable = (pModEnable->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.modDepth = pModDepth->load(std::memory_order_relaxed);
    params.wt.modSpeed = pModSpeed->load(std::memory_order_relaxed);
    params.wt.level = pLevel->load(std::memory_order_relaxed);

    params.wt.adsr.bypass = (pAdsrBypass->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.adsr.stl = pAdsrStl->load(std::memory_order_relaxed);
    params.wt.adsr.ar = pAdsrAr->load(std::memory_order_relaxed);
    params.wt.adsr.dr = pAdsrDr->load(std::memory_order_relaxed);
    params.wt.adsr.sl = pAdsrSl->load(std::memory_order_relaxed);
    params.wt.adsr.rr = pAdsrRr->load(std::memory_order_relaxed);
    params.wt.adsr.kor = pAdsrKor->load(std::memory_order_relaxed) > WtPrValue::boolThread;

    params.wt.pitchAdsr.bypass = (pPitchAdsrBypass->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.pitchAdsr.ar = pPitchAdsrAr->load(std::memory_order_relaxed);
    params.wt.pitchAdsr.dr = pPitchAdsrDr->load(std::memory_order_relaxed);
    params.wt.pitchAdsr.rr = pPitchAdsrRr->load(std::memory_order_relaxed);
    params.wt.pitchAdsr.stl = (int)pPitchAdsrStl->load(std::memory_order_relaxed);
    params.wt.pitchAdsr.atl = (int)pPitchAdsrAtl->load(std::memory_order_relaxed);
    params.wt.pitchAdsr.ssl = (int)pPitchAdsrSsl->load(std::memory_order_relaxed);
    params.wt.pitchAdsr.rll = (int)pPitchAdsrRll->load(std::memory_order_relaxed);

    params.wt.ssgSwEnv.bypass = (pSsgSwEnvBypass->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.ssgSwEnv.steps = (int)pSsgSwEnvSteps->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.loop = (pSsgSwEnvLoop->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.ssgSwEnv.loopTo = (int)pSsgSwEnvLoopTo->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.loopCount = (int)pSsgSwEnvLoopCount->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.stl = pSsgSwEnvStl->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.r1 = pSsgSwEnvR1->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.l1 = pSsgSwEnvL1->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.r2 = pSsgSwEnvR2->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.l2 = pSsgSwEnvL2->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.r3 = pSsgSwEnvR3->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.l3 = pSsgSwEnvL3->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.r4 = pSsgSwEnvR4->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.l4 = pSsgSwEnvL4->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.r5 = pSsgSwEnvR5->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.l5 = pSsgSwEnvL5->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.r6 = pSsgSwEnvR6->load(std::memory_order_relaxed);
    params.wt.ssgSwEnv.l6 = pSsgSwEnvL6->load(std::memory_order_relaxed);

    params.wt.multiple = (int)pMultiple->load(std::memory_order_relaxed);
    params.wt.multipleRatio = pMultipleRatio->load(std::memory_order_relaxed);
    params.wt.detune = (int)pDetune->load(std::memory_order_relaxed);
    params.wt.detune2 = (int)pDetune2->load(std::memory_order_relaxed);
    params.wt.detune3 = (int)pDetune3->load(std::memory_order_relaxed);

    params.wt.lfoPmFreq = pLfoPmFreq->load(std::memory_order_relaxed);
    params.wt.lfoAmFreq = pLfoAmFreq->load(std::memory_order_relaxed);
    params.wt.lfoPmWave = (int)pLfoPmShape->load(std::memory_order_relaxed);
    params.wt.lfoAmWave = (int)pLfoAmShape->load(std::memory_order_relaxed);
    params.wt.lfoAmSmRt = pLfoAmSmoothRatio->load(std::memory_order_relaxed);
    params.wt.lfoAmEnable = (pLfoAm->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.lfoPmEnable = (pLfoPm->load(std::memory_order_relaxed) > WtPrValue::boolThread);
    params.wt.lfoPms = pLfoPms->load(std::memory_order_relaxed);
    params.wt.lfoAms = pLfoAms->load(std::memory_order_relaxed);
    params.wt.lfoPmd = pLfoPmd->load(std::memory_order_relaxed);
    params.wt.lfoAmd = pLfoAmd->load(std::memory_order_relaxed);
    params.wt.lfoSyncDelay = pLfoSyncDelay->load(std::memory_order_relaxed);
}
