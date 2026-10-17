#pragma once

#include <JuceHeader.h>

#include <functional>
#include <memory>
#include <vector>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiContext.h"

// ============================================================================
// TARGET の複写・消去のダイアログ (WT+ / ADPCM+)
// ============================================================================
// 画面の上へ重ねて出す。置く場所はタブが決める (WAVE MEMORY や FORM を
// 隠さないところ)。
//
//   複写: FROM (スライダー) と TO (番号のトグル、いくつでも) を選ぶ。
//         「パラメータも複写」を入れると、写し元のパラメータもすべて写す。
//         切ると波形 (音声) とファイル名だけを写す。
//   消去: TARGET (番号のトグル、いくつでも) を選ぶ。波形もパラメータも
//         初期の状態へ戻す。
//   区分の複写: 区分の見出しの右クリック (DUPLICATE) から出す。写し元は
//         いま見ている単位 (OP / PAD / TARGET) で、写し先をトグルで選ぶ。
//         FROM と「パラメータも複写」は出さない。
//
// ESC で閉じる。開いているときに同じボタンをもう一度押すと閉じる (open)。
class GuiTargetsDialog : public juce::Component, public GuiBaseComponent
{
public:
	enum class Mode { Duplicate, Clear, DuplicateTo };

	struct Config
	{
		Mode mode = Mode::Duplicate;

		// TARGET の数と、いま選んでいる TARGET (0 から数えた番目)
		int count = 32;
		int current = 0;

		// 画面に出す番号の始まり。TARGET のつまみと同じにする (WT+ / ADPCM+ は 0)
		int firstNumber = 0;

		// 複写する。from と to は 0 から数えた番目
		std::function<void(int from, const std::vector<int>& to, bool withParams)> onDuplicate = nullptr;

		// 消す。targets は 0 から数えた番目
		std::function<void(const std::vector<int>& targets)> onClear = nullptr;

		// 区分の複写 (DuplicateTo)。targets は 0 から数えた番目。写し元
		// (current) は選べない
		std::function<void(const std::vector<int>& targets)> onDuplicateTo = nullptr;

		// 見出しとトグルの名前。空なら既定 (見出しは種類から、トグルは番号)
		juce::String title;
		std::vector<juce::String> names;

		// 閉じたあとに呼ぶ。持ち主が片付けるときに使う
		std::function<void()> onClosed = nullptr;
	};

	GuiTargetsDialog(const GuiContext& context);

	// parent の area の中へ出す。高さは中身に合わせ、area を超えない。
	// 同じ種類のダイアログを開いているときは、開き直さずに閉じる。
	void open(juce::Component& parent, juce::Rectangle<int> area, const Config& c);
	void close();

	bool isOpen() const { return isVisible() && getParentComponent() != nullptr; }

	bool keyPressed(const juce::KeyPress& key) override;

	void paint(juce::Graphics& g) override;
	void resized() override;

	// 中身を並べるのに要る高さ (幅を決めたうえで)
	int preferredHeight(int width) const;

	// テストから触る口
	GuiSlider& fromSlider() { return from; }
	GuiToggleButton& targetToggle(int index) { return *toggles[(size_t)index]; }
	GuiToggleButton& withParamsToggle() { return withParams; }
	GuiTextButton& runButton() { return runBtn; }
	GuiTextButton& allOnButton() { return allOnBtn; }
	GuiTextButton& allOffButton() { return allOffBtn; }
private:
	void build();
	void applyFrom();
	std::vector<int> checked() const;
	int columnsFor(int width) const;

	Config config;

	GuiLabel title;
	GuiSlider from;
	GuiLabel targetsLabel;
	std::vector<std::unique_ptr<GuiToggleButton>> toggles;
	GuiTextButton allOnBtn, allOffBtn;
	GuiToggleButton withParams;
	GuiTextButton runBtn, cancelBtn;

	bool built = false;
};
