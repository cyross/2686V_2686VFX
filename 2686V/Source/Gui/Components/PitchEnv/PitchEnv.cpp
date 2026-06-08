#include "./PitchEnv.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"

void GuiComponentPitchEnv::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable)
{
	this->isEnable = isEnable;

	cat.setupSwCategory({
        .parent = parent,
        .title = juce::String("") + "[■]--- PITCH ENV ---",
        .invisibleTitle = juce::String("") + "[□]--- PITCH ENV ---",
        .enableChangeDetailVisible = true
        });

	flag.setup({ .parent = parent, .id = code + flagKey, .title = flagText, .isReset = true });
    flag.setWantsKeyboardFocus(true);
    flag.setExplicitFocusOrder(++tabOrder);

	attack.setup({ .parent = parent, .id = code + "_PITCH_AR", .title = "Ar", .isReset = true, .labelFont = labelFont });
	attack.setWantsKeyboardFocus(true);
	attack.setExplicitFocusOrder(++tabOrder);

	decay.setup({ .parent = parent, .id = code + "_PITCH_DR", .title = "Dr", .isReset = true, .labelFont = labelFont });
	decay.setWantsKeyboardFocus(true);
	decay.setExplicitFocusOrder(++tabOrder);

	release.setup({ .parent = parent, .id = code + "_PITCH_RR", .title = "Rr", .isReset = true, .labelFont = labelFont });
	release.setWantsKeyboardFocus(true);
	release.setExplicitFocusOrder(++tabOrder);

	startLevel.setup({ .parent = parent, .id = code + "_PITCH_STL", .title = "Stl", .isReset = true, .labelFont = labelFont });
	startLevel.setWantsKeyboardFocus(true);
	startLevel.setExplicitFocusOrder(++tabOrder);

	startLevelButtons.setupComponent(parent, startLevel, tabOrder, labelFont);

	attackLevel.setup({ .parent = parent, .id = code + "_PITCH_ATL", .title = "Atl", .isReset = true, .labelFont = labelFont });
	attackLevel.setWantsKeyboardFocus(true);
	attackLevel.setExplicitFocusOrder(++tabOrder);

	attackLevelButtons.setupComponent(parent, attackLevel, tabOrder, labelFont);

	sustainLevel.setup({ .parent = parent, .id = code + "_PITCH_SSL", .title = "Ssl", .isReset = true, .labelFont = labelFont });
	sustainLevel.setWantsKeyboardFocus(true);
	sustainLevel.setExplicitFocusOrder(++tabOrder);

	sustainLevelButtons.setupComponent(parent, sustainLevel, tabOrder, labelFont);

	releaseLevel.setup({ .parent = parent, .id = code + "_PITCH_RLL", .title = "Rll", .isReset = true, .labelFont = labelFont });
	releaseLevel.setWantsKeyboardFocus(true);
	releaseLevel.setExplicitFocusOrder(++tabOrder);

	releaseLevelButtons.setupComponent(parent, releaseLevel, tabOrder, labelFont);
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
	startLevelButtons.setVisibles(visible);
	attackLevel.setVisibleWithLabel(visible);
	attackLevelButtons.setVisibles(visible);
	sustainLevel.setVisibleWithLabel(visible);
	sustainLevelButtons.setVisibles(visible);
	releaseLevel.setVisibleWithLabel(visible);
	releaseLevelButtons.setVisibles(visible);

    if (visible)
    {
		layoutMain({ .mainRect = rect, .component = &flag });
		layoutMain({ .mainRect = rect, .label = &attack.label, .component = &attack, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &decay.label, .component = &decay, .rowHeight = 10 });
        layoutMain({ .mainRect = rect, .label = &release.label, .component = &release, .rowHeight = 10 });
		layoutMain({ .mainRect = rect, .label = &startLevel.label, .component = &startLevel, .rowHeight = 10 });
		startLevelButtons.layoutComponent(rect, 10);
		layoutMain({ .mainRect = rect, .label = &attackLevel.label, .component = &attackLevel, .rowHeight = 10 });
		attackLevelButtons.layoutComponent(rect, 10);
		layoutMain({ .mainRect = rect, .label = &sustainLevel.label, .component = &sustainLevel, .rowHeight = 10 });
		sustainLevelButtons.layoutComponent(rect, 10);
		layoutMain({ .mainRect = rect, .label = &releaseLevel.label, .component = &releaseLevel, .rowHeight = 10 });
		releaseLevelButtons.layoutComponent(rect, 10);
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
	startLevelButtons.setVisibles(visible);
	attackLevel.setVisibleWithLabel(visible);
	attackLevelButtons.setVisibles(visible);
	sustainLevel.setVisibleWithLabel(visible);
	sustainLevelButtons.setVisibles(visible);
	releaseLevel.setVisibleWithLabel(visible);
	releaseLevelButtons.setVisibles(visible);

	if (visible)
	{
		layoutRow({ .rowRect = rect, .component = &flag });
		layoutRow({ .rowRect = rect, .label = &attack.label, .component = &attack, .rowHeight = 8 });
		layoutRow({ .rowRect = rect, .label = &decay.label, .component = &decay, .rowHeight = 8 });
		layoutRow({ .rowRect = rect, .label = &release.label, .component = &release, .rowHeight = 8 });
		layoutRow({ .rowRect = rect, .label = &startLevel.label, .component = &startLevel, .rowHeight = 8 });
		startLevelButtons.layoutComponentRow(rect, 10);
		layoutRow({ .rowRect = rect, .label = &attackLevel.label, .component = &attackLevel, .rowHeight = 8 });
		attackLevelButtons.layoutComponentRow(rect, 10);
		layoutRow({ .rowRect = rect, .label = &sustainLevel.label, .component = &sustainLevel, .rowHeight = 8 });
		sustainLevelButtons.layoutComponentRow(rect, 10);
		layoutRow({ .rowRect = rect, .label = &releaseLevel.label, .component = &releaseLevel, .rowHeight = 8 });
		releaseLevelButtons.layoutComponentRow(rect, 10);
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
	cat.setEnabled(enabled);
	flag.setEnabled(enabled);
	attack.setEnabled(enabled);
	decay.setEnabled(enabled);
	release.setEnabled(enabled);
	startLevel.setEnabled(enabled);
	attackLevel.setEnabled(enabled);
	sustainLevel.setEnabled(enabled);
	releaseLevel.setEnabled(enabled);
}
