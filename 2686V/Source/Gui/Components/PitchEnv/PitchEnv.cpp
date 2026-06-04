#include "./PitchEnv.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"

void GuiComponentPitchEnv::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable)
{
	this->isEnable = isEnable;

	cat.setupOtherCategory({
        .parent = parent,
        .title = juce::String("") + "[■]--- PITCH ENV ---",
        .invisibleTitle = juce::String("") + "[□]--- PITCH ENV ---",
        .enableChangeDetailVisible = true
        });

    flag.setup({ .parent = parent, .id = code + flagKey, .title = flagText, .isReset = true });
    flag.setWantsKeyboardFocus(true);
    flag.setExplicitFocusOrder(++tabOrder);

	attack.setup({ .parent = parent, .id = code + "_PITCH_AR", .title = "Ar", .isReset = true });
	attack.setWantsKeyboardFocus(true);
	attack.setExplicitFocusOrder(++tabOrder);

	decay.setup({ .parent = parent, .id = code + "_PITCH_DR", .title = "Dr", .isReset = true });
	decay.setWantsKeyboardFocus(true);
	decay.setExplicitFocusOrder(++tabOrder);

	release.setup({ .parent = parent, .id = code + "_PITCH_RR", .title = "Rr", .isReset = true });
	release.setWantsKeyboardFocus(true);
	release.setExplicitFocusOrder(++tabOrder);

	startLevel.setup({ .parent = parent, .id = code + "_PITCH_STL", .title = "Stl", .isReset = true });
	startLevel.setWantsKeyboardFocus(true);
	startLevel.setExplicitFocusOrder(++tabOrder);

	attackLevel.setup({ .parent = parent, .id = code + "_PITCH_ATL", .title = "Atl", .isReset = true });
	attackLevel.setWantsKeyboardFocus(true);
	attackLevel.setExplicitFocusOrder(++tabOrder);

	sustainLevel.setup({ .parent = parent, .id = code + "_PITCH_SSL", .title = "Ssl", .isReset = true });
	sustainLevel.setWantsKeyboardFocus(true);
	sustainLevel.setExplicitFocusOrder(++tabOrder);

	releaseLevel.setup({ .parent = parent, .id = code + "_PITCH_RLL", .title = "Rll", .isReset = true });
	releaseLevel.setWantsKeyboardFocus(true);
	releaseLevel.setExplicitFocusOrder(++tabOrder);
}

void GuiComponentPitchEnv::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

	flag.setVisible(visible);
	attack.setVisibleWithLabel(visible);
	decay.setVisibleWithLabel(visible);
	release.setVisibleWithLabel(visible);
	startLevel.setVisibleWithLabel(visible);
	attackLevel.setVisibleWithLabel(visible);
	sustainLevel.setVisibleWithLabel(visible);
	releaseLevel.setVisibleWithLabel(visible);

    if (visible)
    {
		layoutMain({ .mainRect = rect, .component = &flag });
		layoutMain({ .mainRect = rect, .label = &attack.label, .component = &attack });
        layoutMain({ .mainRect = rect, .label = &decay.label, .component = &decay });
        layoutMain({ .mainRect = rect, .label = &release.label, .component = &release });
		layoutMain({ .mainRect = rect, .label = &startLevel.label, .component = &startLevel });
		layoutMain({ .mainRect = rect, .label = &attackLevel.label, .component = &attackLevel });
		layoutMain({ .mainRect = rect, .label = &sustainLevel.label, .component = &sustainLevel });
		layoutMain({ .mainRect = rect, .label = &releaseLevel.label, .component = &releaseLevel });
    }
}

void GuiComponentPitchEnv::layoutComponentRow(juce::Rectangle<int>& rect)
{
	layoutMainCategory({ .mainRect = rect, .component = &cat });

	bool visible = cat.isDetailVisible();

	flag.setVisible(visible);
	attack.setVisibleWithLabel(visible);
	decay.setVisibleWithLabel(visible);
	release.setVisibleWithLabel(visible);
	startLevel.setVisibleWithLabel(visible);
	attackLevel.setVisibleWithLabel(visible);
	sustainLevel.setVisibleWithLabel(visible);
	releaseLevel.setVisibleWithLabel(visible);

	if (visible)
	{
		layoutRow({ .rowRect = rect, .component = &flag });
		layoutRow({ .rowRect = rect, .label = &attack.label, .component = &attack });
		layoutRow({ .rowRect = rect, .label = &decay.label, .component = &decay });
		layoutRow({ .rowRect = rect, .label = &release.label, .component = &release });
		layoutRow({ .rowRect = rect, .label = &startLevel.label, .component = &startLevel });
		layoutRow({ .rowRect = rect, .label = &attackLevel.label, .component = &attackLevel });
		layoutRow({ .rowRect = rect, .label = &sustainLevel.label, .component = &sustainLevel });
		layoutRow({ .rowRect = rect, .label = &releaseLevel.label, .component = &releaseLevel });
	}
}

void GuiComponentPitchEnv::setupGraph(std::function<void()> repaintGraph) {
	flag.onStateChange = repaintGraph;
	attack.onValueChange = repaintGraph;
	decay.onValueChange = repaintGraph;
	release.onValueChange = repaintGraph;
	startLevel.onValueChange = repaintGraph;
	attackLevel.onValueChange = repaintGraph;
	sustainLevel.onValueChange = repaintGraph;
	releaseLevel.onValueChange = repaintGraph;
}

void GuiComponentPitchEnv::updateGraph(GuiEnvelopeGraph& graph, CurveCore* p_curveCore, bool isCurveMode, int posIdx) {
	graph.updateBypass(this->isEnable ? !flag.getToggleState() : flag.getToggleState());

	graph.updatePitchEnv(
		attack,
		decay,
		release,
		startLevel,
		attackLevel,
		sustainLevel,
		releaseLevel,
		p_curveCore,
		isCurveMode,
		posIdx
	);
}

void GuiComponentPitchEnv::setEnabled(bool enabled) {
	flag.setEnabled(enabled);
	attack.setEnabled(enabled);
	decay.setEnabled(enabled);
	release.setEnabled(enabled);
	startLevel.setEnabled(enabled);
	attackLevel.setEnabled(enabled);
	sustainLevel.setEnabled(enabled);
	releaseLevel.setEnabled(enabled);
}
