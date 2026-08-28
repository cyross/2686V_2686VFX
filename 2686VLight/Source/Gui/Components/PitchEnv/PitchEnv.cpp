#include "./PitchEnv.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Const/ConstGlobal.h"

void GuiComponentPitchEnv::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder, const juce::String& flagKey, const juce::String& flagText, bool isEnable)
{
	this->isEnable = isEnable;

	cat.setupSwCategory({
        .parent = parent,
        .title = juce::String("") + "PITCH ENV",
        .enableChangeDetailVisible = true
        });

	flag.setup({ .parent = parent, .id = code + flagKey, .title = flagText, .isReset = true });
    flag.setWantsKeyboardFocus(true);
    flag.setExplicitFocusOrder(++tabOrder);

	flagSeparator.setupComponent(parent);

	attack.setupComponent(parent, code + CPK::PitchAdsr::ar, "AR", tabOrder, std::nullopt, labelFont);

	attackNudge.setupComponent(parent, attack.getSlider(), tabOrder);

	decay.setupComponent(parent, code + CPK::PitchAdsr::dr, "DR", tabOrder, std::nullopt, labelFont);

	decayNudge.setupComponent(parent, decay.getSlider(), tabOrder);

	release.setupComponent(parent, code + CPK::PitchAdsr::rr, "RR", tabOrder, std::nullopt, labelFont);

	releaseNudge.setupComponent(parent, release.getSlider(), tabOrder);

	rateSeparator.setupComponent(parent);

	startLevel.setupComponent(parent, code + CPK::PitchAdsr::stl, "STL", tabOrder, std::nullopt, labelFont);

	startLevelButtons.setupComponent(parent, startLevel.getSlider(), tabOrder, labelFont);

	attackLevel.setupComponent(parent, code + CPK::PitchAdsr::atl, "ATL", tabOrder, std::nullopt, labelFont);

	attackLevelButtons.setupComponent(parent, attackLevel.getSlider(), tabOrder, labelFont);

	sustainLevel.setupComponent(parent, code + CPK::PitchAdsr::ssl, "SSL", tabOrder, std::nullopt, labelFont);

	sustainLevelButtons.setupComponent(parent, sustainLevel.getSlider(), tabOrder, labelFont);

	releaseLevel.setupComponent(parent, code + CPK::PitchAdsr::rll, "RLL", tabOrder, std::nullopt, labelFont);

	releaseLevelButtons.setupComponent(parent, releaseLevel.getSlider(), tabOrder, labelFont);
}

void GuiComponentPitchEnv::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

	flag.setVisible(visible);
	flagSeparator.setVisible(visible);
	attack.setVisibleWithLabel(visible);
	attackNudge.setVisibles(visible && attack.isVisibleNudge());
	decay.setVisibleWithLabel(visible);
	decayNudge.setVisibles(visible && decay.isVisibleNudge());
	release.setVisibleWithLabel(visible);
	releaseNudge.setVisibles(visible && release.isVisibleNudge());
	rateSeparator.setVisible(visible);
	startLevel.setVisibleWithLabel(visible);
	startLevelButtons.setVisibles(visible && startLevel.isVisibleNudge());
	attackLevel.setVisibleWithLabel(visible);
	attackLevelButtons.setVisibles(visible && attackLevel.isVisibleNudge());
	sustainLevel.setVisibleWithLabel(visible);
	sustainLevelButtons.setVisibles(visible && sustainLevel.isVisibleNudge());
	releaseLevel.setVisibleWithLabel(visible);
	releaseLevelButtons.setVisibles(visible && releaseLevel.isVisibleNudge());

    if (visible)
    {
		layoutMain({ .mainRect = rect, .component = &flag });
		flagSeparator.layoutComponent(rect);
		attack.layoutComponent(rect, 13);
		if (attack.isVisibleNudge()) attackNudge.layoutComponent(rect, 13);
        decay.layoutComponent(rect, 13);
        if (decay.isVisibleNudge()) decayNudge.layoutComponent(rect, 13);
        release.layoutComponent(rect, 13);
        if (release.isVisibleNudge()) releaseNudge.layoutComponent(rect, 13);
		rateSeparator.layoutComponent(rect);
		startLevel.layoutComponent(rect, 13);
		if (startLevel.isVisibleNudge()) startLevelButtons.layoutComponent(rect, 13);
		attackLevel.layoutComponent(rect, 13);
		if (attackLevel.isVisibleNudge()) attackLevelButtons.layoutComponent(rect, 13);
		sustainLevel.layoutComponent(rect, 13);
		if (sustainLevel.isVisibleNudge()) sustainLevelButtons.layoutComponent(rect, 13);
		releaseLevel.layoutComponent(rect, 13);
		if (releaseLevel.isVisibleNudge()) releaseLevelButtons.layoutComponent(rect, 13);

		rect.removeFromTop(CoreGuiValue::Category::gapBelow);
	}
}

void GuiComponentPitchEnv::layoutComponentRow(juce::Rectangle<int>& rect)
{
	layoutMainCategory({ .mainRect = rect, .component = &cat });

	bool visible = cat.isDetailVisible();

	flag.setVisible(visible);
	flagSeparator.setVisible(visible);
	attack.setVisibleWithLabel(visible);
	attackNudge.setVisibles(visible && attack.isVisibleNudge());
	decay.setVisibleWithLabel(visible);
	decayNudge.setVisibles(visible && decay.isVisibleNudge());
	release.setVisibleWithLabel(visible);
	releaseNudge.setVisibles(visible && release.isVisibleNudge());
	rateSeparator.setVisible(visible);
	startLevel.setVisibleWithLabel(visible);
	startLevelButtons.setVisibles(visible && startLevel.isVisibleNudge());
	attackLevel.setVisibleWithLabel(visible);
	attackLevelButtons.setVisibles(visible && attackLevel.isVisibleNudge());
	sustainLevel.setVisibleWithLabel(visible);
	sustainLevelButtons.setVisibles(visible && sustainLevel.isVisibleNudge());
	releaseLevel.setVisibleWithLabel(visible);
	releaseLevelButtons.setVisibles(visible && releaseLevel.isVisibleNudge());

	if (visible)
	{
		layoutRow({ .rowRect = rect, .component = &flag });
		flagSeparator.layoutComponent(rect);
		attack.layoutComponentRow(rect, 12);
		if (attack.isVisibleNudge()) attackNudge.layoutComponentRow(rect, 12);
		decay.layoutComponentRow(rect, 12);
		if (decay.isVisibleNudge()) decayNudge.layoutComponentRow(rect, 12);
		release.layoutComponentRow(rect, 12);
		if (release.isVisibleNudge()) releaseNudge.layoutComponentRow(rect, 12);
		rateSeparator.layoutComponent(rect);
		startLevel.layoutComponentRow(rect, 12);
		if (startLevel.isVisibleNudge()) startLevelButtons.layoutComponentRow(rect, 12);
		attackLevel.layoutComponentRow(rect, 12);
		if (attackLevel.isVisibleNudge()) attackLevelButtons.layoutComponentRow(rect, 12);
		sustainLevel.layoutComponentRow(rect, 12);
		if (sustainLevel.isVisibleNudge()) sustainLevelButtons.layoutComponentRow(rect, 12);
		releaseLevel.layoutComponentRow(rect, 12);
		if (releaseLevel.isVisibleNudge()) releaseLevelButtons.layoutComponentRow(rect, 12);

		rect.removeFromTop(CoreGuiValue::Category::gapBelow);
	}
}

void GuiComponentPitchEnv::setupGraph(std::function<void()> repaintGraph) {
	flag.onStateChange = repaintGraph;
	attack.getSlider().onValueChange = repaintGraph;
	decay.getSlider().onValueChange = repaintGraph;
	release.getSlider().onValueChange = repaintGraph;
	startLevel.getSlider().onValueChange = repaintGraph;
	attackLevel.getSlider().onValueChange = repaintGraph;
	sustainLevel.getSlider().onValueChange = repaintGraph;
	releaseLevel.getSlider().onValueChange = repaintGraph;
}

void GuiComponentPitchEnv::updateGraph(GuiEnvelopeGraph& graph) {
	graph.updateBypass(this->isEnable ? !flag.getToggleState() : flag.getToggleState());

	graph.updatePitchEnv(
		attack.getSlider(),
		decay.getSlider(),
		release.getSlider(),
		startLevel.getSlider(),
		attackLevel.getSlider(),
		sustainLevel.getSlider(),
		releaseLevel.getSlider()
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

void GuiComponentPitchEnv::setImportingParams(juce::StringArray& lines, int& index) {
	flag.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	attack.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	decay.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	release.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	startLevel.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	attackLevel.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	sustainLevel.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	releaseLevel.setValue(lines[index++].getFloatValue(), juce::sendNotification);
}

juce::String GuiComponentPitchEnv::getExportedParams() {
	juce::String content = "";

	content += juce::String(flag.getToggleState() ? 1 : 0) + "\n";
	content += juce::String(attack.getValue(), Global::floatDecimalPlaces) + "\n";
	content += juce::String(decay.getValue(), Global::floatDecimalPlaces) + "\n";
	content += juce::String(release.getValue(), Global::floatDecimalPlaces) + "\n";
	content += juce::String(startLevel.getValue(), Global::floatDecimalPlaces) + "\n";
	content += juce::String(attackLevel.getValue(), Global::floatDecimalPlaces) + "\n";
	content += juce::String(sustainLevel.getValue(), Global::floatDecimalPlaces) + "\n";
	content += juce::String(releaseLevel.getValue(), Global::floatDecimalPlaces) + "\n";

	return content;
}
