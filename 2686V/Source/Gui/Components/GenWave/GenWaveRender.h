#pragma once

#include <memory>
#include <vector>

#include <JuceHeader.h>

class AudioPlugin2686V;

// ============================================================================
// 生成波形の計算と、貯めておくファイル
// ============================================================================
// 「リアルタイムではなく、ボタンを押したときだけ作る」ためのもの。
//
// 作り方は WavePreviewSource と同じ考え方で、出荷される音源のクラスを
// そのまま回して出力を拾う。プラグインをもう一つ立てて鳴らすわけでは
// ない。juce::Synthesiser は音を出す道具ではなくボイスの割り当てを
// 受け持つだけの部品なので、鳴っている音とは何も繋がっていない。
//
// 10 秒ぶん作る。押してから離すまでと、切れ際までが 1 本に入る。
//
// このヘッダは PluginProcessor.h を読み込まない。あちらは末尾で
// PluginEditor.h を読み込んでおり、画面側から辿るとこのファイルが
// 先に来て循環するため。音源一式は Rig の中へ隠してある。
namespace GenWaveRender
{
    // 作る長さの上限。ここまで来たら鳴り続けていても打ち切る。
    inline constexpr double seconds = 10.0;

    // 鳴り終わってからここまで見届けて打ち切る。
    //
    // 短いエンベロープの音を 10 秒ぶん作ると、頭の一瞬だけ動いて、
    // あとは止まった波形が延々と出るだけになる。鳴り終わったところで
    // 切り上げれば、その音に見合った長さになる。
    inline constexpr double tailSeconds = 0.5;

    // 「鳴っていない」とみなす大きさ。16 ビットの 1 きざみより下。
    inline constexpr float silenceLevel = 1.0e-4f;

    // 計算するときのサンプリングレート。実際の再生には使わない。
    inline constexpr double renderRate = 44100.0;

    // 貯めるときはここまで間引く。1 周期を描くのに要る細かさと、
    // ファイルの大きさの兼ね合いで決めた。A2 (110Hz) の 1 周期が
    // 145 点になり、10 秒ぶんで 160KB ほどに収まる。
    inline constexpr double storeRate = 16000.0;

    // 鳴らす音。低いほうが 1 周期に点を多く取れるので A2 にしてある。
    inline constexpr int midiNote = 45;

    // 鍵盤を離す位置 (全体に対する割合)。切れ際まで見せる。
    inline constexpr double noteOffAt = 0.75;

    inline constexpr int blockSize = 512;

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

    // 計算に使う一式。
    //
    // 組み立てはメッセージスレッドで行うこと (プロセッサが持っている
    // 波形メモリや PCM への参照を受け取るため)。組み上がったものを
    // 回すのは別のスレッドでよい。
    class Rig
    {
    public:
        Rig();
        ~Rig();

        Rig(const Rig&) = delete;
        Rig& operator=(const Rig&) = delete;

        // ユニゾンぶんのボイスを用意する。1 音しか鳴らさないので、
        // 本体のように同時発音数を掛けたぶんは要らない。
        void build(AudioPlugin2686V& processor);

        // 中身は cpp 側にある。音源一式をヘッダへ出さないための入れ物。
        struct Impl;

        Impl& impl() { return *m_impl; }
    private:
        std::unique_ptr<Impl> m_impl;
    };

    // 10 秒ぶんを計算する。時間が掛かるので、メッセージスレッドから
    // 直に呼ばないこと。
    Wave render(Rig& rig);

    // ---- 貯めておくファイル ----
    bool write(const juce::File& file, const Wave& wave);
    Wave read(const juce::File& file);

    // プレビューを貯めておく場所。無ければ作る。
    juce::File cacheDirectory(const AudioPlugin2686V& processor);

    // パラメータファイル 1 件ぶんの置き場。
    //
    // 名前がぶつからないよう、元の場所から作った短い符号を後ろへ付ける。
    // 同じ名前のファイルが別のフォルダにあっても混ざらない。
    juce::File cacheFileFor(const juce::File& cacheDir, const juce::File& source);
}
