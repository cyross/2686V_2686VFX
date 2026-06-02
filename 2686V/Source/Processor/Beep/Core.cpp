#include "./Core.h"

#include "./Keys.h"
#include "./Values.h"
#include "./Names.h"

void BeepProcessor::createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout)
{
    const juce::String code = BeepPrKey::prefix;

    layout.add(std::make_unique<juce::AudioParameterFloat>(code + BeepPrKey::level, code + BeepPrName::level, BeepPrValue::Level::min, BeepPrValue::Level::max, BeepPrValue::Level::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + BeepPrKey::fix, code + BeepPrName::fix, BeepPrValue::Fix::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + BeepPrKey::fixFreq, code + BeepPrName::fixFreq, BeepPrValue::FixFreq::min, BeepPrValue::FixFreq::max, BeepPrValue::FixFreq::initial));
    layout.add(std::make_unique<juce::AudioParameterBool>(code + BeepPrKey::adsr + BeepPrKey::bypass, code + BeepPrName::Adsr::bypass, BeepPrValue::Adsr::Bypass::initial));

    // PitchEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + BeepPrKey::pitchAdsr + BeepPrKey::bypass, code + BeepPrName::PitchAdsr::bypass, BeepPrValue::PitchAdsr::Bypass::initial));

    // SSG SwEnv Bypass Switch
    layout.add(std::make_unique<juce::AudioParameterBool>(code + BeepPrKey::ssgSwEnv + BeepPrKey::bypass, code + BeepPrName::SsgSwEnv::bypass, BeepPrValue::SsgSwEnv::Bypass::initial));

    // Detune
    layout.add(std::make_unique<juce::AudioParameterInt>(code + BeepPrKey::mul, code + BeepPrName::mul, BeepPrValue::Mul::min, BeepPrValue::Mul::max, BeepPrValue::Mul::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + BeepPrKey::mulRatio, code + BeepPrName::mulRatio, BeepPrValue::MulRatio::min, BeepPrValue::MulRatio::max, BeepPrValue::MulRatio::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + BeepPrKey::dt, code + BeepPrName::dt1, BeepPrValue::Dt1::min, BeepPrValue::Dt1::max, BeepPrValue::Dt1::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + BeepPrKey::dt2, code + BeepPrName::dt2, BeepPrValue::Dt2::min, BeepPrValue::Dt2::max, BeepPrValue::Dt2::initial));

    // ユニゾン・ハーモニー用
    layout.add(std::make_unique<juce::AudioParameterInt>(code + BeepPrKey::Unison::voices, code + BeepPrName::Unison::voices, BeepPrValue::Unison::Voices::min, BeepPrValue::Unison::Voices::max, BeepPrValue::Unison::Voices::initial));
    layout.add(std::make_unique<juce::AudioParameterInt>(code + BeepPrKey::Unison::detune, code + BeepPrName::Unison::detune, BeepPrValue::Unison::Detune::min, BeepPrValue::Unison::Detune::max, BeepPrValue::Unison::Detune::initial));
    layout.add(std::make_unique<juce::AudioParameterFloat>(code + BeepPrKey::Unison::spread, code + BeepPrName::Unison::spread, BeepPrValue::Unison::Spread::min, BeepPrValue::Unison::Spread::max, BeepPrValue::Unison::Spread::initial));

    addEnvParameters(layout, code);
    addPitchEnvParameters(layout, code);
    addSsgSwEnvParameters(layout, code);
}

void BeepProcessor::init(juce::AudioProcessorValueTreeState& apvts) {
    const juce::String code = BeepPrKey::prefix;
}

void BeepProcessor::processBlock(SynthParams& params, juce::AudioProcessorValueTreeState& apvts)
{
    const juce::String code = BeepPrKey::prefix;

    params.beep.level = *apvts.getRawParameterValue(code + BeepPrKey::level);

    // ユニゾン・ハーモニー用
    params.beep.unisonVoices = (int)*apvts.getRawParameterValue(code + BeepPrKey::Unison::voices);
    params.beep.unisonDetuneCents = (int)*apvts.getRawParameterValue(code + BeepPrKey::Unison::detune);
    params.beep.unisonSpread = *apvts.getRawParameterValue(code + BeepPrKey::Unison::spread);

    params.beep.fixedMode = (*apvts.getRawParameterValue(code + BeepPrKey::fix) > BeepPrValue::boolThread);
    params.beep.fixedFreq = *apvts.getRawParameterValue(code + BeepPrKey::fixFreq);
    params.beep.adsr.bypass = (*apvts.getRawParameterValue(code + BeepPrKey::adsr + BeepPrKey::bypass) > BeepPrValue::boolThread);
    params.beep.adsr.stl = *apvts.getRawParameterValue(code + BeepPrKey::Adsr::stl);
    params.beep.adsr.ar = *apvts.getRawParameterValue(code + BeepPrKey::Adsr::ar);
    params.beep.adsr.dr = *apvts.getRawParameterValue(code + BeepPrKey::Adsr::dr);
    params.beep.adsr.sl = *apvts.getRawParameterValue(code + BeepPrKey::Adsr::sl);
    params.beep.adsr.rr = *apvts.getRawParameterValue(code + BeepPrKey::Adsr::rr);

    params.beep.pitchAdsr.bypass = (*apvts.getRawParameterValue(code + BeepPrKey::pitchAdsr + BeepPrKey::bypass) > BeepPrValue::boolThread);
    params.beep.pitchAdsr.ar = *apvts.getRawParameterValue(code + BeepPrKey::PitchAdsr::ar);
    params.beep.pitchAdsr.dr = *apvts.getRawParameterValue(code + BeepPrKey::PitchAdsr::dr);
    params.beep.pitchAdsr.rr = *apvts.getRawParameterValue(code + BeepPrKey::PitchAdsr::rr);
    params.beep.pitchAdsr.stl = (int)*apvts.getRawParameterValue(code + BeepPrKey::PitchAdsr::stl);
    params.beep.pitchAdsr.atl = (int)*apvts.getRawParameterValue(code + BeepPrKey::PitchAdsr::atl);
    params.beep.pitchAdsr.ssl = (int)*apvts.getRawParameterValue(code + BeepPrKey::PitchAdsr::ssl);
    params.beep.pitchAdsr.rll = (int)*apvts.getRawParameterValue(code + BeepPrKey::PitchAdsr::rll);

    params.beep.ssgSwEnv.bypass = (*apvts.getRawParameterValue(code + BeepPrKey::ssgSwEnv + BeepPrKey::bypass) > BeepPrValue::boolThread);
    params.beep.ssgSwEnv.steps = (int)*apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::steps);
    params.beep.ssgSwEnv.loop = (*apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::loop) > BeepPrValue::boolThread);
    params.beep.ssgSwEnv.loopTo = (int)*apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::loopTo);
    params.beep.ssgSwEnv.loopCount = (int)*apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::loopCount);
    params.beep.ssgSwEnv.stl = *apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::stl);
    params.beep.ssgSwEnv.r1 = *apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::r1);
    params.beep.ssgSwEnv.l1 = *apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::l1);
    params.beep.ssgSwEnv.r2 = *apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::r2);
    params.beep.ssgSwEnv.l2 = *apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::l2);
    params.beep.ssgSwEnv.r3 = *apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::r3);
    params.beep.ssgSwEnv.l3 = *apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::l3);
    params.beep.ssgSwEnv.r4 = *apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::r4);
    params.beep.ssgSwEnv.l4 = *apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::l4);
    params.beep.ssgSwEnv.r5 = *apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::r5);
    params.beep.ssgSwEnv.l5 = *apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::l5);
    params.beep.ssgSwEnv.r6 = *apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::r6);
    params.beep.ssgSwEnv.l6 = *apvts.getRawParameterValue(code + BeepPrKey::SsgSwEnv::l6);

    params.beep.multiple = (int)*apvts.getRawParameterValue(code + BeepPrKey::mul);
    params.beep.multipleRatio = *apvts.getRawParameterValue(code + BeepPrKey::mulRatio);
    params.beep.detune = (int)*apvts.getRawParameterValue(code + BeepPrKey::dt);
    params.beep.detune2 = (int)*apvts.getRawParameterValue(code + BeepPrKey::dt2);
}
