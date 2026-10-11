#pragma once

#include <JuceHeader.h>

#include <memory>
#include <vector>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiParamEdit.h"

// ============================================================================
// コピー履歴 (CPHIST)
// ============================================================================
// 写したもの (ParamEdit::Clipboard の履歴) を一覧にする。タブ (チャンネル・
// カーブ編集・FX)、単位 (グローバル / オペレーター・パッド・枠)、区分で
// 絞り込める。行の右端の「再コピー」で、その行をクリップボードへ戻す。
// 戻したものは、いつもどおり PASTE で貼れる。
//
// 画面全体へ重ねて出す。外側をクリックするか「閉じる」か ESC で消える。
class GuiCopyHistory : public juce::Component,
	public GuiBaseComponent,
	private juce::ListBoxModel,
	private juce::ChangeListener
{
public:
	explicit GuiCopyHistory(const GuiContext& context);
	~GuiCopyHistory() override;

	// parent の全体へ重ねて出す
	void open(juce::Component& parent);
	void close();

	void paint(juce::Graphics& g) override;
	void resized() override;
	void mouseDown(const juce::MouseEvent& e) override;
	bool keyPressed(const juce::KeyPress& key) override;

	// 絞り込んだあとの行 (テストから見る)
	const std::vector<ParamEdit::Snapshot>& rows() const { return m_rows; }

	// 絞り込み (テストから触る)
	GuiComboBox& tabFilter() { return tabSelector; }
	GuiComboBox& unitFilter() { return unitSelector; }
	GuiComboBox& categoryFilter() { return categorySelector; }

	// 行の「再コピー」を押す (テストから触る)
	void recopy(int row);

	// 単位の絞り込みの名前 (グローバル / OP / PAD / SLOT)
	static juce::String unitKindOf(const ParamEdit::Snapshot& s);

	static inline constexpr int panelWidth = 760;
	static inline constexpr int panelHeight = 480;
private:
	// ListBoxModel
	int getNumRows() override { return (int)m_rows.size(); }
	void paintListBoxItem(int row, juce::Graphics& g, int width, int height, bool selected) override;
	juce::Component* refreshComponentForRow(int row, bool selected, juce::Component* existing) override;

	void changeListenerCallback(juce::ChangeBroadcaster*) override { refill(); }

	// 履歴から絞り込みの選択肢と行を作り直す
	void refill();
	void applyFilters();

	juce::Rectangle<int> panelBounds() const;

	GuiLabel title;
	GuiComboBox tabSelector, unitSelector, categorySelector;
	juce::ListBox list;
	GuiTextButton closeBtn;

	std::vector<ParamEdit::Snapshot> m_rows;

	// 選択肢の名前 (0 番はすべて)
	juce::StringArray m_tabs, m_units, m_categories;

	class Row;
};
