#include "./AmpEnv.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"

void GuiComponentAmpEnv::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder)
{
	cat.setupSwCategory({
        .parent = parent,
        .title = juce::String("") + "[■]--- AMP ENV ---",
        .invisibleTitle = juce::String("") + "[□]--- AMP ENV ---",
        .enableChangeDetailVisible = true
        });

    bypass.setup({ .parent = parent, .id = code + "_ADSR_BYPASS", .title = "Bypass", .isReset = true });
    bypass.setWantsKeyboardFocus(true);
    bypass.setExplicitFocusOrder(++tabOrder);

	startLevel.setup({ .parent = parent, .id = code + "_STL", .title = "Stl", .isReset = true });
	startLevel.setWantsKeyboardFocus(true);
	startLevel.setExplicitFocusOrder(++tabOrder);

	attack.setup({ .parent = parent, .id = code + "_AR", .title = "Ar", .isReset = true });
	attack.setWantsKeyboardFocus(true);
	attack.setExplicitFocusOrder(++tabOrder);

	decay.setup({ .parent = parent, .id = code + "_DR", .title = "Dr", .isReset = true });
	decay.setWantsKeyboardFocus(true);
	decay.setExplicitFocusOrder(++tabOrder);

	sustain.setup({ .parent = parent, .id = code + "_SL", .title = "Sl", .isReset = true });
	sustain.setWantsKeyboardFocus(true);
	sustain.setExplicitFocusOrder(++tabOrder);

	release.setup({ .parent = parent, .id = code + "_RR", .title = "Rr", .isReset = true });
	release.setWantsKeyboardFocus(true);
	release.setExplicitFocusOrder(++tabOrder);
}

void GuiComponentAmpEnv::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

	bypass.setVisible(visible);
	startLevel.setVisibleWithLabel(visible);
	attack.setVisibleWithLabel(visible);
	decay.setVisibleWithLabel(visible);
	sustain.setVisibleWithLabel(visible);
	release.setVisibleWithLabel(visible);

    if (visible)
    {
		layoutMain({ .mainRect = rect, .component = &bypass });
		layoutMain({ .mainRect = rect, .label = &startLevel.label, .component = &startLevel });
		layoutMain({ .mainRect = rect, .label = &attack.label, .component = &attack });
        layoutMain({ .mainRect = rect, .label = &decay.label, .component = &decay });
		layoutMain({ .mainRect = rect, .label = &sustain.label, .component = &sustain });
		layoutMain({ .mainRect = rect, .label = &release.label, .component = &release });
    }
}

void GuiComponentAmpEnv::layoutComponentRow(juce::Rectangle<int>& rect)
{
	layoutMainCategory({ .mainRect = rect, .component = &cat });

	bool visible = cat.isDetailVisible();

	bypass.setVisible(visible);
	startLevel.setVisibleWithLabel(visible);
	attack.setVisibleWithLabel(visible);
	decay.setVisibleWithLabel(visible);
	sustain.setVisibleWithLabel(visible);
	release.setVisibleWithLabel(visible);

	if (visible)
	{
		layoutRow({ .rowRect = rect, .component = &bypass });
		layoutRow({ .rowRect = rect, .label = &startLevel.label, .component = &startLevel });
		layoutRow({ .rowRect = rect, .label = &attack.label, .component = &attack });
		layoutRow({ .rowRect = rect, .label = &decay.label, .component = &decay });
		layoutRow({ .rowRect = rect, .label = &sustain.label, .component = &sustain });
		layoutRow({ .rowRect = rect, .label = &release.label, .component = &release });
	}
}

void GuiComponentAmpEnv::setupGraph(std::function<void()> repaintGraph) {
	bypass.onStateChange = repaintGraph;
	startLevel.onValueChange = repaintGraph;
	attack.onValueChange = repaintGraph;
	decay.onValueChange = repaintGraph;
	sustain.onValueChange = repaintGraph;
	release.onValueChange = repaintGraph;
}

void GuiComponentAmpEnv::updateGraph(GuiEnvelopeGraph& graph, CurveCore* p_curveCore, bool isCurveMode, int posIdx) {
	graph.updateBypass(bypass.getToggleState());

	graph.updateAmpEnv(
		startLevel,
		attack,
		decay,
		sustain,
		release,
		p_curveCore,
		isCurveMode,
		posIdx
	);
}

void GuiComponentAmpEnv::setEnabled(bool enabled) {
	bypass.setEnabled(enabled);
	attack.setEnabled(enabled);
	decay.setEnabled(enabled);
	sustain.setEnabled(enabled);
	release.setEnabled(enabled);
	startLevel.setEnabled(enabled);
}
