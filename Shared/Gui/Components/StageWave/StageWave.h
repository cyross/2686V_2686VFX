#pragma once

#include <array>
#include <functional>
#include <memory>

#include <JuceHeader.h>

#include "./StageWaveRender.h"
#include "Shared/Core/Gui/GuiColor.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Gui/Components/GenWave/GenWavePlayer.h"

// ============================================================================
// 生成中の波形の窓 (4.4.0)
// ============================================================================
// 音源の中で波形がどう作られていくかを、段ごとに見せる。別の窓 (GuiStageWaveWindow)
// の中身で、従来の生成波形の表示もここへ移した。
//
//   第 1 段: FM は変調前の各オペレーター、RHYTHM は各パッド、その他は加工前の音
//   第 2 段: FM だけ。変調後の各オペレーター
//   第 3 段: ① 加工前 ② 音源の加工後 ③ ユニゾン後 ④ CHIP DAC 後 ⑤ FX 後 (完成)
//
// 枠は横に 8 個。縦・横の倍率は全部の枠で同じで、枠の大きさは変えない。
// 「生成」を押したときだけ作り、作ったものの上を窓が動いていく (従来の生成波形と同じ)。
class GuiStageWave : public juce::Component, private juce::Timer
{
public:
	static constexpr int margin = 10;
	static constexpr int viewWidth = 210;
	static constexpr int viewHeight = 129;
	static constexpr int viewGap = 6;
	static constexpr int controlHeight = 24;
	static constexpr int rowLabelHeight = 20;
	static constexpr int rowGap = 8;

	static constexpr int contentWidth = margin * 2 + StageWaveRender::viewsPerRow * viewWidth + (StageWaveRender::viewsPerRow - 1) * viewGap;
	// 操作 3 段と、1 段目のすぐ下に置く再生の段、その下に枠を 3 段
	static constexpr int contentHeight = margin + (controlHeight + viewGap) * 3 + GenWavePlayer::height + viewGap + rowGap
		+ (rowLabelHeight + viewHeight + rowGap) * 3 - rowGap + margin;

	// 窓の幅の選択肢 (周期数)。RHYTHM は 1 周期を 10ms とみなす
	static constexpr std::array<int, 4> cycleChoices{ 1, 2, 5, 10 };

	static constexpr std::array<int, 13> tempoPresets{ 60, 80, 90, 100, 110, 120, 130, 140, 150, 160, 174, 180, 200 };

	static constexpr int frameMs = 33;

	explicit GuiStageWave(const GuiContext& context);
	~GuiStageWave() override;

	// 作る (ボタン・Ctrl+M から)
	void requestGenerate() { startGenerate(); }

	// 鳴らしている音源の種類が変わっていないかを見直す
	void refreshInfo();

	// 出来上がりを受け取る。メッセージスレッドで呼ぶこと
	void finishGenerate(const StageWaveRender::Result& result);

	const StageWaveRender::Result& result() const { return m_result; }
	const GuiStageInfo& info() const { return m_info; }

	// 今の画面の値から作る条件を組む (テストからも使う)
	StageWaveRender::Options currentOptions() const;

	void paint(juce::Graphics& g) override;
	void resized() override;
	void mouseDown(const juce::MouseEvent& event) override;
	void mouseDrag(const juce::MouseEvent& event) override;
	void mouseUp(const juce::MouseEvent& event) override;
	void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;

private:
	GuiContext ctx;

	juce::TextButton generateBtn;
	juce::TextButton deleteBtn;

	juce::Label keyLabel;
	juce::Slider keySlider;

	juce::Label tempoLabel;
	juce::Slider tempoSlider;
	juce::ComboBox tempoPresetBox;
	juce::ToggleButton followDawToggle;

	juce::Label barsLabel;
	juce::ComboBox barsBox;

	struct NoteControls
	{
		juce::Label label;
		juce::ComboBox division;
		juce::ToggleButton triplet;
		juce::ToggleButton dotted;
	};

	NoteControls intervalControls;
	NoteControls lengthControls;

	juce::Label switchesLabel;
	std::array<juce::ToggleButton, StageSwitch::count> moduleToggles;

	juce::Label padsLabel;
	std::array<juce::ToggleButton, StageProbe::maxOps> padToggles;

	GuiStageInfo m_info;
	StageWaveRender::Result m_result;

	GenWavePlayer m_player;

	bool m_busy = false;

	juce::ThreadPool m_pool{ 1 };

	// 様子を見直す間隔を数える
	int m_idleTicks = 0;

	void setupNoteControls(NoteControls& c, const juce::String& title, int& division, bool& triplet, bool& dotted);
	void syncFromHost();
	void updateVisibility();

	void startGenerate();
	void clearResult();

	// 区画
	juce::Rectangle<int> rowArea(int row) const;
	juce::Rectangle<int> viewArea(int row, int column) const;
	juce::Rectangle<int> cycleArea() const;
	juce::Rectangle<int> cycleCell(int index) const;
	juce::Rectangle<int> zoomBox() const;
	juce::Rectangle<int> playerArea() const;

	// 時間の軸に使う波形 (出来上がり)
	const GenWaveRender::Wave* timeline() const;

	int windowSamples() const;

	void drawView(juce::Graphics& g, juce::Rectangle<int> area, const juce::String& title,
		const GenWaveRender::Wave* wave, bool stereo, bool off);

	juce::String rowTitle(int row) const;
	juce::String viewTitle(int row, int column) const;

	void timerCallback() override;

	// 色を部品へ写す。COLORS で変えたときにも呼ばれる
	void applyColours();

	// 部品より後に置く (先に壊れて、壊れた部品へ写しに来ないように)
	GuiColor::Refresher m_colourRefresher{ [this] { applyColours(); } };
};

// 生成中の波形を入れる別の窓
class GuiStageWaveWindow : public juce::DocumentWindow
{
public:
	GuiStageWaveWindow(const GuiContext& context, std::function<void()> onClose);

	GuiStageWave& content() { return *m_content; }

	void closeButtonPressed() override { if (m_onClose) m_onClose(); }

private:
	GuiStageWave* m_content = nullptr;
	std::function<void()> m_onClose;

	// 窓の地も COLORS の変更に付いていく
	GuiColor::Refresher m_colourRefresher{ [this] { setBackgroundColour(GuiColor::GenWave::WindowBg); } };
};
