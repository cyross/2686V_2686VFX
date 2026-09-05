#pragma once

#include <JuceHeader.h>

#include "../../../Core/Io/ParamFile.h"
#include <array>
#include <vector>
#include <functional>

#include "../../../Core/Const/ConstGlobal.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Core/Gui/GuiValues.h"
#include "../../../Core/Gui/GuiEnvelopeGraph.h"
#include "../../../Gui/Curve/GuiCurve.h"
#include "../../../Advanced/Curve/AdvancedCurve.h"
#include "../../../Gui/Components/Separator/NormalSeparator.h"
#include "../../../Gui/Components/Separator/ShortSeparator.h"

class Quality : public GuiBase {
    GuiCategoryLabel qualityCat;
    GuiComboBox bitSelector;
    GuiComboBox rateSelector;
public:
    Quality(const GuiContext& context) :
        GuiBase(context),
        qualityCat(context),
        bitSelector(context),
        rateSelector(context)
    {
    }

	static std::vector<SelectItem> bdItems;
	static std::vector<SelectItem> rateItems;

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    int getBit() const { return bitSelector.getSelectedItemIndex(); }
    int getRate() const { return rateSelector.getSelectedItemIndex(); }
    void setBit(int index) { bitSelector.setSelectedItemIndex(index, juce::sendNotification); }
    void setRate(int index) { rateSelector.setSelectedItemIndex(index, juce::sendNotification); }
	void setVisibles(bool visible) {
		qualityCat.setVisible(visible);
		bitSelector.setVisibleWithLabel(visible);
		rateSelector.setVisibleWithLabel(visible);
	}
    void setEnableds(bool enabled) {
        qualityCat.setEnabled(enabled);
        bitSelector.setEnabled(enabled);
        rateSelector.setEnabled(enabled);
    }
    void setImportingParams(juce::StringArray& lines, int& index);

    // 名前で受け渡す。行の並びに頼ると、呼ぶ順番を間違えたときに
    // 黙って別の値が入り、項目を足すと後ろが全部ずれるため。
    void readParams(const Io::ParamReader& reader, const juce::String& key);
    void writeParams(Io::ParamWriter& writer, const juce::String& key);
    juce::String getExportedParams();
};
