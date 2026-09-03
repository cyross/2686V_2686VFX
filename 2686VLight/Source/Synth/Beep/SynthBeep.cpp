#include <JuceHeader.h>

#include "./SynthBeep.h"

// タイマの基準クロック(Hz)。実機はこれを整数で分周して矩形波を作る。
static inline double getBeepTimerClock(int index)
{
    switch (index) {
    case 2: return 1193182.0;  // IBM PC / PC-AT  (14.31818MHz / 12)
    case 3: return 1996800.0;  // PC-9801 8MHz系  (7.9872MHz / 4)
    case 4: return 2457600.0;  // PC-9801 5MHz系  (9.8304MHz / 4)
    }

    return 0.0; // 1: Free (分周しない)
}

// PolyBLEP (Polynomial Band-Limited Step)
// 不連続点の前後 1 サンプルだけ多項式で段差を補正し、
// 折り返しノイズを目立たなくする。
// t  : 不連続点からの正規化位相 (0.0〜1.0)
// dt : 1 サンプルあたりの位相の進み
static inline float polyBlep(float t, float dt)
{
    if (t < dt) {
        // 立ち上がり直後
        t /= dt;
        return t + t - t * t - 1.0f;
    }

    if (t > 1.0f - dt) {
        // 次の不連続点の直前
        t = (t - 1.0f) / dt;
        return t * t + t + t + 1.0f;
    }

    return 0.0f;
}

void BeepCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) m_sampleRate = sampleRate;

	m_adsr.prepare(44100.0);
    m_pitchAdsr.prepare(0, 44100.0);
    m_ssgSwEnv.prepare(0, 44100.0);
    m_ssgSwEnv11.prepare(0, 44100.0);
    m_ssgSwPenv11.prepare(0, 44100.0);
    m_fixMode.setParameters({ .enable = false, .freq = 2000.0f });
    m_lfo.prepare(44100.0);
    m_ssgHwEnv.prepare(44100.0);
    m_ssgHwPEnv.prepare(44100.0);
}

void BeepCore::setSampleRate(double sampleRate) {
	if (sampleRate > 0.0) m_sampleRate = sampleRate;

    m_adsr.updateSampleRate(sampleRate);
	m_pitchAdsr.updateSampleRate(sampleRate);
	m_ssgSwEnv.updateSampleRate(sampleRate);
    m_ssgSwEnv11.updateSampleRate(sampleRate);
    m_ssgSwPenv11.updateSampleRate(sampleRate);
    m_ssgHwEnv.updateSampleRate(m_sampleRate);
    m_ssgHwPEnv.updateSampleRate(m_sampleRate);
}

void BeepCore::setParameters(const SynthParams& params) {
    m_level = params.beep.level;

    // ユニゾン・ハーモニー用
    m_isMonoMode = params.monoMode;

    m_pitchResetOnLegato = params.pitchResetOnLegato;

    m_adsr.setParameters(params.beep.adsr);
    m_pitchAdsr.setParameters(params.beep.pitchAdsr);
    m_ssgSwEnv.setParameters(params.beep.ssgSwEnv);
    m_ssgSwEnv11.setParameters(params.beep.ssgSwEnv11);
    m_ssgSwPenv11.setParameters(params.beep.ssgSwPEnv11);
    m_detune.setParameters(params.beep.detune);
    m_fixMode.setParameters(params.beep.fix);
    m_lfo.setParameters(params.beep.lfo);
    m_ssgHwEnv.setParameters(params.beep.ssgHwEnv);
    m_ssgHwPEnv.setParameters(params.beep.ssgHwPEnv);
    m_wtMod.setParameters(params.beep.wtMod);
    m_wtAmpMod.setParameters(params.beep.wtAmpMod);
    m_antiAlias = params.beep.antiAlias;
    m_timerClock = getBeepTimerClock(params.beep.timerClock);
}

void BeepCore::noteOn(float freq, float velocity, int midiNote, bool isLegato) {
    // =====================================================================
    // モノフォニック・レガート時は、音量（ベロシティ）を更新しない！
    // 1音目の音量をそのまま引き継ぐことで、音量ジャンプを完全に防ぐ。
    // =====================================================================
    if (!isLegato) {
        // 新規発音の時だけ、ベロシティからベースレベルを計算する
        m_baseLevel = std::max(0.01f, velocity * 0.25f);
    }
    // (レガート時は m_baseLevel は古い値のまま維持される)

    // ユニゾン・ハーモニー用
    // ユニゾンデチューンの計算
    float finalFreq = m_unison.applyDetune(freq);

    float baseFreq = m_fixMode.noteOn(finalFreq);
    m_baseFreq = m_detune.noteOn(baseFreq);

    m_phaseDelta = m_baseFreq / (float)m_sampleRate;

    if (!isLegato) {
        if (!m_isMonoMode) {
            m_phase = m_unison.getPhaseOffset();

            // 安全のためのラップアラウンド (1.0基準)
            while (m_phase >= 1.0f) {
                m_phase -= 1.0f;
            }
        }

        m_currentLevel = m_adsr.noteOn();

        if (!m_pitchAdsr.isBypass() && !m_pitchResetOnLegato) {
            m_pitchAdsr.noteOn();
        }

        if (!m_ssgSwEnv.isBypass()) {
            m_ssgSwEnv.noteOn();
        }

        if (!m_ssgSwEnv11.isBypass()) {
            m_ssgSwEnv11.noteOn();
        }

        if (!m_ssgSwPenv11.isBypass()) {
            m_ssgSwPenv11.noteOn();
        }

        m_lfo.noteOn();
        m_ssgHwEnv.noteOn();
        m_ssgHwPEnv.noteOn();
    }

    if (!m_pitchAdsr.isBypass() && m_pitchResetOnLegato) {
        m_pitchAdsr.noteOn();
    }

    if (!m_ssgSwPenv11.isBypass() && m_pitchResetOnLegato) {
        m_ssgSwPenv11.noteOn();
    }
}

void BeepCore::noteOff() {
    m_adsr.noteOff();

    if (!m_pitchAdsr.isBypass()) {
        m_pitchAdsr.noteOff();
    }
    if (!m_ssgSwEnv.isBypass()) {
        m_ssgSwEnv.noteOff();
    }

    if (!m_ssgSwEnv11.isBypass()) {
        m_ssgSwEnv11.noteOff();
    }

    if (!m_ssgSwPenv11.isBypass()) {
        m_ssgSwPenv11.noteOff();
    }
}

bool BeepCore::isPlaying() const { return m_adsr.isPlaying() || m_ssgSwEnv.isPlaying() || m_ssgSwEnv11.isPlaying(); }

void BeepCore::setPitchBend(int pitchWheelValue) {
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;
    m_pitchBendRatio = std::pow(2.0f, (norm * 2.0f) / 12.0f);

    // 1.0基準の正しい計算
    m_phaseDelta = (m_baseFreq * m_pitchBendRatio) / (float)m_sampleRate;
}

float BeepCore::getSample() {
    // すべてのアンプエンベロープがバイパスされているかどうかを判定
    bool isAllAmpBypassed = m_adsr.isBypass() && m_ssgSwEnv.isBypass() && m_ssgSwEnv11.isBypass();

    if (isAllAmpBypassed) {
        // 全てのアンプエンベロープがバイパスの時は、完全な矩形波（Gate）動作
        // どれかが Release 状態（noteOffが呼ばれた直後）なら、即座に音を消す
        if (m_adsr.isRelease() || m_ssgSwEnv.isRelease() || m_ssgSwEnv11.isRelease()) {
            m_adsr.bypassedReleasedProcess();
            m_ssgSwEnv.bypassedReleasedProcess();
            m_ssgSwEnv11.bypassedReleasedProcess();
            m_pitchAdsr.bypassedReleasedProcess();
            m_ssgSwPenv11.bypassedReleasedProcess();
            return 0.0f;
        }
    }
    else if (!isPlaying()) {
        // いずれかのアンプエンベロープが有効で、全ての再生が終了（音が減衰しきった）時
        // ピッチエンベロープも強制終了させる（次のノートオンでピッチが変になるのを防ぐ）
        m_pitchAdsr.bypassedReleasedProcess();
        m_ssgSwPenv11.bypassedReleasedProcess();
        return 0.0f;
    }

    float finalEnv = 1.0f;

    if (!isAllAmpBypassed) {
        // 1. ADSR処理
        if (!m_adsr.isBypass()) {
            m_currentLevel = m_adsr.process(m_currentLevel);
            finalEnv *= m_currentLevel;
        }
        else {
            if (m_adsr.isRelease()) {
                m_adsr.bypassedReleasedProcess();
            }
            else {
                m_currentLevel = m_adsr.process(m_currentLevel);
                finalEnv *= m_currentLevel;
            }
        }

        // 2. SSGソフトウェアエンベロープ(SsgSwEnv)処理
        if (!m_ssgSwEnv.isBypass()) {
            finalEnv *= m_ssgSwEnv.process();
        }
        else {
            if (m_ssgSwEnv.isRelease()) m_ssgSwEnv.bypassedReleasedProcess();
        }

        // 3. SSG Sw Env 11 処理
        if (!m_ssgSwEnv11.isBypass()) {
            finalEnv *= m_ssgSwEnv11.process();
        }
        else {
            if (m_ssgSwEnv11.isRelease()) m_ssgSwEnv11.bypassedReleasedProcess();
        }
    }


    // SSGハードウェアエンベロープ(SsgHwEnv)処理
    float sshHwEnvVal = m_ssgHwEnv.process() * m_wtAmpMod.process(m_ampModDelta);

    float newPhaseDelta = m_pitchAdsr.process(m_phaseDelta);
    newPhaseDelta = m_ssgSwPenv11.process(newPhaseDelta);

    // ==========================================
    // Opzx7 LFO の計算 (AM / PM)
    // ==========================================
    m_lfo.getSample();

    // 1. Amplitude Modulation (AM / 音量)
    float amMultiplier = 1.0f;

    if (m_lfo.am.enable) {
        // depthDb はセットアップ時に計算済みなので、そのままdB減衰に変換
        float attenDb = m_lfo.value.am * m_lfo.am.depthDb;
        amMultiplier = std::pow(10.0f, -attenDb / 20.0f);
    }

    // 2. Pitch Modulation (PM / 音程)
    float pitchModCents = 0.0f;

    if (m_lfo.pm.enable) {
        // depthCent も計算済みなので、そのままセント値に変換
        pitchModCents += m_lfo.value.pm * m_lfo.pm.depthCent;
    }

    // セントを周波数倍率(レシオ)に変換
    float opzx7PitchMod = std::pow(2.0f, pitchModCents / 1200.0f);

    float modDepth = m_modWheel * 0.03f;
    float mwPitchMod = 1.0f + (m_lfo.value.pm * modDepth);

    // ==========================================
    // 周波数倍率の決定
    // (PitchBend × Opzx7のPM × ModWheelのPM)
    // ==========================================
    // MODULATION は搬送波の周波数比として掛ける
    m_ampModDelta = newPhaseDelta;
    float freqMult = m_pitchBendRatio * opzx7PitchMod * mwPitchMod * m_wtMod.process(newPhaseDelta) * m_ssgHwPEnv.process(1.0f);

    float phaseInc = 0.0f;

    phaseInc = newPhaseDelta * freqMult;

    // ==========================================
    // タイマの整数分周による音程のスナップ
    // ==========================================
    // 実機は 基準クロック / 整数 でしか周波数を作れないため、
    // 高音ほど出せる音程が飛び飛びになる。
    if (m_timerClock > 0.0 && phaseInc > 0.0f) {
        // phaseInc = 周波数 / サンプリングレート なので、いったん周波数に戻す
        double freq = (double)phaseInc * m_sampleRate;
        double divisor = std::floor(m_timerClock / freq + 0.5);

        // 分周値は 16bit カウンタの範囲に収める
        if (divisor < 1.0) divisor = 1.0;
        if (divisor > 65535.0) divisor = 65535.0;

        phaseInc = (float)((m_timerClock / divisor) / m_sampleRate);
    }

    // ==========================================
    // 波形生成 (50% Duty の矩形波)
    // ==========================================
    // 素の矩形波は帯域無制限なので、高音ではナイキストを超えた倍音が
    // 折り返してエイリアスノイズになる。PolyBLEP は不連続点の前後 1 サンプルを
    // 多項式で補正して段差をなまし、これを抑える。
    float output = (m_phase < 0.5f) ? 1.0f : -1.0f;

    if (m_antiAlias && phaseInc > 0.0f && phaseInc < 0.5f) {
        // 位相 0 の立ち上がりと、位相 0.5 の立ち下がりの 2 箇所を補正する
        output += polyBlep(m_phase, phaseInc);
        output -= polyBlep(std::fmod(m_phase + 0.5f, 1.0f), phaseInc);
    }

    m_phase += phaseInc;

    // ピッチエンベロープや PM で 1 サンプルの進みが 1.0 を超えても
    // 破綻しないよう while で回す
    while (m_phase >= 1.0f) m_phase -= 1.0f;
    while (m_phase < 0.0f) m_phase += 1.0f;

    // 音量に変換
    return output * finalEnv * m_baseLevel * m_level * amMultiplier * sshHwEnvVal;
}

// モジュレーションホイール (0 - 127)
void BeepCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;

    m_wtMod.setModWheel(m_modWheel);
}

void BeepCore::setPitchBendRatio(float ratio)
{
    m_pitchBendRatio = ratio;
}

void BeepCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    // ユニゾン・ハーモニー向けに変更
    float basePanL = 1.0f;
    float basePanR = 1.0f;

    m_unison.applyPan(basePanL, basePanR);
    sample *= m_unison.getGainComp();

    outL[startSample + sampleIdx] += sample * basePanL;
    outR[startSample + sampleIdx] += sample * basePanR;

    isActive = isPlaying();
}
