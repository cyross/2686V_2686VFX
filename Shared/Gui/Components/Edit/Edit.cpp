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
		inline const juce::String cut = "CUT";
		inline const juce::String copy = "COPY";
		inline const juce::String paste = "PASTE";
		inline const juce::String clear = "CLEAR";
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

GuiComponentEdit::GuiComponentEdit(const GuiContext& context) :
	GuiBase(context),
	cat(context),
	duplicateOpsBtn(context),
	fromSelector(context),
	toSelector(context),
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
	clearBtn(context)
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

	setupEditButtons(ctx, parent, tabOrder, cutBtn, copyBtn, pasteBtn, clearBtn, [this] { return target(); });
}

void GuiComponentUnitEdit::layoutComponent(juce::Rectangle<int>& rect)
{
	layoutMainCategory({ .mainRect = rect, .component = &cat });

	const bool visible = cat.isDetailVisible();

	for (auto* b : { &cutBtn, &copyBtn, &pasteBtn, &clearBtn }) b->setVisible(visible);

	if (!visible) return;

	layoutMainFourComps({ .rect = rect, .comp1 = &cutBtn, .comp2 = &copyBtn, .comp3 = &pasteBtn, .comp4 = &clearBtn });

	rect.removeFromTop(CoreGuiValue::Category::gapBelow);
}

void GuiComponentUnitEdit::layoutComponentRow(juce::Rectangle<int>& rect)
{
	layoutRowCategory({ .rowRect = rect, .component = &cat });

	const bool visible = cat.isDetailVisible();

	for (auto* b : { &cutBtn, &copyBtn, &pasteBtn, &clearBtn }) b->setVisible(visible);

	if (!visible) return;

	layoutRowFourComps({ .rect = rect, .comp1 = &cutBtn, .comp2 = &copyBtn, .comp3 = &pasteBtn, .comp4 = &clearBtn });

	rect.removeFromTop(CoreGuiValue::Category::gapBelow);
}
