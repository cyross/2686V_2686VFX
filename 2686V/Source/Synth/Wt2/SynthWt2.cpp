#include "./SynthWt2.h"

#include "../../Core/Synth/SynthHelpers.h"

Wt2Core::Wt2Core() : SynthCore()
{
    // 初期波形: サイン波
    m_sourceWave.resize(256);
    m_tableSizes = { 32, 64, 128, 256 };
    m_customWaves = { m_customWaveCache32.data(), m_customWaveCache64.data(), m_customWaveCache128.data(), m_customWaveCache256.data() };
    generateWaveform(0); // Default Sine
}

void Wt2Core::setCurveCore(CurveCore* p_curveCore)
{
    m_adsr.setCurveCore(p_curveCore);
    m_pitchAdsr.setCurveCore(p_curveCore);
    m_ssgSwEnv.setCurveCore(p_curveCore);
}

void Wt2Core::prepare(double sampleRate)
{
    if (sampleRate > 0.0) m_sampleRate = sampleRate;

    m_adsr.prepare(m_sampleRate);
    m_pitchAdsr.prepare(0, m_sampleRate);
    m_ssgSwEnv.prepare(0, m_sampleRate);
    m_targetRate = getTargetRate(m_rateIndex);

    m_lfo.prepare(m_targetRate);

    updatePhaseDelta();
}

void Wt2Core::setSampleRate(double sampleRate)
{
	if (sampleRate > 0.0) m_sampleRate = sampleRate;

    m_adsr.updateSampleRate(m_sampleRate);
	m_pitchAdsr.updateSampleRate(m_sampleRate);
	m_ssgSwEnv.updateSampleRate(m_sampleRate);

    updatePhaseDelta();
}

void Wt2Core::setParameters(const SynthParams& params)
{
    m_level = params.wt2.level;

    m_fixMode.setParameters(params.wt2.fixedMode, params.wt2.fixedFreq);

    // ユニゾン・ハーモニー用
    m_isMonoMode = params.monoMode;

    m_adsr.setParameters(params.wt2.adsr);
    m_pitchAdsr.setParameters(params.wt2.pitchAdsr);
	m_ssgSwEnv.setParameters(params.wt2.ssgSwEnv);
	m_detune.setParameters(params.wt2.detune, params.wt2.detune2, params.wt2.detune3, params.wt2.multiple, params.wt2.multipleRatio);
    m_lfo.setParameters(
        params.wt2.lfoPmSyncDelay,
        params.wt2.lfoAmSyncDelay,
        params.wt2.lfoPmEnable,
        params.wt2.lfoAmEnable,
        params.wt2.lfoPmFreq,
        params.wt2.lfoAmFreq,
        params.wt2.lfoPmWave,
        params.wt2.lfoAmWave,
        params.wt2.lfoPms,
        params.wt2.lfoPmd,
        params.wt2.lfoAms,
        params.wt2.lfoAmd,
        params.wt2.lfoAmSmRt
    );

    // Bit Depth & Table Size
    m_quantizeSteps = getTargetBitDepth(params.wt2.bitDepth);

    if (m_rateIndex != params.wt2.rateIndex) {
        m_rateIndex = params.wt2.rateIndex;

        m_targetRate = getTargetRate(m_rateIndex);

        m_lfo.updateTargetSampleRate(m_targetRate);
    }

    // 波形・テーブルサイズ変更検知
    int newTableSize = m_tableSizes[params.wt2.tableSize];
    bool waveformChanged = (m_waveform != params.wt2.waveform);
    bool sizeChanged = (m_prevTableSize != newTableSize);
    bool resChanged = (m_customWaveResolution != params.wt2.customWaveResolution);

    m_tableSizeIndex = params.wt2.tableSize;
    m_tableSize = newTableSize;
    m_prevTableSize = newTableSize;
    m_waveform = params.wt2.waveform;
    m_customWaveResolution = params.wt2.customWaveResolution;

    // 波形変更検知
    // カスタム波形データのコピー (値が変わっていれば更新するため)
    if (params.wt2.waveform == 8) // 8 = Custom
    {
        // int配列をメンバに保存
        m_customWaveCache32 = params.wt2.customWave32;
        m_customWaveCache64 = params.wt2.customWave64;
        m_customWaveCache128 = params.wt2.customWave128;
        m_customWaveCache256 = params.wt2.customWave256;
        // 強制的に再生成
        generateWaveform(8);
    }
    else if (waveformChanged || sizeChanged)
    {
        m_waveform = params.wt2.waveform;

        generateWaveform(m_waveform);
    }

    m_modEnable = params.wt2.modEnable;
    m_modDepth = params.wt2.modDepth;
    m_modSpeed = params.wt2.modSpeed;

    m_pitchResetOnLegato = params.pitchResetOnLegato;

    updatePhaseDelta();
}

void Wt2Core::noteOn(float freq, float velocity, int midiNote, bool isLegato)
{
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
 
    // 基本周波数にデチューン成分を加算
    // Save for recalculation
    finalFreq = m_fixMode.noteOn(finalFreq);
    m_currentFrequency = m_detune.noteOn(finalFreq);

    if (!isLegato) {
        if (!m_isMonoMode) {
            m_phase = (m_unisonPhaseOffset * juce::MathConstants<float>::twoPi);

            // 位相が 2π を超えた場合は安全にラップアラウンド（折り返し）させる
            while (m_phase >= juce::MathConstants<float>::twoPi) {
                m_phase -= juce::MathConstants<float>::twoPi;
            }
            m_lfo.noteOn();
        }

        m_modPhase = 0.0f;
        m_rateAccumulator = 0.0;
        m_lastSample = 0.0f;
    }

    updatePhaseDelta();

    if (!isLegato) {
        m_currentLevel = m_adsr.noteOn();

        if (!m_pitchAdsr.isBypass() && !m_pitchResetOnLegato) {
            m_pitchAdsr.noteOn();
        }

        if (!m_ssgSwEnv.isBypass()) {
            m_ssgSwEnv.noteOn();
        }
    }

    if (!m_pitchAdsr.isBypass() && m_pitchResetOnLegato) {
        m_pitchAdsr.noteOn();
    }
}

void Wt2Core::noteOff()
{
    m_adsr.noteOff();

    if (!m_pitchAdsr.isBypass()) {
        m_pitchAdsr.noteOff();
    }
    if (!m_ssgSwEnv.isBypass()) {
        m_ssgSwEnv.noteOff();
    }
}

bool Wt2Core::isPlaying() const
{
    return m_adsr.isPlaying() || m_ssgSwEnv.isPlaying();
}

// ピッチベンド (0 - 16383, Center=8192)
void Wt2Core::setPitchBend(int pitchWheelValue)
{
    // 範囲を -1.0 ～ 1.0 に正規化
    float norm = (float)(pitchWheelValue - 8192) / 8192.0f;

    // 半音単位のレンジ (例: +/- 2半音)
    float semitones = 2.0f;

    // 比率計算: 2^(semitones / 12)
    // norm * semitones で変化量を決定
    float ratio = std::pow(2.0f, (norm * semitones) / 12.0f);

    setPitchBendRatio(ratio);
}

// モジュレーションホイール (0 - 127)
void Wt2Core::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;
}

void Wt2Core::setPitchBendRatio(float ratio)
{
    m_pitchBendRatio = ratio;
}

float Wt2Core::getSample()
{
    if (!isPlaying() && !m_adsr.isBypass()) {
        if (m_pitchAdsr.isBypass()) {
            m_pitchAdsr.bypassedReleasedProcess();
        }

        if (m_ssgSwEnv.isBypass()) {
            m_ssgSwEnv.bypassedReleasedProcess();
        }

        return 0.0f;
    }

    float finalEnv = 1.0f;

    // --- ADSR & SwEnv Gate Logic ---
    // 1. 従来のADSR処理 (内部の m_currentLevel はADSR専用として維持する)
    if (!m_adsr.isBypass()) {
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
    if (!m_ssgSwEnv.isBypass()) {
        finalEnv *= m_ssgSwEnv.process(); // 掛け算
    }
    else {
        if (m_ssgSwEnv.isRelease()) m_ssgSwEnv.bypassedReleasedProcess();
    }

    float newPhaseDelta = m_pitchAdsr.process(m_phaseDelta);

    // --- Sample Rate Emulation ---
    double targetRate = getTargetRate(m_rateIndex);
    double step = targetRate / m_sampleRate;
    m_rateAccumulator += step;

    // --- Wavetable Synthesis ---
    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        // --- Synthesis at Target Rate ---

        m_lfo.getSample();

        // ==========================================
        // Opzx7 LFO の計算 (AM / PM)
        // ==========================================

        // 1. Amplitude Modulation (AM / 音量)
        float amMultiplier = 1.0f;
        if (m_lfo.am.enable) {
            float attenDb = m_lfo.value.am * m_lfo.am.depthDb;
            amMultiplier = std::pow(10.0f, -attenDb / 20.0f);
        }

        // 2. Pitch Modulation (PM / 音程)
        float pitchModCents = 0.0f;
        if (m_lfo.pm.enable) {
            pitchModCents += m_lfo.value.pm * m_lfo.pm.depthCent;
        }
        float opzx7PitchMod = std::pow(2.0f, pitchModCents / 1200.0f);

        // ==========================================
        // Wavetable固有の Modulation (Vibrato / Table Scan)
        // ==========================================
        float modLfoVal = std::sin(m_modPhase * 2.0 * juce::MathConstants<float>::pi);

        float totalModDepth = m_modDepth + (m_modWheel * 0.1f);

        float modOffset = 0.0f;
        if (m_modEnable || m_modWheel > 0.0f) // Apply if enable OR wheel is up
        {
            modOffset = modLfoVal * totalModDepth;

            m_modPhase += (newPhaseDelta * m_modSpeed);
            if (m_modPhase >= 1.0f) m_modPhase -= 1.0f;
        }

        // ==========================================
        // 位相 (Phase) の計算
        // ==========================================
        // PitchBend に加えて Opzx7 の PM 倍率も掛け合わせる
        float currentDelta = newPhaseDelta * m_pitchBendRatio * opzx7PitchMod;

        float effectivePhase = m_phase + modOffset;
        effectivePhase -= std::floor(effectivePhase);
        if (effectivePhase < 0.0f) effectivePhase += 1.0f;

        // ==========================================
        // 波形テーブルのルックアップ (線形補間でノイズ除去)
        // ==========================================
        float readIndex = effectivePhase * (float)m_tableSize;
        int idx1 = (int)readIndex;
        int idx2 = idx1 + 1;

        if (idx1 >= m_tableSize) idx1 = m_tableSize - 1;
        if (idx2 >= m_tableSize) idx2 = 0; // ループして繋ぐ

        float frac = readIndex - (float)idx1;

        int sourceIdx1 = idx1 * (256 / m_tableSize);
        int sourceIdx2 = idx2 * (256 / m_tableSize);

        if (sourceIdx1 >= 256) sourceIdx1 = 255;
        if (sourceIdx2 >= 256) sourceIdx2 = 255;

        float raw1 = m_sourceWave[sourceIdx1];
        float raw2 = m_sourceWave[sourceIdx2];

        // 2つのサンプルの間を滑らかに補間する
        float rawSample = raw1 * (1.0f - frac) + raw2 * frac;

        // 量子化(ビットクラッシャー)の前に AM（トレモロ）を適用する
        rawSample *= amMultiplier;

        // ==========================================
        // 量子化 (UIの正確なスナップロジックと完全同期)
        // ==========================================
        if (m_quantizeSteps > 0.0f) {
            // m_quantizeSteps が 15 (4-bit) の場合、16段階になる
            int totalSteps = (int)m_quantizeSteps + 1;
            int maxIndex = totalSteps - 1;
            int zeroIndex = totalSteps / 2 - 1;

            int stepValue = 0;

            // UI側と全く同じ区画計算を行う
            if (rawSample < 0.0f) {
                stepValue = (int)std::round(rawSample * zeroIndex + zeroIndex);
            }
            else {
                stepValue = (int)std::round(rawSample * (maxIndex - zeroIndex) + zeroIndex);
            }

            stepValue = std::clamp(stepValue, 0, maxIndex);

            // 正確な値 (-1.0 〜 1.0) に戻す
            if (stepValue < zeroIndex) {
                m_lastSample = (float)(stepValue - zeroIndex) / (float)zeroIndex;
            }
            else if (stepValue > zeroIndex) {
                m_lastSample = (float)(stepValue - zeroIndex) / (float)(maxIndex - zeroIndex);
            }
            else {
                m_lastSample = 0.0f; // 完全な 0.0 を保証！（ノイズの元を絶つ）
            }
        }
        else {
            m_lastSample = rawSample;
        }

        // メイン位相を進める
        m_phase += currentDelta;

        if (m_phase >= 1.0f) m_phase -= 1.0f;
    }

    return m_lastSample * finalEnv * m_level * m_baseLevel * 8.0f;
 }

// 波形データ生成
void Wt2Core::generateWaveform(int type)
{
    const int N = 256;
    const double PI = juce::MathConstants<double>::pi;

    for (int i = 0; i < N; ++i)
    {
        double phase = (double)i / (double)N; // 0.0 to 1.0
        float sample = 0.0f;

        if (type == 8) // custom
        {
            // 整数値を取り出す
            int rawInt = m_customWaves[m_tableSizeIndex][i / (256 / m_tableSize)];

            // 解像度に応じてセンター(0.0になる値)を算出
            // 0: 8 (0-15)
            // 1: 16 (0-31)
            // 2: 32 (0-63)
            // 4: 64 (0-127)
            // 5: 128 (0-255)
            int center = 8 << m_customWaveResolution;

            // =========================================================
            // 整数から -1.0f 〜 +1.0f の浮動小数点数への変換
            // 例: センターが128の場合
            // 0 -> (0-128)/128 = -1.0f
            // 128 -> (128-128)/128 = 0.0f
            // 255 -> (255-128)/128 = 127/128 = 0.9921875f
            // =========================================================
            sample = (float)(rawInt - center) / (float)center;
        }
        else
        {
            switch (type)
            {
            case 0: // Sine
                sample = (float)std::sin(2.0 * PI * phase);
                break;
            case 1: // Triangle
                sample = (float)(phase < 0.5 ? (-1.0 + 4.0 * phase) : (3.0 - 4.0 * phase));
                break;
            case 2: // Saw Up
                sample = (float)(-1.0 + 2.0 * phase);
                break;
            case 3: // Saw Down
                sample = (float)(1.0 - 2.0 * phase);
                break;
            case 4: // Square (50%)
                sample = (phase < 0.5) ? 1.0f : -1.0f;
                break;
            case 5: // Pulse 25%
                sample = (phase < 0.25) ? 1.0f : -1.0f;
                break;
            case 6: // Pulse 12.5%
                sample = (phase < 0.125) ? 1.0f : -1.0f;
                break;
            case 7: // Noise (Pseudo-random but fixed cycle for "Digital" feel)
                sample = ((float)(i * 12345 % 100) / 50.0f) - 1.0f;
                break;
            break;
            }
        }

        m_sourceWave[i] = sample;
    }
}

void Wt2Core::updatePhaseDelta()
{
    m_phaseDelta = m_currentFrequency / m_targetRate;
}

void Wt2Core::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
{
    float sample = getSample();

    // ユニゾン・ハーモニー向けに変更
    float basePanL = 1.0f;
    float basePanR = 1.0f;

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
