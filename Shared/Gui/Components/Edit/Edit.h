#pragma once

#include <JuceHeader.h>

#include <functional>
#include <memory>
#include <optional>
#include <vector>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiParamEdit.h"
#include "../Separator/NormalSeparator.h"

// ============================================================================
// EDIT 区分 (チャンネルのグローバル部分)
// ============================================================================
// MIDI の下に置く。上から
//
//   Duplicate Ops Params (FM で 4 オペレーター以上) と FROM / TO
//   SWAP (FROM と TO を入れ替える)
//   FROM / TO それぞれの SHIFT / UNSHIFT (1 オペレーターずつずらす)
//   Duplicate Targets / Clear Targets (WT+ / ADPCM+)
//   Duplicate Pad Params (RHYTHM): FROM (番号のコンボボックス)、TO (番号の
//     トグル、いくつでも)、WITH FILE (入れると音声とファイル名も写す)
//   CUT / COPY / PASTE / CLEAR
//   Paste Note (RHYTHM): 切っていると、貼るときに NOTE を読み飛ばす
//   ── 区切り ──
//   タブから移したもの (Level -> All Ch、Duplicate Op Params など)
//
// CUT などはチャンネルのグローバル部分 (オペレーター・パッドを除く) を
// 単位にする。貼り先は音源の種類を問わず、同じ名前のパラメータへ入る。
class GuiComponentEdit : public GuiBase
{
public:
	// 複写できるオペレーターのまとまり (0 から数える)
	struct OpGroup
	{
		int first = 0;
		int size = 2;

		bool overlaps(const OpGroup& o) const { return first < o.first + o.size && o.first < first + size; }
		juce::String name() const;
	};

	// 番号付きの単位 1 つを、いくつかの単位へ写す (RHYTHM の Duplicate Pad Params)
	struct UnitDuplicate
	{
		juce::String title;  // ボタンの文字 (Duplicate Pad Params)
		juce::String head;   // 単位の頭 (PAD)
		int count = 8;       // 単位の数
	};

	struct Config
	{
		juce::Component& parent;

		// APVTS のプレフィックス (OPM など)
		juce::String code;

		// Duplicate Ops Params で選べるまとまり。空なら置かない
		std::vector<OpGroup> opGroups = {};

		// まとまりを写す。写せなかったときは false
		std::function<void(const OpGroup& from, const OpGroup& to)> onDuplicateOps = nullptr;

		// WT+ / ADPCM+ の TARGET の複写と消去。どちらも nullptr なら置かない
		std::function<void()> onDuplicateTargets = nullptr;
		std::function<void()> onClearTargets = nullptr;

		// 単位 1 つを、いくつかの単位へ写す。無ければ置かない
		std::optional<UnitDuplicate> unitDuplicate = std::nullopt;

		// Paste Note のトグルを置く (RHYTHM)。初めは切っていて、NOTE を貼らない
		bool pasteNoteToggle = false;

		// タブから移した部品の出し入れと並べ方
		std::function<void(bool)> setMovedVisible = nullptr;
		std::function<void(juce::Rectangle<int>&)> layoutMoved = nullptr;
	};

	GuiComponentEdit(const GuiContext& context);

	void setupComponent(const Config& c, int& tabOrder);
	void layoutComponent(juce::Rectangle<int>& rect);

	// 簡易表示などで区分ごと隠す
	void setHidden(bool hidden) { cat.setHidden(hidden); }

	// 4 オペレーター音源のまとまり ([1,2] と [3,4])
	static std::vector<OpGroup> fourOpGroups();

	// OPZX7 のまとまり (2 つ・3 つの並び、4 つは [1-4] と [5-8])
	static std::vector<OpGroup> opzx7Groups();

	// FROM / TO の選び方で、写せるかどうか
	static bool canDuplicate(const OpGroup& from, const OpGroup& to) { return from.size == to.size && !from.overlaps(to); }

	// groups[index] を 1 オペレーターずらしたまとまりの添字。direction は
	// +1 (SHIFT: 番号を足す) か -1 (UNSHIFT: 番号を引く)。
	//
	// はみ出すときは反対の端へ回る (8 オペレーターで [6,7,8] を SHIFT すると
	// [1,2,3]、[1,2,3] を UNSHIFT すると [6,7,8])。ずらした先が選べる
	// まとまりに無いとき (4 オペレーターの [2,3] など) は、さらに先へ進める。
	static int shiftedGroup(const std::vector<OpGroup>& groups, int index, int direction);

	GuiCategoryLabel& category() { return cat; }

	// 単位の複写 (Duplicate Pad Params) を押したときと同じ。TO に何も
	// 選んでいなければ何もしない
	void runUnitDuplicate();

private:
	ParamEdit::Target target() const;

	void refillTo();
	void applyDuplicateEnabled();

	void applyUnitFrom();
	void applyUnitEnabled();
	void setUnitVisible(bool visible);
	void layoutUnit(juce::Rectangle<int>& rect);

	// FROM / TO で選んでいるまとまり (groups の添字)。選んでいなければ -1
	int fromGroup() const;
	int toGroup() const;
	void selectTo(int group);

	void swapFromTo();
	void shiftFrom(int direction);
	void shiftTo(int direction);

	GuiCategoryLabel cat;

	GuiTextButton duplicateOpsBtn;
	GuiComboBox fromSelector, toSelector;
	GuiTextButton swapBtn;
	GuiLabel fromShiftLabel, toShiftLabel;
	GuiTextButton fromShiftBtn, fromUnshiftBtn, toShiftBtn, toUnshiftBtn;
	NormalSeparator opsSeparator;

	GuiTextButton duplicateTargetsBtn, clearTargetsBtn;
	NormalSeparator targetsSeparator;

	GuiTextButton unitDuplicateBtn;
	GuiComboBox unitFromSelector;
	GuiLabel unitToLabel;
	std::vector<std::unique_ptr<GuiToggleButton>> unitToToggles;
	GuiToggleButton withFileToggle;
	NormalSeparator unitSeparator;

	GuiTextButton cutBtn, copyBtn, pasteBtn, clearBtn;
	GuiToggleButton pasteNoteToggle;

	NormalSeparator movedSeparator;

	// 参照を持つので、作り直しで入れる
	std::optional<Config> config;
	std::vector<OpGroup> groups;

	// TO に並べているまとまり (groups の添字)
	std::vector<int> toIndices;

	bool hasOps() const { return !groups.empty(); }
	bool hasTargets() const { return config.has_value() && (config->onDuplicateTargets != nullptr || config->onClearTargets != nullptr); }
	bool hasMoved() const { return config.has_value() && config->layoutMoved != nullptr; }
	bool hasUnitDuplicate() const { return config.has_value() && config->unitDuplicate.has_value(); }
	bool hasPasteNote() const { return config.has_value() && config->pasteNoteToggle; }

	// Paste Note のトグルの入り切りを、貼り付けの読み飛ばしへ写す
	void applyPasteNote();
};

// ============================================================================
// EDIT 区分 (オペレーター / パッド 1 つ)
// ============================================================================
// MASK の下 (RHYTHM は OPTIONAL の下) に置く。TARGET で選んでいる
// オペレーター / パッドを単位にする。
class GuiComponentUnitEdit : public GuiBase
{
public:
	GuiComponentUnitEdit(const GuiContext& context);

	// unitOf は、いま選んでいる単位 (OP2 / PAD3 など) を返す
	void setupComponent(juce::Component& parent, const juce::String& code,
		std::function<juce::String()> unitOf, int& tabOrder);

	void layoutComponent(juce::Rectangle<int>& rect);

	// 列 (幅の狭いところ) に置くとき
	void layoutComponentRow(juce::Rectangle<int>& rect);

	GuiCategoryLabel& category() { return cat; }
private:
	ParamEdit::Target target() const;

	GuiCategoryLabel cat;
	GuiTextButton cutBtn, copyBtn, pasteBtn, clearBtn;

	juce::String m_code;
	std::function<juce::String()> m_unitOf;
};
