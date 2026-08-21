#pragma once

#include <JuceHeader.h>
#include <array>
#include <vector>
#include <functional>

#include "../../../Core/Const/ConstGlobal.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Gui/GuiBase.h"
#include "../../../Core/Gui/GuiContext.h"
#include "../../../Core/Gui/GuiValues.h"
#include "../../../Core/Gui/GuiEnvelopeGraph.h"
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
    juce::String getExportedParams();
};
