#pragma once

#include <JuceHeader.h>

#include "../NudgeSlider/NudgeSliderInt.h"
#include "../ValueNudge/ValueNudge.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"

// ============================================================================
// FORM の TONE / NOISE を実機の設定値で決める (SsgRegister)
// ============================================================================
// SSG・RHYTHM・ADPCM・ADPCM+ の FORM で、N.FREQ の下に置く。
//
//   TONE: Use VALUE    鍵盤の周波数へ T.VALUE から決まる倍率を掛ける
//   T.VALUE            トーンの設定値 To (Fnote = 667000 / (32 * To))。
//                      倍率は Fnote(To) / Fnote(47)。初期値 47 で 1 倍
//   NOISE: Use VALUE   N.FREQ の代わりに N.VALUE で決める
//   N.VALUE            ノイズの設定値 Tn (Fnois = 667000 / (32 * Tn))
//
// VALUE を入れていないあいだは T.VALUE / N.VALUE を、NOISE の VALUE を
// 入れているあいだは N.FREQ を押せなくする。
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
		toneValueSlider.setValue(r.getInt(Key::toneValue, (int)toneValueSlider.getValue()), juce::sendNotification);
		noiseValueToggle.setToggleState(r.getBool(Key::noiseValueOn, noiseValueToggle.getToggleState()), juce::sendNotification);
		noiseValueSlider.setValue(r.getInt(Key::noiseValue, (int)noiseValueSlider.getValue()), juce::sendNotification);
	}

	template <typename Writer>
	void writeParams(Writer& w)
	{
		w.set(Key::toneValueOn, toneValueToggle.getToggleState());
		w.set(Key::toneValue, (int)toneValueSlider.getValue());
		w.set(Key::noiseValueOn, noiseValueToggle.getToggleState());
		w.set(Key::noiseValue, (int)noiseValueSlider.getValue());
	}

	// ファイルのキー
	struct Key
	{
		static inline const juce::String toneValueOn = "toneValueEnable";
		static inline const juce::String toneValue = "toneValue";
		static inline const juce::String noiseValueOn = "noiseValueEnable";
		static inline const juce::String noiseValue = "noiseValue";
	};

	// テストから触る口
	GuiToggleButton& toneValue() { return toneValueToggle; }
	GuiSlider& toneValueKnob() { return toneValueSlider.getSlider(); }
	GuiToggleButton& noiseValue() { return noiseValueToggle; }
	GuiSlider& noiseValueKnob() { return noiseValueSlider.getSlider(); }
private:
	// 効いているつまみだけ押せる
	void applyActive();

	GuiToggleButton toneValueToggle;
	GuiComponentNudgeSliderInt toneValueSlider;
	GuiComponentValueNudge toneValueNudge;
	GuiToggleButton noiseValueToggle;
	GuiComponentNudgeSliderInt noiseValueSlider;
	GuiComponentValueNudge noiseValueNudge;

	GuiSlider* noiseFreqSlider = nullptr;
};
