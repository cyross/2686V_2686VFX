#pragma once

#include <JuceHeader.h>

#include <memory>
#include <vector>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"

// ==========================================================
// 整数のつまみの N ボタンで出す、値を動かすボタンの並び
// ==========================================================
// LEVEL のナッジ (GuiComponentLevelNudge) と同じ様式で、足し引きするボタンと
// よく使う値へ直に置くボタンを、行ごとに並べる。並びは表で渡す。
//
// SSG の設定値 (SSG HW AMP ENV / SSG HW PITCH ENV の VALUE、FORM の
// T.VALUE / N.VALUE) で使う。表は下の xxxRows が持つ。
class GuiComponentValueNudge : public GuiBase
{
public:
	// 押したときに、いまの値へ足すか、その値へ置くか
	enum class Action { Add, Set };

	struct Spec
	{
		juce::String title;
		Action action;
		double value;
		juce::Colour colour;
	};

	using Rows = std::vector<std::vector<Spec>>;

	GuiComponentValueNudge(const GuiContext& context);

	// 押したときに動かすつまみを渡す。TARGET で繋ぎ替えるつまみでも、
	// つまみそのものは同じなのでそのまま効く。
	void setupComponent(juce::Component& parent, GuiSlider& slider, const Rows& rows, int& tabOrder);

	void layoutComponent(juce::Rectangle<int>& rect);
	void layoutComponentRow(juce::Rectangle<int>& rect);

	void setVisibles(bool visible);
	void setEnables(bool enabled);

	// ---- SSG の設定値の表
	// FORM の T.VALUE (To)。47 が 1 倍、2 倍で 1 オクターブ下
	static Rows toneValueRows();
	// FORM の N.VALUE (Tn)
	static Rows noiseValueRows();
	// SSG HW AMP ENV / SSG HW PITCH ENV の VALUE (Te)。1303 が 1 Hz
	static Rows envValueRows();

	// テストから触る口
	int buttonCount() const { return (int)buttons.size(); }
	GuiTextButton& button(int index) { return *buttons[(size_t)index]; }
private:
	void layoutRows(juce::Rectangle<int>& rect, int rowHeight, int paddingTop, int paddingBottom);

	Rows rows;
	std::vector<std::unique_ptr<GuiTextButton>> buttons;
};
