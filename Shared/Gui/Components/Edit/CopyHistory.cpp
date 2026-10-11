#include "./CopyHistory.h"

#include "Shared/Core/Gui/GuiColor.h"
#include "Shared/Core/Gui/GuiI18n.h"

namespace
{
	constexpr int padding = 12;
	constexpr int rowHeight = 26;
	constexpr int filterHeight = 24;
	constexpr int recopyWidth = 90;

	namespace Text
	{
		inline juce::String title() { return I18n::pick(u8"コピー履歴", u8"Copy history"); }
		inline juce::String all() { return I18n::pick(u8"すべて", u8"All"); }
		inline juce::String tab() { return I18n::pick(u8"タブ", u8"Tab"); }
		inline juce::String unit() { return I18n::pick(u8"単位", u8"Unit"); }
		inline juce::String category() { return I18n::pick(u8"区分", u8"Category"); }
		inline juce::String noCategory() { return I18n::pick(u8"(区分以外)", u8"(not a category)"); }
		inline juce::String global() { return I18n::pick(u8"グローバル", u8"Global"); }
		inline juce::String op() { return I18n::pick(u8"オペレーター", u8"Operator"); }
		inline juce::String pad() { return I18n::pick(u8"パッド", u8"Pad"); }
		inline juce::String slot() { return I18n::pick(u8"TARGET (枠)", u8"Target (slot)"); }
		inline juce::String recopy() { return I18n::pick(u8"再コピー", u8"Copy again"); }
		inline juce::String close() { return I18n::pick(u8"閉じる", u8"Close"); }
		inline juce::String items() { return I18n::pick(u8"項目", u8"items"); }
		inline juce::String empty() { return I18n::pick(u8"まだ何も写していません。", u8"Nothing has been copied yet."); }
	}
}

// ---------------------------------------------------------------- 行
class GuiCopyHistory::Row : public juce::Component
{
public:
	Row(GuiCopyHistory& owner) : m_owner(owner), recopyBtn(owner.ctx)
	{
		recopyBtn.setup({ .parent = *this, .title = Text::recopy(),
			.textColor = GuiColor::Edit::ButtonText, .bgColor = GuiColor::Edit::DuplicateOpsBg });
		recopyBtn.onClick = [this] { m_owner.recopy(m_row); };
	}

	void update(int row) { m_row = row; repaint(); }

	void resized() override
	{
		recopyBtn.setBounds(getLocalBounds().removeFromRight(recopyWidth).reduced(2));
	}

	void paint(juce::Graphics& g) override
	{
		if (m_row < 0 || m_row >= (int)m_owner.m_rows.size()) return;

		const auto& s = m_owner.m_rows[(size_t)m_row];
		const auto& current = ParamEdit::Clipboard::get().current();
		const bool isCurrent = current.has_value() && current->serial == s.serial;

		auto b = getLocalBounds();

		g.setColour(isCurrent ? GuiColor::TableList::SelectedBg.get()
			: (m_row % 2 == 0 ? GuiColor::TableList::RowBg1.get() : GuiColor::TableList::RowBg2.get()));
		g.fillRect(b);

		b.removeFromRight(recopyWidth);
		b.reduce(6, 0);

		g.setColour(GuiColor::TableList::Text);
		g.setFont(juce::Font(juce::FontOptions(14.0f)));

		auto cell = [&](int width, const juce::String& text) {
			g.drawText(text, b.removeFromLeft(width), juce::Justification::centredLeft, true);
		};

		cell(70, s.time.formatted("%H:%M:%S"));
		cell(90, s.tab);
		cell(110, s.scopeName());
		cell(220, s.category.isEmpty() ? juce::String("-") : s.category);
		cell(b.getWidth(), juce::String((int)s.values.size()) + " " + Text::items()
			+ (s.assets.empty() ? juce::String() : " + " + juce::String((int)s.assets.size()) + " file"));
	}
private:
	GuiCopyHistory& m_owner;
	GuiTextButton recopyBtn;
	int m_row = -1;
};

// ---------------------------------------------------------------- 本体
GuiCopyHistory::GuiCopyHistory(const GuiContext& context) :
	GuiBaseComponent(context),
	title(context),
	tabSelector(context),
	unitSelector(context),
	categorySelector(context),
	closeBtn(context)
{
	title.setup({ .parent = *this, .title = Text::title(), .font = juce::Font(juce::FontOptions(18.0f, juce::Font::bold)),
		.justification = juce::Justification::centredLeft });

	const std::vector<SelectItem> none;

	tabSelector.setup({ .parent = *this, .title = Text::tab(), .items = none, .isReset = false });
	unitSelector.setup({ .parent = *this, .title = Text::unit(), .items = none, .isReset = false });
	categorySelector.setup({ .parent = *this, .title = Text::category(), .items = none, .isReset = false });

	for (auto* c : { &tabSelector, &unitSelector, &categorySelector }) c->onChange = [this] { applyFilters(); };

	list.setModel(this);
	list.setRowHeight(rowHeight);
	list.setColour(juce::ListBox::backgroundColourId, GuiColor::Edit::DialogBg);
	addAndMakeVisible(list);

	closeBtn.setup({ .parent = *this, .title = Text::close() });
	closeBtn.onClick = [this] { close(); };

	ParamEdit::Clipboard::get().addChangeListener(this);
}

GuiCopyHistory::~GuiCopyHistory()
{
	ParamEdit::Clipboard::get().removeChangeListener(this);
	list.setModel(nullptr);
}

juce::String GuiCopyHistory::unitKindOf(const ParamEdit::Snapshot& s)
{
	if (s.unit.isEmpty() || s.scope == ParamEdit::Scope::Channel) return Text::global();
	if (s.unit.startsWith("OP")) return Text::op();
	if (s.unit.startsWith("PAD")) return Text::pad();
	if (s.unit.startsWith("SLOT")) return Text::slot();

	return Text::global();
}

void GuiCopyHistory::open(juce::Component& parent)
{
	parent.addAndMakeVisible(*this);
	setBounds(parent.getLocalBounds());
	refill();

	// ESC を受けるため、キーボードの行き先をこちらへ移す
	setWantsKeyboardFocus(true);
	toFront(true);
	grabKeyboardFocus();
}

void GuiCopyHistory::close()
{
	setVisible(false);

	if (auto* parent = getParentComponent()) parent->removeChildComponent(this);
}

// 選択肢を履歴から作り直す。選んでいたものは名前で選び直す。
void GuiCopyHistory::refill()
{
	auto rebuild = [](GuiComboBox& box, juce::StringArray& names, const juce::StringArray& items) {
		const auto keep = names[box.getSelectedItemIndex()];

		names.clear();
		names.add(Text::all());
		names.addArray(items);

		box.clear(juce::dontSendNotification);

		for (int i = 0; i < names.size(); ++i) box.addItem(names[i], i + 1);

		box.setSelectedItemIndex(juce::jmax(0, names.indexOf(keep)), juce::dontSendNotification);
	};

	juce::StringArray tabs, cats;

	for (const auto& s : ParamEdit::Clipboard::get().history())
	{
		if (s.tab.isNotEmpty()) tabs.addIfNotAlreadyThere(s.tab);
		if (s.category.isNotEmpty()) cats.addIfNotAlreadyThere(s.category);
	}

	tabs.sort(true);
	cats.sort(true);
	cats.insert(0, Text::noCategory());

	rebuild(tabSelector, m_tabs, tabs);
	rebuild(unitSelector, m_units, juce::StringArray{ Text::global(), Text::op(), Text::pad(), Text::slot() });
	rebuild(categorySelector, m_categories, cats);

	applyFilters();
}

void GuiCopyHistory::applyFilters()
{
	const int t = tabSelector.getSelectedItemIndex();
	const int u = unitSelector.getSelectedItemIndex();
	const int c = categorySelector.getSelectedItemIndex();

	m_rows.clear();

	for (const auto& s : ParamEdit::Clipboard::get().history())
	{
		if (t > 0 && s.tab != m_tabs[t]) continue;
		if (u > 0 && unitKindOf(s) != m_units[u]) continue;

		if (c > 0)
		{
			const bool notCategory = m_categories[c] == Text::noCategory();

			if (notCategory ? s.scope == ParamEdit::Scope::Category : s.category != m_categories[c]) continue;
		}

		m_rows.push_back(s);
	}

	list.updateContent();
	list.repaint();
}

void GuiCopyHistory::recopy(int row)
{
	if (row < 0 || row >= (int)m_rows.size()) return;

	ParamEdit::Clipboard::get().select(m_rows[(size_t)row].serial);

	list.repaint();
}

void GuiCopyHistory::paintListBoxItem(int, juce::Graphics&, int, int, bool)
{
	// 行は Row が描く
}

juce::Component* GuiCopyHistory::refreshComponentForRow(int row, bool, juce::Component* existing)
{
	auto* r = dynamic_cast<Row*>(existing);

	if (row < 0 || row >= (int)m_rows.size())
	{
		delete existing;
		return nullptr;
	}

	if (r == nullptr)
	{
		delete existing;
		r = new Row(*this);
	}

	r->update(row);

	return r;
}

juce::Rectangle<int> GuiCopyHistory::panelBounds() const
{
	return getLocalBounds().withSizeKeepingCentre(juce::jmin(panelWidth, getWidth() - 40), juce::jmin(panelHeight, getHeight() - 40));
}

void GuiCopyHistory::resized()
{
	auto area = panelBounds().reduced(padding);

	auto top = area.removeFromTop(28);

	closeBtn.setBounds(top.removeFromRight(100));
	title.setBounds(top);

	area.removeFromTop(8);

	auto filters = area.removeFromTop(filterHeight);
	const int each = filters.getWidth() / 3;

	for (auto* box : { &tabSelector, &unitSelector, &categorySelector })
	{
		auto cell = filters.removeFromLeft(each).reduced(4, 0);

		box->label.setBounds(cell.removeFromLeft(60));
		box->setBounds(cell);
	}

	area.removeFromTop(8);

	list.setBounds(area);
}

void GuiCopyHistory::paint(juce::Graphics& g)
{
	// 下の画面を覆う地
	g.fillAll(GuiColor::ParamBrowser::Backdrop);

	const auto panel = panelBounds().toFloat();

	g.setColour(GuiColor::Edit::DialogBg);
	g.fillRoundedRectangle(panel, 6.0f);

	g.setColour(GuiColor::Edit::DialogBorder);
	g.drawRoundedRectangle(panel.reduced(1.0f), 6.0f, 2.0f);

	if (ParamEdit::Clipboard::get().history().empty())
	{
		g.setColour(GuiColor::TableList::Text);
		g.drawText(Text::empty(), list.getBounds(), juce::Justification::centred, true);
	}
}

// 外側をクリックしたら閉じる
void GuiCopyHistory::mouseDown(const juce::MouseEvent& e)
{
	if (!panelBounds().contains(e.getPosition())) close();
}

// 中の部品にキーボードの行き先があるときも、使われなかったキーはここへ来る
bool GuiCopyHistory::keyPressed(const juce::KeyPress& key)
{
	if (key == juce::KeyPress::escapeKey)
	{
		close();
		return true;
	}

	return false;
}
