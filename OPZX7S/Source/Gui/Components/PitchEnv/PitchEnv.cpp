#include "./PitchEnv.h"

#include "../../../Core/Gui/GuiRefresh.h"

#include "../../../Core/Io/ParamFile.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat pitchEnvFormat{ "pitchEnv", 1 };
}

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

void GuiComponentPitchEnv::updateGraph(GuiEnvelopeGraph& graph, CurveCore* p_curveCore, bool isCurveMode, int posIdx) {
	graph.updateBypass(this->isEnable ? !flag.getToggleState() : flag.getToggleState());

	graph.updatePitchEnv(
		attack.getSlider(),
		decay.getSlider(),
		release.getSlider(),
		startLevel.getSlider(),
		attackLevel.getSlider(),
		sustainLevel.getSlider(),
		releaseLevel.getSlider(),
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
		defaultDir = ctx.audioProcessor.getPluginDirectory();
	}

	fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importPitchEnvParamFile, defaultDir, Io::ExtensionGlob::PitchEnvParam);
	fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
		[this](const juce::FileChooser& fc) {
			auto file = fc.getResult();
			if (file.existsAsFile()) {

				// 次回のダイアログ用にディレクトリを保存
				ctx.audioProcessor.defaultPitchEnvParamDir = file.getParentDirectory().getFullPathName();

				auto reader = Io::ParamReader::open(file, pitchEnvFormat);

				if (!reader.has_value()) return;

				// 読み終えてからまとめて描き直す。値を 1 つ入れるたびに
				// 波形を作り直すと、項目の多いファイルでは目に見えて遅くなる。
				GuiRefresh::Batch batch;

				flag.setToggleState(reader->getBool("flag", flag.getToggleState()), juce::sendNotification);
				attack.setValue(reader->getFloat("attack", (float)attack.getValue()), juce::sendNotification);
				decay.setValue(reader->getFloat("decay", (float)decay.getValue()), juce::sendNotification);
				release.setValue(reader->getFloat("release", (float)release.getValue()), juce::sendNotification);
				startLevel.setValue(reader->getFloat("startLevel", (float)startLevel.getValue()), juce::sendNotification);
				attackLevel.setValue(reader->getFloat("attackLevel", (float)attackLevel.getValue()), juce::sendNotification);
				sustainLevel.setValue(reader->getFloat("sustainLevel", (float)sustainLevel.getValue()), juce::sendNotification);
				releaseLevel.setValue(reader->getFloat("releaseLevel", (float)releaseLevel.getValue()), juce::sendNotification);
			}
		});
}

void GuiComponentPitchEnv::exportParams() {
	juce::File defaultDir(ctx.audioProcessor.defaultPitchEnvParamDir);
	if (!defaultDir.isDirectory()) {
		defaultDir = ctx.audioProcessor.getPluginDirectory();
	}

	fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportPitchEnvParamFile, defaultDir.getChildFile("default.pitchEnv.json"), Io::ExtensionGlob::PitchEnvParam);
	fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
		[this](const juce::FileChooser& fc) {
			auto file = fc.getResult();
			if (file != juce::File{}) {

				// 次回のダイアログ用にディレクトリを保存
				ctx.audioProcessor.defaultPitchEnvParamDir = file.getParentDirectory().getFullPathName();

				Io::ParamWriter writer(pitchEnvFormat);

				writer.set("flag", flag.getToggleState());
				writer.set("attack", (float)attack.getValue());
				writer.set("decay", (float)decay.getValue());
				writer.set("release", (float)release.getValue());
				writer.set("startLevel", (float)startLevel.getValue());
				writer.set("attackLevel", (float)attackLevel.getValue());
				writer.set("sustainLevel", (float)sustainLevel.getValue());
				writer.set("releaseLevel", (float)releaseLevel.getValue());

				writer.writeTo(file);
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

void GuiComponentPitchEnv::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

	flag.setToggleState(r.getBool("flag", flag.getToggleState()), juce::sendNotification);
	attack.setValue(r.getFloat("attack", (float)attack.getValue()), juce::sendNotification);
	decay.setValue(r.getFloat("decay", (float)decay.getValue()), juce::sendNotification);
	release.setValue(r.getFloat("release", (float)release.getValue()), juce::sendNotification);
	startLevel.setValue(r.getFloat("startLevel", (float)startLevel.getValue()), juce::sendNotification);
	attackLevel.setValue(r.getFloat("attackLevel", (float)attackLevel.getValue()), juce::sendNotification);
	sustainLevel.setValue(r.getFloat("sustainLevel", (float)sustainLevel.getValue()), juce::sendNotification);
	releaseLevel.setValue(r.getFloat("releaseLevel", (float)releaseLevel.getValue()), juce::sendNotification);
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

void GuiComponentPitchEnv::writeParams(Io::ParamWriter& writer, const juce::String& key)
{
    auto w = writer.child(key);

	w.set("flag", flag.getToggleState());
	w.set("attack", (float)attack.getValue());
	w.set("decay", (float)decay.getValue());
	w.set("release", (float)release.getValue());
	w.set("startLevel", (float)startLevel.getValue());
	w.set("attackLevel", (float)attackLevel.getValue());
	w.set("sustainLevel", (float)sustainLevel.getValue());
	w.set("releaseLevel", (float)releaseLevel.getValue());
}
