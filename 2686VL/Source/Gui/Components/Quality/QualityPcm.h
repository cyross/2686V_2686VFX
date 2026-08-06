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

class QualityPcm : public GuiBase {
    GuiCategoryLabel qualityCat;
    GuiComboBox modeSelector;
    GuiComboBox rateSelector;
    GuiComboBox interpSelector;
public:
    QualityPcm(const GuiContext& context) :
        GuiBase(context),
        qualityCat(context),
		modeSelector(context),
		rateSelector(context),
		interpSelector(context)
    {
    }

    static std::vector<SelectItem> qualityItems;
    static std::vector<SelectItem> rateItems;
    static std::vector<SelectItem> interpItems;

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
	int getMode() const { return modeSelector.getSelectedItemIndex(); }
	int getRate() const { return rateSelector.getSelectedItemIndex(); }
	int getInterp() const { return interpSelector.getSelectedItemIndex(); }
	void setMode(int index) { modeSelector.setSelectedItemIndex(index, juce::sendNotification); }
	void setRate(int index) { rateSelector.setSelectedItemIndex(index, juce::sendNotification); }
	void setInterp(int index) { interpSelector.setSelectedItemIndex(index, juce::sendNotification); }
	void setVisibles(bool visible) {
		qualityCat.setVisible(visible);
		modeSelector.setVisibleWithLabel(visible);
		rateSelector.setVisibleWithLabel(visible);
		interpSelector.setVisibleWithLabel(visible);
	}
	void setEnableds(bool enabled) {
		qualityCat.setEnabled(enabled);
		modeSelector.setEnabled(enabled);
		rateSelector.setEnabled(enabled);
		interpSelector.setEnabled(enabled);
	}
};
