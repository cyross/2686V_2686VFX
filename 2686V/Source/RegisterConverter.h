#pragma once
#include <cmath>
#include <algorithm>
#include "Mode.h"

class RegisterConverter
{
public:
    // ==============================================================================
    // FM Parameters (OPNA/OPN Standard)
    // ==============================================================================

    // --- Envelope Generator Rate (AR) ---
    // Register: 0 (Slowest) - 31 (Fastest)
    // VST Param: Time in Seconds (0.03s - 5.0s)
    static float convertFmAr(int regValue)
    {
        if (regValue == 0) return 5.0f; // Slowest
        if (regValue >= 31) return 0.03f; // Fastest

        // Clamp 0-31
        float r = (float)std::clamp(regValue, 0, 31);

        // 31 -> 0.0s (Instant)
        // 0  -> 5.0s (Slow)
        // カーブは指数的にするのが理想ですが、簡易的に反転線形＋補正で近似します
        // Rateが高いほど時間は急激に短くなる

        // 簡易変換: (31 - reg) をベースに係数を掛ける
        // 例: Reg=0 -> 5.0s, Reg=20 -> 短い, Reg=31 -> 0s
        float inv = 31.0f - r;

        // 指数カーブ近似: pow(inv) で急峻に変化させる
        // regが小さい(invが大きい)ほど時間が大幅に増える
        return 0.03f + (0.0052f * std::pow(inv, 2.0f));
    }

    // --- Envelope Generator Rate (RR) ---
    // Register: 0 (Slowest) - 15 (Fastest)
    // VST Param: Time in Seconds (0.03s - 5.0s)
    static float convertFmRr(int regValue)
    {
        if (regValue == 0) return 5.0f; // Slowest
        if (regValue >= 15) return 0.03f; // Fastest

        // Clamp 0-15
        float r = (float)std::clamp(regValue, 0, 15);

        // 15 -> 0.03s (Instant)
        // 0  -> 5.0s (Slow)
        // カーブは指数的にするのが理想ですが、簡易的に反転線形＋補正で近似します
        // Rateが高いほど時間は急激に短くなる

        // 簡易変換: (15 - reg) をベースに係数を掛ける
        // 例: Reg=0 -> 5.0s, Reg=10 -> 短い, Reg=15 -> 0s
        float inv = 15.0f - r;

        // 指数カーブ近似: pow(inv) で急峻に変化させる
        // regが小さい(invが大きい)ほど時間が大幅に増える
        return 0.03f + (0.022f * std::pow(inv, 2.0f));
    }

    // --- Envelope Generator Rate (DR, SR) ---
    // Register: 0 (Slowest) - 31 (Fastest)
    // VST Param: Time in Seconds (0.0s - 5.0s)
    static float convertFmDr(int regValue)
    {
        // Clamp 0-31
        float r = (float)std::clamp(regValue, 0, 31);

        // 31 -> 0.0s (Fast)
        // 0  -> 5.0s (Slow)
        // カーブは指数的にするのが理想ですが、簡易的に反転線形＋補正で近似します
        // Rateが高いほど時間は急激に短くなる

        if (regValue >= 31) return 0.0f; // Fastest

        // 簡易変換: (31 - reg) をベースに係数を掛ける
        // 例: Reg=0 -> 5.0s, Reg=20 -> 短い, Reg=31 -> 0s
        float inv = 31.0f - r;

        // 指数カーブ近似: pow(inv) で急峻に変化させる
        // regが小さい(invが大きい)ほど時間が大幅に増える
        return 0.005f * std::pow(inv, 2.0f);
    }

    // --- Sustain Rate (SR) Converter ---
    // Register: 0 (Slowest) - 31 (Fastest)
    // Output: Time in Seconds (0.03s - 10.0s)
    static float convertFmSr(int regValue)
    {
        // ガード処理
        if (regValue <= 0) return 10.0f;  // Slowest (10.0s)
        if (regValue >= 31) return 0.03f; // Fastest (30ms)

        // Clamp 0-31
        float r = (float)std::clamp(regValue, 0, 31);

        // 反転: 31(速) -> 0, 0(遅) -> 31
        float inv = 31.0f - r;

        // 計算式
        // 目標: inv=31 (Reg0) のとき 10.0s
        // 式: Time = MinTime + (Coefficient * inv^2)
        // 10.0 = 0.03 + (Coef * 961)
        // Coef = 9.97 / 961 ≈ 0.01037... -> 0.0104f を採用

        return 0.03f + (0.0104f * std::pow(inv, 2.0f));
    }

    // --- Sustain Level (SL) ---
    // Register: 0 (Max Level, 0dB) - 15 (Min Level, -93dB)
    // VST Param: 0.0 (Silent) - 1.0 (Max)
    static float convertFmSl(int regValue)
    {
        int v = std::clamp(regValue, 0, 15);
        // 0 -> 1.0
        // 15 -> 0.0
        // -3dB step approximation
        if (v == 15) return 0.0f;
        // linear mapping for simplicity in VST UI, or -3dB steps
        // 1.0 * pow(10, -3dB * v / 20) ?
        // ここではUIの操作感を重視して線形で返しますが、
        // 実機感重視なら対数カーブを入れてください。
        return 1.0f - (float)v / 15.0f;
    }

    // --- Total Level (TL) ---
    // Register: 0 (Max Level) - 127 (Min Level)
    // VST Param: 0.0 (Max) - 1.0 (Min)  <-- VST内部ロジックに合わせる
    // (SynthVoice内では 1.0 - m_params.totalLevel でゲイン計算しているため)
    static float convertFmTl(int regValue)
    {
        int v = std::clamp(regValue, 0, 127);
        // Reg 0 -> Param 0.0 (Max Gain)
        // Reg 127 -> Param 1.0 (Min Gain)
        // 線形でOK
        return (float)v / 127.0f;
    }

    // --- Multiple (MUL) ---
    // Register: 0-15 (0 is 0.5x, 1 is 1x, ...)
    // VST Param: Integer 0-15
    static int convertFmMul(int regValue)
    {
        return std::clamp(regValue, 0, 15);
    }

    // --- Detune (DT) ---
    // Register: 0-7
    // VST Param: Integer 0-7
    static int convertFmDt(int regValue)
    {
        return std::clamp(regValue, 0, 7);
    }

    // --- Detune (DT2) ---
    // Register: 0-3
    // VST Param: Integer 0-3
    static int convertFmDt2(int regValue)
    {
        return std::clamp(regValue, 0, 3);
    }

    // ==============================================================================
    // SSG Parameters
    // ==============================================================================

    // --- Volume ---
    // Register: 0-15
    // VST Param: 0.0 - 1.0
    static float convertSsgVol(int regValue)
    {
        int v = std::clamp(regValue, 0, 15);
        return (float)v / 15.0f; // Linear
    }

    // --- Mixer (Enable Flags) ---
    // Register 0x07 bits:
    // Bit 0: Tone A Enable (0=On, 1=Off)
    // Bit 3: Noise A Enable (0=On, 1=Off)
    //
    // VST Param (Mix): 0.0(Tone) - 1.0(Noise) - 0.5(Both)
    // ※VSTの実装はクロスフェード式なので完全再現は難しいですが、近似値を返します
    static float convertSsgMixer(bool toneOff, bool noiseOff)
    {
        bool toneOn = !toneOff;
        bool noiseOn = !noiseOff;

        if (toneOn && !noiseOn) return 0.0f; // Tone Only
        if (!toneOn && noiseOn) return 1.0f; // Noise Only
        if (toneOn && noiseOn)  return 0.5f; // Both
        return 0.0f; // Silent (Tone default)
    }

    // --- Envelope Period ---
    // Register: 0-65535 (EP)
    // VST Param: Frequency (Hz) or Period?
    // SsgCoreでは `m_envFreq` を保持し、位相を進めている。
    // 実機: Period = EP * T_clock * ... => Freq = Clock / (256 * EP) roughly
    // 近似: 入力をHzに変換
    static float convertSsgEnvPeriod(int regValue)
    {
        if (regValue <= 0) return 20.0f; // Fast
        // 簡易計算: レジスタ値が大きいほど周波数は低い
        // BaseClock(approx 2MHz) / (256 * Reg)
        // e.g. 2000000 / (256 * 1000) = 7.8Hz
        return 7812.5f / (float)regValue;
    }
};
