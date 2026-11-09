#pragma once

#include <memory>
#include <vector>

#include <JuceHeader.h>

#include "Shared/Core/Gui/GuiHost.h"

// ============================================================================
// 生成波形と、貯めておくファイル
// ============================================================================
// 「リアルタイムではなく、ボタンを押したときだけ作る」ためのもの。
//
// 作り方は WavePreviewSource と同じ考え方で、出荷される音源のクラスを
// そのまま回して出力を拾う。プラグインをもう一つ立てて鳴らすわけでは
// ない。juce::Synthesiser は音を出す道具ではなくボイスの割り当てを
// 受け持つだけの部品なので、鳴っている音とは何も繋がっていない。
//
// 4.4.1 から、ファイルブラウザーの行のプレビューは打鍵の並び (テンポ・小節・
// キーオン周期・音符長・キー) どおりに作る (StageWaveRender::renderPattern)。
// ここは波形の入れ物と、貯めておくファイルを受け持つ。
//
// 音源一式 (GuiRenderRig) はプラグインごとにボイスとパラメータが違うので、
// 各プラグインのプロセッサが組み立てる (GuiProcessorHost::createRenderRig)。
namespace GenWaveRender
{
    // 計算するときのサンプリングレート。実際の再生には使わない。
    inline constexpr double renderRate = 44100.0;

    // 貯めるときはここまで間引く。1 周期を描くのに要る細かさと、
    // ファイルの大きさの兼ね合いで決めた。A2 (110Hz) の 1 周期が
    // 145 点になり、10 秒ぶんで 160KB ほどに収まる。
    inline constexpr double storeRate = 16000.0;

    // 貯めたファイルを読むときの、長さの上限 (壊れたファイルを読まないため)。
    // 打鍵の並び (最長 30 秒) と余韻 (最長 10 秒) が収まる長さ
    inline constexpr double maxSeconds = 60.0;

    // 貯めるファイルの見出し
    // 2 から L と R を別々に持つ。1 のものは混ざったものしか
    // 入っていないので、読むときに左右へ同じものを入れる。
    inline constexpr int fileVersion = 2;
    inline const juce::String fileExtension = ".genwave";

    struct Wave
    {
        // storeRate で持つ。-1.0〜1.0。
        //
        // チャンネルによっては左右で違う音が出る (ステレオの波形メモリ、
        // パンを振ったもの) ので、混ぜずに両方持つ。真ん中は描くときに
        // その場で作る。
        std::vector<float> left;
        std::vector<float> right;

        double sampleRate = storeRate;
        double noteHz = 0.0;

        bool isEmpty() const { return left.empty(); }

        size_t size() const { return left.size(); }

        // 0 = L, 1 = M, 2 = R
        float at(int channel, size_t index) const
        {
            if (index >= left.size()) return 0.0f;

            switch (channel)
            {
            case 0: return left[index];
            case 2: return right[index];
            default: return (left[index] + right[index]) * 0.5f;
            }
        }

        double lengthSeconds() const
        {
            return (sampleRate > 0.0) ? (double)left.size() / sampleRate : 0.0;
        }

        // 1 周期ぶんの点数
        double samplesPerCycle() const
        {
            return (noteHz > 0.0) ? sampleRate / noteHz : 0.0;
        }
    };

    // ---- 貯めておくファイル ----
    bool write(const juce::File& file, const Wave& wave);
    Wave read(const juce::File& file);

    // プレビューを貯めておく場所。無ければ作る。
    juce::File cacheDirectory(const GuiProcessorHost& processor);

    // パラメータファイル 1 件ぶんの置き場。
    //
    // 名前がぶつからないよう、元の場所から作った短い符号を後ろへ付ける。
    // 同じ名前のファイルが別のフォルダにあっても混ざらない。
    //
    // patternKey は作ったときの打鍵の並びの符号 (StageWaveRender::patternKey)。
    // 並びを変えると別の置き場になるので、前の並びで作ったものは「未生成」に見える。
    juce::File cacheFileFor(const juce::File& cacheDir, const juce::File& source, const juce::String& patternKey);
}
