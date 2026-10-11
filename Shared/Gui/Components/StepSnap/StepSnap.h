#pragma once

#include <JuceHeader.h>

#include <vector>

#include "Shared/Core/Gui/GuiI18n.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"

// ==========================================================
// 0.0〜1.0 のつまみを決まった刻みへ丸める STEPS
// ==========================================================
// LEVEL の STEPS と同じく、選んだ刻み (1/2・1/4・… 1/100) へつまみの値を
// 寄せる。範囲が 0.0〜1.0 なので、LEVEL にある「1 刻み」は置かない。
//
// 選んだ刻みは画面だけのもので、保存しない (LEVEL と同じ)。
// 1 つの STEPS で、同じ区分の幾つかのつまみ (SSG HW AMP ENV の MIN / MAX
// など) をまとめて丸められる。
class GuiComponentStepSnap : public GuiBase, private juce::Slider::Listener
{
public:
	GuiComponentStepSnap(const GuiContext& context);
	~GuiComponentStepSnap() override;

	// 丸めるつまみを渡す。TARGET で繋ぎ替えるつまみでも、つまみそのものは
	// 同じなのでそのまま効く。
	void setupComponent(juce::Component& parent, const std::vector<GuiSlider*>& sliders, int& tabOrder,
		const juce::String& title = "STEPS", std::optional<juce::Font> labelFont = std::nullopt);

	// rowHeight が 0 なら既定の高さ。区分の中のつまみに高さを合わせるときに渡す
	void layoutComponent(juce::Rectangle<int>& rect, int rowHeight = 0);
	void layoutComponentRow(juce::Rectangle<int>& rect, int rowHeight = 0);

	void setVisibleWithLabel(bool visible);
	void setEnabledWithLabel(bool enabled);

	// 選んでいる刻みで、いまの値を丸める
	void snapAll();

	// value を 1/steps の刻みへ丸め、[lo, hi] へ収める。steps が 0 なら丸めない
	static double snap(double value, int steps, double lo, double hi);

	// 選択肢の番号 (0 が自由) から刻みの数を出す。0 は丸めない
	static int stepsOf(int index);

	// テストから触る口
	GuiComboBox& selector() { return stepSelector; }
private:
	void sliderValueChanged(juce::Slider* slider) override;
	void snapOne(GuiSlider& slider);

	GuiComboBox stepSelector;
	std::vector<GuiSlider*> m_sliders;
};
