#include <JuceHeader.h>

#include "./Core.h"

void BeepCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_sampleRate = sampleRate;

	m_adsr.prepare(44100.0);
    m_pitchAdsr.prepare(0, 44100.0);
    m_ssgSwEnv.prepare(0, 44100.0);
    m_fixMode.setParameters(false, 2000.0f);
}

void BeepCore::setCurveCore(CurveCore* p_curveCore)
{
    m_adsr.setCurveCore(p_curveCore);
    m_pitchAdsr.setCurveCore(p_curveCore);
    m_ssgSwEnv.setCurveCore(p_curveCore);
}

void BeepCore::setSampleRate(double sampleRate) {
	if (sampleRate > 0.0) m_sampleRate = sampleRate;

    m_adsr.updateSampleRate(sampleRate);
	m_pitchAdsr.updateSampleRate(sampleRate);
	m_ssgSwEnv.updateSampleRate(sampleRate);
}

void BeepCore::setParameters(const SynthParams& params) {
    m_level = params.beep.level;

    // ユニゾン・ハーモニー用
    m_isMonoMode = params.monoMode;

    m_adsr.setParameters(params.beep.adsr);
    m_pitchAdsr.setParameters(params.beep.pitchAdsr);
    m_ssgSwEnv.setParameters(params.beep.ssgSwEnv);
    m_detune.setParameters(params.beep.detune, params.beep.detune2, params.beep.multiple, params.beep.mutipleRatio);

	m_fixMode.setParameters(params.beep.fixedMode, params.beep.fixedFreq);
}

void BeepCore::noteOn(float freq, float velocity, int midiNote) {
    // ユニゾン・ハーモニー用
    // ユニゾンデチューンの計算
    float finalFreq = freq;

    // ユニゾン時の位相のズレ(0.0〜1.0)を算出
    m_unisonPhaseOffset = 0.0f;

    if (m_unisonTotal > 1) {
        // 現在のボイスが全体のどこに配置されるかを -1.0(一番下) 〜 1.0(一番上) で算出
        // 例(3ボイス): -1.0,  0.0,  1.0
        // 例(4ボイス): -1.0, -0.33, 0.33, 1.0
        float spreadPos = ((float)m_unisonIndex / (float)(m_unisonTotal - 1)) * 2.0f - 1.0f;

        // 最大デチューン幅に位置を掛け合わせて、このボイスのズレ量(セント)を決定
        float centOffset = spreadPos * (float)m_unisonDetuneAmt;

        // セント値(Cents) を周波数倍率(Ratio)に変換する
        // (1200セント ＝ 1オクターブ ＝ 周波数2倍)
        finalFreq = freq * std::pow(2.0f, centOffset / 1200.0f);

        // ボイスインデックスに応じて位相を均等に散らす (例: 3ボイスなら 0.0, 0.33, 0.66)
        m_unisonPhaseOffset = (float)m_unisonIndex / (float)m_unisonTotal;
    }

    float baseFreq = m_fixMode.noteOn(finalFreq);
    m_baseFreq = m_detune.noteOn(baseFreq);

    m_phase = (m_unisonPhaseOffset * juce::MathConstants<float>::twoPi);

    // 位相が 2π を超えた場合は安全にラップアラウンド（折り返し）させる
    while (m_phase >= juce::MathConstants<float>::twoPi) {
        m_phase -= juce::MathConstants<float>::twoPi;
    }

    m_phaseDelta = m_baseFreq / (float)m_sampleRate;

    m_baseLevel = std::max(0.01f, velocity * 0.25f);
    m_currentLevel = m_adsr.noteOn();
    m_pitchAdsr.noteOn();
	m_ssgSwEnv.noteOn();
}

void BeepCore::noteOff() {
    m_adsr.noteOff();
    m_pitchAdsr.noteOff();
	m_ssgSwEnv.noteOff();
}

bool BeepCore::isPlaying() const { return m_adsr.isPlaying() || m_ssgSwEnv.isPlaying(); }

void BeepCore::setPitchBend(int pitchWheelValue) {
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;
    m_pitchBendRatio = std::pow(2.0f, (norm * 2.0f) / 12.0f);
    m_phaseDelta = (m_baseFreq * m_pitchBendRatio) / (float)m_sampleRate;
}

float BeepCore::getSample() {
    if (!isPlaying()) {
        // ADSRとSwEnvの両方がバイパスの時は、完全な矩形波（Gate）動作
        // ピッチエンベロープは強制的に終了させる（そうしないと、次のノートオンでピッチが変になったりする）
        m_pitchAdsr.bypassedReleasedProcess();

        return 0.0f;
    }

    float finalEnv = 1.0f;

    // --- ADSR & SwEnv Gate Logic ---
    // 1. 従来のADSR処理 (内部の m_currentLevel はADSR専用として維持する)
    if (!m_adsr.isBypassed()) {
        m_currentLevel = m_adsr.process(m_currentLevel);
        finalEnv *= m_currentLevel; // 掛け算
    }
    else {
        if (m_adsr.isRelease()) m_adsr.bypassedReleasedProcess();
        else {
            m_currentLevel = m_adsr.process(m_currentLevel);
            finalEnv *= m_currentLevel; // 掛け算
        }
    }

    // 2. SSGソフトウェアエンベロープ(SsgSwEnv)処理
    if (!m_ssgSwEnv.isBypassed()) {
        finalEnv *= m_ssgSwEnv.process(); // 掛け算
    }
    else {
        if (m_ssgSwEnv.isRelease()) m_ssgSwEnv.bypassedReleasedProcess();
    }

    // 究極にシンプルな1-bit矩形波（50% Duty）
    float output = (m_phase < 0.5f) ? 1.0f : -1.0f;

    float newPhaseDelta = m_pitchAdsr.process(m_phaseDelta);

    m_phase += newPhaseDelta;
    if (m_phase >= 1.0f) m_phase -= 1.0f;

    // 音量に変換
    return output * finalEnv * m_baseLevel * m_level;
}

// モジュレーションホイール (0 - 127)
void BeepCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;
}

void BeepCore::setPitchBendRatio(float ratio)
{
    m_pitchBendRatio = ratio;
}

void BeepCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    // ユニゾン・ハーモニー向けに変更
    float basePanL = 0.5f;
    float basePanR = 0.5f;

    if (m_unisonTotal > 1) {
        float spreadPos = ((float)m_unisonIndex / (float)(m_unisonTotal - 1)) * 2.0f - 1.0f; // -1.0 to 1.0

        // spreadPosが -1(L) の時、Right側の音量を下げる。逆も然り。
        float panOffset = spreadPos * m_unisonSpreadAmt * 0.5f; // 最大で ±0.5 動く

        basePanL = std::clamp(basePanL - panOffset, 0.0f, 1.0f);
        basePanR = std::clamp(basePanR + panOffset, 0.0f, 1.0f);

        // 音量補正 (ボイス数が増えると爆音になるため下げる)
        // ルートを取るか、単純に割るかは好みですが、単純割りの方が安全です
        float gainComp = 1.0f / std::sqrt((float)m_unisonTotal);
        sample *= gainComp;
    }

    outL[startSample + sampleIdx] += sample * basePanL;
    outR[startSample + sampleIdx] += sample * basePanR;

    isActive = isPlaying();
}
