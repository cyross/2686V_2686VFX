#pragma once

#include <JuceHeader.h>
#include <vector>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"
#include "../../Core/Gui/GuiColor.h"

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

	std::unique_ptr<juce::FileChooser> fileChooser;

	// 表示している行。registry を並べ替えたもの。
	std::vector<juce::String> ids;

	void rebuildRows();

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
		saveButton(context)
	{
	}

	// 壁紙が透けると文字が読みにくくなるので、見出しの後ろへ板を敷く。
	// 板の範囲は layout で決まるので覚えておく。
	juce::Rectangle<int> headerArea;

	void paint(juce::Graphics& g) override;
	void setup() override;
	void layout(juce::Rectangle<int> content) override;
};
