#pragma once

#include <JuceHeader.h>

#include <functional>
#include <vector>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiContext.h"

// ============================================================================
// DAC (実機の出力段に合わせて BIT・RATE・INTERP をまとめて切り替える)
// ============================================================================
// 「DAC」のラベル、機種を選ぶコンボボックス、「適応」ボタンを 1 行に並べる。
// ボタンを押すと、選んだ機種の値を BIT・RATE・INTERP へ入れる。
//
// 選んだ機種は画面だけのもので、パラメータにもファイルにも書かない。
// 残るのは、入れた BIT・RATE・INTERP の値だけ。
class GuiComponentDac
{
public:
    // どの一覧から選ぶか。入れる先のコンボボックスの中身が違う。
    enum class Kind
    {
        Quality,    // 音源の QUALITY (BIT RATE は 4〜24bit)
        QualityPcm, // PCM の QUALITY (BIT RATE に ADPCM などの形式がある)
        FxPcm,      // 2686VFX の PCM ビットクラッシャー (BIT は PCM の頭の 12 個)
    };

    // 入れる値。どれもコンボボックスの項目の位置 (0 始まり)。
    struct Values
    {
        int bit;
        int rate;
        int interp;
    };

    struct Preset
    {
        juce::String name;
        Values values;
    };

    // 同じ値になる機種は、1 つの項目にまとめて「98/88」のように名前を並べる。
    static const std::vector<Preset>& presets(Kind kind);

    GuiComponentDac(const GuiContext& context);

    // onApply は「適応」を押したときに呼ぶ。選んでいなければ呼ばない。
    void setupComponent(juce::Component& parent, Kind kind, int& tabOrder,
        std::function<void(const Values&)> onApply);

    // ラベル・コンボボックス・ボタンの幅は PCM ファイルを読む行に倣う。
    // 行の幅に合わせたいときは、それぞれの幅を渡す。
    void layoutComponent(juce::Rectangle<int>& rect, int rowHeight, int paddingTop, int paddingBottom,
        int labelWidth, int comboWidth, int buttonWidth);

    void setVisibles(bool visible);
    void setEnableds(bool enabled);

private:
    GuiComboBox selector;
    GuiTextButton applyBtn;

    Kind kind = Kind::Quality;
    std::function<void(const Values&)> applyValues;
};
