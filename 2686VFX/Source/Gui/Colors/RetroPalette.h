#pragma once

#include <JuceHeader.h>
#include <vector>

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiContext.h"

// ============================================================================
// 実機のパレット
// ============================================================================
// 当時の機種が出せた色を並べて、そこから選べるようにする。
//
// 各機種の色は「1 色あたり何ビットか」で決まるので、表を持たずに計算で
// 作れる。ただし段の明るさが等間隔でない機種があるので、その場合だけ
// 実測に基づく段の表を持つ。
class RetroPalette : public juce::Component
{
public:
	enum class Machine
	{
		Pc8801,     // RGB 各 3bit
		Pc9801,     // RGB 各 4bit
		PcEngine,   // RGB 各 3bit
		MegaDrive,  // RGB 各 3bit (段が等間隔でない)
		X68000,     // RGB 各 5bit + 輝度 1bit
	};

	// その機種が出せる色をすべて作る
	static std::vector<juce::Colour> colours(Machine machine);

	// 画面に出す名前と色数
	static juce::String name(Machine machine);
	static int columns(Machine machine);

	RetroPalette() { setInterceptsMouseClicks(true, false); }

	void setMachine(Machine machine);

	// 色が選ばれたときに呼ばれる
	std::function<void(juce::Colour)> onColourPicked;

	// 並べ終えたときの高さ。呼び出し側が領域を決めるのに使う。
	int preferredHeight(int width) const;

	void paint(juce::Graphics& g) override;
	void mouseDown(const juce::MouseEvent& e) override;
private:
	Machine m_machine = Machine::Pc8801;
	std::vector<juce::Colour> m_colours;

	// 1 色ぶんの大きさ
	static inline constexpr int cell = 10;

	int indexAt(juce::Point<int> position) const;
};
