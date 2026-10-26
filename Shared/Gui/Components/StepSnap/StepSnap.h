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
//
// 選択肢の組は 2 つ (4.3.0)。
//   Level : 1/2・1/4・… 1/256・1/3・1/6・1/7・1/10・1/100 (音量など 0.0〜1.0 のつまみ)
//   Ratio : 1/10・1/4・1/5・1/8・1/16。刻みはつまみの範囲の幅に対して決める
//           (DUTY RATIO ENVELOPE は 0〜100% なので 10% / 25% / 20% / 12.5% / 6.25%、
//            PEAK RATIO ENVELOPE は 0.1 / 0.25 / 0.2 / 0.125 / 0.0625)
class GuiComponentStepSnap : public GuiBase, private juce::Slider::Listener
{
public:
	enum class Choices { Level, Ratio };

	GuiComponentStepSnap(const GuiContext& context);
	~GuiComponentStepSnap() override;

	// 丸めるつまみを渡す。TARGET で繋ぎ替えるつまみでも、つまみそのものは
	// 同じなのでそのまま効く。
	void setupComponent(juce::Component& parent, const std::vector<GuiSlider*>& sliders, int& tabOrder,
		const juce::String& title = "STEPS", std::optional<juce::Font> labelFont = std::nullopt, Choices choices = Choices::Level);

	// rowHeight が 0 なら既定の高さ。区分の中のつまみに高さを合わせるときに渡す
	void layoutComponent(juce::Rectangle<int>& rect, int rowHeight = 0);
	void layoutComponentRow(juce::Rectangle<int>& rect, int rowHeight = 0);

	void setVisibleWithLabel(bool visible);
	void setEnabledWithLabel(bool enabled);

	// 選んでいる刻みで、いまの値を丸める
	void snapAll();

	// value を scale/steps の刻みへ丸め、[lo, hi] へ収める。steps が 0 なら丸めない
	static double snap(double value, int steps, double lo, double hi, double scale = 1.0);

	// 選択肢の番号 (0 が自由) から刻みの数を出す。0 は丸めない
	static int stepsOf(int index, Choices choices = Choices::Level);

	// テストから触る口
	GuiComboBox& selector() { return stepSelector; }
private:
	void sliderValueChanged(juce::Slider* slider) override;
	void snapOne(GuiSlider& slider);

	GuiComboBox stepSelector;
	std::vector<GuiSlider*> m_sliders;
	Choices m_choices = Choices::Level;

	// 刻みの基準の幅。Level は 1、Ratio はつまみの範囲の幅
	double scaleOf(const GuiSlider& slider) const;
};
