#pragma once

#include <JuceHeader.h>

#include <initializer_list>
#include <vector>

#include "./GuiEnvelopeGraph.h"

class GuiProcessorHost;
class GuiWaveformPreview;

// ============================================================================
// リアルタイムのオシロの縦の倍率 (4.4.0)
// ============================================================================
// 0.1〜8 倍。値は GuiProcessorHost::scopeZoom に持ち、設定ファイルへ保存する。
// L / M / R のオシロはいつも同じ倍率にそろえる。
//
// フルビューでは、オシロの見出しの下につまみを置く (この部品)。ミニプレイヤーの
// ようにつまみを置く場所が無くても、オシロの上でホイールを回せば変えられる。
class GuiScopeZoom : public juce::Component
{
public:
	static constexpr float zoomMin = 0.1f;
	static constexpr float zoomMax = 8.0f;

	static float clampZoom(float zoom) { return juce::jlimit(zoomMin, zoomMax, std::isfinite(zoom) ? zoom : 1.0f); }

	explicit GuiScopeZoom(GuiProcessorHost& host);

	// 倍率をそろえるオシロ。ホイールで変えられるようにもする
	void attach(std::initializer_list<GuiWaveformPreview*> scopes);

	// 倍率を変える (つまみ・ホイール・設定の読み込みから)
	void setZoom(float zoom);

	// 設定を読み直したときに、持っている値を画面へ映す
	void sync() { setZoom(currentZoom()); }

	void resized() override;

private:
	GuiProcessorHost& host;

	juce::Label label;
	GuiZoomSlider slider;

	std::vector<GuiWaveformPreview*> scopes;

	float currentZoom() const;
};
