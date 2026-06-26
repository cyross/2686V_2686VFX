#include "./PitchEnv.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Const/ConstGlobal.h"

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

	attack.setup({ .parent = parent, .id = code + "_PITCH_AR", .title = "AR", .isReset = true, .labelFont = labelFont });
	attack.setWantsKeyboardFocus(true);
	attack.setExplicitFocusOrder(++tabOrder);

	decay.setup({ .parent = parent, .id = code + "_PITCH_DR", .title = "DR", .isReset = true, .labelFont = labelFont });
	decay.setWantsKeyboardFocus(true);
	decay.setExplicitFocusOrder(++tabOrder);

	release.setup({ .parent = parent, .id = code + "_PITCH_RR", .title = "RR", .isReset = true, .labelFont = labelFont });
	release.setWantsKeyboardFocus(true);
	release.setExplicitFocusOrder(++tabOrder);

	startLevel.setup({ .parent = parent, .id = code + "_PITCH_STL", .title = "STL", .isReset = true, .labelFont = labelFont });
	startLevel.setWantsKeyboardFocus(true);
	startLevel.setExplicitFocusOrder(++tabOrder);

	startLevelButtons.setupComponent(parent, startLevel, tabOrder, labelFont);

	attackLevel.setup({ .parent = parent, .id = code + "_PITCH_ATL", .title = "ATL", .isReset = true, .labelFont = labelFont });
	attackLevel.setWantsKeyboardFocus(true);
	attackLevel.setExplicitFocusOrder(++tabOrder);

	attackLevelButtons.setupComponent(parent, attackLevel, tabOrder, labelFont);

	sustainLevel.setup({ .parent = parent, .id = code + "_PITCH_SSL", .title = "SSL", .isReset = true, .labelFont = labelFont });
	sustainLevel.setWantsKeyboardFocus(true);
	sustainLevel.setExplicitFocusOrder(++tabOrder);

	sustainLevelButtons.setupComponent(parent, sustainLevel, tabOrder, labelFont);

	releaseLevel.setup({ .parent = parent, .id = code + "_PITCH_RLL", .title = "RLL", .isReset = true, .labelFont = labelFont });
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
		layoutMain({ .mainRect = rect, .label = &attack.label, .component = &attack, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &decay.label, .component = &decay, .rowHeight = 13 });
        layoutMain({ .mainRect = rect, .label = &release.label, .component = &release, .rowHeight = 13 });
		layoutMain({ .mainRect = rect, .label = &startLevel.label, .component = &startLevel, .rowHeight = 13 });
		startLevelButtons.layoutComponent(rect, 13);
		layoutMain({ .mainRect = rect, .label = &attackLevel.label, .component = &attackLevel, .rowHeight = 13 });
		attackLevelButtons.layoutComponent(rect, 13);
		layoutMain({ .mainRect = rect, .label = &sustainLevel.label, .component = &sustainLevel, .rowHeight = 13 });
		sustainLevelButtons.layoutComponent(rect, 13);
		layoutMain({ .mainRect = rect, .label = &releaseLevel.label, .component = &releaseLevel, .rowHeight = 13 });
		releaseLevelButtons.layoutComponent(rect, 13);
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
		layoutRow({ .rowRect = rect, .label = &attack.label, .component = &attack, .rowHeight = 12 });
		layoutRow({ .rowRect = rect, .label = &decay.label, .component = &decay, .rowHeight = 12 });
		layoutRow({ .rowRect = rect, .label = &release.label, .component = &release, .rowHeight = 12 });
		layoutRow({ .rowRect = rect, .label = &startLevel.label, .component = &startLevel, .rowHeight = 12 });
		startLevelButtons.layoutComponentRow(rect, 12);
		layoutRow({ .rowRect = rect, .label = &attackLevel.label, .component = &attackLevel, .rowHeight = 12 });
		attackLevelButtons.layoutComponentRow(rect, 12);
		layoutRow({ .rowRect = rect, .label = &sustainLevel.label, .component = &sustainLevel, .rowHeight = 12 });
		sustainLevelButtons.layoutComponentRow(rect, 12);
		layoutRow({ .rowRect = rect, .label = &releaseLevel.label, .component = &releaseLevel, .rowHeight = 12 });
		releaseLevelButtons.layoutComponentRow(rect, 12);
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

void GuiComponentPitchEnv::copyParams(CopyEnvPitchAdsr& copyObj) {
	copyObj.flag = flag.getToggleState();
	copyObj.ar = attack.getValue();
	copyObj.dr = decay.getValue();
	copyObj.rr = release.getValue();
	copyObj.stl = startLevel.getValue();
	copyObj.atl = attackLevel.getValue();
	copyObj.ssl = sustainLevel.getValue();
	copyObj.rll = releaseLevel.getValue();
}

void GuiComponentPitchEnv::pasteParams(CopyEnvPitchAdsr& copyObj) {
	flag.setToggleState(copyObj.flag, juce::sendNotification);
	attack.setValue(copyObj.ar, juce::sendNotification);
	decay.setValue(copyObj.dr, juce::sendNotification);
	release.setValue(copyObj.rr, juce::sendNotification);
	startLevel.setValue(copyObj.stl, juce::sendNotification);
	attackLevel.setValue(copyObj.atl, juce::sendNotification);
	sustainLevel.setValue(copyObj.ssl, juce::sendNotification);
	releaseLevel.setValue(copyObj.rll, juce::sendNotification);
}

void GuiComponentPitchEnv::importParams() {
	juce::File defaultDir(ctx.audioProcessor.defaultPitchEnvParamDir);
	if (!defaultDir.isDirectory()) {
		defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
	}

	fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importPitchEnvParamFile, defaultDir, Io::ExtensionGlob::PitchEnvParam);
	fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
		[this](const juce::FileChooser& fc) {
			auto file = fc.getResult();
			if (file.existsAsFile()) {

				// 次回のダイアログ用にディレクトリを保存
				ctx.audioProcessor.defaultPitchEnvParamDir = file.getParentDirectory().getFullPathName();

				juce::StringArray lines;
				file.readLines(lines);

				int size = lines.size();

				if (size < 8) return;

				flag.setToggleState(lines[0].getIntValue() == 1, juce::sendNotification);
				attack.setValue(lines[1].getFloatValue(), juce::sendNotification);
				decay.setValue(lines[2].getFloatValue(), juce::sendNotification);
				release.setValue(lines[3].getFloatValue(), juce::sendNotification);
				startLevel.setValue(lines[4].getFloatValue(), juce::sendNotification);
				attackLevel.setValue(lines[5].getFloatValue(), juce::sendNotification);
				sustainLevel.setValue(lines[6].getFloatValue(), juce::sendNotification);
				releaseLevel.setValue(lines[7].getFloatValue(), juce::sendNotification);
			}
		});
}

void GuiComponentPitchEnv::exportParams() {
	juce::File defaultDir(ctx.audioProcessor.defaultPitchEnvParamDir);
	if (!defaultDir.isDirectory()) {
		defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
	}

	fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportPitchEnvParamFile, defaultDir.getChildFile("default.pitchEnv"), Io::ExtensionGlob::PitchEnvParam);
	fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
		[this](const juce::FileChooser& fc) {
			auto file = fc.getResult();
			if (file != juce::File{}) {

				// 次回のダイアログ用にディレクトリを保存
				ctx.audioProcessor.defaultPitchEnvParamDir = file.getParentDirectory().getFullPathName();

				juce::String content = "";

				content += juce::String(flag.getToggleState() ? 1 : 0) + "\n";
				content += juce::String(attack.getValue(), Global::floatDecimalPlaces) + "\n";
				content += juce::String(decay.getValue(), Global::floatDecimalPlaces) + "\n";
				content += juce::String(release.getValue(), Global::floatDecimalPlaces) + "\n";
				content += juce::String(startLevel.getValue(), Global::floatDecimalPlaces) + "\n";
				content += juce::String(attackLevel.getValue(), Global::floatDecimalPlaces) + "\n";
				content += juce::String(sustainLevel.getValue(), Global::floatDecimalPlaces) + "\n";
				content += juce::String(releaseLevel.getValue(), Global::floatDecimalPlaces) + "\n";

				file.replaceWithText(content);
			}
		});
}
