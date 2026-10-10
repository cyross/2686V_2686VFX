#include "./Edit.h"

#include "Shared/Core/Gui/GuiColor.h"
#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Gui/GuiStructs.h"
#include "Shared/Core/Gui/GuiI18n.h"

namespace
{
	namespace Text
	{
		inline const juce::String title = "EDIT";
		inline const juce::String duplicateOps = "Duplicate Ops Params";
		inline const juce::String duplicateTargets = "Duplicate Targets";
		inline const juce::String clearTargets = "Clear Targets";
		inline const juce::String from = "FROM";
		inline const juce::String to = "TO";
		inline const juce::String swap = "SWAP";
		inline const juce::String shift = "SHIFT";
		inline const juce::String unshift = "UNSHIFT";
		inline const juce::String cut = "CUT";
		inline const juce::String copy = "COPY";
		inline const juce::String paste = "PASTE";
		inline const juce::String clear = "CLEAR";
		inline const juce::String withFile = "WITH FILE";
	}

	// CUT / COPY / PASTE / CLEAR の 4 つ
	void setupEditButtons(const GuiContext& ctx, juce::Component& parent, int& tabOrder,
		GuiTextButton& cut, GuiTextButton& copy, GuiTextButton& paste, GuiTextButton& clear,
		std::function<ParamEdit::Target()> target)
	{
		juce::ignoreUnused(ctx);

		for (auto [btn, title] : { std::pair<GuiTextButton*, juce::String>{ &cut, Text::cut }, { &copy, Text::copy },
			{ &paste, Text::paste }, { &clear, Text::clear } })
		{
			btn->setup({ .parent = parent, .title = title });
			btn->setWantsKeyboardFocus(true);
			btn->setExplicitFocusOrder(++tabOrder);
		}

		cut.onClick = [ctx, target] { ParamEdit::cut(ctx, target()); };
		copy.onClick = [ctx, target] { ParamEdit::copy(ctx, target()); };
		paste.onClick = [ctx, target] { ParamEdit::pasteClipboard(ctx, target()); };
		clear.onClick = [ctx, target] { ParamEdit::clear(ctx, target()); };
	}
}

// ============================================================================
// グローバル部分
// ============================================================================
juce::String GuiComponentEdit::OpGroup::name() const
{
	juce::StringArray ops;

	for (int i = 0; i < size; ++i) ops.add(juce::String(first + i + 1));

	return "[" + ops.joinIntoString(",") + "]";
}

std::vector<GuiComponentEdit::OpGroup> GuiComponentEdit::fourOpGroups()
{
	return { { 0, 2 }, { 2, 2 } };
}

std::vector<GuiComponentEdit::OpGroup> GuiComponentEdit::opzx7Groups()
{
	std::vector<OpGroup> g;

	for (int i = 0; i + 2 <= 8; ++i) g.push_back({ i, 2 });
	for (int i = 0; i + 3 <= 8; ++i) g.push_back({ i, 3 });

	g.push_back({ 0, 4 });
	g.push_back({ 4, 4 });

	return g;
}

int GuiComponentEdit::shiftedGroup(const std::vector<OpGroup>& groups, int index, int direction)
{
	if (index < 0 || index >= (int)groups.size() || direction == 0) return index;

	const auto& g = groups[(size_t)index];

	int ops = 0;

	for (const auto& o : groups) ops = juce::jmax(ops, o.first + o.size);

	const int last = ops - g.size;

	if (last < 0) return index;

	int first = g.first;

	// 頭の番号を 1 つずつ進め、同じ数のまとまりに行き当たるまで回る
	for (int step = 0; step <= last; ++step)
	{
		first += direction > 0 ? 1 : -1;

		if (first > last) first = 0;
		if (first < 0) first = last;

		for (int i = 0; i < (int)groups.size(); ++i)
		{
			if (groups[(size_t)i].first == first && groups[(size_t)i].size == g.size) return i;
		}
	}

	return index;
}

GuiComponentEdit::GuiComponentEdit(const GuiContext& context) :
	GuiBase(context),
	cat(context),
	duplicateOpsBtn(context),
	fromSelector(context),
	toSelector(context),
	swapBtn(context),
	fromShiftLabel(context),
	toShiftLabel(context),
	fromShiftBtn(context),
	fromUnshiftBtn(context),
	toShiftBtn(context),
	toUnshiftBtn(context),
	opsSeparator(context),
	duplicateTargetsBtn(context),
	clearTargetsBtn(context),
	targetsSeparator(context),
	cutBtn(context),
	copyBtn(context),
	pasteBtn(context),
	clearBtn(context),
	movedSeparator(context)
{
}

ParamEdit::Target GuiComponentEdit::target() const
{
	ParamEdit::Target t;

	t.scope = ParamEdit::Scope::Channel;
	t.channel = config.has_value() ? config->code : juce::String();
	t.tab = ParamEdit::tabNameOf(const_cast<GuiCategoryLabel&>(cat));

	return t;
}

void GuiComponentEdit::setupComponent(const Config& c, int& tabOrder)
{
	config.emplace(c);
	groups = c.opGroups;

	auto& parent = c.parent;

	// EDIT はほかの「その他」の区分と同じ緑
	cat.setupOtherCategory({ .parent = parent, .title = Text::title, .enableChangeDetailVisible = true });

	// ---- Duplicate Ops Params
	if (hasOps())
	{
		// Duplicate Op Params (1 つずつ) と見分けるため、地を青にする
		duplicateOpsBtn.setup({ .parent = parent, .title = Text::duplicateOps,
			.textColor = GuiColor::Edit::ButtonText, .bgColor = GuiColor::Edit::DuplicateOpsBg });
		duplicateOpsBtn.setWantsKeyboardFocus(true);
		duplicateOpsBtn.setExplicitFocusOrder(++tabOrder);
		duplicateOpsBtn.onClick = [this] {
			const int from = fromSelector.getSelectedItemIndex();
			const int to = toSelector.getSelectedItemIndex();

			if (from < 0 || to < 0 || to >= (int)toIndices.size() || config->onDuplicateOps == nullptr) return;

			config->onDuplicateOps(groups[(size_t)from], groups[(size_t)toIndices[(size_t)to]]);
		};

		std::vector<SelectItem> items;

		for (int i = 0; i < (int)groups.size(); ++i) items.push_back({ .name = groups[(size_t)i].name(), .value = i + 1 });

		fromSelector.setup({ .parent = parent, .title = Text::from, .items = items, .isReset = false });
		fromSelector.setWantsKeyboardFocus(true);
		fromSelector.setExplicitFocusOrder(++tabOrder);

		toSelector.setup({ .parent = parent, .title = Text::to, .items = {}, .isReset = false });
		toSelector.setWantsKeyboardFocus(true);
		toSelector.setExplicitFocusOrder(++tabOrder);

		fromSelector.setSelectedItemIndex(0, juce::dontSendNotification);
		refillTo();

		// 選び方が変わったら、TO の並びとボタンの入り切りを作り直す
		fromSelector.onChange = [this] { refillTo(); };
		toSelector.onChange = [this] { applyDuplicateEnabled(); };

		// SWAP と、FROM / TO それぞれの SHIFT / UNSHIFT
		auto setupButton = [&](GuiTextButton& btn, const juce::String& title, std::function<void()> onClick) {
			btn.setup({ .parent = parent, .title = title });
			btn.setWantsKeyboardFocus(true);
			btn.setExplicitFocusOrder(++tabOrder);
			btn.onClick = std::move(onClick);
		};

		setupButton(swapBtn, Text::swap, [this] { swapFromTo(); });

		fromShiftLabel.setup({ .parent = parent, .title = Text::from });
		setupButton(fromShiftBtn, Text::shift, [this] { shiftFrom(+1); });
		setupButton(fromUnshiftBtn, Text::unshift, [this] { shiftFrom(-1); });

		toShiftLabel.setup({ .parent = parent, .title = Text::to });
		setupButton(toShiftBtn, Text::shift, [this] { shiftTo(+1); });
		setupButton(toUnshiftBtn, Text::unshift, [this] { shiftTo(-1); });

		opsSeparator.setupComponent(parent);
	}

	// ---- Duplicate Targets / Clear Targets
	if (hasTargets())
	{
		duplicateTargetsBtn.setup({ .parent = parent, .title = Text::duplicateTargets,
			.textColor = GuiColor::Edit::ButtonText, .bgColor = GuiColor::Edit::DuplicateOpsBg });
		duplicateTargetsBtn.setWantsKeyboardFocus(true);
		duplicateTargetsBtn.setExplicitFocusOrder(++tabOrder);
		duplicateTargetsBtn.onClick = [this] { if (config->onDuplicateTargets) config->onDuplicateTargets(); };

		clearTargetsBtn.setup({ .parent = parent, .title = Text::clearTargets,
			.textColor = GuiColor::Edit::ButtonText, .bgColor = GuiColor::Edit::MovedButtonBg });
		clearTargetsBtn.setWantsKeyboardFocus(true);
		clearTargetsBtn.setExplicitFocusOrder(++tabOrder);
		clearTargetsBtn.onClick = [this] { if (config->onClearTargets) config->onClearTargets(); };

		targetsSeparator.setupComponent(parent);
	}

	// ---- CUT / COPY / PASTE / CLEAR
	setupEditButtons(ctx, parent, tabOrder, cutBtn, copyBtn, pasteBtn, clearBtn, [this] { return target(); });

	if (hasMoved()) movedSeparator.setupComponent(parent);
}

// FROM と同じ数のまとまりだけを TO に並べる
void GuiComponentEdit::refillTo()
{
	const int from = fromSelector.getSelectedItemIndex();
	const int keep = toSelector.getSelectedItemIndex();
	const int keepGroup = (keep >= 0 && keep < (int)toIndices.size()) ? toIndices[(size_t)keep] : -1;

	toSelector.clear(juce::dontSendNotification);
	toIndices.clear();

	if (from < 0) return;

	const auto& f = groups[(size_t)from];

	for (int i = 0; i < (int)groups.size(); ++i)
	{
		if (groups[(size_t)i].size != f.size) continue;

		toIndices.push_back(i);
		toSelector.addItem(groups[(size_t)i].name(), (int)toIndices.size());
	}

	// 前に選んでいたものが残っていれば、それを選び直す。無ければ、
	// 重ならない最初のもの。
	int select = -1;

	for (int k = 0; k < (int)toIndices.size(); ++k)
	{
		if (toIndices[(size_t)k] == keepGroup) select = k;
	}

	if (select < 0)
	{
		for (int k = 0; k < (int)toIndices.size(); ++k)
		{
			if (!f.overlaps(groups[(size_t)toIndices[(size_t)k]])) { select = k; break; }
		}
	}

	toSelector.setSelectedItemIndex(juce::jmax(0, select), juce::dontSendNotification);

	applyDuplicateEnabled();
}

int GuiComponentEdit::fromGroup() const
{
	const int from = fromSelector.getSelectedItemIndex();

	return (from >= 0 && from < (int)groups.size()) ? from : -1;
}

int GuiComponentEdit::toGroup() const
{
	const int to = toSelector.getSelectedItemIndex();

	return (to >= 0 && to < (int)toIndices.size()) ? toIndices[(size_t)to] : -1;
}

void GuiComponentEdit::selectTo(int group)
{
	for (int k = 0; k < (int)toIndices.size(); ++k)
	{
		if (toIndices[(size_t)k] == group) toSelector.setSelectedItemIndex(k, juce::dontSendNotification);
	}

	applyDuplicateEnabled();
}

void GuiComponentEdit::swapFromTo()
{
	const int from = fromGroup();
	const int to = toGroup();

	if (from < 0 || to < 0) return;

	fromSelector.setSelectedItemIndex(to, juce::dontSendNotification);
	refillTo();
	selectTo(from);
}

void GuiComponentEdit::shiftFrom(int direction)
{
	const int from = fromGroup();

	if (from < 0) return;

	fromSelector.setSelectedItemIndex(shiftedGroup(groups, from, direction), juce::dontSendNotification);

	// 数は変わらないので、TO はそのまま残る
	refillTo();
}

void GuiComponentEdit::shiftTo(int direction)
{
	const int to = toGroup();

	if (to < 0) return;

	selectTo(shiftedGroup(groups, to, direction));
}

void GuiComponentEdit::applyDuplicateEnabled()
{
	const int from = fromSelector.getSelectedItemIndex();
	const int to = toSelector.getSelectedItemIndex();

	bool ok = from >= 0 && to >= 0 && to < (int)toIndices.size();

	if (ok) ok = canDuplicate(groups[(size_t)from], groups[(size_t)toIndices[(size_t)to]]);

	duplicateOpsBtn.setEnabled(ok);
}

void GuiComponentEdit::layoutComponent(juce::Rectangle<int>& rect)
{
	layoutMainCategory({ .mainRect = rect, .component = &cat });

	const bool visible = cat.isDetailVisible();

	duplicateOpsBtn.setVisible(visible && hasOps());
	fromSelector.setVisibleWithLabel(visible && hasOps());
	toSelector.setVisibleWithLabel(visible && hasOps());

	for (juce::Component* c : { (juce::Component*)&swapBtn, (juce::Component*)&fromShiftLabel, (juce::Component*)&toShiftLabel,
		(juce::Component*)&fromShiftBtn, (juce::Component*)&fromUnshiftBtn, (juce::Component*)&toShiftBtn, (juce::Component*)&toUnshiftBtn })
		c->setVisible(visible && hasOps());

	opsSeparator.setVisible(visible && hasOps());

	duplicateTargetsBtn.setVisible(visible && config->onDuplicateTargets != nullptr);
	clearTargetsBtn.setVisible(visible && config->onClearTargets != nullptr);
	targetsSeparator.setVisible(visible && hasTargets());

	cutBtn.setVisible(visible);
	copyBtn.setVisible(visible);
	pasteBtn.setVisible(visible);
	clearBtn.setVisible(visible);

	movedSeparator.setVisible(visible && hasMoved());

	if (config->setMovedVisible) config->setMovedVisible(visible);

	if (!visible) return;

	if (hasOps())
	{
		layoutMain({ .mainRect = rect, .component = &duplicateOpsBtn });
		layoutMain({ .mainRect = rect, .label = &fromSelector.label, .component = &fromSelector });
		layoutMain({ .mainRect = rect, .label = &toSelector.label, .component = &toSelector });

		layoutMain({ .mainRect = rect, .component = &swapBtn });

		// ラベルの右を半分ずつ、SHIFT と UNSHIFT に分ける
		auto shiftRow = [&](GuiLabel& label, GuiTextButton& shift, GuiTextButton& unshift) {
			layoutMain({ .mainRect = rect, .label = &label, .component = &shift });

			auto area = shift.getBounds();
			const int inner = CoreGuiValue::MainGroup::Comps::Two::paddingInner;
			const int half = (area.getWidth() - inner) / 2;

			shift.setBounds(area.removeFromLeft(half));
			area.removeFromLeft(inner);
			unshift.setBounds(area);
		};

		shiftRow(fromShiftLabel, fromShiftBtn, fromUnshiftBtn);
		shiftRow(toShiftLabel, toShiftBtn, toUnshiftBtn);

		opsSeparator.layoutComponent(rect);
	}

	if (hasTargets())
	{
		if (config->onDuplicateTargets) layoutMain({ .mainRect = rect, .component = &duplicateTargetsBtn });
		if (config->onClearTargets) layoutMain({ .mainRect = rect, .component = &clearTargetsBtn });

		targetsSeparator.layoutComponent(rect);
	}

	layoutMainFourComps({ .rect = rect, .comp1 = &cutBtn, .comp2 = &copyBtn, .comp3 = &pasteBtn, .comp4 = &clearBtn });

	if (hasMoved())
	{
		movedSeparator.layoutComponent(rect);

		config->layoutMoved(rect);
	}

	rect.removeFromTop(CoreGuiValue::Category::gapBelow);
}

// ============================================================================
// オペレーター / パッド 1 つ
// ============================================================================
GuiComponentUnitEdit::GuiComponentUnitEdit(const GuiContext& context) :
	GuiBase(context),
	cat(context),
	cutBtn(context),
	copyBtn(context),
	pasteBtn(context),
	clearBtn(context),
	duplicateBtn(context),
	dupFromSelector(context),
	dupToLabel(context),
	withFileToggle(context),
	dupSeparator(context)
{
}

ParamEdit::Target GuiComponentUnitEdit::target() const
{
	ParamEdit::Target t;

	t.scope = ParamEdit::Scope::Unit;
	t.channel = m_code;
	t.unit = m_unitOf ? m_unitOf() : juce::String();
	t.tab = ParamEdit::tabNameOf(const_cast<GuiCategoryLabel&>(cat));

	return t;
}

void GuiComponentUnitEdit::setupComponent(juce::Component& parent, const juce::String& code,
	std::function<juce::String()> unitOf, int& tabOrder)
{
	m_code = code;
	m_unitOf = std::move(unitOf);

	cat.setupOtherCategory({ .parent = parent, .title = Text::title, .enableChangeDetailVisible = true });

	// ---- 1 つを、いくつかへ写す
	if (hasDuplicate())
	{
		duplicateBtn.setup({ .parent = parent, .title = duplicate->title,
			.textColor = GuiColor::Edit::ButtonText, .bgColor = GuiColor::Edit::DuplicateOpsBg });
		duplicateBtn.setWantsKeyboardFocus(true);
		duplicateBtn.setExplicitFocusOrder(++tabOrder);
		duplicateBtn.onClick = [this] { runDuplicate(); };

		// 番号は画面のパッドと同じく 1 から
		std::vector<SelectItem> items;

		for (int i = 0; i < duplicate->count; ++i) items.push_back({ .name = juce::String(i + 1), .value = i + 1 });

		dupFromSelector.setup({ .parent = parent, .title = Text::from, .items = items, .isReset = false });
		dupFromSelector.setWantsKeyboardFocus(true);
		dupFromSelector.setExplicitFocusOrder(++tabOrder);
		dupFromSelector.setSelectedItemIndex(0, juce::dontSendNotification);
		dupFromSelector.onChange = [this] { applyDuplicateFrom(); };

		dupToLabel.setup({ .parent = parent, .title = Text::to });

		dupToToggles.clear();

		for (int i = 0; i < duplicate->count; ++i)
		{
			auto t = std::make_unique<GuiToggleButton>(ctx);

			t->setup({ .parent = parent, .title = juce::String(i + 1) });
			t->setWantsKeyboardFocus(true);
			t->setExplicitFocusOrder(++tabOrder);
			t->onClick = [this] { applyDuplicateEnabled(); };

			dupToToggles.push_back(std::move(t));
		}

		withFileToggle.setup({ .parent = parent, .title = Text::withFile });
		withFileToggle.setWantsKeyboardFocus(true);
		withFileToggle.setExplicitFocusOrder(++tabOrder);

		dupSeparator.setupComponent(parent);

		applyDuplicateFrom();
	}

	setupEditButtons(ctx, parent, tabOrder, cutBtn, copyBtn, pasteBtn, clearBtn, [this] { return target(); });
}

// 写し元と同じ番号は写し先に選べない
void GuiComponentUnitEdit::applyDuplicateFrom()
{
	const int from = dupFromSelector.getSelectedItemIndex();

	for (int i = 0; i < (int)dupToToggles.size(); ++i)
	{
		const bool self = i == from;

		if (self) dupToToggles[(size_t)i]->setToggleState(false, juce::dontSendNotification);

		dupToToggles[(size_t)i]->setEnabled(!self);
	}

	applyDuplicateEnabled();
}

// 写し先を 1 つも選んでいないうちは押せない
void GuiComponentUnitEdit::applyDuplicateEnabled()
{
	bool any = false;

	for (auto& t : dupToToggles) any = any || (t->getToggleState() && t->isEnabled());

	duplicateBtn.setEnabled(any);
}

void GuiComponentUnitEdit::runDuplicate()
{
	if (!hasDuplicate()) return;

	const int from = dupFromSelector.getSelectedItemIndex();

	std::vector<int> to;

	for (int i = 0; i < (int)dupToToggles.size(); ++i)
	{
		if (i != from && dupToToggles[(size_t)i]->getToggleState()) to.push_back(i);
	}

	if (from < 0 || to.empty()) return;

	// WITH FILE を切っているときは、写し先の音声をそのまま残す
	ParamEdit::duplicateUnitTo(ctx, m_code, duplicate->head, from, to, true, withFileToggle.getToggleState());
}

void GuiComponentUnitEdit::setDuplicateVisible(bool visible)
{
	const bool show = visible && hasDuplicate();

	duplicateBtn.setVisible(show);
	dupFromSelector.setVisibleWithLabel(show);
	dupToLabel.setVisible(show);

	for (auto& t : dupToToggles) t->setVisible(show);

	withFileToggle.setVisible(show);
	dupSeparator.setVisible(show);
}

void GuiComponentUnitEdit::layoutDuplicate(juce::Rectangle<int>& rect, bool row)
{
	if (!hasDuplicate()) return;

	auto place = [&](GuiLabel* label, juce::Component* component) {
		if (row) layoutRow({ .rowRect = rect, .label = label, .component = component });
		else layoutMain({ .mainRect = rect, .label = label, .component = component });
	};

	place(nullptr, &duplicateBtn);
	place(&dupFromSelector.label, &dupFromSelector);
	place(&dupToLabel, nullptr);

	// 番号のトグルは 4 つずつ並べる
	const int rowHeight = row ? CoreGuiValue::ParamGroup::Row::height : CoreGuiValue::MainGroup::Row::height;
	const int paddingTop = row ? CoreGuiValue::ParamGroup::Row::paddingTop : CoreGuiValue::MainGroup::Row::paddingTop;
	const int paddingBottom = row ? CoreGuiValue::ParamGroup::Row::paddingBottom : CoreGuiValue::MainGroup::Row::paddingBottom;
	constexpr int perRow = 4;
	constexpr int inner = 1;

	for (int i = 0; i < (int)dupToToggles.size(); i += perRow)
	{
		auto area = rect.removeFromTop(rowHeight);
		rect.removeFromTop(paddingTop);

		const int cellW = (area.getWidth() - inner * (perRow - 1)) / perRow;

		for (int k = i; k < juce::jmin(i + perRow, (int)dupToToggles.size()); ++k)
		{
			dupToToggles[(size_t)k]->setBounds(area.removeFromLeft(cellW));
			area.removeFromLeft(inner);
		}

		rect.removeFromTop(paddingBottom);
	}

	place(nullptr, &withFileToggle);

	dupSeparator.layoutComponent(rect);
}

void GuiComponentUnitEdit::layoutComponent(juce::Rectangle<int>& rect)
{
	layoutMainCategory({ .mainRect = rect, .component = &cat });

	const bool visible = cat.isDetailVisible();

	for (auto* b : { &cutBtn, &copyBtn, &pasteBtn, &clearBtn }) b->setVisible(visible);

	setDuplicateVisible(visible);

	if (!visible) return;

	layoutDuplicate(rect, false);

	layoutMainFourComps({ .rect = rect, .comp1 = &cutBtn, .comp2 = &copyBtn, .comp3 = &pasteBtn, .comp4 = &clearBtn });

	rect.removeFromTop(CoreGuiValue::Category::gapBelow);
}

void GuiComponentUnitEdit::layoutComponentRow(juce::Rectangle<int>& rect)
{
	layoutRowCategory({ .rowRect = rect, .component = &cat });

	const bool visible = cat.isDetailVisible();

	for (auto* b : { &cutBtn, &copyBtn, &pasteBtn, &clearBtn }) b->setVisible(visible);

	setDuplicateVisible(visible);

	if (!visible) return;

	layoutDuplicate(rect, true);

	layoutRowFourComps({ .rect = rect, .comp1 = &cutBtn, .comp2 = &copyBtn, .comp3 = &pasteBtn, .comp4 = &clearBtn });

	rect.removeFromTop(CoreGuiValue::Category::gapBelow);
}
