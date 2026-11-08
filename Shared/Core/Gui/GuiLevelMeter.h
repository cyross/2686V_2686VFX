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
// SETTINGS で変えたらそのまま効く。色は描き方ごとに GuiColor::LevelMeter に
// あり、COLORS タブで変えられる。
//
// 下から伸びる帯なので、描き直すのは自分の矩形だけ。上に重なる部品も
// その範囲は描き直しになるので、速さの初期値は低めにしてある。
class GuiLevelMeter : public juce::Component, private juce::Timer
{
public:
	// 描き方。番号は設定ファイルに書くので、並びを変えないこと (足すのは後ろへ)
	enum Style
	{
		led = 0,
		bar,
		line,

		// 80s-90s 01〜04。当時のオーディオ機器の表示を思わせる描き方
		retro01,	// 青い丸い LED の点の並び
		retro02,	// 奥へすぼまる縞の柱 (カーオーディオの液晶)
		retro03,	// 蛍光表示管の横長の段と、点線の目盛り
		retro04,	// 細かな点を敷きつめた柱と、両端の目盛り

		styleCount
	};

	// 画面に出す名前 (SETTINGS の選択肢)
	static juce::String styleName(int style);

	// 選べる帯の数。初期値は GuiProcessorHost::levelMeterBands
	static constexpr int bandChoices[] = { 16, 32 };

	// 帯の数を選べるもののどれかへ寄せる (設定ファイルに書かれた値は選択肢と限らない)
	static int clampBands(int bands) { return bands <= 16 ? 16 : 32; }
	static int clampStyle(int style) { return juce::jlimit(0, (int)styleCount - 1, style); }

	explicit GuiLevelMeter(GuiProcessorHost& host);
	~GuiLevelMeter() override;

	// 狭い枠 (ミニプレイヤー・ミニマム) では、L / R の幅と段の刻みを詰める
	void setCompact(bool compact);

	void paint(juce::Graphics& g) override;

	// 出すかどうかが変わったとき (SETTINGS・表示モード) に呼ぶ。描き直しの速さも合わせる
	void refresh();

	// 音の側が書いた音を取り込み、値を進めて描き直す。刻みごとに呼ばれる (出していないときは何もしない)
	void pull();

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

	// 段の刻み (px)。描き方と枠の広さで変わる
	float pitchOf(int style) const;

	// 点を敷きつめる描き方 (80s-90s 01 / 04) の、1 刻みぶんの絵。柱ごとに点を 1 つずつ
	// 描くと数千回になるので、絵を作っておいて柱は 1 回の塗りで済ませる。
	// 色か大きさが変わったときだけ作り直す
	enum TileSlot { dotsOff = 0, dotsOn, dotsPeak, matrixOff, matrixOn, matrixPeak, tileSlots };

	struct Tile
	{
		juce::Image image;
		juce::Colour fill;
		juce::Colour glow;
		int size = 0;
	};

	// スペクトラムと L / R で色が違うので、別々に持つ
	mutable std::array<Tile, tileSlots * 2> tiles;

	const juce::Image& tileFor(TileSlot slot, bool lr, int size, juce::Colour fill, juce::Colour glow) const;

	// ---- 描き方ごと。column は 1 本の柱の矩形、level / peak は 0〜1
	// lr は L / R の柱か (色を Lr の下から取る)
	void drawColumn(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, int style, bool peakHold, bool lr) const;

	void drawLed(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, bool peakHold, bool lr) const;
	void drawBar(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, bool peakHold, bool lr) const;
	void drawLineColumn(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, bool peakHold, bool lr) const;
	void drawBlueDots(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, bool peakHold, bool lr) const;
	void drawStripes(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, bool peakHold, bool lr) const;
	void drawVfd(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, bool peakHold, bool lr) const;
	void drawDotMatrix(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, bool peakHold, bool lr) const;

	// スペクトラムを 1 枚の絵として描くもの (線・奥へすぼまる柱)
	void drawSpectrumLine(juce::Graphics& g, juce::Rectangle<float> area, int bands, bool peakHold) const;
	void drawSpectrumPerspective(juce::Graphics& g, juce::Rectangle<float> area, int bands, bool peakHold) const;

	// 柱の並びの飾り (蛍光表示管の点線、点の柱の両端の目盛り)
	void drawVfdScale(juce::Graphics& g, float x, juce::Rectangle<float> area) const;
	void drawDotScale(juce::Graphics& g, float x, juce::Rectangle<float> area) const;
};
