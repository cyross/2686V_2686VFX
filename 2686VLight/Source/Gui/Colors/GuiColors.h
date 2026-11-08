#pragma once

#include <JuceHeader.h>
#include <set>
#include <vector>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiColor.h"

#include "./GuiColorsText.h"
#include "./GuiColorsValues.h"

// ============================================================================
// COLORS タブ
// ============================================================================
// GuiColor へ登録されている色を一覧し、1 つずつ差し替えられるようにする。
//
// 一覧の中身は GuiColor::registry() から作る。色を足したときに、ここへ
// 書き足す必要が無いようにするため。
class GuiColors : public GuiBase
{
	GuiLabel titleLabel;
	GuiLabel hintLabel;

	// 独自に付けた色名についての断り書き
	GuiLabel noticeLabel;
	GuiTableList table;
	GuiTextButton resetAllButton;
	GuiTextButton loadButton;
	GuiTextButton saveButton;

	// 読み込みの対象を絞る (4.4.0)。検索の欄・全てチェック・全て外す・ワイルドカード
	GuiTextEditor searchEditor;
	GuiTextButton searchClearButton;
	GuiTextButton checkAllButton;
	GuiTextButton uncheckAllButton;
	GuiToggleButton wildcardToggle;

	std::unique_ptr<juce::FileChooser> fileChooser;

	// 表示している行。registry のうち、検索に合うもの。
	std::vector<juce::String> ids;

	// 読み込みの対象から外した色。既定は全部が対象なので、外したほうを持つ。
	// 後から色が増えても、増えたものは対象に入る。
	std::set<juce::String> excluded;

	void rebuildRows();

	// 読み込みの対象か
	bool isImportTarget(const juce::String& id) const { return excluded.count(id) == 0; }

	// 対象の入り切り。ワイルドカードが入っていれば、同じ親を持つ色をまとめて変える
	void setImportTarget(const juce::String& id, bool on);

	// 表示している行をまとめて入り切りする
	void setShownImportTargets(bool on);

	// その色が既定から変わっているか
	bool isChanged(int row) const;
	void openEditor(int row);

	void loadFromFile();
	void saveToFile();
public:
	GuiColors(const GuiContext& context) :
		GuiBase(context),
		titleLabel(context),
		hintLabel(context),
		noticeLabel(context),
		table(context),
		resetAllButton(context),
		loadButton(context),
		saveButton(context),
		searchEditor(context),
		searchClearButton(context),
		checkAllButton(context),
		uncheckAllButton(context),
		wildcardToggle(context)
	{
	}

	// 壁紙が透けると文字が読みにくくなるので、見出しの後ろへ板を敷く。
	// 板の範囲は layout で決まるので覚えておく。
	juce::Rectangle<int> headerArea;

	void paint(juce::Graphics& g) override;
	void setup() override;
	void layout(juce::Rectangle<int> content) override;
};
