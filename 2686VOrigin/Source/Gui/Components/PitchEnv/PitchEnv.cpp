#include "./PitchEnv.h"
#include "../../../Core/Editor/EditorGuiText.h"

#include "../../../Core/Editor/PluginEditor.h"

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

	cat.setupSwPitchCategory({
        .parent = parent,
        .title = juce::String("") + "PITCH ENV",
        .enableChangeDetailVisible = true
        });

	m_flagKey = flagKey;

	flag.setup({ .parent = parent, .id = code + flagKey, .title = flagText, .isReset = true });
    flag.setWantsKeyboardFocus(true);
    flag.setExplicitFocusOrder(++tabOrder);

	flagSeparator.setupComponent(parent);

	keep.setup({ .parent = parent, .id = code + CPK::PitchAdsr::keep, .title = "KEEP", .isReset = true });
	keep.setWantsKeyboardFocus(true);
	keep.setExplicitFocusOrder(++tabOrder);

	keepSeparator.setupComponent(parent);

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

	endLevelEnable.setup({ .parent = parent, .id = code + CPK::PitchAdsr::endlEnable, .title = "Use Endl", .isReset = true });
	endLevelEnable.setWantsKeyboardFocus(true);
	endLevelEnable.setExplicitFocusOrder(++tabOrder);
	endLevelEnable.onClick = [this] { applyEndLevelEnable(); };

	endLevel.setupComponent(parent, code + CPK::PitchAdsr::endl, "ENDL", tabOrder, std::nullopt, labelFont);

	endLevelButtons.setupComponent(parent, endLevel.getSlider(), tabOrder, labelFont);

	applyEndLevelEnable();
}

// 束縛先を丸ごと差し替える。
//
// 同じ部品を並べる代わりに 1 つだけ置き、TARGET で指し先を切り替える
// ための口。setup で組んだ見た目はそのままに、APVTS への繋ぎだけを
// 張り替える。
void GuiComponentPitchEnv::rebind(const juce::String& code)
{
    // 入り切りの鍵は呼ぶ側が決めるので、setup で受けたものを使う。
    flag.rebind(code + m_flagKey);
    keep.rebind(code + CPK::PitchAdsr::keep);

    attack.getSlider().rebind(code + CPK::PitchAdsr::ar);
    decay.getSlider().rebind(code + CPK::PitchAdsr::dr);
    release.getSlider().rebind(code + CPK::PitchAdsr::rr);

    startLevel.getSlider().rebind(code + CPK::PitchAdsr::stl);
    attackLevel.getSlider().rebind(code + CPK::PitchAdsr::atl);
    sustainLevel.getSlider().rebind(code + CPK::PitchAdsr::ssl);
    releaseLevel.getSlider().rebind(code + CPK::PitchAdsr::rll);

    endLevelEnable.rebind(code + CPK::PitchAdsr::endlEnable);
    endLevel.getSlider().rebind(code + CPK::PitchAdsr::endl);
}

void GuiComponentPitchEnv::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

	flag.setVisible(visible);
	flagSeparator.setVisible(visible);
	keep.setVisible(visible);
	keepSeparator.setVisible(visible);
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
	endLevelEnable.setVisible(visible);
	endLevel.setVisibleWithLabel(visible);
	endLevelButtons.setVisibles(visible && endLevel.isVisibleNudge());

    if (visible)
    {
		layoutMain({ .mainRect = rect, .component = &flag });
		flagSeparator.layoutComponent(rect);
		layoutMain({ .mainRect = rect, .component = &keep });
		keepSeparator.layoutComponent(rect);
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
		layoutMain({ .mainRect = rect, .component = &endLevelEnable });
		endLevel.layoutComponent(rect, 13);
		if (endLevel.isVisibleNudge()) endLevelButtons.layoutComponent(rect, 13);

		rect.removeFromTop(CoreGuiValue::Category::gapBelow);
	}
}

void GuiComponentPitchEnv::layoutComponentRow(juce::Rectangle<int>& rect)
{
	layoutMainCategory({ .mainRect = rect, .component = &cat });

	bool visible = cat.isDetailVisible();

	flag.setVisible(visible);
	flagSeparator.setVisible(visible);
	keep.setVisible(visible);
	keepSeparator.setVisible(visible);
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
	endLevelEnable.setVisible(visible);
	endLevel.setVisibleWithLabel(visible);
	endLevelButtons.setVisibles(visible && endLevel.isVisibleNudge());

	if (visible)
	{
		layoutRow({ .rowRect = rect, .component = &flag });
		flagSeparator.layoutComponent(rect);
		layoutRow({ .rowRect = rect, .component = &keep });
		keepSeparator.layoutComponent(rect);
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
		layoutRow({ .rowRect = rect, .component = &endLevelEnable });
		endLevel.layoutComponentRow(rect, 12);
		if (endLevel.isVisibleNudge()) endLevelButtons.layoutComponentRow(rect, 12);

		rect.removeFromTop(CoreGuiValue::Category::gapBelow);
	}
}

// ENDL を使わないときは、つまみを押せなくする。効いていないものが
// 触れてしまうと、動かしたのに音が変わらない、という形で迷う。
void GuiComponentPitchEnv::applyEndLevelEnable() {
	const bool on = endLevelEnable.getToggleState();

	endLevel.setEnabled(on);
	endLevelButtons.setEnables(on);
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
	endLevel.getSlider().onValueChange = repaintGraph;
	endLevelEnable.onStateChange = repaintGraph;
	keep.onStateChange = repaintGraph;
}

void GuiComponentPitchEnv::updateGraph(GuiEnvelopeGraph& graph) {
	graph.updateBypass(this->isEnable ? !flag.getToggleState() : flag.getToggleState());

	// KEEP のときはカーブを効かせない。音の側も補間そのものを止めてある。
	const bool keepOn = keep.getToggleState();

	graph.setKeepLevels(keepOn);

	// つまみが出している値と幅を、そのまま束にして渡す。
	GuiEnvelopeGraph::PitchEnvValues v;

	v.ar = (float)attack.getSlider().getValue();
	v.dr = (float)decay.getSlider().getValue();
	v.rr = (float)release.getSlider().getValue();

	v.arMax = (float)attack.getSlider().getMaximum();
	v.drMax = (float)decay.getSlider().getMaximum();
	v.rrMax = (float)release.getSlider().getMaximum();

	v.stl = (float)startLevel.getSlider().getValue();
	v.atl = (float)attackLevel.getSlider().getValue();
	v.ssl = (float)sustainLevel.getSlider().getValue();
	v.rll = (float)releaseLevel.getSlider().getValue();

	graph.updatePitchEnv(
		v
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
	endLevelEnable.setEnabled(enabled);
	endLevel.setEnabled(enabled && endLevelEnable.getToggleState());
	keep.setEnabled(enabled);
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
	copyObj.endl = (int)endLevel.getValue();
	copyObj.endlEnable = endLevelEnable.getToggleState();
	copyObj.keep = keep.getToggleState();
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
	endLevel.setValue(copyObj.endl, juce::sendNotification);
	endLevelEnable.setToggleState(copyObj.endlEnable, juce::sendNotification);
	keep.setToggleState(copyObj.keep, juce::sendNotification);
}

void GuiComponentPitchEnv::importParams()
{
	// ファイルを選ぶダイアログではなく、一覧から選ぶ画面を出す。
	// 読めるのはこの区分だけなので、ほかは選べない。
	ctx.editor.openParamBrowser(ctx.audioProcessor.defaultPitchEnvParamDir,
		{ EditorGuiText::ParamBrowser::kindPitchEnv },
		[this](const juce::File& file) { applyParamFile(file); });
}

// ブラウザから直に渡せるよう、ダイアログを出すところと
// 読んで反映するところを分けてある。
void GuiComponentPitchEnv::applyParamFile(const juce::File& file)
{
	if (!file.existsAsFile()) return;


	// 次回のダイアログ用にディレクトリを保存
	ctx.audioProcessor.defaultPitchEnvParamDir = file.getParentDirectory().getFullPathName();

	// 3.0.0 より前のファイルは、当時の処理で読み込んでから
	// 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
	// 読み込みは当時のものをそのまま使う。
	if (Io::isLegacyFile(file)) {
		juce::StringArray lines;

		file.readLines(lines);

		int index = 0;

		{
			// 読み終えてからまとめて描き直す
			GuiRefresh::Batch batch;

			setImportingParams(lines, index);
		}

		// 単体のファイルは入れ子にせず、そのまま中身として書く
		Io::ParamWriter writer(pitchEnvFormat);

		writeParams(writer, Io::ParamKey::values);
		writer.hoist(Io::ParamKey::values);

		Io::writeConverted(file, writer);

		return;
	}

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
	endLevel.setValue(reader->getFloat("endLevel", (float)endLevel.getValue()), juce::sendNotification);
	endLevelEnable.setToggleState(reader->getBool("endLevelEnable", endLevelEnable.getToggleState()), juce::sendNotification);
	keep.setToggleState(reader->getBool("keep", keep.getToggleState()), juce::sendNotification);
}

void GuiComponentPitchEnv::exportParams()
{
    // 書き出す先も一覧から決める。名前は下の欄で直せる。
    ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultPitchEnvParamDir,
        { EditorGuiText::ParamBrowser::kindPitchEnv }, Io::Extension::PitchEnvParam,
        [this](const juce::File& file) { writeParamsFile(file); });
}

// ブラウザから直に渡せるよう、書き出す先を決めるところと
// 実際に書くところを分けてある。
void GuiComponentPitchEnv::writeParamsFile(const juce::File& file)
{
    if (file == juce::File{}) return;

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
	writer.set("endLevel", (float)endLevel.getValue());
	writer.set("endLevelEnable", endLevelEnable.getToggleState());
	writer.set("keep", keep.getToggleState());

	writer.writeTo(file);
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
	endLevel.setValue(r.getFloat("endLevel", (float)endLevel.getValue()), juce::sendNotification);
	endLevelEnable.setToggleState(r.getBool("endLevelEnable", endLevelEnable.getToggleState()), juce::sendNotification);
	keep.setToggleState(r.getBool("keep", keep.getToggleState()), juce::sendNotification);
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
	w.set("endLevel", (float)endLevel.getValue());
	w.set("endLevelEnable", endLevelEnable.getToggleState());
	w.set("keep", keep.getToggleState());
}
