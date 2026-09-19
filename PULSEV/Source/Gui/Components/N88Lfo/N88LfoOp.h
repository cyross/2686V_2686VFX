#pragma once

#include <JuceHeader.h>

#include "../../../Core/Io/ParamFile.h"

#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Core/Gui/GuiValues.h"

#include "../../../Core/Gui/GuiCopyObj.h"

// N88 LFO のオペレーター側 (AMS)。OPN と OPNA のタブで使う。
//
// チップ全体側は GuiComponentN88Lfo が持つ。
class GuiComponentN88LfoOp : public GuiBase {
    GuiCategoryLabel cat;
    GuiSlider ams;

public:
    GuiComponentN88LfoOp(const GuiContext& context) :
        GuiBase(context),
        cat(context),
        ams(context)
    {
    }

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder);

    // 束縛先を丸ごと差し替える。TARGET で指し先を切り替えるときに使う。
    void rebind(const juce::String& code);
    void layoutComponentRow(juce::Rectangle<int>& rect);

    void copyParams(CopyLfoN88Op& copyObj);
    void pasteParams(CopyLfoN88Op& copyObj);

    // オペレーターの CH Params。名前で受け渡す。
    void readParams(const Io::ParamReader& reader);
    void writeParams(Io::ParamWriter& writer);

    // 3.0.0 より前のオペレーターの CH Params (行の並び)
    void setImportingParams(juce::StringArray& lines, int& index);

    // N88 LFO のファイルは AMS を整数で持つ
    int getAms() const { return (int)ams.getValue(); }
    void setAms(int value) { ams.setValue(value, juce::sendNotification); }
};
