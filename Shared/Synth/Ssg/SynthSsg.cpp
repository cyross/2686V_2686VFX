#include "./SynthSsg.h"
#include "Shared/Core/Synth/SsgRegister.h"

#include "Shared/Core/Synth/SynthHelpers.h"

const std::array<float, 9> SsgCore::dutyPresets = { 0.5f, 0.4375f, 0.375f, 0.3125f, 0.25f, 0.20f, 0.1875f, 0.125f, 0.0625f };

SsgCore::SsgCore() : SynthCore() {
}

void SsgCore::prepare(double sampleRate) {
    if (sampleRate > 0.0) {
        m_sampleRate = sampleRate;
    }

    m_adsr.prepare(m_sampleRate);
	m_pitchAdsr.prepare(0, m_sampleRate);
    m_ssgSwEnv256.prepare(0, m_sampleRate);
    m_ssgSwPenv256.prepare(0, m_sampleRate);

    m_targetRate = getTargetRate(m_rateIndex);

    m_lfo.prepare(m_targetRate);
    m_noiseGen.prepare(m_targetRate);
    m_ssgHwEnv.prepare(m_targetRate);
    m_ssgHwPEnv.prepare(m_targetRate);
    m_phaseDelta = m_currentFrequency / m_targetRate;
}

void SsgCore::setCurveCore(CurveCore* p_curveCore)
{
    m_adsr.setCurveCore(p_curveCore);
    m_pitchAdsr.setCurveCore(p_curveCore);
    m_ssgSwEnv256.setCurveCore(p_curveCore);
    m_ssgSwPenv256.setCurveCore(p_curveCore);
}

void SsgCore::setSampleRate(double sampleRate) {
    m_sampleRate = sampleRate;

    m_adsr.updateSampleRate(m_sampleRate);
    m_pitchAdsr.updateSampleRate(m_sampleRate);
    m_ssgSwEnv256.updateSampleRate(m_sampleRate);
    m_ssgSwPenv256.updateSampleRate(m_sampleRate);
}

void SsgCore::setParameters(const SynthCoreParams& params)
{
    m_level = params.ssg.level;
    m_delaySeconds = params.ssg.delay;
    m_speed = params.ssg.speed;
    m_hold.setParameters(params.ssg.hold);

    m_tone = params.ssg.tn.tone;
    m_toneRatio = params.ssg.tn.toneRatio;
    m_mix = params.ssg.tn.mix;

    // ユニゾン・ハーモニー用
    m_isMonoMode = params.monoMode;

    m_adsr.setParameters(params.ssg.adsr);
	m_pitchAdsr.setParameters(params.ssg.pitchAdsr);
    m_detune.setParameters(params.ssg.detune);
    m_ssgSwEnv256.setParameters(params.ssg.ssgSwEnv256);
    m_ssgSwPenv256.setParameters(params.ssg.ssgSwPEnv256);
    m_lfo.setParameters(params.ssg.lfo);
    m_ssgHwEnv.setParameters(params.ssg.env);
    m_ssgHwPEnv.setParameters(params.ssg.ssgHwPEnv);
    m_wtMod.setParameters(params.ssg.wtMod);
    m_wtAmpMod.setParameters(params.ssg.wtAmpMod);

    m_fixMode.setParameters(params.ssg.fix);

    m_waveform = params.ssg.waveform;

    m_dutyMode = params.ssg.duty.mode;
    m_dutyPreset = params.ssg.duty.preset;
    m_dutyVar = params.ssg.duty.var;
    m_dutyInvert = params.ssg.duty.invert;
    m_dutyFc = params.ssg.duty.fc;
    m_dutyFcFluc = params.ssg.duty.fcFluc;
    m_dutyMoreFc = params.ssg.duty.moreFc;
    m_dutyMoreNes = params.ssg.duty.moreNes;

    m_triKeyTrack = params.ssg.tri.keyTrack;
    m_triPeak = params.ssg.tri.peak;
    m_triFreq = params.ssg.tri.freq;

    m_noiseGen.setParameters({ .level = params.ssg.tn.noiseLevel, .noiseOnNote = params.ssg.tn.noiseOnNote, .baseFreq = params.ssg.tn.noiseFreq });

    if (m_rateIndex != params.ssg.quality.rate) {
        m_rateIndex = params.ssg.quality.rate;

        m_targetRate = getTargetRate(m_rateIndex);

        m_noiseGen.updateTargetRate(m_targetRate);
        m_lfo.updateTargetSampleRate(m_targetRate);
        m_ssgHwEnv.updateTargetSampleRate(m_targetRate);
        m_ssgHwPEnv.updateTargetSampleRate(m_targetRate);
        m_phaseDelta = m_currentFrequency / m_targetRate;
    }

    m_noiseGen.updateFrequency(m_currentFrequency);
    m_noiseGen.updateDelta();

    m_quantizeSteps = getTargetBitDepth(params.ssg.quality.bit);
	m_interpMode = params.ssg.quality.interp;

    m_pitchResetOnLegato = params.pitchResetOnLegato;

    // MUL/DET・FIX を鳴らしている最中にも反映する (SETTINGS)
    if (params.liveDetune && m_keyFreq > 0.0f && isPlaying()) updateFrequency();
}

// MUL/DET・FIX から周波数を決める
void SsgCore::updateFrequency()
{
    m_currentFrequency = m_detune.noteOn(m_fixMode.noteOn(m_keyFreq));
    m_phaseDelta = m_currentFrequency / m_targetRate;
    m_noiseGen.updateFrequency(m_currentFrequency);
    m_noiseGen.updateDelta();
}

void SsgCore::noteOn(float freq, float velocity, int midiNote, bool isLegato)
{
    m_hold.reset();

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

    // 基本周波数にデチューン成分を加算
    // Save for recalculation
    m_keyFreq = finalFreq;
    updateFrequency();

    if (!isLegato) {
        if (!m_isMonoMode) {
            m_phase = (m_unison.getPhaseOffset() * juce::MathConstants<float>::twoPi);

            // 位相が 2π を超えた場合は安全にラップアラウンド（折り返し）させる
            while (m_phase >= juce::MathConstants<float>::twoPi) {
                m_phase -= juce::MathConstants<float>::twoPi;
            }
            m_lfo.noteOn();
        }

        // 2A03 は $4003 / $4007 を書くとシーケンサが先頭へ戻る。ドライバは
        // 押すたびに書くので、ユニゾンのずらしを使わず先頭から鳴らす
        // (デューティ比 12.5〜50% は Low の段から始まる)。
        if (is2A03()) m_phase = 0.0f;

        m_ssgHwEnv.noteOn();
        m_ssgHwPEnv.noteOn();
        m_rateAccumulator = 0.0;
        m_interp.reset();
        m_lastSample = 0.0f;
        m_2a03Filter.reset();
    }

    if (!isLegato) {
        m_currentLevel = m_adsr.noteOn();

        if (!m_pitchAdsr.isBypass() && !m_pitchResetOnLegato) {
            m_pitchAdsr.noteOn();
        }

        if (!m_ssgSwEnv256.isBypass()) {
            m_ssgSwEnv256.noteOn();
        }

        if (!m_ssgSwPenv256.isBypass()) {
            m_ssgSwPenv256.noteOn();
        }
    }

    if (!m_pitchAdsr.isBypass() && m_pitchResetOnLegato) {
        m_pitchAdsr.noteOn();
    }

    if (!m_ssgSwPenv256.isBypass() && m_pitchResetOnLegato) {
        m_ssgSwPenv256.noteOn();
    }
}

void SsgCore::noteOff()
{
    m_adsr.noteOff();

    if (!m_pitchAdsr.isBypass()) {
        m_pitchAdsr.noteOff();
    }

    if (!m_ssgSwEnv256.isBypass()) {
        m_ssgSwEnv256.noteOff();
    }

    if (!m_ssgSwPenv256.isBypass()) {
        m_ssgSwPenv256.noteOff();
    }
}

bool SsgCore::isPlaying() const { return m_adsr.isPlaying() || m_ssgSwEnv256.isPlaying(); }

// ピッチベンド (0 - 16383, Center=8192)
void SsgCore::setPitchBend(int pitchWheelValue)
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
void SsgCore::setModulationWheel(int wheelValue)
{
    // 0.0 ～ 1.0 に正規化
    m_modWheel = (float)wheelValue / 127.0f;

    m_wtMod.setModWheel(m_modWheel);
    m_wtAmpMod.setModWheel(m_modWheel);
}

void SsgCore::setPitchBendRatio(float ratio)
{ m_pitchBendRatio = ratio; }

// --- SsgCore.cpp : getSample() を丸ごと差し替え ---

float SsgCore::getSample()
{
    // すべてのアンプエンベロープがバイパスされているかどうかを判定
    bool isAllAmpBypassed = m_adsr.isBypass() && m_ssgSwEnv256.isBypass();

    if (isAllAmpBypassed) {
        // 全てのアンプエンベロープがバイパスの時は、完全な矩形波（Gate）動作
        // どれかが Release 状態（noteOffが呼ばれた直後）なら、即座に音を消す
        if (m_adsr.isRelease() || m_ssgSwEnv256.isRelease()) {
            m_adsr.bypassedReleasedProcess();
            m_ssgSwEnv256.bypassedReleasedProcess();
            m_pitchAdsr.bypassedReleasedProcess();
            m_ssgSwPenv256.bypassedReleasedProcess();
            return 0.0f;
        }
    }
    else if (!isPlaying()) {
        // いずれかのアンプエンベロープが有効で、全ての再生が終了（音が減衰しきった）時
        // ピッチエンベロープも強制終了させる（次のノートオンでピッチが変になるのを防ぐ）
        m_pitchAdsr.bypassedReleasedProcess();
        m_ssgSwPenv256.bypassedReleasedProcess();
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

        // 3. SSG Sw Env 11 処理
        if (!m_ssgSwEnv256.isBypass()) {
            finalEnv *= m_ssgSwEnv256.process();
        }
        else {
            if (m_ssgSwEnv256.isRelease()) m_ssgSwEnv256.bypassedReleasedProcess();
        }
    }

    // --- Sample Rate Emulation ---
    double stepSize = m_targetRate / m_sampleRate;
    m_rateAccumulator += stepSize;

    float newPhaseDelta = m_pitchAdsr.process(m_phaseDelta);
    newPhaseDelta = m_ssgSwPenv256.process(newPhaseDelta);

    // Update core logic only when virtual clock ticks
    while (m_rateAccumulator >= 1.0)
    {
        m_rateAccumulator -= 1.0;

        // 前回のサンプルを保存 (線形補間用)
        m_prevSample = m_lastSample;

        m_lfo.getSample();

        // ==========================================
        // Opzx7 LFO の計算 (AM / PM)
        // ==========================================

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
        if (m_waveform == 1 && !m_triKeyTrack) {
            phaseInc = (m_triFreq / (float)m_targetRate) * freqMult;
        }
        else {
            phaseInc = newPhaseDelta * freqMult;
        }

        // FORM の TONE の VALUE: 鍵盤の周波数へ To から決まる倍率を掛ける
        phaseInc *= m_toneRatio;

        // 2A03 の鳴り方では、周波数を 11bit タイマーの刻みへ寄せる。
        // 実際に位相を進める量 (速さを掛けたもの) で寄せ、t < 8 は鳴らさない。
        const bool use2A03 = is2A03();
        bool silent2A03 = false;

        if (use2A03) {
            m_2a03Filter.prepare(m_targetRate, m_dutyMoreFc ? Pulse2A03::OutputFilter::Model::Fc : Pulse2A03::OutputFilter::Model::Nes);

            const double effective = (double)phaseInc * (double)m_speed * m_targetRate;
            const double quantized = Pulse2A03::quantizeFrequency(effective);

            if (quantized <= 0.0 || m_speed <= 0.0f) silent2A03 = true;
            else phaseInc = (float)(quantized / m_targetRate / (double)m_speed);
        }

        // ==========================================
        // 1. Hardware Envelope Update
        // ==========================================
        float hwEnvGain = m_ssgHwEnv.process() * m_wtAmpMod.process(m_ampModDelta);

        // ==========================================
        // 2. Waveform Generation
        // ==========================================
        // 部分再生。波形を引く位相だけを動かし、進み方は変えない。
        double windowed = m_phase;

        const bool muted = m_hold.windowPhase(windowed);

        const float wavePhase = (float)windowed;

        float toneSample = 0.0f;
        float fcFluc = 0.0f;

        if (m_waveform == 0) // Pulse
        {
            if (use2A03) {
                // FC FLUC とは別物。実機の 4 種のデューティ比へ寄せ、0 / 1 の単極で出す。
                // 直流はあとのフィルタが抜く。
                float currentDuty = m_dutyMode == 0 ? dutyPresets[m_dutyPreset] : m_dutyVar;

                if (m_dutyInvert) currentDuty = 1.0f - currentDuty;

                toneSample = silent2A03 ? 0.0f : Pulse2A03::sample(wavePhase, Pulse2A03::nearestDuty(currentDuty));
            }
            else if (m_dutyFc) {
                float currentDuty = m_dutyMode == 0 ? dutyPresets[m_dutyPreset] : m_dutyVar;

                if (m_dutyInvert) currentDuty = 1.0f - currentDuty;

                // 極端なデューティ比による波形消失を防ぐ最低保証
                float minDuty = phaseInc;
                if (currentDuty < minDuty) currentDuty = minDuty;
                if (currentDuty > 1.0f - minDuty) currentDuty = 1.0f - minDuty;

                toneSample = (wavePhase < currentDuty) ? 1.0f : -1.0f;
                fcFluc = (wavePhase < currentDuty) ? -m_dutyFcFluc * (wavePhase / currentDuty) : m_dutyFcFluc * ((wavePhase - currentDuty) / (1.0f - currentDuty));
            }
            else {
                float currentDuty = m_dutyMode == 0 ? dutyPresets[m_dutyPreset] : m_dutyVar;

                if (m_dutyInvert) currentDuty = 1.0f - currentDuty;

                // 極端なデューティ比による波形消失を防ぐ最低保証
                float minDuty = phaseInc;
                if (currentDuty < minDuty) currentDuty = minDuty;
                if (currentDuty > 1.0f - minDuty) currentDuty = 1.0f - minDuty;

                toneSample = (wavePhase < currentDuty) ? 1.0f : -1.0f;
            }
        }
        else // Triangle
        {
            float phaseNorm = wavePhase;
            float k = m_triPeak;
            if (k < 0.001f) k = 0.001f;
            if (k > 0.999f) k = 0.999f;

            if (phaseNorm < k) toneSample = -1.0f + 2.0f * (phaseNorm / k);
            else                toneSample = 1.0f - 2.0f * ((phaseNorm - k) / (1.0f - k));
        }

        // 区間の外で端の値を保たない側は 0 にする
        if (muted) toneSample = 0.0f;

        // 決めた回数まで回したら、キーが離れるまで同じ値を出し続ける
        if (m_hold.isHolding()) toneSample = m_hold.holdValue();

        // 2A03 の出力の後ろのフィルタ。単極の 0〜1 を通すので、振れ幅を
        // ほかの矩形波 (-1〜1) へ揃えるために 2 倍する。
        if (use2A03) toneSample = m_2a03Filter.process(toneSample) * 2.0f;

        m_phase += phaseInc * m_speed;
        // ピッチエンベロープや PM で 1 サンプルの進みが 1.0 を超えても
        // 破綻しないよう while で回す
        while (m_phase >= 1.0f) {
            m_phase -= 1.0f;

            m_hold.countCycle();
        }
        while (m_phase < 0.0f) m_phase += 1.0f;

        // ==========================================
        // 3. Noise Generator
        // ==========================================
        m_noiseGen.generate();

        // ==========================================
        // 4. Mixing
        // ==========================================
        float toneGain = 1.0f - m_mix;
        float noiseGain = m_mix;
        float rawMixed = (toneSample * m_tone * toneGain) + m_noiseGen.generateSample(noiseGain);

        // AM（トレモロ効果）をここで掛け合わせる
        rawMixed *= hwEnvGain * amMultiplier;

        // ==========================================
        // 5. Quantize (Bit Depth)
        // ==========================================
        float finalOut = 0.0f;
        if (m_quantizeSteps > 0.0f) {
            if (rawMixed > 1.0f) rawMixed = 1.0f;
            if (rawMixed < -1.0f) rawMixed = -1.0f;

            // ディザ(dither)を削除し、実機DACと同じ純粋な四捨五入にする(プレビューのゴミ解消)
            // 0〜1 に詰め替えてから丸めると 0.0 が中央ステップから外れて
            // 直流オフセットになるため、バイポーラのまま量子化する
            finalOut = quantizeSample(rawMixed, m_quantizeSteps);
        }
        else {
            finalOut = rawMixed;
        }

        m_lastSample = finalOut + fcFluc;
        m_interp.push(m_lastSample);
    }

    // 線形補間を適用して波形を滑らかに出力する
    // m_rateAccumulator は直近に生成したサンプルからの進み具合を
    // ソースサンプル単位 (0.0〜1.0) で保持しているので、そのまま補間係数になる。
    // prev→last を補間する形なので、出力はソース 1 サンプル分だけ遅れる。
    float fraction = (float)m_rateAccumulator;

    float interpolatedSample = m_interp.read(m_interpMode, fraction);

    // 2A03 の音量は 4bit。エンベロープの値を 16 段へ丸め、DAC の曲がりを通す。
    if (is2A03()) finalEnv = Pulse2A03::volume(finalEnv);

    return interpolatedSample * finalEnv * m_baseLevel * m_level * 4.0f;
}

void SsgCore::renderNextBlock(float* outR, float* outL, int startSample, int sampleIdx, bool& isActive)
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
