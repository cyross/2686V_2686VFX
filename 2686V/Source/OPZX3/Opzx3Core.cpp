#include "Opzx3Core.h"
#include "../synth/SynthHelpers.h"

void Opzx3Core::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_hostSampleRate = sampleRate;
    double target = getTargetRate(m_rateIndex);
    for (auto& op : m_operators) {
        op.setSampleRate(target);
        op.setHostSampleRate(m_hostSampleRate);
    }
    m_lfoPhase = 0.0;
    m_rateAccumulator = 1.0;

    updateNoiseDelta(target);

    m_amSmooth = 0.0f;
}

void Opzx3Core::setParameters(const SynthParams& params) {
    m_algorithm = params.algorithm; // Range: 0-27
    m_lfoFreq = params.lfoFreq;
    m_am = params.amEnable;
    m_pm = params.pmEnable;
    m_pms = params.lfoPms;
    m_ams = params.lfoAms;
    m_pmd = params.lfoPmd;
    m_amd = params.lfoAmd;
    m_lfoWave = params.lfoWave;
    m_amSmoothRate = params.lfoAmSmRt;

    if (m_rateIndex != params.fmRateIndex) {
        m_rateIndex = params.fmRateIndex;
        double target = getTargetRate(m_rateIndex);
        for (auto& op : m_operators) op.setSampleRate(target);
        updateNoiseDelta(target);
    }

    m_quantizeSteps = getTargetBitDepth(params.fmBitDepth);

    for (int i = 0; i < 4; ++i) {
        float fb = 0.0f;

        if (i == 0) // OP0
        {
            fb = params.feedback;
        }

        if (i == 2) // OP2
        {
            fb = params.feedback2;
        }

        // WaveSelect=True, SSG-EG=True, OpmEg=True
        m_operators[i].setParameters(params.fmOp[i], fb, true, true, true, params.fmOp[i].fmSsgEgFreq);
        m_opMask[i] = params.fmOp[i].mask;
    }

    // OPX特有の外部フィードバックアルゴリズムの場合、OP0/OP2の自己FBをオフにする
    bool useExtFb = false;
    if (m_algorithm == 1 || m_algorithm == 5 || m_algorithm == 7 || m_algorithm == 11) useExtFb = true; // 4OP: OP1->OP0 FB
    if (m_algorithm == 17 || m_algorithm == 21) useExtFb = true; // 3OP: OP2->OP0 FB
    if (m_algorithm == 25) useExtFb = true; // 2OP: OP1->OP0 FB

    m_operators[0].setExternalFeedbackMode(useExtFb);
    m_operators[2].setExternalFeedbackMode(useExtFb);
}

void Opzx3Core::noteOn(float freq, float velocity) {
    int noteNum = (int)(69.0 + 12.0 * std::log2(freq / 440.0));
    for (auto& op : m_operators) op.noteOn(freq, velocity, noteNum);
    m_rateAccumulator = 1.0;
}

void Opzx3Core::noteOff()
{
    for (auto& op : m_operators) op.noteOff();
}

bool Opzx3Core::isPlaying() const
{
    for (const auto& op : m_operators) {
        if (op.isPlaying()) return true;
    }
    return false;
}

void Opzx3Core::setPitchBend(int pitchWheelValue)
{
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;
    float semitones = 2.0f;
    float ratio = std::pow(2.0f, (norm * semitones) / 12.0f);
    for (auto& op : m_operators) op.setPitchBendRatio(ratio);
}

void Opzx3Core::setModulationWheel(int wheelValue)
{
    m_modWheel = (float)wheelValue / 127.0f;
}

float Opzx3Core::getSample() {
    double targetRate = getTargetRate(m_rateIndex);

    double stepSize = targetRate / m_hostSampleRate;
    m_rateAccumulator += stepSize;

    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        m_prevSample = m_lastSample;

        // --- LFO Processing ---
        double lfoInc = m_lfoFreq / targetRate;
        m_lfoPhase += lfoInc;

        if (m_lfoPhase >= 1.0) {
            m_lfoPhase -= 1.0;
            unsigned int bit0 = m_lfsr & 1;
            unsigned int bit3 = (m_lfsr >> 3) & 1;
            unsigned int nextBit = bit0 ^ bit3;
            m_lfsr >>= 1;
            if (nextBit) m_lfsr |= (1 << 16);
            m_currentNoiseSample = ((m_lfsr % 1000) / 500.0f) - 1.0f; // -1.0 ~ 1.0
        }

        float amLfoVal = 0.0f;
        float pmLfoVal = 0.0f;

        switch (m_lfoWave) {
        case 0: // Sine (従来用)
            pmLfoVal = (float)std::sin(m_lfoPhase * 2.0 * juce::MathConstants<double>::pi);
            amLfoVal = (pmLfoVal + 1.0f) * 0.5f;
            break;
        case 1: // Saw Down (実機のノコギリ波)
            pmLfoVal = (float)(1.0 - m_lfoPhase * 2.0); // 1.0 -> -1.0
            amLfoVal = (float)(1.0 - m_lfoPhase);       // 1.0 -> 0.0
            break;
        case 2: // Square
            pmLfoVal = (m_lfoPhase < 0.5) ? 1.0f : -1.0f;
            amLfoVal = (m_lfoPhase < 0.5) ? 1.0f : 0.0f;
            break;
        case 3: // Triangle
            // PM: 0 -> 1 -> -1 -> 0
            if (m_lfoPhase < 0.25)       pmLfoVal = (float)(m_lfoPhase * 4.0);
            else if (m_lfoPhase < 0.75)  pmLfoVal = (float)(1.0 - (m_lfoPhase - 0.25) * 4.0);
            else                         pmLfoVal = (float)(-1.0 + (m_lfoPhase - 0.75) * 4.0);

            // AM: 0 -> 1 -> 0 (PMの半分の速度で折り返す)
            if (m_lfoPhase < 0.5)        amLfoVal = (float)(m_lfoPhase * 2.0);
            else                         amLfoVal = (float)(1.0 - (m_lfoPhase - 0.5) * 2.0);
            break;
        case 4: // Noise (Sample & Hold)
            pmLfoVal = m_currentNoiseSample;
            amLfoVal = (m_currentNoiseSample + 1.0f) * 0.5f;
            break;
        }

        m_amSmooth += (amLfoVal - m_amSmooth) * m_amSmoothRate;

        // Outputs
        float out1 = 0.0f, out2 = 0.0f, out3 = 0.0f, out4 = 0.0f;
        float finalOut = 0.0f;

        // Op1 is Feedback Source
        m_operators[0].getSample(out1, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

        if (m_opMask[0]) out1 = 0.0f;

        // =================================================================
        // Algorithm : OPX (0-27) + MA-3 (28-35)
        // =================================================================
        switch (m_algorithm) {
        case 0:
            //	{{"([M:FB->2])", "([M->3])", "([M->4])", "([C])"}},       // 00: <OPX-00>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out4;

            break;
        case 1:
            //	{{"([M->2])", "([M:FB1->3])", "([M->4])", "([C])"}},      // 01: <OPX-01>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[0].pushFeedback(out2);

            m_operators[2].getSample(out3, out2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out4;

            break;
        case 2:
            //	{{"([M:FB->3])", "([M->3])", "([M->4])", "([C])"}},       // 02: <OPX-02>
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out1 + out2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out4;

            break;
        case 3:
            //	{{"([M:FB->4])", "([M->3])", "([M->4])", "([C])"}},       // 03: <OPX-03>
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out1 + out3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out4;

            break;
        case 4:
            //	{{"([M:FB->2])", "([M->4])", "([M->4])", "([C])"}},       // 04: <OPX-04>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out2 + out3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out4;

            break;
        case 5:
            //	{{"([M:FB->2])", "([M:FB1->4])", "([M->4])", "([C])"}},   // 05: <OPX-05>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[0].pushFeedback(out2);

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out2 + out3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out4;

            break;
        case 6:
            //	{{"([M:FB->2])", "([C])", "([M->4])", "([C])"}},          // 06: <OPX-06>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out2 + out4;

            break;
        case 7:
            //	{{"([M->2])", "([C:FB1])", "([M->4])", "([C])"}},         // 07: <OPX-07>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[0].pushFeedback(out2);

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out2 + out4;

            break;
        case 8:
            //	{{"([C:FB])", "([M->3])", "([M->4])", "([C])"}},          // 08: <OPX-08>
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out1 + out4;

            break;
        case 9:
            //	{{"([C:FB])", "([M->4])", "([M->4])", "([C])"}},          // 09: <OPX-09>
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out2 + out3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out1 + out4;

            break;
        case 10:
            //	{{"([M:FB->2])", "([C])", "([C])", "([C])"}},             // 10: <OPX-10>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out2 + out3 + out4;

            break;
        case 11:
            //	{{"([M->2])", "([C:FB1])", "([C])", "([C])"}},            // 11: <OPX-11>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[0].pushFeedback(out2);

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out2 + out3 + out4;

            break;
        case 12:
            //	{{"([M:FB->2/3/4])", "([C])", "([C])", "([C])"}},         // 12: <OPX-12>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out2 + out3 + out4;

            break;
        case 13:
            //	{{"([C:FB])", "([M->3])", "([C])", "([C])"}},             // 13: <OPX-13>
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out1 + out3 + out4;

            break;
        case 14:
            //	{{"([C:FB][M:FB->2])", "([C])", "([M->4])", "([C])"}},    // 14: <OPX-14>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out1 + out2 + out4;
            break;
        case 15:
            //	{{"([C:FB])", "([C])", "([C])", "([C])"}},                // 15: <OPX-15>
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out1 + out2 + out3 + out4;

            break;
        case 16:
            //	{{"([M:FB->2])", "([M->3])", "([C])", "(--)"}},           // 16: <OPX-16>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            // OP3を空回し
            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal);

            finalOut = out3;

            break;
        case 17:
            //	{{"([M->2])", "([M:FB1->3])", "([C])", "(--)"}},          // 17: <OPX-17>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[0].pushFeedback(out2);

            m_operators[2].getSample(out3, out2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            // OP3を空回し
            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal);

            finalOut = out3;

            break;
        case 18:
            //	{{"([M:FB->3])", "([M->3])", "([C])", "(--)"}},           // 18: <OPX-18>
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out1 + out2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            // OP3を空回し
            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal);

            finalOut = out3;

            break;
        case 19:
            //	{{"([C:FB])", "([M->3])", "([C])", "(--)"}},              // 19: <OPX-19>
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            // OP3を空回し
            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal);

            finalOut = out1 + out2;

            break;
        case 20:
            //	{{"([M:FB->2])", "([C])", "([C])", "(--)"}},              // 20: <OPX-20>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            // OP3を空回し
            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal);

            finalOut = out2 + out3;

            break;
        case 21:
            //	{{"([M->2])", "([C:FB1])", "([C])", "(--)"}},             // 21: <OPX-21>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[0].pushFeedback(out2);

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            // OP3を空回し
            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal);

            finalOut = out2 + out3;

            break;
        case 22:
            //	{{"([C:FB])", "([C])", "([C])", "(--)"}},                 // 22: <OPX-22>
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            finalOut = out1 + out2 + out3;

            // OP3を空回し
            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal);

            break;
        case 23:
            //	{{"([C:FB][M:FB->2])", "([C])", "([C])", "(--)"}},        // 23: <OPX-23>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            finalOut = out1 + out2 + out3;

            // OP3を空回し
            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal);

            break;
        case 24:
            //	{{"([M:FB->2])", "([C])", "(--)", "(--)"}},               // 24: <OPX-24>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal);
            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal);

            finalOut = out2;

            break;
        case 25:
            //	{{"([M->2])", "([C:FB1])", "(--)", "(--)"}},              // 25: <OPX-25>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[0].pushFeedback(out2);

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal);
            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal);

            finalOut = out2;

            break;
        case 26:
            //	{{"([C:FB])", "([C])", "([--])", "([--])"}},              // 26: <OPX-26>
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal);
            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal);

            finalOut = out1 + out2;

            break;
        case 27:
            //	{{"([C:FB][M->2])", "([C])", "([--])", "([--])"}},        // 27: <OPX-27>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal);
            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal);

            finalOut = out1 + out2;

            break;
        case 28:
            //	{{"([M:FB->2])", "([C])", "([--])", "([--])"}},           // 28: <MA3-00>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal);
            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal);

            finalOut = out2;

            break;
        case 29:
            //	{{"([C:FB])", "([C])", "([--])", "([--])"}},              // 29: <MA3-01>
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal);
            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal);

            finalOut = out1 + out2;

            break;
        case 30:
            //	{{"([C:FB])", "([C])", "([C:FB])", "([C])"}},             // 30: <MA3-02>
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out1 + out2 + out3 + out4;

            break;
        case 31:
            //	{{"([M:FB->4])", "([M->3])", "([M->4])", "([C])"}}, // 31: <MA3-03>
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[0].pushFeedback(out2);

            m_operators[2].getSample(out3, out2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out1 + out3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out4;

            break;
        case 32:
            //	{{"([M:FB->2])", "([M->3])", "([M->4])", "([C])"}},       // 32: <MA3-04>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out4;

            break;
        case 33:
            //	{{"([M:FB->2])", "([C])", "([M:FB->4])", "([C])"}},       // 33: <MA3-05>
            m_operators[1].getSample(out2, out1, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out2 + out4;

            break;
        case 34:
            //	{{"([C:FB])", "([M->3])", "([M->4])", "([C])"}},          // 34: <MA3-06>
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[2].getSample(out3, out2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, out3, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out1 + out4;

            break;
        case 35:
            //	{{"([C:FB])", "([M->3])", "([C])", "([C])"}}              // 35: <MA3-07>
            m_operators[1].getSample(out2, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[1]) out2 = 0.0f;

            m_operators[0].pushFeedback(out2);

            m_operators[2].getSample(out3, out2, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[2]) out3 = 0.0f;

            m_operators[3].getSample(out4, 0.0f, m_amSmooth, pmLfoVal, m_pm, m_am, m_pms, m_ams, (float)m_pmd, (float)m_amd, m_modWheel);

            if (m_opMask[3]) out4 = 0.0f;

            finalOut = out1 + out3 + out4;

            break;
        default:
            finalOut = 0.0f;

            break;
        }

        // =======================================================
        // 複数のキャリアが加算されても1.0を超えないように音量を調整
        // =======================================================
        finalOut *= 0.25f;

        // =======================================================
        // 無音(0.0)が完全に0.0になるBipolar(双極性)量子化
        // =======================================================
        if (m_quantizeSteps > 0.0f) {
            // 単純に掛け算して丸め、割り算で戻すだけでゼロクロスが保証されます
            finalOut = std::round(finalOut * m_quantizeSteps) / m_quantizeSteps;

            // 安全のためのクリップ
            finalOut = std::clamp(finalOut, -1.0f, 1.0f);
        }

        m_lastSample = finalOut;
    }

    float fraction = (float)(m_rateAccumulator / stepSize);
    if (fraction > 1.0f) fraction = 1.0f;

    return m_prevSample + (m_lastSample - m_prevSample) * fraction;
}

void Opzx3Core::updateNoiseDelta(double targetRate) {
    if (targetRate > 0.0) m_noiseDelta = m_targetNoiseFreq / targetRate;
}

void Opzx3Core::setPcmBuffer(int opIndex, const std::vector<float>* pcmData)
{
    if (opIndex >= 0 && opIndex < 4) {
        m_operators[opIndex].setPcmBuffer(pcmData);
    }
}
