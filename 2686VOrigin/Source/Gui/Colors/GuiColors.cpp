#include "./GuiColors.h"

#include "./RetroPalette.h"

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

// ============================================================================
// 色の変更ダイアログの中身
// ============================================================================
// 色を決める道を 3 つ並べる。
//
//   1. 色名から選ぶ    … 名前の付いた色をそのまま当てる
//   2. 実機のパレット  … 当時の機種が出せた色から選ぶ
//   3. ColourSelector … RGB と色空間で好きに作る
//
// どれで選んでも行き先は ColourSelector なので、最後は必ずそこの色が
// 採用される。上の 2 つは入口を増やしているだけ。
class ColorEditorPanel : public juce::Component
{
	juce::ColourSelector selector;

	juce::Label nameLabel;
	juce::ComboBox nameBox;

	juce::Label paletteLabel;
	juce::ComboBox paletteBox;

	juce::Viewport paletteView;
	RetroPalette palette;

	// 名前の一覧。選択肢の番号から引くために持っておく。
	std::vector<std::pair<juce::String, juce::Colour>> names;
public:
	ColorEditorPanel(juce::Colour initial) :
		selector(juce::ColourSelector::showColourAtTop
			| juce::ColourSelector::showSliders
			| juce::ColourSelector::showColourspace)
	{
		addAndMakeVisible(selector);
		selector.setCurrentColour(initial);

		// ---- 色名から選ぶ ----
		addAndMakeVisible(nameLabel);
		nameLabel.setText(ColorsGuiText::Dialog::nameList, juce::dontSendNotification);
		nameLabel.setJustificationType(juce::Justification::centredLeft);

		addAndMakeVisible(nameBox);
		nameBox.addItem(ColorsGuiText::Dialog::nameNone, 1);

		names = GuiColor::namedColours();

		for (int i = 0; i < (int)names.size(); ++i) {
			nameBox.addItem(names[(size_t)i].first, i + 2);
		}

		nameBox.setSelectedId(1, juce::dontSendNotification);

		nameBox.onChange = [this] {
			int index = nameBox.getSelectedId() - 2;

			if (index < 0 || index >= (int)names.size()) return;

			selector.setCurrentColour(names[(size_t)index].second);
			};

		// ---- 実機のパレットから選ぶ ----
		addAndMakeVisible(paletteLabel);
		paletteLabel.setText(ColorsGuiText::Dialog::palette, juce::dontSendNotification);
		paletteLabel.setJustificationType(juce::Justification::centredLeft);

		addAndMakeVisible(paletteBox);

		for (int i = 0; i < (int)machines.size(); ++i) {
			paletteBox.addItem(RetroPalette::name(machines[(size_t)i]), i + 1);
		}

		paletteBox.setSelectedId(1, juce::dontSendNotification);

		paletteBox.onChange = [this] {
			int index = paletteBox.getSelectedId() - 1;

			if (index < 0 || index >= (int)machines.size()) return;

			setPalette(machines[(size_t)index]);
			};

		addAndMakeVisible(paletteView);
		paletteView.setViewedComponent(&palette, false);
		paletteView.setScrollBarsShown(true, true);

		palette.onColourPicked = [this](juce::Colour colour) {
			selector.setCurrentColour(colour);
			};

		setPalette(machines[0]);

		setSize(ColorsGuiValue::Editor::width,
			ColorsGuiValue::Row::height * 2
			+ ColorsGuiValue::Row::padding * 3
			+ ColorsGuiValue::Editor::selectorHeight
			+ ColorsGuiValue::Editor::paletteHeight);
	}

	juce::Colour currentColour() const { return selector.getCurrentColour(); }

	void resized() override
	{
		auto area = getLocalBounds();

		auto nameRow = area.removeFromTop(ColorsGuiValue::Row::height);
		nameLabel.setBounds(nameRow.removeFromLeft(ColorsGuiValue::Editor::labelWidth));
		nameBox.setBounds(nameRow);

		area.removeFromTop(ColorsGuiValue::Row::padding);

		auto paletteRow = area.removeFromTop(ColorsGuiValue::Row::height);
		paletteLabel.setBounds(paletteRow.removeFromLeft(ColorsGuiValue::Editor::labelWidth));
		paletteBox.setBounds(paletteRow);

		area.removeFromTop(ColorsGuiValue::Row::padding);

		paletteView.setBounds(area.removeFromTop(ColorsGuiValue::Editor::paletteHeight));

		area.removeFromTop(ColorsGuiValue::Row::padding);

		selector.setBounds(area);
	}
private:
	// 並べる順番。色数の少ないものから。
	static inline const std::vector<RetroPalette::Machine> machines = {
		RetroPalette::Machine::Pc8801,
		RetroPalette::Machine::PcEngine,
		RetroPalette::Machine::MegaDrive,
		RetroPalette::Machine::Pc9801,
		RetroPalette::Machine::X68000,
	};

	void setPalette(RetroPalette::Machine machine)
	{
		palette.setMachine(machine);

		// 窓より大きいときは中で送れるよう、実寸で置く
		int width = RetroPalette::columns(machine) * 10;

		palette.setSize(width, palette.preferredHeight(width));
	}
};

// 1 行ぶんの色を変えるダイアログ。
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

	auto panel = std::make_shared<ColorEditorPanel>(it->second->get());

	window->addCustomComponent(panel.get());

	window->addButton(ColorsGuiText::Dialog::apply, 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
	window->addButton(ColorsGuiText::Dialog::reset, 2);
	window->addButton(ColorsGuiText::Dialog::cancel, 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

	GuiDialog::styleButtons(*window);

	// 所有権はダイアログへ渡らないので、閉じるまでこちらで抱える
	window->enterModalState(true, juce::ModalCallbackFunction::create(
		[this, id, panel](int result) {
			if (result == 1) GuiColor::setColour(id, panel->currentColour());
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
