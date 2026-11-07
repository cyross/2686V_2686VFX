#include "./GuiLevelMeter.h"

#include <cmath>

#include "./GuiColor.h"
#include "./GuiHost.h"
#include "./GuiLiveGraph.h"

namespace
{
	// 0 にあたる大きさ。これより小さい音は帯を立てない
	constexpr float floorDb = -60.0f;

	// スペクトラムの範囲
	constexpr double lowHz = 30.0;
	constexpr double highHz = 16000.0;

	// 落ちる速さ (1 秒あたり、0〜1 の範囲に対して)
	constexpr float fallPerSecond = 1.2f;
	constexpr float peakFallPerSecond = 0.5f;

	// ピークを保つ時間 (ms)
	constexpr double peakHoldMs = 1000.0;

	// 黄色・赤にする高さ (0〜1)
	constexpr float midFrom = 0.70f;
	constexpr float highFrom = 0.90f;

	// LED の 1 段 (灯る部分 + すき間)
	constexpr float ledPitch = 6.0f;
	constexpr float ledPitchCompact = 4.0f;

	// L / R の柱の幅とすき間
	constexpr float lrColumnWidth = 18.0f;
	constexpr float lrColumnWidthCompact = 10.0f;
	constexpr float lrGap = 6.0f;
	constexpr float lrGapCompact = 3.0f;

	// スペクトラムと L / R のあいだ
	constexpr float sectionGap = 16.0f;
	constexpr float sectionGapCompact = 8.0f;

	juce::Colour colourAt(float height)
	{
		if (height >= highFrom) return GuiColor::LevelMeter::High;
		if (height >= midFrom) return GuiColor::LevelMeter::Mid;

		return GuiColor::LevelMeter::Low;
	}
}

GuiLevelMeter::GuiLevelMeter(GuiProcessorHost& h) : host(h)
{
	setInterceptsMouseClicks(false, false);
	setOpaque(false);

	// Hann 窓。正弦波の大きさがそのまま読めるよう、あとで 4 / fftSize を掛ける
	for (int i = 0; i < fftSize; ++i) {
		window[(size_t)i] = 0.5f - 0.5f * std::cos(juce::MathConstants<float>::twoPi * (float)i / (float)(fftSize - 1));
	}

	refresh();
}

GuiLevelMeter::~GuiLevelMeter()
{
	stopTimer();

	host.levelMeterFeed.active.store(false, std::memory_order_relaxed);
}

void GuiLevelMeter::setCompact(bool c)
{
	if (compact == c) return;

	compact = c;
	repaint();
}

bool GuiLevelMeter::shouldRun() const
{
	return host.levelMeterShow && isVisible();
}

void GuiLevelMeter::refresh()
{
	const bool run = shouldRun();

	auto& feed = host.levelMeterFeed;

	if (run != feed.active.load(std::memory_order_relaxed)) {
		if (!run) feed.clear();

		feed.active.store(run, std::memory_order_relaxed);
	}

	if (!run) {
		std::fill(bandLevel.begin(), bandLevel.end(), 0.0f);
		std::fill(bandPeak.begin(), bandPeak.end(), 0.0f);
		lrLevel.fill(0.0f);
		lrPeak.fill(0.0f);
	}

	// 切っているあいだも、入れ直したときに追えるようゆっくり見に行く
	LiveGraph::pace(*this, run ? LiveGraph::clampFps(host.levelMeterFps) : LiveGraph::idleHz);

	repaint();
}

void GuiLevelMeter::timerCallback()
{
	const bool run = shouldRun();

	// SETTINGS で入り切り・速さを変えたら、ここで追う
	if (run != host.levelMeterFeed.active.load(std::memory_order_relaxed)
		|| getTimerInterval() != 1000 / (run ? LiveGraph::clampFps(host.levelMeterFps) : LiveGraph::idleHz)) {
		refresh();
	}

	if (!run) return;

	const double now = juce::Time::getMillisecondCounterHiRes();
	const double dt = lastTickMs > 0.0 ? juce::jlimit(0.0, 0.25, (now - lastTickMs) / 1000.0) : 0.0;

	lastTickMs = now;

	updateSpectrum(clampBands(host.levelMeterBands), dt, now);
	updateLr(dt, now);

	repaint();
}

float GuiLevelMeter::toNorm(float magnitude)
{
	if (magnitude <= 0.0f) return 0.0f;

	const float db = juce::Decibels::gainToDecibels(magnitude, floorDb);

	return juce::jlimit(0.0f, 1.0f, (db - floorDb) / -floorDb);
}

void GuiLevelMeter::follow(float input, float& level, float& peak, double& peakTime, double dt, double now)
{
	level = juce::jmax(input, level - fallPerSecond * (float)dt);

	if (level >= peak) {
		peak = level;
		peakTime = now;
	}
	else if (now - peakTime > peakHoldMs) {
		peak = juce::jmax(level, peak - peakFallPerSecond * (float)dt);
	}
}

void GuiLevelMeter::updateSpectrum(int bands, double dt, double now)
{
	if ((int)bandLevel.size() != bands) {
		bandLevel.assign((size_t)bands, 0.0f);
		bandPeak.assign((size_t)bands, 0.0f);
		bandPeakTime.assign((size_t)bands, 0.0);
	}

	auto& feed = host.levelMeterFeed;

	feed.copyLatest(fftData.data(), fftSize);

	for (int i = 0; i < fftSize; ++i) fftData[(size_t)i] *= window[(size_t)i];

	std::fill(fftData.begin() + fftSize, fftData.end(), 0.0f);

	fft.performFrequencyOnlyForwardTransform(fftData.data(), true);

	const double rate = juce::jmax(1.0, feed.sampleRate.load(std::memory_order_relaxed));
	const double binHz = rate / (double)fftSize;
	const double top = juce::jmin(highHz, rate * 0.5);
	const int lastBin = fftSize / 2 - 1;

	// 正弦波の振幅がそのまま読めるようにする (Hann の和は fftSize / 2、実数の正弦波は半分ずつ)
	const float scale = 4.0f / (float)fftSize;

	for (int b = 0; b < bands; ++b) {
		// 帯の端は対数で等しく分ける
		const double lo = lowHz * std::pow(top / lowHz, (double)b / bands);
		const double hi = lowHz * std::pow(top / lowHz, (double)(b + 1) / bands);

		int from = (int)std::ceil(lo / binHz);
		int to = (int)std::floor(hi / binHz);

		from = juce::jlimit(1, lastBin, from);
		to = juce::jlimit(1, lastBin, to);

		float magnitude = 0.0f;

		if (to >= from) {
			for (int k = from; k <= to; ++k) magnitude = juce::jmax(magnitude, fftData[(size_t)k]);
		}
		else {
			// 低い帯は 1 つの点より狭いので、真ん中にいちばん近い点を使う
			const int k = juce::jlimit(1, lastBin, (int)std::lround(std::sqrt(lo * hi) / binHz));

			magnitude = fftData[(size_t)k];
		}

		follow(toNorm(magnitude * scale), bandLevel[(size_t)b], bandPeak[(size_t)b], bandPeakTime[(size_t)b], dt, now);
	}
}

void GuiLevelMeter::updateLr(double dt, double now)
{
	auto& feed = host.levelMeterFeed;

	const float input[2] = { feed.takePeakL(), feed.takePeakR() };

	for (size_t ch = 0; ch < 2; ++ch) follow(toNorm(input[ch]), lrLevel[ch], lrPeak[ch], lrPeakTime[ch], dt, now);
}

juce::Rectangle<float> GuiLevelMeter::lrArea() const
{
	const float column = compact ? lrColumnWidthCompact : lrColumnWidth;
	const float gap = compact ? lrGapCompact : lrGap;

	return getLocalBounds().toFloat().removeFromRight(column * 2.0f + gap);
}

juce::Rectangle<float> GuiLevelMeter::spectrumArea() const
{
	auto r = getLocalBounds().toFloat();

	r.removeFromRight(lrArea().getWidth() + (compact ? sectionGapCompact : sectionGap));

	return r;
}

void GuiLevelMeter::paint(juce::Graphics& g)
{
	if (!host.levelMeterShow) return;

	const int style = clampStyle(host.levelMeterStyle);
	const bool peakHold = host.levelMeterPeakHold;

	// ---- スペクトラム
	const auto spectrum = spectrumArea();
	const int bands = (int)bandLevel.size();

	if (bands > 0 && spectrum.getWidth() > 0.0f) {
		if (style == line) {
			drawSpectrumLine(g, spectrum, bands, peakHold);
		}
		else {
			const float pitch = spectrum.getWidth() / (float)bands;
			const float gap = juce::jmax(1.0f, pitch * 0.2f);

			for (int b = 0; b < bands; ++b) {
				const juce::Rectangle<float> column(spectrum.getX() + pitch * (float)b, spectrum.getY(), pitch - gap, spectrum.getHeight());

				drawColumn(g, column, bandLevel[(size_t)b], bandPeak[(size_t)b], style, peakHold);
			}
		}
	}

	// ---- L / R
	auto lr = lrArea();
	const float column = compact ? lrColumnWidthCompact : lrColumnWidth;
	const float gap = compact ? lrGapCompact : lrGap;

	for (size_t ch = 0; ch < 2; ++ch) {
		const auto area = lr.removeFromLeft(column);

		lr.removeFromLeft(gap);

		drawColumn(g, area, lrLevel[ch], lrPeak[ch], style, peakHold);
	}
}

void GuiLevelMeter::drawColumn(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, int style, bool peakHold) const
{
	const float h = column.getHeight();
	const float bottom = column.getBottom();

	if (style == led) {
		const float pitch = compact ? ledPitchCompact : ledPitch;
		const int count = juce::jmax(1, (int)(h / pitch));
		const float lit = level * (float)count;
		const int peakIndex = (int)std::ceil(peak * (float)count) - 1;

		for (int i = 0; i < count; ++i) {
			const float y = bottom - pitch * (float)(i + 1);
			const juce::Rectangle<float> seg(column.getX(), y + 1.0f, column.getWidth(), pitch - 2.0f);
			const float height = (float)(i + 1) / (float)count;

			const bool on = (float)i < lit;
			const bool isPeak = peakHold && i == peakIndex && peak > 0.0f;

			g.setColour(on || isPeak ? colourAt(height) : (juce::Colour)GuiColor::LevelMeter::LedOff);
			g.fillRect(seg);
		}

		return;
	}

	if (style == bar) {
		if (level > 0.0f) {
			const float top = bottom - h * level;

			juce::ColourGradient grad(GuiColor::LevelMeter::Low, 0.0f, bottom, GuiColor::LevelMeter::High, 0.0f, column.getY(), false);
			grad.addColour(midFrom, GuiColor::LevelMeter::Mid);

			g.setGradientFill(grad);
			g.fillRect(juce::Rectangle<float>(column.getX(), top, column.getWidth(), bottom - top));
		}
	}
	else {
		// 線。いまの高さに横線を引き、下を薄く塗る
		const float top = bottom - h * level;

		g.setColour(GuiColor::LevelMeter::LineFill);
		g.fillRect(juce::Rectangle<float>(column.getX(), top, column.getWidth(), bottom - top));

		if (level > 0.0f) {
			g.setColour(GuiColor::LevelMeter::Line);
			g.drawLine(column.getX(), top, column.getRight(), top, 2.0f);
		}
	}

	if (peakHold && peak > 0.0f) {
		const float y = bottom - h * peak;

		g.setColour(GuiColor::LevelMeter::Peak);
		g.drawLine(column.getX(), y, column.getRight(), y, 2.0f);
	}
}

void GuiLevelMeter::drawSpectrumLine(juce::Graphics& g, juce::Rectangle<float> area, int bands, bool peakHold) const
{
	const float pitch = area.getWidth() / (float)bands;
	const float bottom = area.getBottom();
	const float h = area.getHeight();

	auto pointAt = [&](const std::vector<float>& values, int b) {
		return juce::Point<float>(area.getX() + pitch * ((float)b + 0.5f), bottom - h * values[(size_t)b]);
	};

	juce::Path path;
	path.startNewSubPath(pointAt(bandLevel, 0));

	for (int b = 1; b < bands; ++b) path.lineTo(pointAt(bandLevel, b));

	juce::Path fill(path);
	fill.lineTo(area.getX() + pitch * ((float)bands - 0.5f), bottom);
	fill.lineTo(area.getX() + pitch * 0.5f, bottom);
	fill.closeSubPath();

	g.setColour(GuiColor::LevelMeter::LineFill);
	g.fillPath(fill);

	g.setColour(GuiColor::LevelMeter::Line);
	g.strokePath(path, juce::PathStrokeType(2.0f));

	if (!peakHold) return;

	// ピークは帯ごとの短い横線
	g.setColour(GuiColor::LevelMeter::Peak);

	for (int b = 0; b < bands; ++b) {
		if (bandPeak[(size_t)b] <= 0.0f) continue;

		const auto p = pointAt(bandPeak, b);

		g.drawLine(p.x - pitch * 0.35f, p.y, p.x + pitch * 0.35f, p.y, 2.0f);
	}
}
