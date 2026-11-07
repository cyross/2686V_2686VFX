#include "./GuiLevelMeter.h"

#include <cmath>
#include <functional>

#include "./GuiColor.h"
#include "./GuiHost.h"
#include "./GuiI18n.h"
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

	// LED・バーを黄色・赤にする高さ (0〜1)
	constexpr float midFrom = 0.70f;
	constexpr float highFrom = 0.90f;

	// L / R の柱の幅とすき間
	constexpr float lrColumnWidth = 18.0f;
	constexpr float lrColumnWidthCompact = 10.0f;
	constexpr float lrGap = 6.0f;
	constexpr float lrGapCompact = 3.0f;

	// スペクトラムと L / R のあいだ
	constexpr float sectionGap = 16.0f;
	constexpr float sectionGapCompact = 8.0f;

	// 80s-90s 02 の柱が、てっぺんでどこまですぼまるか (下の幅に対する割合)
	constexpr float perspectiveTop = 0.55f;

	// 80s-90s 04 の両端の目盛りの幅と、赤い印の高さ (0〜1。-12dB のあたり)
	constexpr float dotScaleWidth = 8.0f;
	constexpr float dotScaleMark = 0.8f;
	constexpr int dotScaleCount = 12;

	const I18n::Text styleNames[] = {
		I18n::Text{ u8"LED", u8"LED" },
		I18n::Text{ u8"バー", u8"Bar" },
		I18n::Text{ u8"線", u8"Line" },
		I18n::Text{ u8"80s-90s 01" },
		I18n::Text{ u8"80s-90s 02" },
		I18n::Text{ u8"80s-90s 03" },
		I18n::Text{ u8"80s-90s 04" },
	};

	static_assert(std::size(styleNames) == GuiLevelMeter::styleCount);

	juce::Colour ledColourAt(float height)
	{
		if (height >= highFrom) return GuiColor::LevelMeter::Led::High;
		if (height >= midFrom) return GuiColor::LevelMeter::Led::Mid;

		return GuiColor::LevelMeter::Led::Low;
	}

	// 下から数えて、level (0〜1) のうち灯る段の数
	int litCount(float level, int count) { return juce::jlimit(0, count, (int)std::ceil(level * (float)count - 1.0e-4f)); }

	// ピークの段 (下から 0 始まり)。無ければ -1
	int peakIndex(float peak, int count) { return peak > 0.0f ? juce::jlimit(0, count - 1, (int)std::ceil(peak * (float)count) - 1) : -1; }

	// 縞の柱 (80s-90s 02)。mapX で横の位置を変える (奥へすぼめる)。t は下から 0〜1
	void drawStriped(juce::Graphics& g, float x0, float x1, juce::Rectangle<float> area, float level, float peak, bool peakHold,
		float pitch, const std::function<float(float x, float t)>& mapX)
	{
		const float bottom = area.getBottom();
		const float h = area.getHeight();
		const int count = juce::jmax(1, (int)(h / pitch));
		const int lit = litCount(level, count);

		auto quad = [&](float yLow, float yHigh) {
			const float tLow = (bottom - yLow) / h;
			const float tHigh = (bottom - yHigh) / h;

			juce::Path p;
			p.startNewSubPath(mapX(x0, tLow), yLow);
			p.lineTo(mapX(x1, tLow), yLow);
			p.lineTo(mapX(x1, tHigh), yHigh);
			p.lineTo(mapX(x0, tHigh), yHigh);
			p.closeSubPath();

			return p;
		};

		for (int i = 0; i < lit; ++i) {
			const float yLow = bottom - pitch * (float)i - 1.0f;
			const float yHigh = yLow - pitch * 0.6f;
			const float t = (float)(i + 1) / (float)count;

			g.setColour(GuiColor::LevelMeter::Retro02::Bottom.get().interpolatedWith(GuiColor::LevelMeter::Retro02::Top, t));
			g.fillPath(quad(yLow, yHigh));
		}

		const int pi = peakIndex(peak, count);

		if (peakHold && pi >= 0) {
			const float yLow = bottom - pitch * (float)pi - 1.0f;

			g.setColour(GuiColor::LevelMeter::Retro02::Peak);
			g.fillPath(quad(yLow, yLow - pitch * 0.6f));
		}
	}
}

juce::String GuiLevelMeter::styleName(int style)
{
	return styleNames[(size_t)clampStyle(style)];
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

	pull();
}

void GuiLevelMeter::pull()
{
	if (!shouldRun()) return;

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

float GuiLevelMeter::pitchOf(int style) const
{
	switch (style) {
	case retro01: return compact ? 6.0f : 12.0f;
	case retro02: return compact ? 4.0f : 7.0f;
	case retro03: return compact ? 4.0f : 6.0f;
	case retro04: return compact ? 3.0f : 4.0f;
	default:      return compact ? 4.0f : 6.0f;
	}
}

void GuiLevelMeter::paint(juce::Graphics& g)
{
	if (!host.levelMeterShow) return;

	const int style = clampStyle(host.levelMeterStyle);
	const bool peakHold = host.levelMeterPeakHold;

	// ---- スペクトラム。まだ測っていなくても (鳴らす前)、灯っていない段は描く
	const int bands = clampBands(host.levelMeterBands);

	if ((int)bandLevel.size() != bands) {
		bandLevel.assign((size_t)bands, 0.0f);
		bandPeak.assign((size_t)bands, 0.0f);
		bandPeakTime.assign((size_t)bands, 0.0);
	}

	auto spectrum = spectrumArea();

	if (bands > 0 && spectrum.getWidth() > 0.0f) {
		if (style == line) {
			drawSpectrumLine(g, spectrum, bands, peakHold);
		}
		else if (style == retro02) {
			drawSpectrumPerspective(g, spectrum, bands, peakHold);
		}
		else {
			// 点の柱は、両端に目盛りを置く
			if (style == retro04) {
				drawDotScale(g, spectrum.getX(), spectrum);
				drawDotScale(g, spectrum.getRight() - dotScaleWidth, spectrum);

				spectrum = spectrum.reduced(dotScaleWidth + 4.0f, 0.0f);
			}

			const float pitch = spectrum.getWidth() / (float)bands;
			const float gap = juce::jmax(1.0f, pitch * (style == retro03 ? 0.3f : 0.2f));

			for (int b = 0; b < bands; ++b) {
				const float x = spectrum.getX() + pitch * (float)b;

				// 蛍光表示管は、柱のあいだに点線の目盛り
				if (style == retro03) drawVfdScale(g, x, spectrum);

				const juce::Rectangle<float> column(x + (style == retro03 ? gap * 0.5f : 0.0f), spectrum.getY(), pitch - gap, spectrum.getHeight());

				drawColumn(g, column, bandLevel[(size_t)b], bandPeak[(size_t)b], style, peakHold);
			}

			if (style == retro03) drawVfdScale(g, spectrum.getRight(), spectrum);
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
	switch (style) {
	case bar:     drawBar(g, column, level, peak, peakHold); break;
	case line:    drawLineColumn(g, column, level, peak, peakHold); break;
	case retro01: drawBlueDots(g, column, level, peak, peakHold); break;
	case retro02: drawStripes(g, column, level, peak, peakHold); break;
	case retro03: drawVfd(g, column, level, peak, peakHold); break;
	case retro04: drawDotMatrix(g, column, level, peak, peakHold); break;
	default:      drawLed(g, column, level, peak, peakHold); break;
	}
}

// ---------------------------------------------------------------- LED
void GuiLevelMeter::drawLed(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, bool peakHold) const
{
	const float pitch = pitchOf(led);
	const int count = juce::jmax(1, (int)(column.getHeight() / pitch));
	const int lit = litCount(level, count);
	const int pi = peakHold ? peakIndex(peak, count) : -1;

	for (int i = 0; i < count; ++i) {
		const float y = column.getBottom() - pitch * (float)(i + 1);
		const juce::Rectangle<float> seg(column.getX(), y + 1.0f, column.getWidth(), pitch - 2.0f);

		const bool on = i < lit || i == pi;

		g.setColour(on ? ledColourAt((float)(i + 1) / (float)count) : (juce::Colour)GuiColor::LevelMeter::Led::Off);
		g.fillRect(seg);
	}

	if (pi >= 0) {
		const float y = column.getBottom() - pitch * (float)(pi + 1);

		g.setColour(GuiColor::LevelMeter::Led::Peak);
		g.drawRect(juce::Rectangle<float>(column.getX(), y + 1.0f, column.getWidth(), pitch - 2.0f), 1.0f);
	}
}

// ---------------------------------------------------------------- バー
void GuiLevelMeter::drawBar(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, bool peakHold) const
{
	const float h = column.getHeight();
	const float bottom = column.getBottom();

	if (level > 0.0f) {
		const float top = bottom - h * level;

		juce::ColourGradient grad(GuiColor::LevelMeter::Bar::Low, 0.0f, bottom, GuiColor::LevelMeter::Bar::High, 0.0f, column.getY(), false);
		grad.addColour(midFrom, GuiColor::LevelMeter::Bar::Mid);

		g.setGradientFill(grad);
		g.fillRect(juce::Rectangle<float>(column.getX(), top, column.getWidth(), bottom - top));
	}

	if (peakHold && peak > 0.0f) {
		const float y = bottom - h * peak;

		g.setColour(GuiColor::LevelMeter::Bar::Peak);
		g.drawLine(column.getX(), y, column.getRight(), y, 2.0f);
	}
}

// ---------------------------------------------------------------- 線 (L / R の柱)
void GuiLevelMeter::drawLineColumn(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, bool peakHold) const
{
	const float h = column.getHeight();
	const float bottom = column.getBottom();
	const float top = bottom - h * level;

	g.setColour(GuiColor::LevelMeter::Line::Fill);
	g.fillRect(juce::Rectangle<float>(column.getX(), top, column.getWidth(), bottom - top));

	if (level > 0.0f) {
		g.setColour(GuiColor::LevelMeter::Line::Stroke);
		g.drawLine(column.getX(), top, column.getRight(), top, 2.0f);
	}

	if (peakHold && peak > 0.0f) {
		const float y = bottom - h * peak;

		g.setColour(GuiColor::LevelMeter::Line::Peak);
		g.drawLine(column.getX(), y, column.getRight(), y, 2.0f);
	}
}

// ---------------------------------------------------------------- 80s-90s 01 (青い丸い LED)
void GuiLevelMeter::drawBlueDots(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, bool peakHold) const
{
	const float pitch = pitchOf(retro01);
	const int p = juce::roundToInt(pitch);
	const int count = juce::jmax(1, (int)(column.getHeight() / pitch));
	const int lit = litCount(level, count);
	const int pi = peakHold ? peakIndex(peak, count) : -1;

	// 柱 1 本に点は 1 列。柱の真ん中に寄せる
	const float w = pitch;
	const float x = column.getCentreX() - w * 0.5f;
	const float bottom = column.getBottom();

	const auto& off = tileFor(dotsOff, p, GuiColor::LevelMeter::Retro01::Off, juce::Colours::transparentBlack);
	const auto& on = tileFor(dotsOn, p, GuiColor::LevelMeter::Retro01::Dot, GuiColor::LevelMeter::Retro01::Glow);

	const float litTop = bottom - pitch * (float)lit;

	g.setTiledImageFill(off, juce::roundToInt(x), juce::roundToInt(bottom), 1.0f);
	g.fillRect(juce::Rectangle<float>(x, bottom - pitch * (float)count, w, pitch * (float)(count - lit)));

	if (lit > 0) {
		g.setTiledImageFill(on, juce::roundToInt(x), juce::roundToInt(bottom), 1.0f);
		g.fillRect(juce::Rectangle<float>(x, litTop, w, bottom - litTop));
	}

	if (pi >= 0) {
		g.setTiledImageFill(tileFor(dotsPeak, p, GuiColor::LevelMeter::Retro01::Peak, GuiColor::LevelMeter::Retro01::Glow), juce::roundToInt(x), juce::roundToInt(bottom), 1.0f);
		g.fillRect(juce::Rectangle<float>(x, bottom - pitch * (float)(pi + 1), w, pitch));
	}
}

// ---------------------------------------------------------------- 80s-90s 02 (縞の柱)
void GuiLevelMeter::drawStripes(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, bool peakHold) const
{
	drawStriped(g, column.getX(), column.getRight(), column, level, peak, peakHold, pitchOf(retro02), [](float x, float) { return x; });
}

void GuiLevelMeter::drawSpectrumPerspective(juce::Graphics& g, juce::Rectangle<float> area, int bands, bool peakHold) const
{
	const float cx = area.getCentreX();
	const float bottom = area.getBottom();
	const float h = area.getHeight();

	// てっぺんへ行くほど真ん中へ寄せる (奥へすぼまって見える)
	auto mapX = [cx](float x, float t) { return cx + (x - cx) * (1.0f - (1.0f - perspectiveTop) * juce::jlimit(0.0f, 1.0f, t)); };

	// 床の格子。奥へすぼまる台形の横線と、両端の斜めの線
	g.setColour(GuiColor::LevelMeter::Retro02::Grid);

	for (int k = 0; k <= 4; ++k) {
		const float t = (float)k / 4.0f;
		const float y = bottom - h * t;

		g.drawLine(mapX(area.getX(), t), y, mapX(area.getRight(), t), y, 1.0f);
	}

	g.drawLine(area.getX(), bottom, mapX(area.getX(), 1.0f), area.getY(), 1.0f);
	g.drawLine(area.getRight(), bottom, mapX(area.getRight(), 1.0f), area.getY(), 1.0f);

	const float pitch = area.getWidth() / (float)bands;
	const float gap = juce::jmax(1.0f, pitch * 0.25f);

	for (int b = 0; b < bands; ++b) {
		const float x0 = area.getX() + pitch * (float)b + gap * 0.5f;
		const float x1 = x0 + pitch - gap;

		drawStriped(g, x0, x1, area, bandLevel[(size_t)b], bandPeak[(size_t)b], peakHold, pitchOf(retro02), mapX);
	}
}

// ---------------------------------------------------------------- 80s-90s 03 (蛍光表示管)
void GuiLevelMeter::drawVfd(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, bool peakHold) const
{
	const float pitch = pitchOf(retro03);
	const int count = juce::jmax(1, (int)(column.getHeight() / pitch));
	const int lit = litCount(level, count);
	const int pi = peakHold ? peakIndex(peak, count) : -1;
	const float segH = juce::jmax(1.0f, pitch * 0.5f);

	auto segment = [&](int i) {
		const float y = column.getBottom() - pitch * (float)(i + 1) + (pitch - segH) * 0.5f;

		return juce::Rectangle<float>(column.getX(), y, column.getWidth(), segH);
	};

	// 灯った段は、にじみを広めに敷いてから本体
	if (lit > 0) {
		const float top = segment(lit - 1).getY();

		g.setColour(GuiColor::LevelMeter::Retro03::Glow);
		g.fillRect(juce::Rectangle<float>(column.getX() - 1.5f, top - 1.5f, column.getWidth() + 3.0f, column.getBottom() - top + 1.5f));
	}

	g.setColour(GuiColor::LevelMeter::Retro03::Segment);

	for (int i = 0; i < lit; ++i) g.fillRect(segment(i));

	if (pi >= 0 && pi >= lit) {
		const auto seg = segment(pi);

		g.setColour(GuiColor::LevelMeter::Retro03::Glow);
		g.fillRect(seg.expanded(1.5f));
		g.setColour(GuiColor::LevelMeter::Retro03::Peak);
		g.fillRect(seg);
	}
}

void GuiLevelMeter::drawVfdScale(juce::Graphics& g, float x, juce::Rectangle<float> area) const
{
	const float pitch = pitchOf(retro03);
	const int count = juce::jmax(1, (int)(area.getHeight() / pitch));
	const float dot = compact ? 1.0f : 1.5f;

	g.setColour(GuiColor::LevelMeter::Retro03::Scale);

	for (int i = 0; i < count; ++i) {
		const float y = area.getBottom() - pitch * (float)i - pitch * 0.5f;

		g.fillRect(x - dot * 0.5f, y - dot * 0.5f, dot, dot);
	}
}

// ---------------------------------------------------------------- 80s-90s 04 (点を敷きつめた柱)
void GuiLevelMeter::drawDotMatrix(juce::Graphics& g, juce::Rectangle<float> column, float level, float peak, bool peakHold) const
{
	// 1 段は、点 2 行と空き 1 行
	const float dot = pitchOf(retro04);
	const int d = juce::roundToInt(dot);
	const float segPitch = dot * 3.0f;
	const int across = juce::jmax(1, (int)(column.getWidth() / dot));
	const int count = juce::jmax(1, (int)(column.getHeight() / segPitch));
	const int lit = litCount(level, count);
	const int pi = peakHold ? peakIndex(peak, count) : -1;

	const float w = dot * (float)across;
	const float x = column.getCentreX() - w * 0.5f;
	const float bottom = column.getBottom();

	const auto& off = tileFor(matrixOff, d, GuiColor::LevelMeter::Retro04::Off, juce::Colours::transparentBlack);
	const auto& on = tileFor(matrixOn, d, GuiColor::LevelMeter::Retro04::Dot, juce::Colours::transparentBlack);

	const float litTop = bottom - segPitch * (float)lit;

	g.setTiledImageFill(off, juce::roundToInt(x), juce::roundToInt(bottom), 1.0f);
	g.fillRect(juce::Rectangle<float>(x, bottom - segPitch * (float)count, w, segPitch * (float)(count - lit)));

	if (lit > 0) {
		g.setTiledImageFill(on, juce::roundToInt(x), juce::roundToInt(bottom), 1.0f);
		g.fillRect(juce::Rectangle<float>(x, litTop, w, bottom - litTop));
	}

	if (pi >= 0) {
		g.setTiledImageFill(tileFor(matrixPeak, d, GuiColor::LevelMeter::Retro04::Peak, juce::Colours::transparentBlack), juce::roundToInt(x), juce::roundToInt(bottom), 1.0f);
		g.fillRect(juce::Rectangle<float>(x, bottom - segPitch * (float)(pi + 1), w, segPitch));
	}
}

void GuiLevelMeter::drawDotScale(juce::Graphics& g, float x, juce::Rectangle<float> area) const
{
	const float size = compact ? 2.0f : 3.0f;
	const int mark = juce::roundToInt(dotScaleMark * (float)dotScaleCount);

	for (int i = 0; i <= dotScaleCount; ++i) {
		const float y = area.getBottom() - area.getHeight() * (float)i / (float)dotScaleCount;

		g.setColour(i == mark ? (juce::Colour)GuiColor::LevelMeter::Retro04::Mark : (juce::Colour)GuiColor::LevelMeter::Retro04::Scale);
		g.fillRect(x + (dotScaleWidth - size) * 0.5f, juce::jmax(area.getY(), y - size), size, size);
	}
}

// ---------------------------------------------------------------- 線 (スペクトラム)
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

	g.setColour(GuiColor::LevelMeter::Line::Fill);
	g.fillPath(fill);

	g.setColour(GuiColor::LevelMeter::Line::Stroke);
	g.strokePath(path, juce::PathStrokeType(2.0f));

	if (!peakHold) return;

	// ピークは帯ごとの短い横線
	g.setColour(GuiColor::LevelMeter::Line::Peak);

	for (int b = 0; b < bands; ++b) {
		if (bandPeak[(size_t)b] <= 0.0f) continue;

		const auto p = pointAt(bandPeak, b);

		g.drawLine(p.x - pitch * 0.35f, p.y, p.x + pitch * 0.35f, p.y, 2.0f);
	}
}

// ---------------------------------------------------------------- 点を敷きつめる絵
const juce::Image& GuiLevelMeter::tileFor(TileSlot slot, int size, juce::Colour fill, juce::Colour glow) const
{
	auto& t = tiles[(size_t)slot];

	if (t.image.isValid() && t.size == size && t.fill == fill && t.glow == glow) return t.image;

	t.size = juce::jmax(1, size);
	t.fill = fill;
	t.glow = glow;

	const bool matrix = slot >= matrixOff;
	const int w = t.size;
	const int h = matrix ? t.size * 3 : t.size;

	t.image = juce::Image(juce::Image::ARGB, w, h, true);

	juce::Graphics g(t.image);

	if (matrix) {
		// 80s-90s 04。点 2 行と空き 1 行 (上から空き・点・点。下へ揃えて敷くので、段の頭が空く)
		const float dot = juce::jmax(1.0f, (float)w * 0.6f);

		g.setColour(fill);
		g.fillRect(0.0f, (float)w, dot, dot);
		g.fillRect(0.0f, (float)w * 2.0f, dot, dot);
	}
	else {
		// 80s-90s 01。丸い点。灯った点は、にじみを下に敷く
		const float c = (float)w * 0.5f;
		const float dot = (float)w * 0.6f;

		if (!glow.isTransparent()) {
			g.setColour(glow);
			g.fillEllipse(0.0f, 0.0f, (float)w, (float)w);
		}

		g.setColour(fill);
		g.fillEllipse(c - dot * 0.5f, c - dot * 0.5f, dot, dot);
	}

	return t.image;
}
