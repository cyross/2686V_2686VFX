#include "./AmpEnv.h"

#include "../../../Core/Io/ParamFile.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat ampEnvFormat{ "ampEnv", 1 };
}

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Const/ConstGlobal.h"

void GuiComponentAmpEnv::setupComponent(juce::Component& parent, const juce::String& code, int &tabOrder)
{
	cat.setupSwCategory({
        .parent = parent,
        .title = juce::String("") + "AMP ENV",
        .enableChangeDetailVisible = true
        });

    bypass.setup({ .parent = parent, .id = code + CPK::adsr + CPK::bypass, .title = "Bypass", .isReset = true });
    bypass.setWantsKeyboardFocus(true);
    bypass.setExplicitFocusOrder(++tabOrder);

	separator1.setupComponent(parent);

	startLevel.setup({ .parent = parent, .id = code + CPK::Adsr::stl, .title = "STL", .isReset = true });
	startLevel.setWantsKeyboardFocus(true);
	startLevel.setExplicitFocusOrder(++tabOrder);

	attack.setup({ .parent = parent, .id = code + CPK::Adsr::ar, .title = "AR", .isReset = true });
	attack.setWantsKeyboardFocus(true);
	attack.setExplicitFocusOrder(++tabOrder);

	decay.setup({ .parent = parent, .id = code + CPK::Adsr::dr, .title = "DR", .isReset = true });
	decay.setWantsKeyboardFocus(true);
	decay.setExplicitFocusOrder(++tabOrder);

	sustain.setup({ .parent = parent, .id = code + CPK::Adsr::sl, .title = "SL", .isReset = true });
	sustain.setWantsKeyboardFocus(true);
	sustain.setExplicitFocusOrder(++tabOrder);

	release.setup({ .parent = parent, .id = code + CPK::Adsr::rr, .title = "RR", .isReset = true });
	release.setWantsKeyboardFocus(true);
	release.setExplicitFocusOrder(++tabOrder);

	separator2.setupComponent(parent);

	kor.setup({ .parent = parent, .id = code + CPK::Adsr::kor, .title = "KOR", .isReset = true });
	kor.setWantsKeyboardFocus(true);
	kor.setExplicitFocusOrder(++tabOrder);
}

void GuiComponentAmpEnv::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

	bypass.setVisible(visible);
	separator1.setVisible(visible);
	startLevel.setVisibleWithLabel(visible);
	attack.setVisibleWithLabel(visible);
	decay.setVisibleWithLabel(visible);
	sustain.setVisibleWithLabel(visible);
	release.setVisibleWithLabel(visible);
	separator2.setVisible(visible);
	kor.setVisible(visible);

    if (visible)
    {
		layoutMain({ .mainRect = rect, .component = &bypass });
		separator1.layoutComponent(rect);
		layoutMain({ .mainRect = rect, .label = &startLevel.label, .component = &startLevel });
		layoutMain({ .mainRect = rect, .label = &attack.label, .component = &attack });
        layoutMain({ .mainRect = rect, .label = &decay.label, .component = &decay });
		layoutMain({ .mainRect = rect, .label = &sustain.label, .component = &sustain });
		layoutMain({ .mainRect = rect, .label = &release.label, .component = &release });
		separator2.layoutComponent(rect);
		layoutMain({ .mainRect = rect, .component = &kor });

		rect.removeFromTop(CoreGuiValue::Category::gapBelow);
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
	kor.setVisible(visible);

	if (visible)
	{
		layoutRow({ .rowRect = rect, .component = &bypass });
		layoutRow({ .rowRect = rect, .label = &startLevel.label, .component = &startLevel });
		layoutRow({ .rowRect = rect, .label = &attack.label, .component = &attack });
		layoutRow({ .rowRect = rect, .label = &decay.label, .component = &decay });
		layoutRow({ .rowRect = rect, .label = &sustain.label, .component = &sustain });
		layoutRow({ .rowRect = rect, .label = &release.label, .component = &release });
		layoutRow({ .rowRect = rect, .component = &kor });

		rect.removeFromTop(CoreGuiValue::Category::gapBelow);
	}
}

void GuiComponentAmpEnv::setupGraph(std::function<void()> repaintGraph) {
	bypass.onStateChange = repaintGraph;
	startLevel.onValueChange = repaintGraph;
	attack.onValueChange = repaintGraph;
	decay.onValueChange = repaintGraph;
	sustain.onValueChange = repaintGraph;
	release.onValueChange = repaintGraph;
	kor.onStateChange = repaintGraph;
}

void GuiComponentAmpEnv::updateGraph(GuiEnvelopeGraph& graph) {
	graph.updateBypass(bypass.getToggleState());

	graph.updateAmpEnv(
		startLevel,
		attack,
		decay,
		sustain,
		release,
		kor
	);
}

void GuiComponentAmpEnv::setEnabled(bool enabled) {
	bypass.setEnabled(enabled);
	attack.setEnabled(enabled);
	decay.setEnabled(enabled);
	sustain.setEnabled(enabled);
	release.setEnabled(enabled);
	startLevel.setEnabled(enabled);
	kor.setEnabled(enabled);
}

void GuiComponentAmpEnv::copyParams(CopyEnvAmpAdsr& copyObj) {
	copyObj.bypass = bypass.getToggleState();
	copyObj.ar = attack.getValue();
	copyObj.dr = decay.getValue();
	copyObj.sl = sustain.getValue();
	copyObj.rr = release.getValue();
	copyObj.stl = startLevel.getValue();
	copyObj.kor = kor.getToggleState();
}

void GuiComponentAmpEnv::pasteParams(CopyEnvAmpAdsr& copyObj) {
	bypass.setToggleState(copyObj.bypass, juce::sendNotification);
	attack.setValue(copyObj.ar, juce::sendNotification);
	decay.setValue(copyObj.dr, juce::sendNotification);
	sustain.setValue(copyObj.sl, juce::sendNotification);
	release.setValue(copyObj.rr, juce::sendNotification);
	startLevel.setValue(copyObj.stl, juce::sendNotification);
	kor.setToggleState(copyObj.kor, juce::sendNotification);
}

void GuiComponentAmpEnv::importParams() {
	juce::File defaultDir(ctx.audioProcessor.defaultAmpEnvParamDir);
	if (!defaultDir.isDirectory()) {
		defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
	}

	fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importAmpEnvParamFile, defaultDir, Io::ExtensionGlob::AmpEnvParam);
	fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
		[this](const juce::FileChooser& fc) {
			auto file = fc.getResult();
			if (file.existsAsFile()) {

				// 次回のダイアログ用にディレクトリを保存
				ctx.audioProcessor.defaultAmpEnvParamDir = file.getParentDirectory().getFullPathName();

				auto reader = Io::ParamReader::open(file, ampEnvFormat);

				if (!reader.has_value()) return;

				bypass.setToggleState(reader->getBool("bypass", bypass.getToggleState()), juce::sendNotification);
				startLevel.setValue(reader->getFloat("startLevel", (float)startLevel.getValue()), juce::sendNotification);
				attack.setValue(reader->getFloat("attack", (float)attack.getValue()), juce::sendNotification);
				decay.setValue(reader->getFloat("decay", (float)decay.getValue()), juce::sendNotification);
				sustain.setValue(reader->getFloat("sustain", (float)sustain.getValue()), juce::sendNotification);
				release.setValue(reader->getFloat("release", (float)release.getValue()), juce::sendNotification);
				kor.setToggleState(reader->getBool("kor", kor.getToggleState()), juce::sendNotification);
			}
		});
}

void GuiComponentAmpEnv::exportParams() {
	juce::File defaultDir(ctx.audioProcessor.defaultAmpEnvParamDir);
	if (!defaultDir.isDirectory()) {
		defaultDir = juce::File::getSpecialLocation(juce::File::userDocumentsDirectory);
	}

	fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportAmpEnvParamFile, defaultDir.getChildFile("default.ampEnv.json"), Io::ExtensionGlob::AmpEnvParam);
	fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
		[this](const juce::FileChooser& fc) {
			auto file = fc.getResult();
			if (file != juce::File{}) {

				// 次回のダイアログ用にディレクトリを保存
				ctx.audioProcessor.defaultAmpEnvParamDir = file.getParentDirectory().getFullPathName();

				Io::ParamWriter writer(ampEnvFormat);

				writer.set("bypass", bypass.getToggleState());
				writer.set("startLevel", (float)startLevel.getValue());
				writer.set("attack", (float)attack.getValue());
				writer.set("decay", (float)decay.getValue());
				writer.set("sustain", (float)sustain.getValue());
				writer.set("release", (float)release.getValue());
				writer.set("kor", kor.getToggleState());

				writer.writeTo(file);
			}
		});
}

void GuiComponentAmpEnv::setImportingParams(juce::StringArray& lines, int& index) {
	bypass.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
	startLevel.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	attack.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	decay.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	sustain.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	release.setValue(lines[index++].getFloatValue(), juce::sendNotification);
	kor.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
}

void GuiComponentAmpEnv::readParams(const Io::ParamReader& reader, const juce::String& prefix)
{
    Io::ParamReader r(reader, prefix);

	bypass.setToggleState(r.getBool("bypass", bypass.getToggleState()), juce::sendNotification);
	startLevel.setValue(r.getFloat("startLevel", (float)startLevel.getValue()), juce::sendNotification);
	attack.setValue(r.getFloat("attack", (float)attack.getValue()), juce::sendNotification);
	decay.setValue(r.getFloat("decay", (float)decay.getValue()), juce::sendNotification);
	sustain.setValue(r.getFloat("sustain", (float)sustain.getValue()), juce::sendNotification);
	release.setValue(r.getFloat("release", (float)release.getValue()), juce::sendNotification);
	kor.setToggleState(r.getBool("kor", kor.getToggleState()), juce::sendNotification);
}

juce::String GuiComponentAmpEnv::getExportedParams() {
	juce::String content = "";

	content += juce::String(bypass.getToggleState() ? 1 : 0) + "\n";
	content += juce::String(startLevel.getValue(), Global::floatDecimalPlaces) + "\n";
	content += juce::String(attack.getValue(), Global::floatDecimalPlaces) + "\n";
	content += juce::String(decay.getValue(), Global::floatDecimalPlaces) + "\n";
	content += juce::String(sustain.getValue(), Global::floatDecimalPlaces) + "\n";
	content += juce::String(release.getValue(), Global::floatDecimalPlaces) + "\n";
	content += juce::String(kor.getToggleState() ? 1 : 0) + "\n";

	return content;
}

void GuiComponentAmpEnv::writeParams(Io::ParamWriter& writer, const juce::String& prefix)
{
    Io::ParamWriter w(writer, prefix);

	w.set("bypass", bypass.getToggleState());
	w.set("startLevel", (float)startLevel.getValue());
	w.set("attack", (float)attack.getValue());
	w.set("decay", (float)decay.getValue());
	w.set("sustain", (float)sustain.getValue());
	w.set("release", (float)release.getValue());
	w.set("kor", kor.getToggleState());
}
