#pragma once

#include <cmath>

// ============================================================================
// サンプルとサンプルの間を埋める (INTERP)
// ============================================================================
// 音源は QUALITY の SMP.RATE で決めたレートで波を作る。ホストのレートへ戻す
// ときに、作った点と点の間をどう埋めるかがここ。
//
// 番号と式は QUALITY(PCM) の INTERP と同じ。あちらは読み込んだ音を読み戻す
// ときに使い、こちらは音源が作った波をホストのレートへ戻すときに使う。
//
//   0: Nearest           補間しない。粗い
//   1: Linear            線形補間 (既定。3.6.0 より前はこれだけだった)
//   2: Gaussian/Cubic    丸みのある補間
//   3: Zero-Order Hold   階段のまま。実機の出力に近い
//   4: Cosine            Linear と Cubic の中間
//   5: B-Spline          強いローパス。こもる
//   6: Lagrange          4 点補間。Cubic とは違う倍音
//
// s_m1 〜 s_2 は連続する 4 点で、s_0 と s_1 の間を frac (0.0〜1.0) で埋める。
// 2・5・6 は s_2 (1 つ先) を使うので、呼ぶ側は 1 サンプル遅らせて渡すこと。
namespace SynthInterp
{
    inline constexpr int modeNearest = 0;
    inline constexpr int modeLinear = 1;
    inline constexpr int modeGaussian = 2;
    inline constexpr int modeZeroOrderHold = 3;
    inline constexpr int modeCosine = 4;
    inline constexpr int modeBSpline = 5;
    inline constexpr int modeLagrange = 6;

    // 1 つ先の点 (s_2) が要るかどうか。要らないものは、これまでどおり
    // 遅れなしで出せる。
    inline bool needsLookahead(int mode)
    {
        return mode == modeGaussian || mode == modeBSpline || mode == modeLagrange;
    }

    inline float process(int mode, float s_m1, float s_0, float s_1, float s_2, float frac)
    {
        switch (mode)
        {
        case modeNearest:
            return (frac < 0.5f) ? s_0 : s_1;

        case modeGaussian:
        {
            // 3 次エルミートスプライン近似による滑らかなカーブ生成
            const float c0 = s_0;
            const float c1 = 0.5f * (s_1 - s_m1);
            const float c2 = s_m1 - 2.5f * s_0 + 2.0f * s_1 - 0.5f * s_2;
            const float c3 = 0.5f * (s_2 - s_m1) + 1.5f * (s_0 - s_1);

            return ((c3 * frac + c2) * frac + c1) * frac + c0;
        }

        case modeZeroOrderHold:
            return s_0;

        case modeCosine:
        {
            const float mu2 = (1.0f - std::cos(frac * 3.14159265358979323846f)) * 0.5f;

            return s_0 * (1.0f - mu2) + s_1 * mu2;
        }

        case modeBSpline:
        {
            const float c0 = (s_m1 + 4.0f * s_0 + s_1) / 6.0f;
            const float c1 = (s_1 - s_m1) / 2.0f;
            const float c2 = (s_m1 - 2.0f * s_0 + s_1) / 2.0f;
            const float c3 = (s_2 - 3.0f * s_1 + 3.0f * s_0 - s_m1) / 6.0f;

            return ((c3 * frac + c2) * frac + c1) * frac + c0;
        }

        case modeLagrange:
        {
            const float l_m1 = -frac * (frac - 1.0f) * (frac - 2.0f) / 6.0f;
            const float l_0 = (frac + 1.0f) * (frac - 1.0f) * (frac - 2.0f) / 2.0f;
            const float l_1 = -(frac + 1.0f) * frac * (frac - 2.0f) / 2.0f;
            const float l_2 = (frac + 1.0f) * frac * (frac - 1.0f) / 6.0f;

            return s_m1 * l_m1 + s_0 * l_0 + s_1 * l_1 + s_2 * l_2;
        }

        case modeLinear:
        default:
            // 式の形は音源が 3.6.0 より前から使っていたものに合わせてある。
            // s_0 * (1 - frac) + s_1 * frac と同じ値に見えるが、float では
            // 最後のビットが違う。既定の Linear で音が変わらないようにするため、
            // ここは必ずこの形で書くこと。
            return s_0 + (s_1 - s_0) * frac;
        }
    }

    // ============================================================================
    // 音源が作った点を貯めておく入れ物
    // ============================================================================
    // 音源は 1 点ずつ作るので、4 点 (s_m1 〜 s_2) を並べておくのはこちらの役目。
    //
    // 1 つ先を使わないモードでは、これまでどおり「いちばん新しい 2 点」の間を
    // 埋める (遅れなし)。1 つ先を使うモードでは 1 点ぶん遅らせて、その前後を
    // 埋める。初期値の Linear は 3.6.0 より前と同じ出力になる。
    struct History
    {
        float s[4] = { 0.0f, 0.0f, 0.0f, 0.0f }; // 古い順

        void reset()
        {
            s[0] = s[1] = s[2] = s[3] = 0.0f;
        }

        // 音源が 1 点作るたびに呼ぶ
        void push(float sample)
        {
            s[0] = s[1];
            s[1] = s[2];
            s[2] = s[3];
            s[3] = sample;
        }

        // frac は、いちばん新しい点までの進み具合 (0.0〜1.0)
        float read(int mode, float frac) const
        {
            if (needsLookahead(mode)) return process(mode, s[0], s[1], s[2], s[3], frac);

            return process(mode, s[1], s[2], s[3], s[3], frac);
        }
    };
}
