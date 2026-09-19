#pragma once

#include <JuceHeader.h>

#include "../../../Core/Io/ParamFile.h"
#include <vector>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiValues.h"
#include "../../../Gui/Components/Separator/NormalSeparator.h"
#include "../WavePreview/WavePreview.h"

#include "Shared/Core/Gui/GuiCopyObj.h"

// N88 LFO のチップ全体側 (速さ・形・PM・AM)。OPN と OPNA のタブで使う。
//
// オペレーターごとの AMS は GuiComponentN88LfoOp が持つ。
class GuiComponentN88Lfo : public GuiBase {
    GuiCategoryLabel cat;
    GuiSlider freq;
    GuiComboBox shape;
    GuiSlider amSmRt;

    // Shape は 1 つだが、同じ番号でも PM と AM で波形が違うので両方出す。
    GuiWavePreview pmPreview;
    GuiWavePreview amPreview;
    GuiSlider syncDelay;
    GuiTextButton syncDelayToZero;
    GuiTextButton syncDelayToOne;
    GuiToggleButton pmEnable;
    GuiToggleButton amEnable;
    GuiSlider pmd;
    GuiSlider pms;
    GuiSlider amd;
    NormalSeparator sep1;
    NormalSeparator sep2;

public:
    GuiComponentN88Lfo(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        freq(context),
        shape(context),
        amSmRt(context),
        pmPreview(context),
        amPreview(context),
        syncDelay(context),
        syncDelayToZero(context),
        syncDelayToOne(context),
        pmEnable(context),
        amEnable(context),
        pmd(context),
        pms(context),
        amd(context),
        sep1(context),
        sep2(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder);
    void layoutComponent(juce::Rectangle<int>& rect);
    void updatePreviews();

    // PM / AM それぞれ専用のもの (深さ・波形など) はその札に従う。両方で
    // 使うもの (速さ・形・同期) は、どちらかが入っているあいだだけ押せる。
    void applyActive();

    void copyParams(CopyLfoN88& copyObj);
    void pasteParams(CopyLfoN88& copyObj);

    // CH Params。名前で受け渡す。
    void readChParams(const Io::ParamReader& reader);
    void writeChParams(Io::ParamWriter& writer);

    // 3.0.0 より前の CH Params (行の並び)
    void setImportingChParams(juce::StringArray& lines, int& index);

    // N88 LFO のファイル。CH Params と違い、形は並びの番号、数値は整数で持つ。
    void readFileParams(const Io::ParamReader& reader);
    void writeFileParams(Io::ParamWriter& writer);

    // 3.0.0 より前の N88 LFO のファイル (先頭 9 行)
    void setImportingFileParams(const juce::StringArray& lines);
};
