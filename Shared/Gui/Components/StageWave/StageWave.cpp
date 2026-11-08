#include "./StageWave.h"

#include <algorithm>
#include <cmath>

#include "Shared/Core/Editor/EditorStageWaveText.h"
#include "Shared/Core/Gui/GuiColor.h"

namespace
{
	constexpr int itemGap = 4;
	constexpr int groupGap = 14;

	// 様子を見直す間隔 (コマの数)。60 コマで 2 秒ほど
	constexpr int infoRefreshTicks = 60;

	// RHYTHM (音程の無い音) で 1 周期とみなす長さ
	constexpr double rhythmCycleSeconds = 0.01;

	juce::String noteName(int note)
	{
		return juce::MidiMessage::getMidiNoteName(note, true, true, 3);
	}

	juce::String divisionName(int division)
	{
		if (division <= 1) return EditorGuiText::StageWave::whole;

		return juce::String(division) + juce::String(EditorGuiText::StageWave::fraction);
	}

	int divisionIndex(int division)
	{
		for (int i = 0; i < (int)StageWaveRender::divisions.size(); ++i) {
			if (StageWaveRender::divisions[(size_t)i] == division) return i;
		}

		return 0;
	}

	bool dawFollowAvailable()
	{
		return !juce::JUCEApplicationBase::isStandaloneApp();
	}

	void styleLabel(juce::Label& label, const juce::String& text)
	{
		label.setText(text, juce::dontSendNotification);
		label.setFont(juce::FontOptions(12.0f));
		label.setJustificationType(juce::Justification::centredLeft);
	}
}

// ============================================================================
// 組み立て
// ============================================================================
GuiStageWave::GuiStageWave(const GuiContext& context) : ctx(context)
{
	auto& host = ctx.audioProcessor;

	generateBtn.setButtonText(EditorGuiText::StageWave::generate);
	generateBtn.setTooltip(EditorGuiText::StageWave::generateTooltip);
	generateBtn.onClick = [this] { startGenerate(); };
	addAndMakeVisible(generateBtn);

	deleteBtn.setButtonText(EditorGuiText::StageWave::remove);
	deleteBtn.onClick = [this] { clearResult(); };
	deleteBtn.setEnabled(false);
	addAndMakeVisible(deleteBtn);

	// ---- キー (RHYTHM では出さない)
	styleLabel(keyLabel, EditorGuiText::StageWave::key);
	addAndMakeVisible(keyLabel);

	keySlider.setSliderStyle(juce::Slider::LinearHorizontal);
	keySlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 56, 18);
	keySlider.setRange(0.0, 127.0, 1.0);
	keySlider.textFromValueFunction = [](double v) { return noteName((int)v); };
	keySlider.valueFromTextFunction = [](const juce::String& text) {
		for (int n = 0; n < 128; ++n) {
			if (noteName(n).equalsIgnoreCase(text.trim())) return (double)n;
		}

		return (double)text.getIntValue();
	};
	keySlider.setDoubleClickReturnValue(true, 60.0);
	keySlider.onValueChange = [this] { ctx.audioProcessor.stageMidiNote = (int)keySlider.getValue(); };
	addAndMakeVisible(keySlider);

	// ---- 再生の FPS
	styleLabel(fpsLabel, EditorGuiText::StageWave::fps);
	fpsLabel.setTooltip(EditorGuiText::StageWave::fpsTooltip);
	addAndMakeVisible(fpsLabel);

	for (int i = 0; i < (int)fpsChoices.size(); ++i) fpsBox.addItem(juce::String(fpsChoices[(size_t)i]), fpsChoices[(size_t)i]);

	fpsBox.setTooltip(EditorGuiText::StageWave::fpsTooltip);
	fpsBox.onChange = [this] {
		if (fpsBox.getSelectedId() > 0) ctx.audioProcessor.stageFps = fpsBox.getSelectedId();

		applyFps();
	};
	addAndMakeVisible(fpsBox);

	// ---- テンポ
	styleLabel(tempoLabel, EditorGuiText::StageWave::tempo);
	addAndMakeVisible(tempoLabel);

	tempoSlider.setSliderStyle(juce::Slider::IncDecButtons);
	tempoSlider.setTextBoxStyle(juce::Slider::TextBoxLeft, false, 56, 18);
	tempoSlider.setRange(20.0, 300.0, 0.1);
	tempoSlider.setNumDecimalPlacesToDisplay(1);
	tempoSlider.onValueChange = [this] { ctx.audioProcessor.stageBpm = (float)tempoSlider.getValue(); };
	addAndMakeVisible(tempoSlider);

	for (int i = 0; i < (int)tempoPresets.size(); ++i) tempoPresetBox.addItem(juce::String(tempoPresets[(size_t)i]), i + 1);

	tempoPresetBox.setTextWhenNothingSelected("BPM");
	tempoPresetBox.onChange = [this] {
		const int index = tempoPresetBox.getSelectedItemIndex();

		if (index >= 0) tempoSlider.setValue(tempoPresets[(size_t)index], juce::sendNotificationSync);
	};
	addAndMakeVisible(tempoPresetBox);

	followDawToggle.setButtonText(EditorGuiText::StageWave::followDaw);
	followDawToggle.setTooltip(EditorGuiText::StageWave::followDawTooltip);
	followDawToggle.setEnabled(dawFollowAvailable());
	followDawToggle.onClick = [this] {
		ctx.audioProcessor.stageFollowDaw = followDawToggle.getToggleState();
		tempoSlider.setEnabled(!(followDawToggle.getToggleState() && dawFollowAvailable()));
	};
	addAndMakeVisible(followDawToggle);

	// ---- 小節数
	styleLabel(barsLabel, EditorGuiText::StageWave::bars);
	addAndMakeVisible(barsLabel);

	for (int i = 1; i <= 16; ++i) barsBox.addItem(juce::String(i), i);

	barsBox.onChange = [this] { ctx.audioProcessor.stageBars = juce::jlimit(1, 16, barsBox.getSelectedId()); };
	addAndMakeVisible(barsBox);

	// ---- 間隔と音符
	setupNoteControls(intervalControls, EditorGuiText::StageWave::interval,
		host.stageIntervalDiv, host.stageIntervalTriplet, host.stageIntervalDotted);
	setupNoteControls(lengthControls, EditorGuiText::StageWave::length,
		host.stageLengthDiv, host.stageLengthTriplet, host.stageLengthDotted);

	// ---- 入れ切り
	styleLabel(switchesLabel, EditorGuiText::StageWave::switches);
	switchesLabel.setTooltip(EditorGuiText::StageWave::switchesTooltip);
	addAndMakeVisible(switchesLabel);

	for (int m = 0; m < StageSwitch::count; ++m)
	{
		auto& t = moduleToggles[(size_t)m];

		t.setButtonText(EditorGuiText::StageWave::moduleNames[m]);
		t.setTooltip(EditorGuiText::StageWave::switchesTooltip);
		t.setToggleState(true, juce::dontSendNotification);
		addChildComponent(t);
	}

	// ---- RHYTHM の第 3 段に混ぜるパッド
	styleLabel(padsLabel, EditorGuiText::StageWave::pads);
	addChildComponent(padsLabel);

	for (int i = 0; i < StageProbe::maxOps; ++i)
	{
		auto& t = padToggles[(size_t)i];

		t.setButtonText("PAD" + juce::String(i + 1));
		t.setToggleState(true, juce::dontSendNotification);
		addChildComponent(t);
	}

	// 縦の倍率は再生の段に置かず、周期のボタンの右に置く
	m_player.setZoomInRow(false);

	syncFromHost();
	applyColours();

	setSize(contentWidth, contentHeight);

	refreshInfo();

	applyFps();
}

GuiStageWave::~GuiStageWave()
{
	stopTimer();

	// 走っている計算が終わるまで待つ。待たずに壊すと、出来上がりを書き戻す先が無くなる
	m_pool.removeAllJobs(true, 8000);
}

void GuiStageWave::setupNoteControls(NoteControls& c, const juce::String& title, int& division, bool& triplet, bool& dotted)
{
	styleLabel(c.label, title);
	addAndMakeVisible(c.label);

	for (int i = 0; i < (int)StageWaveRender::divisions.size(); ++i) {
		c.division.addItem(divisionName(StageWaveRender::divisions[(size_t)i]), i + 1);
	}

	c.division.onChange = [&c, &division] {
		const int index = c.division.getSelectedItemIndex();

		if (index >= 0) division = StageWaveRender::divisions[(size_t)index];
	};
	addAndMakeVisible(c.division);

	c.triplet.setButtonText(EditorGuiText::StageWave::triplet);
	c.triplet.onClick = [&c, &triplet] { triplet = c.triplet.getToggleState(); };
	addAndMakeVisible(c.triplet);

	c.dotted.setButtonText(EditorGuiText::StageWave::dotted);
	c.dotted.onClick = [&c, &dotted] { dotted = c.dotted.getToggleState(); };
	addAndMakeVisible(c.dotted);
}

// 色を部品へ写す。部品の色は写したときの値が残るので、COLORS で変えたら写し直す
void GuiStageWave::applyColours()
{
	using namespace GuiColor::GenWave;

	generateBtn.setColour(juce::TextButton::buttonColourId, GenerateBg);
	generateBtn.setColour(juce::TextButton::textColourOffId, GenerateText);
	generateBtn.setColour(juce::TextButton::textColourOnId, GenerateText);

	deleteBtn.setColour(juce::TextButton::buttonColourId, DeleteBg);
	deleteBtn.setColour(juce::TextButton::textColourOffId, DeleteText);
	deleteBtn.setColour(juce::TextButton::textColourOnId, DeleteText);

	for (auto* label : { &keyLabel, &fpsLabel, &tempoLabel, &barsLabel, &intervalControls.label, &lengthControls.label, &switchesLabel, &padsLabel }) {
		label->setColour(juce::Label::textColourId, RowText);
	}

	for (auto* box : { &fpsBox, &tempoPresetBox, &barsBox, &intervalControls.division, &lengthControls.division })
	{
		box->setColour(juce::ComboBox::backgroundColourId, WindowControlBg);
		box->setColour(juce::ComboBox::textColourId, WindowControlText);
		box->setColour(juce::ComboBox::outlineColourId, WindowControlOutline);
		box->setColour(juce::ComboBox::arrowColourId, WindowControlText);
	}

	for (auto* slider : { &keySlider, &tempoSlider })
	{
		slider->setColour(juce::Slider::textBoxBackgroundColourId, WindowControlBg);
		slider->setColour(juce::Slider::textBoxTextColourId, WindowControlText);
		slider->setColour(juce::Slider::textBoxOutlineColourId, WindowControlOutline);
		slider->setColour(juce::Slider::backgroundColourId, WindowControlBg);
		slider->setColour(juce::Slider::trackColourId, WindowControlAccent);
		slider->setColour(juce::Slider::thumbColourId, WindowControlAccent);
	}

	std::vector<juce::ToggleButton*> toggles{ &followDawToggle,
		&intervalControls.triplet, &intervalControls.dotted, &lengthControls.triplet, &lengthControls.dotted };

	for (auto& t : moduleToggles) toggles.push_back(&t);
	for (auto& t : padToggles) toggles.push_back(&t);

	for (auto* t : toggles)
	{
		t->setColour(juce::ToggleButton::textColourId, RowText);
		t->setColour(juce::ToggleButton::tickColourId, WindowControlAccent);
		t->setColour(juce::ToggleButton::tickDisabledColourId, WindowControlOutline);
	}

	repaint();
}

void GuiStageWave::applyFps()
{
	int fps = ctx.audioProcessor.stageFps;

	if (std::find(fpsChoices.begin(), fpsChoices.end(), fps) == fpsChoices.end()) fps = 30;

	ctx.audioProcessor.stageFps = fps;

	// コマの番号が変わるので、見ている位置を保ったまま頭から数え直す
	const double nowMs = juce::Time::getMillisecondCounterHiRes();
	const auto* wave = timeline();
	const bool paused = m_player.isPaused();
	const double seconds = (wave != nullptr) ? m_player.frame(*wave, nowMs) * m_player.frameSamples(*wave) / wave->sampleRate : 0.0;

	m_player.setFramesPerSecond(fps);

	if (wave != nullptr)
	{
		m_player.showFrame(*wave, (int)std::floor(seconds * wave->sampleRate / m_player.frameSamples(*wave)), nowMs);

		if (!paused) m_player.resume(wave, nowMs);
	}

	// 描き直しはコマの速さに合わせる
	startTimerHz(fps);

	repaint();
}

// 設定ファイルから読んだ値を画面へ映す
void GuiStageWave::syncFromHost()
{
	auto& host = ctx.audioProcessor;

	keySlider.setValue(juce::jlimit(0, 127, host.stageMidiNote), juce::dontSendNotification);
	tempoSlider.setValue(juce::jlimit(20.0, 300.0, (double)host.stageBpm), juce::dontSendNotification);
	followDawToggle.setToggleState(host.stageFollowDaw && dawFollowAvailable(), juce::dontSendNotification);
	tempoSlider.setEnabled(!followDawToggle.getToggleState());
	barsBox.setSelectedId(juce::jlimit(1, 16, host.stageBars), juce::dontSendNotification);

	intervalControls.division.setSelectedItemIndex(divisionIndex(host.stageIntervalDiv), juce::dontSendNotification);
	intervalControls.triplet.setToggleState(host.stageIntervalTriplet, juce::dontSendNotification);
	intervalControls.dotted.setToggleState(host.stageIntervalDotted, juce::dontSendNotification);

	lengthControls.division.setSelectedItemIndex(divisionIndex(host.stageLengthDiv), juce::dontSendNotification);
	lengthControls.triplet.setToggleState(host.stageLengthTriplet, juce::dontSendNotification);
	lengthControls.dotted.setToggleState(host.stageLengthDotted, juce::dontSendNotification);

	host.stageCycles = juce::jlimit(0, (int)cycleChoices.size() - 1, host.stageCycles);

	if (std::find(fpsChoices.begin(), fpsChoices.end(), host.stageFps) == fpsChoices.end()) host.stageFps = 30;

	fpsBox.setSelectedId(host.stageFps, juce::dontSendNotification);
}

void GuiStageWave::refreshInfo()
{
	if (m_busy) return;

	const auto info = StageWaveRender::inspect(ctx.audioProcessor);

	const bool changed = info.kind != m_info.kind || info.opCount != m_info.opCount
		|| info.modules != m_info.modules || info.padNotes != m_info.padNotes;

	m_info = info;

	if (changed) {
		updateVisibility();
		resized();
		repaint();
	}
}

void GuiStageWave::updateVisibility()
{
	const bool rhythm = m_info.kind == StageProbe::Kind::rhythm;

	keyLabel.setVisible(!rhythm);
	keySlider.setVisible(!rhythm);

	for (int m = 0; m < StageSwitch::count; ++m) {
		moduleToggles[(size_t)m].setVisible((m_info.modules & StageSwitch::bit(m)) != 0);
	}

	padsLabel.setVisible(rhythm);

	for (int i = 0; i < StageProbe::maxOps; ++i)
	{
		auto& t = padToggles[(size_t)i];

		t.setVisible(rhythm && i < m_info.opCount);
		t.setButtonText("PAD" + juce::String(i + 1) + " " + noteName(m_info.padNotes[(size_t)i]));
	}
}

StageWaveRender::Options GuiStageWave::currentOptions() const
{
	StageWaveRender::Options options;

	options.pattern = StageWaveRender::patternFrom(ctx.audioProcessor,
		ctx.audioProcessor.stageFollowDaw && dawFollowAvailable());

	for (int m = 0; m < StageSwitch::count; ++m)
	{
		const bool present = (m_info.modules & StageSwitch::bit(m)) != 0;

		if (present && !moduleToggles[(size_t)m].getToggleState()) options.off |= StageSwitch::bit(m);
	}

	for (int i = 0; i < StageProbe::maxOps; ++i) options.padChecked[(size_t)i] = padToggles[(size_t)i].getToggleState();

	return options;
}

// ============================================================================
// 並べ方
// ============================================================================
void GuiStageWave::resized()
{
	int x = margin;
	int y = margin;

	const auto place = [&](juce::Component& c, int width) {
		if (c.isVisible()) {
			c.setBounds(x, y, width, controlHeight);
			x += width + itemGap;
		}
	};

	// ---- 1 段目: 生成・削除・キー・周期
	place(generateBtn, 90);
	place(deleteBtn, 60);
	x += groupGap;
	place(keyLabel, 34);
	place(keySlider, 260);

	// 周期の欄は cycleArea、倍率の欄は zoomBox で描く。その右に FPS
	{
		const auto zoom = zoomBox();

		fpsLabel.setBounds(zoom.getRight() + groupGap, margin, 30, controlHeight);
		fpsBox.setBounds(fpsLabel.getRight() + itemGap, margin, 70, controlHeight);
	}

	// ---- その下に再生の段 (playerArea で描く)。2 段目: テンポ・小節・間隔・音符
	x = margin;
	y = playerArea().getBottom() + viewGap;

	place(tempoLabel, 48);
	place(tempoSlider, 110);
	place(tempoPresetBox, 70);
	place(followDawToggle, 100);
	x += groupGap;
	place(barsLabel, 36);
	place(barsBox, 60);

	for (auto* c : { &intervalControls, &lengthControls })
	{
		x += groupGap;
		place(c->label, 36);
		place(c->division, 90);
		place(c->triplet, 56);
		place(c->dotted, 56);
	}

	// ---- 3 段目: 入れ切り
	x = margin;
	y += controlHeight + viewGap;

	place(switchesLabel, 48);

	for (auto& t : moduleToggles) place(t, 112);

	// ---- RHYTHM の混ぜるパッドは、第 3 段の見出しの右に置く
	auto strip = rowArea(2).withHeight(rowLabelHeight);

	x = strip.getX() + 220;
	y = strip.getY();

	padsLabel.setBounds(x, y, 90, rowLabelHeight);
	x += 90;

	for (auto& t : padToggles)
	{
		t.setBounds(x, y, 104, rowLabelHeight);
		x += 104;
	}
}

juce::Rectangle<int> GuiStageWave::rowArea(int row) const
{
	const int top = margin + (controlHeight + viewGap) * 3 + GenWavePlayer::height + viewGap + rowGap;

	return { margin, top + row * (rowLabelHeight + viewHeight + rowGap), contentWidth - margin * 2, rowLabelHeight + viewHeight };
}

juce::Rectangle<int> GuiStageWave::viewArea(int row, int column) const
{
	const auto area = rowArea(row);

	return { area.getX() + column * (viewWidth + viewGap), area.getY() + rowLabelHeight, viewWidth, viewHeight };
}

juce::Rectangle<int> GuiStageWave::cycleArea() const
{
	// 1 段目の右側
	const int width = 40 + (int)cycleChoices.size() * 34;

	return { margin + 90 + 60 + itemGap * 2 + groupGap + 34 + 260 + itemGap * 2 + groupGap, margin, width, controlHeight };
}

juce::Rectangle<int> GuiStageWave::cycleCell(int index) const
{
	auto area = cycleArea().withTrimmedLeft(40);

	return { area.getX() + index * 34, area.getY(), 34, area.getHeight() };
}

juce::Rectangle<int> GuiStageWave::zoomBox() const
{
	// 周期のボタンの右
	return { cycleArea().getRight() + itemGap * 2, margin + 2, GenWavePlayer::zoomBoxWidth, controlHeight - 4 };
}

juce::Rectangle<int> GuiStageWave::playerArea() const
{
	// 1 段目 (生成・削除・キー・周期) のすぐ下
	return { margin, margin + controlHeight + viewGap, contentWidth - margin * 2, GenWavePlayer::height };
}

// ============================================================================
// 作る
// ============================================================================
void GuiStageWave::startGenerate()
{
	if (m_busy) return;

	auto job = StageWaveRender::prepare(ctx.audioProcessor, currentOptions());

	if (job == nullptr) return;

	m_busy = true;
	m_info = job->info;

	updateVisibility();
	resized();

	generateBtn.setEnabled(false);
	deleteBtn.setEnabled(false);

	repaint();

	juce::Component::SafePointer<GuiStageWave> safe(this);

	m_pool.addJob([safe, job] {
		auto result = std::make_shared<StageWaveRender::Result>(StageWaveRender::run(*job));

		juce::MessageManager::callAsync([safe, result] {
			if (safe != nullptr) safe->finishGenerate(*result);
		});
	});
}

void GuiStageWave::finishGenerate(const StageWaveRender::Result& result)
{
	m_result = result;
	m_info = result.info;
	m_busy = false;

	m_player.restart(juce::Time::getMillisecondCounterHiRes());

	generateBtn.setEnabled(true);
	generateBtn.setButtonText(EditorGuiText::StageWave::regenerate);
	deleteBtn.setEnabled(!m_result.isEmpty());

	updateVisibility();
	resized();
	repaint();
}

void GuiStageWave::clearResult()
{
	m_result = {};

	generateBtn.setButtonText(EditorGuiText::StageWave::generate);
	deleteBtn.setEnabled(false);

	repaint();
}

void GuiStageWave::timerCallback()
{
	if (++m_idleTicks >= infoRefreshTicks) {
		m_idleTicks = 0;

		refreshInfo();
	}

	if (!m_result.isEmpty() && !m_player.isPaused()) repaint();
}

// ============================================================================
// マウス
// ============================================================================
void GuiStageWave::mouseDown(const juce::MouseEvent& event)
{
	const double nowMs = juce::Time::getMillisecondCounterHiRes();
	const auto* wave = timeline();

	switch (m_player.mouseDown(playerArea(), event.getPosition(), wave, nowMs, event.mods.isPopupMenu()))
	{
	case GenWavePlayer::Hit::frameInput:
	{
		if (wave == nullptr) return;

		juce::Component::SafePointer<GuiStageWave> safe(this);

		GenWavePlayer::showFrameInput(*this, playerArea(), m_player.frame(*wave, nowMs) + 1, m_player.frameCount(*wave),
			[safe](int shown) {
				if (safe == nullptr || safe->timeline() == nullptr) return;

				safe->m_player.showFrame(*safe->timeline(), shown - 1, juce::Time::getMillisecondCounterHiRes());
				safe->repaint();
			}, m_player.isZoomInRow());
		return;
	}
	case GenWavePlayer::Hit::handled:
		repaint();
		return;
	default:
		break;
	}

	if (zoomBox().contains(event.getPosition()))
	{
		juce::Component::SafePointer<GuiStageWave> safe(this);

		GenWavePlayer::showZoomMenuAt(*this, zoomBox(), m_player.zoom(), [safe](int zoom) {
			if (safe == nullptr) return;

			safe->m_player.setZoom(zoom);
			safe->repaint();
		});
		return;
	}

	for (int i = 0; i < (int)cycleChoices.size(); ++i)
	{
		if (!cycleCell(i).contains(event.getPosition())) continue;

		ctx.audioProcessor.stageCycles = i;
		repaint();
		return;
	}
}

void GuiStageWave::mouseDrag(const juce::MouseEvent& event)
{
	if (m_player.mouseDrag(playerArea(), event.getPosition(), timeline(), juce::Time::getMillisecondCounterHiRes())) repaint();
}

void GuiStageWave::mouseUp(const juce::MouseEvent&)
{
	m_player.mouseUp();
}

void GuiStageWave::mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel)
{
	if (m_player.mouseWheel(playerArea(), event.getPosition(), timeline(), juce::Time::getMillisecondCounterHiRes(), wheel.deltaY))
	{
		repaint();
		return;
	}

	// 倍率の欄と枠の上のホイールは縦の倍率 (全部の枠で同じ)
	if ((zoomBox().contains(event.getPosition()) || event.getPosition().y >= rowArea(0).getY()) && wheel.deltaY != 0.0f)
	{
		m_player.setZoom(m_player.zoom() + (wheel.deltaY > 0.0f ? 1 : -1));
		repaint();
		return;
	}

	juce::Component::mouseWheelMove(event, wheel);
}

// ============================================================================
// 描く
// ============================================================================
const GenWaveRender::Wave* GuiStageWave::timeline() const
{
	const auto& w = m_result.stages[StageWaveRender::stageFx];

	return w.isEmpty() ? nullptr : &w;
}

int GuiStageWave::windowSamples() const
{
	const auto* wave = timeline();

	if (wave == nullptr) return 2;

	const int cycles = cycleChoices[(size_t)juce::jlimit(0, (int)cycleChoices.size() - 1, ctx.audioProcessor.stageCycles)];

	const double perCycle = wave->noteHz > 0.0 ? wave->samplesPerCycle() : wave->sampleRate * rhythmCycleSeconds;

	return juce::jlimit(2, juce::jmax(2, (int)wave->size()), (int)std::lround(perCycle * cycles));
}

juce::String GuiStageWave::rowTitle(int row) const
{
	using namespace EditorGuiText::StageWave;

	switch (row)
	{
	case 0:
		if (m_info.kind == StageProbe::Kind::fm) return row1Fm;
		if (m_info.kind == StageProbe::Kind::rhythm) return row1Rhythm;
		return row1Other;
	case 1:
		return m_info.kind == StageProbe::Kind::fm ? juce::String(row2Fm) : juce::String();
	default:
		return row3;
	}
}

juce::String GuiStageWave::viewTitle(int row, int column) const
{
	using namespace EditorGuiText::StageWave;

	if (row == 2)
	{
		switch (column)
		{
		case StageWaveRender::stageRaw: return stageRaw;
		case StageWaveRender::stageCore: return stageCore;
		case StageWaveRender::stageUnison: return stageUnison;
		case StageWaveRender::stageDac: return stageDac;
		case StageWaveRender::stageFx: return stageFx;
		default: return {};
		}
	}

	if (m_info.kind == StageProbe::Kind::rhythm) return "PAD" + juce::String(column + 1) + " " + noteName(m_info.padNotes[(size_t)column]);

	if (m_info.kind == StageProbe::Kind::fm) return "OP" + juce::String(column + 1);

	return rawOnly;
}

void GuiStageWave::paint(juce::Graphics& g)
{
	using namespace StageWaveRender;

	g.fillAll(GuiColor::GenWave::WindowBg);

	// ---- 周期の欄
	{
		g.setFont(juce::FontOptions(12.0f));
		g.setColour(GuiColor::GenWave::RowText);
		g.drawText(EditorGuiText::StageWave::cycles, cycleArea().withWidth(40), juce::Justification::centredLeft, false);

		for (int i = 0; i < (int)cycleChoices.size(); ++i)
		{
			const auto cell = cycleCell(i).reduced(1);
			const bool on = (i == ctx.audioProcessor.stageCycles);

			g.setColour(on ? GuiColor::GenWave::CycleBgOn : GuiColor::GenWave::CycleBg);
			g.fillRect(cell);
			g.setColour(GuiColor::GenWave::Border);
			g.drawRect(cell, 1);
			g.setColour(on ? GuiColor::GenWave::CycleTextOn : GuiColor::GenWave::CycleText);
			g.drawText(juce::String(cycleChoices[(size_t)i]), cell, juce::Justification::centred, false);
		}

		m_player.drawZoom(g, zoomBox());

		// 様子 (作っている最中・打ち切り・音源が無い)
		auto status = juce::Rectangle<int>(fpsBox.getRight() + groupGap, margin,
			contentWidth - margin - fpsBox.getRight() - groupGap, controlHeight);

		juce::String text;

		if (m_busy) text = EditorGuiText::StageWave::working;
		else if (m_result.truncated) text = EditorGuiText::StageWave::truncated;

		g.setColour(GuiColor::GenWave::HintText);
		g.drawText(text, status, juce::Justification::centredLeft, true);
	}

	const bool fm = m_info.kind == StageProbe::Kind::fm;
	const bool rhythm = m_info.kind == StageProbe::Kind::rhythm;
	const bool built = !m_result.isEmpty();

	// ---- 段の見出し
	for (int row = 0; row < 3; ++row)
	{
		const auto title = rowTitle(row);

		if (title.isEmpty()) continue;

		g.setColour(GuiColor::GenWave::RowText);
		g.setFont(juce::FontOptions(12.0f));
		g.drawText(title, rowArea(row).withHeight(rowLabelHeight).withWidth(rhythm && row == 2 ? 216 : 600),
			juce::Justification::centredLeft, true);
	}

	// ---- 第 1 段・第 2 段
	const int columns = (fm || rhythm) ? juce::jlimit(0, viewsPerRow, m_info.opCount) : 1;

	for (int row = 0; row < 2; ++row)
	{
		if (row == 1 && !fm) continue;

		for (int c = 0; c < columns; ++c)
		{
			const auto& w = row == 0 ? m_result.row1[(size_t)c] : m_result.row2[(size_t)c];

			drawView(g, viewArea(row, c), viewTitle(row, c), built ? &w : nullptr, false, false);
		}
	}

	// ---- 第 3 段
	for (int s = 0; s < stageCount; ++s)
	{
		// ① は FM だけ。それ以外は第 1 段に出している
		const bool off = built ? !m_result.stageOn[(size_t)s] : (s == stageRaw && !fm);

		drawView(g, viewArea(2, s), viewTitle(2, s), built ? &m_result.stages[(size_t)s] : nullptr, s >= stageUnison, off);
	}

	m_player.draw(g, playerArea(), timeline(), juce::Time::getMillisecondCounterHiRes());
}

void GuiStageWave::drawView(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& title,
	const GenWaveRender::Wave* wave, bool stereo, bool off)
{
	g.setColour(GuiColor::GenWave::Bg);
	g.fillRect(area);
	g.setColour(GuiColor::GenWave::Border);
	g.drawRect(area, 1);

	const auto inner = area.reduced(1);

	g.setColour(GuiColor::GenWave::Axis);
	g.drawHorizontalLine(inner.getCentreY(), (float)inner.getX(), (float)inner.getRight());

	if (off)
	{
		g.setColour(GuiColor::GenWave::OffText);
		g.setFont(juce::FontOptions(14.0f));
		g.drawText(EditorGuiText::StageWave::off, inner, juce::Justification::centred, false);
	}
	else if (wave == nullptr || wave->isEmpty())
	{
		g.setColour(GuiColor::GenWave::HintText);
		g.setFont(juce::FontOptions(10.0f));
		g.drawText(EditorGuiText::StageWave::empty, inner, juce::Justification::centred, true);
	}
	else if (const auto* time = timeline())
	{
		const double nowMs = juce::Time::getMillisecondCounterHiRes();
		const int window = windowSamples();
		const int start = m_player.windowStart(*time, window, nowMs);

		const float centreY = (float)inner.getCentreY();
		const float halfHeight = (inner.getHeight() * 0.5f - 1.0f) * (float)m_player.zoom();
		const int width = juce::jmax(1, inner.getWidth());

		const auto lane = [&](int channel, GuiColor::Entry& colour) {
			juce::Path path;

			for (int i = 0; i < width; ++i)
			{
				const size_t index = (size_t)start + (size_t)((juce::int64)window * i / width);

				if (index >= wave->size()) break;

				const float y = centreY - wave->at(channel, index) * halfHeight;

				if (i == 0) path.startNewSubPath((float)inner.getX(), y);
				else path.lineTo((float)(inner.getX() + i), y);
			}

			g.setColour(colour);
			g.strokePath(path, juce::PathStrokeType(1.1f));
		};

		juce::Graphics::ScopedSaveState clip(g);

		g.reduceClipRegion(inner);

		// L / R を重ね、真ん中をいちばん上に描く
		if (stereo)
		{
			lane(0, GuiColor::GenWave::LineL);
			lane(2, GuiColor::GenWave::LineR);
		}

		lane(1, GuiColor::GenWave::LineM);

		// 全体のうち今どこか
		g.setColour(GuiColor::GenWave::Progress);
		g.fillRect((float)inner.getX(), (float)inner.getBottom() - 2.0f, inner.getWidth() * m_player.progress(*time, nowMs), 2.0f);
	}

	g.setColour(GuiColor::GenWave::RowText);
	g.setFont(juce::FontOptions(10.0f));
	g.drawText(title, area.reduced(4, 2).withHeight(12), juce::Justification::topLeft, true);
}

// ============================================================================
// 別の窓
// ============================================================================
GuiStageWaveWindow::GuiStageWaveWindow(const GuiContext& context, std::function<void()> onClose)
	: juce::DocumentWindow(EditorGuiText::StageWave::windowTitle, GuiColor::GenWave::WindowBg, juce::DocumentWindow::closeButton),
	m_onClose(std::move(onClose))
{
	auto content = std::make_unique<GuiStageWave>(context);

	m_content = content.get();

	setUsingNativeTitleBar(true);
	setContentOwned(content.release(), true);
	setResizable(false, false);

	// DAW の窓の後ろへ隠れないように
	setAlwaysOnTop(true);

	centreWithSize(getWidth(), getHeight());
}
