#pragma once

// SSG HW ENV の波形スロット。
// 0〜7 は実機 AY-3-8910 / YM2149 のエンベロープ形状 (shape 8〜15) に対応する。
// 8 以降はこのプラグイン独自の追加波形で、すべて繰り返し形。
enum class SsgHwShape {
    // --- 実機準拠 (AY-3-8910 shape 8〜15) ---
    SawDown = 0,     // shape 8  : 下降を繰り返す
    SawDownHold,     // shape 9  : 1回下降して最小値で保持
    Triangle,        // shape 10 : 下降と上昇を繰り返す
    AltSawDownHold,  // shape 11 : 1回下降して最大値で保持
    SawUp,           // shape 12 : 上昇を繰り返す
    SawUpHold,       // shape 13 : 1回上昇して最大値で保持
    TriangleInvert,  // shape 14 : 上昇と下降を繰り返す
    AltSawUpHold,    // shape 15 : 1回上昇して最小値で保持

    // --- 矩形波 (最大値から始まる) ---
    Square75,        // EX-001 : デューティ 75%
    Square50,        // EX-002 : デューティ 50%
    Square25,        // EX-003 : デューティ 25%
    Square125,       // EX-004 : デューティ 12.5%

    // --- 矩形波 (最小値から始まる) ---
    SquareInv75,     // EX-005 : 最小値の区間が 75%
    SquareInv50,     // EX-006 : 最小値の区間が 50%
    SquareInv25,     // EX-007 : 最小値の区間が 25%
    SquareInv125,    // EX-008 : 最小値の区間が 12.5%

    // --- 階段上昇 ---
    StepUp4,         // EX-009 : 4段
    StepUp5,         // EX-010 : 5段
    StepUp6,         // EX-011 : 6段
    StepUp11,        // EX-012 : 11段

    // --- 階段降下 ---
    StepDown4,       // EX-013 : 4段
    StepDown5,       // EX-014 : 5段
    StepDown6,       // EX-015 : 6段
    StepDown11,      // EX-016 : 11段

    // --- 階段山形 (上って下る) ---
    StepPeak4,       // EX-017 : 4段
    StepPeak5,       // EX-018 : 5段
    StepPeak6,       // EX-019 : 6段
    StepPeak11,      // EX-020 : 11段

    // --- 階段谷形 (下って上る) ---
    StepValley4,     // EX-021 : 4段
    StepValley5,     // EX-022 : 5段
    StepValley6,     // EX-023 : 6段
    StepValley11,    // EX-024 : 11段

    // --- Sample & Hold (N 周期ごとに新しいレベルを引く) ---
    SampleHold,      // EX-025 : 毎周期
    SampleHold4,     // EX-026 : 4周期ごと
    SampleHold8,     // EX-027 : 8周期ごと
    SampleHold16,    // EX-028 : 16周期ごと
    SampleHold32,    // EX-029 : 32周期ごと
    SampleHold64,    // EX-030 : 64周期ごと

    // --- 追加提案分 ---
    ExpDecay,        // EX-031 : 指数減衰。実楽器に近い減衰で打楽器向き
    ExpAttack,       // EX-032 : 指数上昇。逆再生シンバル風のスウェル
    Sine,            // EX-033 : 正弦波。三角より滑らかなトレモロ
    AttackDecay,     // EX-034 : 急な立ち上がり + 指数減衰
    DoublePulse,     // EX-035 : 1周期に 2回パルスが出る
    Random,          // EX-036 : 毎サンプル乱数

    Size
};

struct SsgHwEnvParams {
    // Hardware Envelope Enable Switch
    bool enable = false;

    // Hardware Envelope Shape Index (SsgHwShape)
    int shape = 0;

    // Hardware Envelope Period Freq
    float period = 1.0f;

    float min = 0.0f;

    float max = 1.0f;

    // 出力にスムース処理を掛けるかどうか。
    // 波形の折り返しで生じる段差がブツブツ音の原因なので、
    // Period が大きいほど効果が分かりやすい。
    bool smooth = false;
};
