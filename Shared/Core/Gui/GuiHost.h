#pragma once

#include <JuceHeader.h>

#include <array>
#include <functional>
#include <map>
#include <memory>

#include "./GuiToggleAlign.h"
#include "../Synth/WtModWave.h"

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

// ---------------------------------------------------------------- プロセッサ
class GuiProcessorHost
{
public:
    virtual ~GuiProcessorHost() = default;

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

    // トグルボタンの並べ方。ToggleAlign::Centred で従来どおり行の真ん中、
    // ToggleAlign::Left で左端へ寄せる。見た目だけの話で、音には影響しない。
    int toggleAlign = ToggleAlign::Centred;

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
};
