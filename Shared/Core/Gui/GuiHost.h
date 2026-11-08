#pragma once

#include <JuceHeader.h>

#include <array>
#include <functional>
#include <map>
#include <memory>

#include "./GuiToggleAlign.h"
#include "./GuiLiveGraph.h"
#include "../Synth/WtModWave.h"
#include "../Synth/StepTable.h"
#include "../Synth/StepTableUndo.h"
#include "../Synth/LevelMeterFeed.h"
#include "../Synth/StageProbe.h"
#include "../Io/IoPathRoot.h"

// ============================================================================
// 画面の部品から見たプロセッサとエディタ (窓口)
// ============================================================================
// 12 本で共有する画面の部品は、どのプラグインのプロセッサとエディタに入るのか
// を知らない。部品が使うものだけをここに並べ、各プラグインのプロセッサと
// エディタがこれを受け継いで実装する。
//
// 部品はここに無いものを使わないこと。要るときはここへ足し、各プラグインの
// プロセッサ / エディタで実装する。
//
// タブなどプラグインの側に残るコードは、pluginOf(ctx) / editorOf(ctx)
// (各プラグインの Core/Gui/GuiPluginContext.h) で具体的なクラスへ戻して使う。

// ---------------------------------------------------------------- 生成波形の計算
// 生成波形 (GenWave) を作るための音源一式。プラグインのボイスとパラメータを
// そのまま使うので、中身は各プラグインのプロセッサが組み立てる
// (GuiProcessorHost::createRenderRig)。部品はこれを回して出力を拾うだけ。
//
// 組み立てはメッセージスレッドで、回すのは別のスレッドでよい。
class GuiRenderRig
{
public:
    virtual ~GuiRenderRig() = default;

    virtual void renderNextBlock(juce::AudioBuffer<float>& buffer, const juce::MidiBuffer& midi,
        int startSample, int numSamples) = 0;
};

// ---------------------------------------------------------------- 生成中の波形の計算 (4.4.0)
// 生成中の波形の窓 (GuiStageWave) が回す音源一式。GuiRenderRig と同じく各プラグインが
// 組み立てる (GuiProcessorHost::createStageRig) が、こちらは音源の途中の音
// (StageProbe) と、CHIP DAC・FX を通したあとの音も出す。
struct GuiStageInfo
{
    StageProbe::Kind kind = StageProbe::Kind::other;

    // FM はオペレーター、RHYTHM はパッドの数
    int opCount = 0;

    // 鳴らしている音源が持っている、入れ切りできるまとまり (StageSwitch::Module)
    StageSwitch::Mask modules = 0;

    // RHYTHM のパッドの鍵
    std::array<int, StageProbe::maxOps> padNotes{};

    // CHIP DAC を通すか
    bool chipDac = false;
};

class GuiStageRig
{
public:
    virtual ~GuiStageRig() = default;

    // numSamples ぶん回す。synth にボイスを混ぜた音 (ユニゾンのあと)、dac に CHIP DAC の
    // あと、fx に FX のあと (出来上がり) を書く。途中の音は probe() に入る。
    // numSamples は 3 つのバッファの長さ以下
    virtual void render(const juce::MidiBuffer& midi, int numSamples,
        juce::AudioBuffer<float>& synth, juce::AudioBuffer<float>& dac, juce::AudioBuffer<float>& fx) = 0;

    // 鍵を離した音を、余韻を待たずに止める (オペレーターの EG を切ったとき)
    virtual void stopNote(int midiNote) = 0;

    virtual StageProbe& probe() = 0;

    // 鍵を押しているあいだは、コアが鳴り終わったと見ても鳴らし続ける (オペレーターの EG を切ったとき)
    virtual void setHoldWhileKeyDown(bool hold) = 0;

    virtual GuiStageInfo info() const = 0;
};

// ---------------------------------------------------------------- プロセッサ
class GuiProcessorHost
{
public:
    virtual ~GuiProcessorHost() = default;

    // ---- state を差し替えた回数 (プリセット・DAW の状態を読んだとき)。
    // DAW がほかのスレッドで差し替えることもあるので、画面は apvts.state へ
    // リスナーを付けずに、この回数が変わったかを見に行く (GuiGroupFolds)
    std::atomic<int> stateLoads{ 0 };

    // ---- 区分ごとのパラメータファイルの既定の置き場
    juce::String defaultWavetableDir; // 波形メモリ (WT / WT2)
    juce::String defaultChannelParamDir;
    juce::String defaultLfoParamDir;
    juce::String defaultAmpEnvParamDir;
    juce::String defaultPitchEnvParamDir;
    juce::String defaultSsgHwEnvParamDir;
    juce::String defaultWtModParamDir;
    juce::String defaultSsgSwEnvParamDir;
    juce::String defaultDetuneParamDir;
    juce::String defaultUnisonParamDir;

    // 読み込める音声ファイルの形式
    juce::AudioFormatManager formatManager;

    juce::UndoManager undoManager;

    // チャンネルごとの MODULATION 変調波形ファイルのパス。
    // キーは APVTS のプレフィックス (OPL / SSG / WT など)。
    // 波形そのものは modWaveSlots が持つので、ここは表示用。
    // 1 チャンネルにつきスロットの数だけ持つ。
    using WtModWavePaths = std::array<juce::String, Global::WtMod::slots>;
    std::map<juce::String, WtModWavePaths> modWavePaths;

    // WT PITCH MOD の変調波形。チャンネルごとに複数スロット持つ。
    // 32 サンプル × 枚数をパラメータで持つと数が膨大になるため、
    // 実データはここが所有し、state には相対パスだけを保存する。
    WtModWaveStore modWaveSlots;

    // SSG SW AMP / PITCH ENV[256] などの段の表。256 段 × 2 をパラメータで
    // 持つと数が膨大になるため、ここが持って state へ書く。
    StepTableStore stepTables;

    // 段の表の書き換えを Undo に載せるための、最後に載せた中身 (GuiStepTableUndo)
    StepTableUndo::History stepTableHistory;

    // トグルボタンの並べ方。ToggleAlign::Centred で従来どおり行の真ん中、
    // ToggleAlign::Left で左端へ寄せる。見た目だけの話で、音には影響しない。
    int toggleAlign = ToggleAlign::Centred;

    // FM のレジスタの AR / DR / SR / RR / SL / TL を、98 の N88-BASIC(86) と
    // 同じ向き (大小を逆にした値) で見せるか。見た目だけの話で、パラメータの
    // 値と音は変わらない (GuiSlider::applyN88Display)。
    bool fmN88Display = false;

    // ---- グラフの、鳴っている位置を動かして見せるか (4.3.0。GuiLiveGraph.h)
    // TAP SEQUENCER はいまの段、PARAM ENV[256] はエンベロープの位置。
    // 見た目だけの話で、音には影響しない。
    bool tapLiveGraph = true;
    int tapLiveFps = LiveGraph::fpsInitial;
    bool paramEnvLiveGraph = true;
    int paramEnvLiveFps = LiveGraph::fpsInitial;

    // AMP / PITCH ENV・SSG SW AMP / PITCH ENV[256]・FM 音源の EG (グラフの AMP / PIT / S256 / P256)
    bool envLiveGraph = true;
    int envLiveFps = LiveGraph::fpsInitial;

    // グラフが読みに行く速さ。切っているあいだは 0 (GuiEnvelopeGraph::setLiveSource)
    std::function<int()> paramEnvLiveRate()
    {
        return [this] { return paramEnvLiveGraph ? LiveGraph::clampFps(paramEnvLiveFps) : 0; };
    }

    std::function<int()> envLiveRate()
    {
        return [this] { return envLiveGraph ? LiveGraph::clampFps(envLiveFps) : 0; };
    }

    // ---- レベルメーター (4.4.0。GuiLevelMeter)。壁紙と UI のあいだに描く。見た目だけで、音は変わらない
    bool levelMeterShow = true;
    int levelMeterFps = 15;
    int levelMeterStyle = 0;      // GuiLevelMeter::Style (0 = LED, 1 = バー, 2 = 線)
    int levelMeterBands = 32;     // スペクトラムの帯の数 (16 / 32)
    bool levelMeterPeakHold = true;

    // 音の側が出し終えた音を書き、メーターが読む
    LevelMeterFeed levelMeterFeed;

    // ---- リアルタイムのオシロの縦の倍率 (4.4.0。GuiScopeZoom)。0.1〜16 倍
    float scopeZoom = 1.0f;

    // ---- 生成中の波形の窓 (4.4.0。GuiStageWave)。打鍵の並びと見せ方。パッチには入れない
    float stageBpm = 120.0f;
    bool stageFollowDaw = false;      // 「生成」を押した瞬間の DAW のテンポ・拍子を使う
    int stageBars = 1;                // 1〜16 小節
    int stageIntervalDiv = 4;         // 打鍵の間隔 (1 = 全音符, 2, 4, 8, 16, 32)。既定は 4 分音符
    bool stageIntervalTriplet = false;
    bool stageIntervalDotted = false;
    int stageLengthDiv = 4;           // 押している長さ。既定は 4 分音符
    bool stageLengthTriplet = false;
    bool stageLengthDotted = false;
    int stageMidiNote = 60;           // C3
    int stageCycles = 1;              // 枠の幅 (周期の数の選択肢の番号)
    int stageFps = 30;                // 再生の 1 秒あたりのコマ数 (15 / 30 / 60 / 120)
    bool stageLoop = true;            // 再生を最後まで行ったら頭へ戻るか
    bool stageReleaseTail = true;     // 最後に離したあとの余韻 (リリース) まで作るか。切ると打鍵の並びの長さで切る
    int stageBeatDiv = 4;             // 1 小節の拍の音符 (4 = 4 分音符, 8 = 8 分音符)
    int stageBeatCount = 4;           // 1 小節の拍の数 (1〜12)

    // DAW のテンポと拍子。音の側が書き、「生成」を押したときに画面が読む。0 なら分からない
    std::atomic<double> hostBpm{ 0.0 };
    std::atomic<int> hostTimeSigNumerator{ 4 };
    std::atomic<int> hostTimeSigDenominator{ 4 };

    // 鳴らしている音から DAW のテンポを拾う。processBlock の頭で呼ぶ
    void captureHostTempo(juce::AudioPlayHead* playHead)
    {
        if (playHead == nullptr) return;

        const auto position = playHead->getPosition();

        if (!position.hasValue()) return;

        if (const auto bpm = position->getBpm()) hostBpm.store(*bpm, std::memory_order_relaxed);

        if (const auto sig = position->getTimeSignature()) {
            hostTimeSigNumerator.store(sig->numerator, std::memory_order_relaxed);
            hostTimeSigDenominator.store(sig->denominator, std::memory_order_relaxed);
        }
    }

    // ---- 音声・波形ファイルの場所をどこからの相対で書くか (Io::PathRoot)
    // 環境設定に 1 つだけ持つ。どのチャンネルの UTILITY から変えても同じ値。
    int relativePathRoot = Io::PathRoot::settingsFolder;

    // いま読み書きしているプリセット / チャンネルパラメータのファイル。
    // 読み書きのあいだだけ PathDocumentScope で入れる。
    juce::File pathDocument;

    // 書くときの形と、書かれた場所の読み方。settingsDir は設定の置き場
    // (音声なら Samples、波形なら Wavetable)。
    juce::String toStoredPath(const juce::String& path, const juce::String& settingsDir, bool relativeToSettings) const
    {
        return Io::toStoredPath(path, relativePathRoot, pathDocument, settingsDir, relativeToSettings);
    }

    juce::File fromStoredPath(const juce::String& text, const juce::String& settingsDir) const
    {
        return Io::fromStoredPath(text, relativePathRoot, pathDocument, settingsDir);
    }

    // fromStoredPath の文字列版。組み立てられなければ、書かれていたまま返す。
    juce::String resolveStoredPath(const juce::String& text, const juce::String& settingsDir) const
    {
        const auto file = fromStoredPath(text, settingsDir);

        return file == juce::File() ? text : file.getFullPathName();
    }

    // 変調波形の読み書き。実データは modWaveSlots が持ち、
    // state へは相対パスだけを保存して読み直す。
    virtual void loadWtModWaveFile(const juce::String& code, int slot, const juce::File& file) = 0;
    virtual void unloadWtModWaveFile(const juce::String& code, int slot) = 0;

    // プラグインが使うフォルダ。既定の保存先はすべてこの中にする。
    virtual juce::File getPluginDirectory() const = 0;

    // プラグインの状態。juce::AudioProcessor の同じ名前の関数が実装を兼ねる。
    virtual void getStateInformation(juce::MemoryBlock& destData) = 0;
    virtual void setStateInformation(const void* data, int sizeInBytes) = 0;

    // 生成波形の計算に使う音源一式を、今の設定で組み立てる。メッセージスレッド
    // から呼ぶこと。音源を持たないプラグイン (2686VFX) は nullptr を返す。
    virtual std::unique_ptr<GuiRenderRig> createRenderRig(double sampleRate) = 0;

    // 生成中の波形の窓に使う音源一式。off のまとまりを切り、noModulation なら FM の
    // 変調を入れずに回す。voices はボイスの数 (様子を見るだけなら 1 でよい)。
    // メッセージスレッドから呼ぶこと。音源を持たないプラグインは nullptr を返す。
    virtual std::unique_ptr<GuiStageRig> createStageRig(double sampleRate, StageSwitch::Mask off,
        bool noModulation, int voices) = 0;
};

// プリセット / チャンネルパラメータを読み書きするあいだ置いておく。
// そのあいだ、中に書く音声・波形の場所はこのファイルを基準にできる。
class PathDocumentScope
{
    GuiProcessorHost& host;
    juce::File saved;
public:
    PathDocumentScope(GuiProcessorHost& h, const juce::File& document) : host(h), saved(h.pathDocument)
    {
        host.pathDocument = document;
    }

    ~PathDocumentScope() { host.pathDocument = saved; }

    PathDocumentScope(const PathDocumentScope&) = delete;
    PathDocumentScope& operator=(const PathDocumentScope&) = delete;
};

// ---------------------------------------------------------------- エディタ
class GuiEditorHost
{
public:
    virtual ~GuiEditorHost() = default;

    // 並べ直す。juce::Component の同じ名前の関数が実装を兼ねる。
    virtual void resized() = 0;

    // 読み込み中の表示
    virtual void showLoading(const juce::String& message = {},
        std::function<void()> onCancel = nullptr) = 0;
    virtual void updateLoading(const juce::String& message) = 0;
    virtual void hideLoading() = 0;

    // パラメータファイルを選ぶ。allowed はファイルの種類
    // (EditorGuiText::ParamBrowser::kind*)。
    //
    // 画面の中の一覧で選ばせるか、OS のダイアログを出すかはプラグインが決める
    // (2686VFX はダイアログ)。
    virtual void openParamBrowser(const juce::StringArray& allowed,
        std::function<void(const juce::File&)> onChoose) = 0;
    virtual void openParamBrowser(const juce::String& settingsDir, const juce::StringArray& allowed,
        std::function<void(const juce::File&)> onChoose,
        const juce::String& nameMustContain = {}) = 0;

    // 書き出す先を選ぶ。base は拡張子 (Io::Extension::*)。
    virtual void openParamBrowserToSave(const juce::String& settingsDir, const juce::StringArray& allowed,
        const juce::String& base, std::function<void(const juce::File&)> onChoose,
        const juce::String& nameMustContain = {}, const juce::String& defaultName = {}) = 0;

    // 波形ファイル (.wt / .wt2 など) を選ぶ
    virtual void openWaveBrowser(const juce::StringArray& allowed,
        std::function<void(const juce::File&)> onChoose) = 0;

    // チャンネルのパラメータファイルを読んで反映する。チャンネルを持たない
    // プラグインは何もせず false を返す。
    virtual bool applyChannelParamFile(const juce::File& file) = 0;

    // ---- パラメータの切り取り・コピー・貼り付け (ParamEdit)
    // パラメータの外に持っている素材 (音声・波形ファイル) の場所を集める・
    // 入れる。prefix は APVTS のプレフィックス (ADPCM / RHYTHM_PAD3 /
    // WTPLUS_SLOT2 / OPZX7_OP1 など)。キーは prefix から見た相対の名前。
    // wholeChannel のときは、チャンネルの下の枠 (WT+ / ADPCM+ の SLOT) の
    // 素材も含める。
    //
    // 素材を持たないチャンネルは何もしない。
    virtual void collectClipAssets(const juce::String& prefix, bool wholeChannel,
        std::map<juce::String, juce::String>& out)
    {
        juce::ignoreUnused(prefix, wholeChannel, out);
    }

    // clearOthers のときは、assets に無い素材を外す。
    virtual void applyClipAssets(const juce::String& prefix, bool wholeChannel,
        const std::map<juce::String, juce::String>& assets, bool clearOthers)
    {
        juce::ignoreUnused(prefix, wholeChannel, assets, clearOthers);
    }
};
