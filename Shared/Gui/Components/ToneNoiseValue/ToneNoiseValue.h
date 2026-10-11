#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"

// ============================================================================
// FORM の TONE / NOISE を実機の設定値で決める (SsgRegister)
// ============================================================================
// SSG・RHYTHM・ADPCM・ADPCM+ の FORM で、N.FREQ の下に置く。
//
//   TONE: Use VALUE    鳴らす周波数を To (Fnote = 667000 / (32 * To)) の
//                      整数の格子へ寄せる。切っていれば鳴らした音のまま
//   NOISE: Use VALUE   N.FREQ の代わりに N.VALUE で決める
//   N.VALUE            ノイズの設定値 Tn (Fnois = 667000 / (32 * Tn))
//
// NOISE の VALUE を入れているあいだは、N.FREQ を押せなくする。
class GuiComponentToneNoiseValue : public GuiBase
{
public:
	GuiComponentToneNoiseValue(const GuiContext& context);

	// prefix はパラメータの頭 (SSG / RHYTHM_PAD0 / ADPCMP_SLOT0 など)。
	// noiseFreq は同じ FORM の N.FREQ のつまみ
	void setupComponent(juce::Component& parent, const juce::String& prefix, int& tabOrder, GuiSlider* noiseFreq);

	// 指し先を差し替える (TARGET の切り替え)
	void rebind(const juce::String& prefix);

	void setVisibles(bool visible);
	void layoutComponent(juce::Rectangle<int>& rect);
	void layoutComponentRow(juce::Rectangle<int>& rect);

	// チャンネル・区分のファイルの読み書き。キーは N.FREQ と同じ階層に置く
	template <typename Reader>
	void readParams(const Reader& r)
	{
		toneValueToggle.setToggleState(r.getBool(Key::toneValueOn, toneValueToggle.getToggleState()), juce::sendNotification);
		noiseValueToggle.setToggleState(r.getBool(Key::noiseValueOn, noiseValueToggle.getToggleState()), juce::sendNotification);
		noiseValueSlider.setValue(r.getInt(Key::noiseValue, (int)noiseValueSlider.getValue()), juce::sendNotification);
	}

	template <typename Writer>
	void writeParams(Writer& w)
	{
		w.set(Key::toneValueOn, toneValueToggle.getToggleState());
		w.set(Key::noiseValueOn, noiseValueToggle.getToggleState());
		w.set(Key::noiseValue, (int)noiseValueSlider.getValue());
	}

	// ファイルのキー
	struct Key
	{
		static inline const juce::String toneValueOn = "toneValueEnable";
		static inline const juce::String noiseValueOn = "noiseValueEnable";
		static inline const juce::String noiseValue = "noiseValue";
	};

	// テストから触る口
	GuiToggleButton& toneValue() { return toneValueToggle; }
	GuiToggleButton& noiseValue() { return noiseValueToggle; }
	GuiSlider& noiseValueKnob() { return noiseValueSlider; }
private:
	// N.FREQ と N.VALUE は、効いている方だけ押せる
	void applyActive();

	GuiToggleButton toneValueToggle;
	GuiToggleButton noiseValueToggle;
	GuiSlider noiseValueSlider;

	GuiSlider* noiseFreqSlider = nullptr;
};
