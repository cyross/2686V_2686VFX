#include "./TargetsDialog.h"

#include "Shared/Core/Gui/GuiColor.h"
#include "Shared/Core/Gui/GuiI18n.h"

namespace
{
	constexpr int padding = 10;
	constexpr int rowHeight = 24;
	constexpr int gap = 6;
	constexpr int labelWidth = 60;
	constexpr int toggleWidth = 56;

	namespace Text
	{
		inline juce::String duplicateTitle() { return "Duplicate Targets"; }
		inline juce::String clearTitle() { return "Clear Targets"; }
		inline juce::String from() { return "FROM"; }
		inline juce::String to() { return "TO"; }
		inline juce::String target() { return "TARGET"; }
		inline juce::String allOn() { return I18n::pick(u8"全てチェックを入れる", u8"Check all"); }
		inline juce::String allOff() { return I18n::pick(u8"全てチェックを外す", u8"Uncheck all"); }
		inline juce::String withParams() { return I18n::pick(u8"パラメータも複写", u8"Copy parameters too"); }
		inline juce::String duplicate() { return I18n::pick(u8"複写", u8"Duplicate"); }
		inline juce::String clear() { return I18n::pick(u8"クリア", u8"Clear"); }
		inline juce::String cancel() { return I18n::pick(u8"キャンセル", u8"Cancel"); }
	}
}

GuiTargetsDialog::GuiTargetsDialog(const GuiContext& context) :
	GuiBaseComponent(context),
	title(context),
	from(context),
	targetsLabel(context),
	allOnBtn(context),
	allOffBtn(context),
	withParams(context),
	runBtn(context),
	cancelBtn(context)
{
}

void GuiTargetsDialog::build()
{
	const bool duplicate = config.mode == Mode::Duplicate;
	const bool duplicateTo = config.mode == Mode::DuplicateTo;

	const juce::String heading = config.title.isNotEmpty() ? config.title
		: (duplicate || duplicateTo) ? Text::duplicateTitle() : Text::clearTitle();

	title.setup({ .parent = *this, .title = heading,
		.font = juce::Font(juce::FontOptions(16.0f, juce::Font::bold)) });

	if (duplicate)
	{
		from.setup({ .parent = *this, .title = Text::from(), .isReset = false });
		from.setRange((double)config.firstNumber, (double)(config.firstNumber + config.count - 1), 1.0);
		from.setNumDecimalPlacesToDisplay(0);
		from.onValueChange = [this] { applyFrom(); };
	}

	targetsLabel.setup({ .parent = *this, .title = (duplicate || duplicateTo) ? Text::to() : Text::target(),
		.justification = juce::Justification::centredLeft });

	toggles.clear();

	for (int i = 0; i < config.count; ++i)
	{
		auto t = std::make_unique<GuiToggleButton>(ctx);

		const juce::String name = i < (int)config.names.size() ? config.names[(size_t)i] : juce::String(config.firstNumber + i);

		t->setup({ .parent = *this, .title = name });
		t->setWantsKeyboardFocus(true);

		toggles.push_back(std::move(t));
	}

	allOnBtn.setup({ .parent = *this, .title = Text::allOn(),
		.textColor = GuiColor::Edit::ButtonText, .bgColor = GuiColor::Edit::CheckAllBg });
	allOnBtn.onClick = [this] {
		for (auto& t : toggles) {
			if (t->isEnabled()) t->setToggleState(true, juce::dontSendNotification);
		}
	};

	allOffBtn.setup({ .parent = *this, .title = Text::allOff(),
		.textColor = GuiColor::Edit::ButtonText, .bgColor = GuiColor::Edit::CheckAllBg });
	allOffBtn.onClick = [this] {
		for (auto& t : toggles) t->setToggleState(false, juce::dontSendNotification);
	};

	if (duplicate)
	{
		withParams.setup({ .parent = *this, .title = Text::withParams() });
		withParams.setToggleState(true, juce::dontSendNotification);
	}

	runBtn.setup({ .parent = *this, .title = (duplicate || duplicateTo) ? Text::duplicate() : Text::clear(),
		.textColor = GuiColor::Edit::ButtonText, .bgColor = (duplicate || duplicateTo) ? GuiColor::Edit::DuplicateOpsBg : GuiColor::Edit::ClearBg });
	runBtn.onClick = [this] {
		const auto targets = checked();

		if (targets.empty()) return;

		if (config.mode == Mode::Duplicate)
		{
			if (config.onDuplicate) config.onDuplicate((int)from.getValue() - config.firstNumber, targets, withParams.getToggleState());
		}
		else if (config.mode == Mode::DuplicateTo)
		{
			if (config.onDuplicateTo) config.onDuplicateTo(targets);
		}
		else
		{
			if (config.onClear) config.onClear(targets);
		}

		close();
	};

	cancelBtn.setup({ .parent = *this, .title = Text::cancel() });
	cancelBtn.onClick = [this] { close(); };

	built = true;
}

void GuiTargetsDialog::open(juce::Component& parent, juce::Rectangle<int> area, const Config& c)
{
	// 開いているときに同じボタンをもう一度押したら閉じる。作り直すと
	// 選んでいたものが消えてしまうので、開き直しはしない。
	if (isOpen() && getParentComponent() == &parent && config.mode == c.mode)
	{
		close();
		return;
	}

	config = c;

	// 作り直す。前に開いたときの選び方は持ち越さない
	removeAllChildren();
	build();

	if (config.mode == Mode::Duplicate)
	{
		from.setValue(config.firstNumber + juce::jlimit(0, config.count - 1, config.current), juce::dontSendNotification);
		applyFrom();
	}
	else if (config.mode == Mode::DuplicateTo)
	{
		// 写し元は写し先に選べない
		for (int i = 0; i < (int)toggles.size(); ++i) toggles[(size_t)i]->setEnabled(i != config.current);
	}

	parent.addAndMakeVisible(*this);

	const int height = juce::jmin(area.getHeight(), preferredHeight(area.getWidth()));

	setBounds(area.withHeight(height));

	// 中身は作り直したばかり。前と同じ大きさだと setBounds は resized を
	// 呼ばないので、自分で並べる (呼ばないと、新しいトグルが大きさ 0 のまま
	// 見えなくなる)
	resized();

	setVisible(true);

	// ESC を受けるため、キーボードの行き先をこちらへ移す
	setWantsKeyboardFocus(true);
	toFront(true);
	grabKeyboardFocus();
}

// 中の部品にキーボードの行き先があるときも、使われなかったキーはここへ来る
bool GuiTargetsDialog::keyPressed(const juce::KeyPress& key)
{
	if (key == juce::KeyPress::escapeKey)
	{
		close();
		return true;
	}

	return false;
}

void GuiTargetsDialog::close()
{
	setVisible(false);

	if (auto* parent = getParentComponent()) parent->removeChildComponent(this);

	if (config.onClosed) config.onClosed();
}

// 写し元と同じ番号は写し先に選べない
void GuiTargetsDialog::applyFrom()
{
	const int f = (int)from.getValue() - config.firstNumber;

	for (int i = 0; i < (int)toggles.size(); ++i)
	{
		const bool self = i == f;

		if (self) toggles[(size_t)i]->setToggleState(false, juce::dontSendNotification);

		toggles[(size_t)i]->setEnabled(!self);
	}
}

std::vector<int> GuiTargetsDialog::checked() const
{
	std::vector<int> out;

	for (int i = 0; i < (int)toggles.size(); ++i)
	{
		if (toggles[(size_t)i]->getToggleState() && toggles[(size_t)i]->isEnabled()) out.push_back(i);
	}

	return out;
}

int GuiTargetsDialog::columnsFor(int width) const
{
	return juce::jlimit(1, 8, (width - padding * 2) / toggleWidth);
}

int GuiTargetsDialog::preferredHeight(int width) const
{
	const int cols = columnsFor(width);
	const int rows = (config.count + cols - 1) / cols;
	const bool duplicate = config.mode == Mode::Duplicate;

	int h = padding;

	h += rowHeight + gap;                  // 見出し
	if (duplicate) h += rowHeight + gap;   // FROM
	h += rowHeight;                        // TO / TARGET
	h += rows * rowHeight + gap;           // 番号
	h += rowHeight + gap;                  // 全部入れる・外す
	if (duplicate) h += rowHeight + gap;   // パラメータも複写
	h += rowHeight;                        // 実行・取り消し
	h += padding;

	return h;
}

void GuiTargetsDialog::resized()
{
	if (!built) return;

	auto area = getLocalBounds().reduced(padding);
	const bool duplicate = config.mode == Mode::Duplicate;

	title.setBounds(area.removeFromTop(rowHeight));
	area.removeFromTop(gap);

	if (duplicate)
	{
		auto row = area.removeFromTop(rowHeight);

		from.label.setBounds(row.removeFromLeft(labelWidth));
		row.removeFromLeft(gap);
		from.setBounds(row);

		area.removeFromTop(gap);
	}

	targetsLabel.setBounds(area.removeFromTop(rowHeight));

	const int cols = columnsFor(getWidth());
	const int rows = ((int)toggles.size() + cols - 1) / cols;
	const int cellW = area.getWidth() / cols;

	auto grid = area.removeFromTop(rows * rowHeight);

	for (int i = 0; i < (int)toggles.size(); ++i)
	{
		const int r = i / cols;
		const int c = i % cols;

		toggles[(size_t)i]->setBounds(grid.getX() + c * cellW, grid.getY() + r * rowHeight, cellW, rowHeight);
	}

	area.removeFromTop(gap);

	{
		auto row = area.removeFromTop(rowHeight);
		const int half = (row.getWidth() - gap) / 2;

		allOnBtn.setBounds(row.removeFromLeft(half));
		row.removeFromLeft(gap);
		allOffBtn.setBounds(row);
	}

	area.removeFromTop(gap);

	if (duplicate)
	{
		withParams.setBounds(area.removeFromTop(rowHeight));
		area.removeFromTop(gap);
	}

	{
		auto row = area.removeFromTop(rowHeight);
		const int half = (row.getWidth() - gap) / 2;

		runBtn.setBounds(row.removeFromLeft(half));
		row.removeFromLeft(gap);
		cancelBtn.setBounds(row);
	}
}

void GuiTargetsDialog::paint(juce::Graphics& g)
{
	auto b = getLocalBounds().toFloat();

	g.setColour(GuiColor::Edit::DialogBg);
	g.fillRoundedRectangle(b, 6.0f);

	g.setColour(GuiColor::Edit::DialogBorder);
	g.drawRoundedRectangle(b.reduced(1.0f), 6.0f, 2.0f);
}
