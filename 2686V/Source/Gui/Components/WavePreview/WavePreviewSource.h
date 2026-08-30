#pragma once

#include <array>
#include <vector>

// ============================================================================
// 波形プレビューへ渡すものを作る
// ============================================================================
// 描画専用の近似を書くと本物と少しずつずれていくので、ここでは出荷される
// 音源部をそのまま走らせ、その出力を間引いたものを返している。
//
// 呼び出し側は結果を GuiWavePreview へ渡すだけでよい。
// 両振りの波形は -1.0〜1.0、エンベロープや AM は 0.0〜1.0 の片側なので、
// setPoints の bipolar を合わせること。
namespace WavePreviewSource
{
    // 折れ線の点数。表示幅ぶんあれば足りる。
    inline constexpr int points = 256;

    // ---- 読み込んだオーディオファイル ----
    // 1 点ずつ拾っても形が分からないので、区間ごとの最小値と最大値で返す。
    // GuiWavePreview::setEnvelope へそのまま渡せる。
    struct Envelope
    {
        std::vector<float> mins;
        std::vector<float> maxs;
    };

    // offsetMs と ratio は実際に鳴る範囲の切り出しに使う。
    // 音源側と同じく、オフセットから後ろを ratio ぶんだけ鳴らす扱い。
    Envelope audioFile(const std::vector<float>& data, double sourceRate, float offsetMs, float ratio);

    // ---- SSG HW ENV ---- (0.0〜1.0 の片側)
    std::vector<float> ssgHwEnv(int shapeIndex, float minLevel, float maxLevel, bool smooth);

    // ---- WT MODULATION ----
    // 変調の深さは最大で回すので、Depth の設定は渡さなくてよい。
    // wave は HuC6280 モードの 32 サンプル、fdsTable は FdsUser モードの
    // 32 エントリ。それぞれ使わない Shape では無視される。
    std::vector<float> wtMod(int shapeIndex, const std::array<float, 32>& wave, const std::array<int, 32>& fdsTable);

    // ---- OPZX7 WS (オペレータの波形) ----
    // 波形メモリ (WT / WT2) と PCM を選ぶ番号があるので、その中身も渡す。
    // 未読込のときは空を渡してよい (音源側と同じくサイン波が返る)。
    //
    // offsetMs / ratio / sampleRate は PCM の切り出しに使う。
    // それ以外の番号では無視される。
    std::vector<float> opzx7Ws(int shapeIndex, std::vector<float>& wt, std::vector<float>& wt2, std::vector<float>& pcm,
        float offsetMs = 0.0f, float ratio = 1.0f, double sampleRate = 44100.0);

    // ---- OPZX7 LFO ----
    std::vector<float> opzx7LfoPm(int shapeIndex);
    std::vector<float> opzx7LfoAm(int shapeIndex, float smoothRate);

    // ---- N88 LFO ----
    std::vector<float> n88LfoPm(int shapeIndex);
    std::vector<float> n88LfoAm(int shapeIndex, float smoothRate);

    // ---- OPM LFO ----
    std::vector<float> opmLfoPm(int shapeIndex);
    std::vector<float> opmLfoAm(int shapeIndex, float smoothRate);
}
