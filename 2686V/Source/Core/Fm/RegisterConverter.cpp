#include "RegisterConverter.h"

auto RegisterConverter::convertFmParam31(int regValue) -> std::optional<float>
{
    if (regValue <= 0) return 5.0f; // Slowest
    if (regValue >= 31) return 0.0f; // Fastest

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
    return 0.005f * std::pow(inv, 2.0f);
}

auto RegisterConverter::convertFmParam15(int regValue) -> std::optional<float>
{
    if (regValue <= 0) return 5.0f; // Slowest
    if (regValue >= 15) return 0.0f; // Fastest

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
    return 0.0222f * std::pow(inv, 2.0f);
}

auto RegisterConverter::convertFmParamSl(int regValue) -> std::optional<float>
{
    int v = std::clamp(regValue, 0, 15);

    if (v == 0) return 1.0f; // Slowest
    if (v == 15) return 0.0f; // Silent

    // 線形での簡易変換 (0->1.0, 15->0.0)
    return 1.0f - ((float)v / 15.0f);
}

// ==============================================================================
// FM Parameters (OPNA/OPN Standard)
// ==============================================================================

// --- Envelope Generator Rate (AR) ---
// Register: 0 (Slowest) - 31 (Fastest)
// VST Param: Time in Seconds (0.03s - 5.0s)
float RegisterConverter::convertFmAr(int regValue)
{
	return convertFmParam31(regValue).value_or(5.0f);
}

// --- Envelope Generator Rate (RR) ---
// Register: 0 (Slowest) - 15 (Fastest)
// VST Param: Time in Seconds (0.03s - 5.0s)
float RegisterConverter::convertFmRr(int regValue)
{
    return convertFmParam15(regValue).value_or(5.0f);
}

// --- Envelope Generator Rate (DR, SR) ---
// Register: 0 (Slowest) - 31 (Fastest)
// VST Param: Time in Seconds (0.0s - 5.0s)
float RegisterConverter::convertFmDr(int regValue)
{
    return convertFmParam31(regValue).value_or(5.0f);
}

// --- Sustain Rate (SR) Converter ---
// Register: 0 (Slowest) - 31 (Fastest)
// Output: Time in Seconds (0.03s - 10.0s)
float RegisterConverter::convertFmSr(int regValue)
{
    return convertFmParam31(regValue).value_or(5.0f);
}


// --- Sustain Level (SL) ---
// Register: 0 (Max Level, 0dB) - 15 (Min Level, -93dB)
// VST Param: 0.0 (Silent) - 1.0 (Max)
float RegisterConverter::convertFmSl(int regValue)
{
    return convertFmParamSl(regValue).value_or(1.0f);
}

// --- Total Level (TL) ---
// Register: 0 (Max Level) - 127 (Min Level)
// VST Param: 0.0 (Max) - 1.0 (Min)  <-- VST内部ロジックに合わせる
// (SynthVoice内では 1.0 - m_params.totalLevel でゲイン計算しているため)
float RegisterConverter::convertFmTl(int regValue)
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
int RegisterConverter::convertFmMul(int regValue)
{
    return std::clamp(regValue, 0, 15);
}

// --- Detune (DT) ---
// Register: 0-7
// VST Param: Integer 0-7
int RegisterConverter::convertFmDt(int regValue)
{
    return std::clamp(regValue, 0, 7);
}

// --- Detune (DT2) ---
// Register: 0-3
// VST Param: Integer 0-3
int RegisterConverter::convertFmDt2(int regValue)
{
    return std::clamp(regValue, 0, 3);
}

// --- 0-15 ---
int RegisterConverter::convertFmRg15(int regValue)
{
    return std::clamp(regValue, 0, 15);
}

// --- 0-31 ---
int RegisterConverter::convertFmRg31(int regValue)
{
    return std::clamp(regValue, 0, 31);
}

// --- 0-63 ---
int RegisterConverter::convertFmRg63(int regValue)
{
    return std::clamp(regValue, 0, 63);
}

// --- 0-127 ---
int RegisterConverter::convertFmRg127(int regValue)
{
    return std::clamp(regValue, 0, 127);
}

// ==============================================================================
// OPL / OPL3 Parameters (YM3812 / YMF262 Standard)
// ==============================================================================

// --- OPL: Attack Rate (AR) ---
// Register: 0 (Slowest) - 15 (Fastest)
// VST Param: Time in Seconds (0.03s - 5.0s)
float RegisterConverter::convertOplAr(int regValue)
{
    return convertFmParam15(regValue).value_or(5.0f);
}

// --- OPL: Decay Rate (DR) ---
// Register: 0 (Slowest) - 15 (Fastest)
// VST Param: Time in Seconds (0.0s - 5.0s)
float RegisterConverter::convertOplDr(int regValue)
{
    return convertFmParam15(regValue).value_or(5.0f);
}

// --- OPL: Release Rate (RR) ---
// Register: 0 (Slowest) - 15 (Fastest)
// VST Param: Time in Seconds (0.03s - 5.0s)
float RegisterConverter::convertOplRr(int regValue)
{
    return convertFmParam15(regValue).value_or(5.0f);
}

// --- OPL: Sustain Level (SL) ---
// Register: 0 (Max Level) - 15 (Min Level / Silent)
// VST Param: 0.0 (Silent) - 1.0 (Max)
float RegisterConverter::convertOplSl(int regValue)
{
    return convertFmParamSl(regValue).value_or(1.0f);
}

// --- OPL: Total Level (TL) ---
// Register: 0 (Max Volume) - 63 (Min Volume / -47dB)
// VST Param: 0.0 (Max) - 1.0 (Min)
float RegisterConverter::convertOplTl(int regValue)
{
    int v = std::clamp(regValue, 0, 63);

    // OPLのTLは最大63なので、それをVST用の0.0〜1.0スケールに正規化する
    return (float)v / 63.0f;
}

// --- OPL: Multiple (MUL) ---
// Register: 0-15
int RegisterConverter::convertOplMul(int regValue)
{
    return std::clamp(regValue, 0, 15);
}

// --- OPL: Detune (DT) ---
// Register: 0-7
// VST Param: Integer 0-7
int RegisterConverter::convertOplDt(int regValue)
{
    return std::clamp(regValue, 0, 7);
}

// MMLの -3 ～ +3 を、レジスタ値 0 ～ 7 に変換する
int RegisterConverter::convertMmlDtToReg(int mmlDtValue)
{
    if (mmlDtValue == 0) return 0;

    if (mmlDtValue > 0) {
        // +1 ～ +3 -> レジスタ 5 ～ 7
        return std::clamp(mmlDtValue + 4, 5, 7);
    }
    else {
        // -1 ～ -3 -> レジスタ 3 ～ 1
        return std::clamp(4 + mmlDtValue, 1, 3);
    }
}

// MML値を、レジスタ値 0 ～ 3 に変換する
int RegisterConverter::convertMmlDt2ToReg(int mmlDt2Value)
{
    return std::clamp(mmlDt2Value, 0, 3);
}

int RegisterConverter::convertFmKs(int regValue)
{
    return std::clamp(regValue, 0, 3);
}

bool RegisterConverter::convertFmMask(int regValue)
{
    return std::clamp(regValue, 0, 1) == 0;
}

bool RegisterConverter::convertOplAm(int regValue)
{
    return std::clamp(regValue, 0, 1) == 1;
}

bool RegisterConverter::convertOplVib(int regValue)
{
    return std::clamp(regValue, 0, 1) == 1;
}

bool RegisterConverter::convertOplEgType(int regValue)
{
    return std::clamp(regValue, 0, 1) == 1;
}

bool RegisterConverter::convertOplKsr(int regValue)
{
    return std::clamp(regValue, 0, 1) == 1;
}

int RegisterConverter::convertOplKsl(int regValue)
{
    return std::clamp(regValue, 0, 3);
}

std::vector<RegisterUnit> RegisterConverter::convertToRegisterUnit(const juce::String& input)
{
    std::vector<RegisterUnit> units;
    juce::StringArray tokens = juce::StringArray::fromTokens(input.toUpperCase(), false);

    for (const juce::String& token : tokens)
    {
        // 文字列の後ろが数字ではない時は無視
        if (!juce::CharacterFunctions::isDigit(token.getLastCharacter()))
        {
            continue;
        }

        // 文字列の後ろから数えて、数字ではない箇所を見つける
        int lastKeyPos = token.length() - 1;

        while (lastKeyPos >= 0 && (juce::CharacterFunctions::isDigit(token[lastKeyPos]) || token[lastKeyPos] == '-' || token[lastKeyPos] == '+'))
        {

            // +, - が検出されたときは、この時点で検索を中断する
            if (token[lastKeyPos] == '-' || token[lastKeyPos] == '+')
            {
                lastKeyPos--;

                break;
            }

            lastKeyPos--;
        }

        // 数字のみの場合は無視
        if (lastKeyPos < 0)
        {
            continue;
        }

        juce::String key = token.substring(0, lastKeyPos + 1).replaceCharacter(':', ' ').trim();
        int value = token.substring(lastKeyPos + 1).getIntValue();

        units.push_back({ key, value });
    }

    return units;
}

float RegisterConverter::convertFmRrOpzx7(int regValue)
{
    return convertFmParam31(regValue).value_or(5.0f);
}

float RegisterConverter::convertFmTlOpzx7(int regValue)
{
    int v = std::clamp(regValue, 0, 63);
    // Reg 0 -> Param 0.0 (Max Gain)
    // Reg 63 -> Param 1.0 (Min Gain)
    // 線形でOK
    return (float)v / 63.0f;
}

int RegisterConverter::convertFmMulOpzx7(int regValue)
{
    return std::clamp(regValue, 0, 21);
}

int RegisterConverter::convertFmDtOpzx7(int regValue)
{
    return std::clamp(regValue, 0, 15);
}
