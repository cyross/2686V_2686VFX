#pragma once

struct SsgEgParams {
    int eg = 0;         // SE: 0~15 (SSG-EG)
    // FM SSG-EG Frequency (SSG-EGの周期速度)
    // 0.1Hz(ゆっくり) ～ 50Hz(高速) 程度を想定
    float freq = 1.0f;
};

struct QualityParams {
    // --- Bit Depth ---
    // 0:4bit, 1:5bit, 2:6bit, 3:8bit, 4:Raw
    // Default: Raw (32bit float) or 3 (8bit) depending on preference
    int bit = 4;

    // --- Sampling Rate ---
    // 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 22.05k, 7: 16k, 8: 12k, 9: 11k 10: 8k 11: 5.5k 12: 4k 13: 2k
    // Default: 55.5kHz (Typical FM Chip Rate)
    int rate = 2;
};

struct QualityPcmParams {
    int mode = 6; // Default: ADPCM
    // 1:96k, 2:55.5k, 3: 49.7k 4: 48k, 5: 44.1k, 6: 22.05k, 7: 16k, 8: 12k, 9: 11k 10: 8k 11: 5.5k 12: 4k 13: 2k
    int rate = 7;   // Default: 16kHz
    int interp = 1;
};

struct AlgMatrixParams {
    // 0: 従来のアルゴリズム, 1: マトリックスモード
    int mode = 0;

    // キャリア指定 (OP1~OP8)
    std::array<bool, 8> isCarrier = { false };

    // モジュレータ指定 (src から dest への順方向接続 src < dest)
    // [src][dest] の形で管理
    std::array<std::array<bool, 8>, 8> mod = { false };

    // フィードバック指定 (src から dest への逆方向/自己接続 src >= dest)
    std::array<std::array<bool, 8>, 8> fbMod = { false };
};

struct AlgFbParams {
    // --- Algorithm ---
    int algorithm = 0;
    // --- Feedback ---
    int feedback = 0;

	AlgMatrixParams matrix; // マトリックスモードのパラメータ
};

struct WtModParams {
    bool enable = false;
    float depth = 0.0f;
    float speed = 1.0f; // 搬送波の周波数に対する比率
    int shape = 0;      // WtModShape を参照

    // HuC6280 モード用の変調波形 (32 サンプル / -1.0〜1.0)
    std::array<float, 32> wave = { 0.0f };

    // FdsUser モード用の変調テーブル (32 エントリ / 実機と同じ 3bit のレジスタ値)
    std::array<int, 32> fdsTable = { 0 };
};

// WT AMP MOD のパラメータ。
//
// 変調波形の種類 (shape) は WT PITCH MOD と同じ WtModShape を使う。
// 違うのは当て先で、こちらは MIN〜MAX の間で音量へ掛かる。
// MIN / MAX は波形スロットをまたいで 1 組だけ持つ。
struct WtAmpModParams {
    bool enable = false;
    float depth = 0.0f;
    float speed = 1.0f; // 搬送波の周波数に対する比率
    int shape = 0;      // WtModShape を参照

    // 出力の下端と上端 (音量の倍率)
    float min = 0.0f;
    float max = 1.0f;

    // HuC6280 モード用の変調波形 (32 サンプル / -1.0〜1.0)
    std::array<float, 32> wave = { 0.0f };

    // FdsUser モード用の変調テーブル (32 エントリ / 実機と同じ 3bit のレジスタ値)
    std::array<int, 32> fdsTable = { 0 };
};
// MODULATION の変調方式
enum class WtModShape {
    Sine = 0,       // 正弦波 (FDS の簡易版)
    FdsTriangle,    // FDS(2C33) 32ステップ : 対称三角
    FdsSaw,         // FDS(2C33) 32ステップ : 非対称のこぎり
    FdsReset,       // FDS(2C33) 32ステップ : リセットで断ち切る
    FdsPulse,       // FDS(2C33) 32ステップ : 上下端で保持する台形
    WsSweepUp,      // WonderSwan ch3 スイープ (上昇)
    WsSweepDown,    // WonderSwan ch3 スイープ (下降)
    HuC6280Wave,    // PC Engine HuC6280 LFO (波形メモリで分周器を変調)
    FdsUser,        // FDS(2C33) 32ステップ : ユーザーが編集したテーブル
    Size
};

struct ToneNoiseParams {
    // --- Tone Level ---
    float tone = 1.0f;

    // --- Noise Level ---
    float noiseLevel = 0.0f; // Noise

    // --- Noise Speed Freq ---
    float noiseFreq = 12000.0f; // Noise Frequency (Hz)

    // --- Noise On Note ---
    // ノイズ周波数をMIDIキーで変更
    bool noiseOnNote = false;

    // --- Tone and Noise Mix ---
    // 0.0(Tone) - 1.0(Noise)
    float mix = 0.0f;
};

struct PcmParams {
    float offset = 0.0f; // ms
    float ratio = 1.0f;
};

struct LoopPointParams {
    bool enable = false;
    float start = 0.0f; // 0.0 to 1.0
    float end = 1.0f;   // 0.0 to 1.0
};

struct PanpotParams {
    int pan = 0;
    bool enable = false;
};

struct SsgDutyParams {
    // Duty Mode
    // 0: Preset, 1: Variable
    int mode = 0;

    // Duty Preset
    // Index (0-8)
    int preset = 0;

    // Duty Variable Value
    // 0.0-0.5
    float var = 0.5f;

    // Duty Invert Switch
    bool invert = false;

    // Famicom Mode
    bool fc = false;

    // Famicom Mode Fluctuation
    float fcFluc = 0.2f;
};

struct SsgTriParams {
    // Triangle Key Track Switch
    bool keyTrack = true;

    // Triangle Peak Position
    // 0.0=SawDown, 0.5=Tri, 1.0=SawUp
    float peak = 0.5f;

    // Triangle Manual Spped Freq
    float freq = 440.0f;
};
