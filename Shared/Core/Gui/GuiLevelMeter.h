#pragma once

#include <JuceHeader.h>

#include <array>
#include <vector>

#include "Shared/Core/Synth/LevelMeterFeed.h"

class GuiProcessorHost;

// ============================================================================
// レベルメーター (4.4.0)
// ============================================================================
// 壁紙と UI のあいだに置く。左に周波数ごとの大きさ (スペクトラム)、右端に
// L / R の大きさ。エディタは自分の子のいちばん奥へ入れ、場所 (setBounds) を
// 決めるだけでよい。マウスは受けない (上に重なる UI へ素通しする)。
//
// 音の側は出し終えた音を LevelMeterFeed へ書くだけで、FFT はここでやる。
// 入り切り・速さ (FPS)・描き方・帯の数・ピークホールドは SETTINGS で選び、
// GuiProcessorHost に持つ (設定ファイルへ保存)。値は毎回の刻みで読み直すので、
// SETTINGS で変えたらそのまま効く。
//
// 下から伸びる帯なので、描き直すのは自分の矩形だけ。上に重なる部品も
// その範囲は描き直しになるので、速さの初期値は低めにしてある。
class GuiLevelMeter : public juce::Component, private juce::Timer
{
public:
	// 描き方
	enum Style { led = 0, bar = 1, line = 2, styleCount };

	// 選べる帯の数。初期値は GuiProcessorHost::levelMeterBands
	static constexpr int bandChoices[] = { 16, 32 };

	// 帯の数を選べるもののどれかへ寄せる (設定ファイルに書かれた値は選択肢と限らない)
	static int clampBands(int bands) { return bands <= 16 ? 16 : 32; }
	static int clampStyle(int style) { return juce::jlimit(0, (int)styleCount - 1, style); }

	explicit GuiLevelMeter(GuiProcessorHost& host);
	~GuiLevelMeter() override;

	// 狭い枠 (ミニプレイヤー・ミニマム) では、L / R の幅と LED の刻みを詰める
	void setCompact(bool compact);

	void paint(juce::Graphics& g) override;

	// 出すかどうかが変わったとき (SETTINGS・表示モード) に呼ぶ。描き直しの速さも合わせる
	void refresh();

private:
	GuiProcessorHost& host;

	bool compact = false;

	// ---- FFT
	static constexpr int fftOrder = 11;
	static constexpr int fftSize = 1 << fftOrder;

	juce::dsp::FFT fft{ fftOrder };
	std::array<float, fftSize> window{};
	std::array<float, fftSize * 2> fftData{};

	// ---- いま描いている値 (0〜1。-60dB〜0dB)
	std::vector<float> bandLevel;
	std::vector<float> bandPeak;
	std::vector<double> bandPeakTime;

	std::array<float, 2> lrLevel{};
	std::array<float, 2> lrPeak{};
	std::array<double, 2> lrPeakTime{};

	double lastTickMs = 0.0;

	void timerCallback() override;

	bool shouldRun() const;

	void updateSpectrum(int bands, double dt, double now);
	void updateLr(double dt, double now);

	// 新しい値を受け、落ちるときはゆっくり落とす。ピークは少し保ってから落とす
	static void follow(float input, float& level, float& peak, double& peakTime, double dt, double now);

	// dB を 0〜1 へ
	static float toNorm(float magnitude);

	juce::Rectangle<float> lrArea() const;
	juce::Rectangle<float> spectrumArea() const;

	void drawColumn(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, int style, bool peakHold) const;
	void drawSpectrumLine(juce::Graphics& g, juce::Rectangle<float> area, int bands, bool peakHold) const;
};
