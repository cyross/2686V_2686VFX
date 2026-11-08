#pragma once

#include <array>
#include <functional>
#include <memory>
#include <vector>

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
// 枠は横に 8 個。縦・横の倍率は全部の枠で同じ。枠は窓の大きさに合わせて伸び縮みする
// (最小は最初の大きさの縦横半分)。窓の最小も最初の大きさ (contentWidth / contentHeight)
// の縦横半分で、操作の段が窓に収まらないときは、操作の段だけを横へずらすスクロールバーを出す。
// 「生成」を押したときだけ作り、作ったものの上を窓が動いていく (従来の生成波形と同じ)。
class GuiStageWave : public juce::Component, private juce::Timer, private juce::ScrollBar::Listener
{
public:
	static constexpr int margin = 10;
	// 枠 1 つの最初の大きさ。窓に合わせて伸び縮みする (currentViewWidth / currentViewHeight)
	static constexpr int viewWidth = 210;
	static constexpr int viewHeight = 129;

	// 枠 1 つの最小の大きさ (最初の大きさの縦横半分)
	static constexpr int minViewWidth = viewWidth / 2;
	static constexpr int minViewHeight = viewHeight / 2;
	static constexpr int viewGap = 6;
	static constexpr int controlHeight = 24;
	static constexpr int rowLabelHeight = 20;
	static constexpr int rowGap = 8;

	static constexpr int contentWidth = margin * 2 + StageWaveRender::viewsPerRow * viewWidth + (StageWaveRender::viewsPerRow - 1) * viewGap;
	// 操作 3 段と、1 段目のすぐ下に置く再生の段、その下に枠を 3 段
	static constexpr int contentHeight = margin + (controlHeight + viewGap) * 3 + GenWavePlayer::height + viewGap + rowGap
		+ (rowLabelHeight + viewHeight + rowGap) * 3 - rowGap + margin;

	// 窓の最小 (最初の大きさの縦横半分) と最大
	static constexpr int minWindowWidth = contentWidth / 2;
	static constexpr int minWindowHeight = contentHeight / 2;
	static constexpr int maxContentWidth = 8000;
	static constexpr int maxContentHeight = 6000;

	// 操作の段を横へずらすスクロールバーの太さ
	static constexpr int controlScrollHeight = 12;

	// 窓の幅の選択肢 (周期数)。RHYTHM は 1 周期を 10ms とみなす
	static constexpr std::array<int, 4> cycleChoices{ 1, 2, 5, 10 };

	static constexpr std::array<int, 13> tempoPresets{ 60, 80, 90, 100, 110, 120, 130, 140, 150, 160, 174, 180, 200 };

	// 再生の 1 秒あたりのコマ数の選択肢
	static constexpr std::array<int, 4> fpsChoices{ 15, 30, 60, 120 };

	// 拍の音符の選択肢 (4 分音符・8 分音符) と、それぞれの拍の数の既定
	static constexpr std::array<int, 2> beatNotes{ 4, 8 };
	static constexpr int beatCountMax = 12;

	static constexpr int defaultBeatCount(int note) { return note == 8 ? 6 : 4; }

	// 区分のあいだの縦線の太さ
	static constexpr int dividerWidth = 2;

	// 「表示周期」の見出しの幅と、周期のボタン 1 つの幅
	static constexpr int cycleLabelWidth = 58;
	static constexpr int cycleCellWidth = 34;

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

	// 今の窓の大きさでの枠 1 つの大きさ (最小は minViewWidth / minViewHeight)
	int currentViewWidth() const;
	int currentViewHeight() const;

	// 中身がつぶれずに並ぶ最小の大きさ。窓がこれより小さければ外側のスクロールで見せる
	int minimumWidth() const;
	int minimumHeight(int forWidth) const;

	// 操作の段の幅 (ずらす前の右端 + 余白) と、横へのずらし
	int controlsWidth() const { return m_controlsWidth; }
	bool isControlScrollShown() const { return m_controlScrollBar.isVisible(); }
	int controlScroll() const { return m_controlScroll; }
	void setControlScroll(int x);

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

	juce::Label fpsLabel;
	juce::ComboBox fpsBox;

	juce::Label tempoLabel;
	juce::Slider tempoSlider;
	juce::ComboBox tempoPresetBox;
	juce::ToggleButton followDawToggle;

	// 1 小節の拍 (音符と数)。テンポと小節のあいだに置く
	juce::Label beatLabel;
	juce::ComboBox beatNoteBox;
	juce::Slider beatCountSlider;

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

	// 最後のリリースを反映するか
	juce::ToggleButton releaseTailToggle;

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

	// 区分のあいだに引く縦線 (resized で決める)
	std::vector<juce::Rectangle<int>> m_dividers;

	// 操作の段 (1〜3 段目) を横へずらす。窓に収まらないときだけ出す
	juce::ScrollBar m_controlScrollBar{ false };
	int m_controlScroll = 0;
	int m_controlsWidth = 0;

	void scrollBarMoved(juce::ScrollBar* bar, double newRangeStart) override;

	// 枠の段の上端 (操作の段・再生の段・スクロールバーの下)
	int viewsTop(bool controlScrollShown) const;

	// 左の部品の右端と、右の部品の左端のあいだへ縦線を足す
	void addDivider(int leftEdge, int rightEdge, int y);

	// DAW 追従のときはテンポと拍を DAW に任せるので、押せなくする
	void updateFollowDawState();

	// FPS を再生と描き直しの間隔へ当てる
	void applyFps();

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
