#pragma once

#include <JuceHeader.h>

#include <functional>
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
//   Duplicate Targets / Clear Targets (WT+ / ADPCM+)
//   CUT / COPY / PASTE / CLEAR
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

	GuiCategoryLabel& category() { return cat; }

private:
	ParamEdit::Target target() const;

	void refillTo();
	void applyDuplicateEnabled();

	GuiCategoryLabel cat;

	GuiTextButton duplicateOpsBtn;
	GuiComboBox fromSelector, toSelector;
	NormalSeparator opsSeparator;

	GuiTextButton duplicateTargetsBtn, clearTargetsBtn;
	NormalSeparator targetsSeparator;

	GuiTextButton cutBtn, copyBtn, pasteBtn, clearBtn;

	NormalSeparator movedSeparator;

	// 参照を持つので、作り直しで入れる
	std::optional<Config> config;
	std::vector<OpGroup> groups;

	// TO に並べているまとまり (groups の添字)
	std::vector<int> toIndices;

	bool hasOps() const { return !groups.empty(); }
	bool hasTargets() const { return config.has_value() && (config->onDuplicateTargets != nullptr || config->onClearTargets != nullptr); }
	bool hasMoved() const { return config.has_value() && config->layoutMoved != nullptr; }
};

// ============================================================================
// EDIT 区分 (オペレーター / パッド 1 つ)
// ============================================================================
// MASK の下に置く。TARGET で選んでいるオペレーター / パッドを単位にする。
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
