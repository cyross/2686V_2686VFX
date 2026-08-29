#include "./GuiColors.h"

#include "../../Core/Editor/PluginEditor.h"

void GuiColors::setup()
{
	int tabOrder = 0;

	titleLabel.setup({ .parent = *this, .title = ColorsGuiText::title,
		.justification = juce::Justification::centredLeft });

	// GuiLabel::setup は Config の font を見ないので、ここで直接指定する
	titleLabel.setFont(juce::Font(juce::FontOptions(16.0f)).withStyle(juce::Font::bold));

	hintLabel.setup({ .parent = *this, .title = ColorsGuiText::hint,
		.justification = juce::Justification::centredLeft });

	// 断り書きは常に見えるところへ置く。ダイアログを開かないと
	// 読めないのでは意味が無い。
	noticeLabel.setup({ .parent = *this, .title = ColorsGuiText::Dialog::notice,
		.justification = juce::Justification::topLeft });

	resetAllButton.setup({ .parent = *this, .title = ColorsGuiText::resetAll, .isReset = false });
	resetAllButton.setExplicitFocusOrder(++tabOrder);
	resetAllButton.onClick = [this] {
		GuiColor::resetAllColours();

		table.updateContent();
		};

	table.setup({ .parent = *this, .title = ColorsGuiText::title });
	table.setWantsKeyboardFocus(false);
	table.addColumn(ColorsGuiText::Table::swatch, 1, ColorsGuiValue::Table::SwatchWidth);
	table.addColumn(ColorsGuiText::Table::item, 2, ColorsGuiValue::Table::ItemWidth);
	table.addColumn(ColorsGuiText::Table::value, 3, ColorsGuiValue::Table::ValueWidth);

	rebuildRows();

	table.onGetNumRows = [this]() { return (int)ids.size(); };

	table.onGetCellText = [this](int row, int columnId) -> juce::String {
		if (row < 0 || row >= (int)ids.size()) return {};

		const juce::String& id = ids[(size_t)row];

		if (columnId == 2) return id;

		if (columnId == 3) {
			auto it = GuiColor::registry().find(id);

			if (it == GuiColor::registry().end()) return {};

			return GuiColor::describe(it->second->get());
		}

		return {};
		};

	// 1 列目は文字ではなく色そのものを見せる
	table.onPaintCell = [this](juce::Graphics& g, int row, int columnId, int width, int height, bool) {
		if (columnId != 1) return false;

		if (row < 0 || row >= (int)ids.size()) return true;

		auto it = GuiColor::registry().find(ids[(size_t)row]);

		if (it == GuiColor::registry().end()) return true;

		auto area = juce::Rectangle<float>(2.0f, 2.0f, (float)width - 4.0f, (float)height - 4.0f);

		g.setColour(it->second->get());
		g.fillRoundedRectangle(area, guiCornerRadius);

		// 明るい色でも輪郭が分かるように縁を引く
		g.setColour(GuiColor::Outline);
		g.drawRoundedRectangle(area.reduced(0.5f), guiCornerRadius, 1.0f);

		return true;
		};

	table.onDoubleClicked = [this](int row) { openEditor(row); };
}

void GuiColors::rebuildRows()
{
	ids.clear();

	for (const auto& kv : GuiColor::registry()) ids.push_back(kv.first);

	table.updateContent();
}

// 1 行ぶんの色を変えるダイアログ。
//
// JUCE の ColourSelector が RGB と色空間 (HSV) を持っているので、
// 色の指定そのものはそれに任せる。名前から選ぶ道も併せて用意する。
void GuiColors::openEditor(int row)
{
	if (row < 0 || row >= (int)ids.size()) return;

	const juce::String id = ids[(size_t)row];

	auto it = GuiColor::registry().find(id);

	if (it == GuiColor::registry().end()) return;

	auto* window = new juce::AlertWindow(
		ColorsGuiText::Dialog::title + " : " + id,
		juce::String(),
		juce::MessageBoxIconType::NoIcon);

	auto selector = std::make_unique<juce::ColourSelector>(
		juce::ColourSelector::showColourAtTop
		| juce::ColourSelector::showSliders
		| juce::ColourSelector::showColourspace);

	selector->setCurrentColour(it->second->get());
	selector->setSize(300, 300);

	auto* selectorPtr = selector.get();

	window->addCustomComponent(selectorPtr);

	// 所有権はダイアログへ渡らないので、閉じるまでこちらで抱える
	auto keep = std::shared_ptr<juce::ColourSelector>(selector.release());

	window->addButton(ColorsGuiText::Dialog::apply, 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
	window->addButton(ColorsGuiText::Dialog::reset, 2);
	window->addButton(ColorsGuiText::Dialog::cancel, 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

	GuiDialog::styleButtons(*window);

	window->enterModalState(true, juce::ModalCallbackFunction::create(
		[this, id, keep, selectorPtr](int result) {
			if (result == 1) GuiColor::setColour(id, selectorPtr->getCurrentColour());
			else if (result == 2) GuiColor::resetColour(id);

			table.updateContent();
		}), true);
}

void GuiColors::layout(juce::Rectangle<int> content)
{
	auto area = content.withZeroOrigin();

	// タブの下辺と詰まって見えるので、少しだけ離す
	area.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

	area = area.reduced(ColorsGuiValue::Table::PaddingWidth, ColorsGuiValue::Table::PaddingHeight);

	// 見出し・案内・断り書きをまとめて板の上へ載せる
	headerArea = area.removeFromTop(
		ColorsGuiValue::Row::height * 2
		+ ColorsGuiValue::Row::noticeHeight
		+ ColorsGuiValue::Row::padding * 2
		+ ColorsGuiValue::Row::boardPadding * 2);

	auto header = headerArea.reduced(ColorsGuiValue::Row::boardPadding);

	titleLabel.setBounds(header.removeFromTop(ColorsGuiValue::Row::height));
	header.removeFromTop(ColorsGuiValue::Row::padding);

	auto hintRow = header.removeFromTop(ColorsGuiValue::Row::height);

	resetAllButton.setBounds(hintRow.removeFromRight(ColorsGuiValue::Button::width));
	hintRow.removeFromRight(ColorsGuiValue::Row::padding);
	hintLabel.setBounds(hintRow);

	header.removeFromTop(ColorsGuiValue::Row::padding);

	noticeLabel.setBounds(header.removeFromTop(ColorsGuiValue::Row::noticeHeight));

	area.removeFromTop(ColorsGuiValue::Row::padding);

	table.setBounds(area);
}

// 壁紙が透けて文字が読みにくくなるのを防ぐ板。
// カテゴリを開いたときに敷くものと同じ色を使う。
void GuiColors::paint(juce::Graphics& g)
{
	if (headerArea.isEmpty()) return;

	g.setColour(GuiColor::Category::ContentBg);
	g.fillRoundedRectangle(headerArea.toFloat(), guiCornerRadius);
}
