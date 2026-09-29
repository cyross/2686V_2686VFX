#pragma once

#include <JuceHeader.h>

#include "../../../Core/Io/ParamFile.h"
#include <array>
#include <vector>
#include <functional>

#include "Shared/Core/Const/ConstGlobal.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiCopyObj.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiValues.h"
#include "Shared/Core/Gui/GuiEnvelopeGraph.h"
#include "Shared/Advanced/Curve/AdvancedCurve.h"
#include "../../../Gui/Components/Separator/NormalSeparator.h"
#include "../../../Gui/Components/Separator/ShortSeparator.h"
#include "./QualityDac.h"

class QualityPcm : public GuiBase {
    GuiCategoryLabel qualityCat;

    // 実機に合わせて BIT・RATE・INTERP をまとめて入れる
    GuiComponentDac dac;

    GuiComboBox modeSelector;
    GuiComboBox rateSelector;
    GuiComboBox interpSelector;

    // ノイズリダクション。どれも既定は切れていて、いまの音を変えない。
    //
    //   Resample  符号化の前の間引きを、折り返しを防ぐきれいなものにする
    //             (符号化するモードだけに効く)
    //   Gate      直流のずれを取り、ごく小さい音を 0 まで絞る
    //   NR.LPF    素材の帯域から見て高すぎる成分を削る
    NormalSeparator nrSeparator;
    GuiToggleButton nrResampleToggle;
    GuiToggleButton nrGateToggle;
    GuiSlider nrGateLevelSlider;
    GuiComboBox nrLpfSelector;

    // 外から止められているか。setEnableds で入る。
    bool outerEnabled = true;

    // 効いていない設定を止める。きれいな間引きは符号化するモードの
    // ときだけ、しきい値は無音ゲートを入れたときだけ効く。
    void applyActive();
public:
    QualityPcm(const GuiContext& context) :
        GuiBase(context),
        qualityCat(context),
        dac(context),
		modeSelector(context),
		rateSelector(context),
		interpSelector(context),
		nrSeparator(context),
		nrResampleToggle(context),
		nrGateToggle(context),
		nrGateLevelSlider(context),
		nrLpfSelector(context)
    {
    }

    static std::vector<SelectItem> nrLpfItems();

    static std::vector<SelectItem> qualityItems;
    static std::vector<SelectItem> rateItems;
    static std::vector<SelectItem> interpItems();

    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder);
    // 束縛先を丸ごと差し替える。TARGET で指し先を切り替えるときに使う。
    void rebind(const juce::String& code);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
	int getMode() const { return modeSelector.getSelectedItemIndex(); }
	int getRate() const { return rateSelector.getSelectedItemIndex(); }
	int getInterp() const { return interpSelector.getSelectedItemIndex(); }
	void setMode(int index) { modeSelector.setSelectedItemIndex(index, juce::sendNotification); }
	void setRate(int index) { rateSelector.setSelectedItemIndex(index, juce::sendNotification); }
	void setInterp(int index) { interpSelector.setSelectedItemIndex(index, juce::sendNotification); }
	void setVisibles(bool visible) {
		qualityCat.setVisible(visible);
		dac.setVisibles(visible);
		modeSelector.setVisibleWithLabel(visible);
		rateSelector.setVisibleWithLabel(visible);
		interpSelector.setVisibleWithLabel(visible);
		nrSeparator.setVisible(visible);
		nrResampleToggle.setVisible(visible);
		nrGateToggle.setVisible(visible);
		nrGateLevelSlider.setVisibleWithLabel(visible);
		nrLpfSelector.setVisibleWithLabel(visible);
	}
	void setEnableds(bool enabled) {
		outerEnabled = enabled;

		qualityCat.setEnabled(enabled);
		dac.setEnableds(enabled);
		modeSelector.setEnabled(enabled);
		rateSelector.setEnabled(enabled);
		interpSelector.setEnabled(enabled);

		applyActive();
	}
	void setImportingParams(juce::StringArray& lines, int& index);

	// 名前で受け渡す。行の並びに頼ると、呼ぶ順番を間違えたときに
	// 黙って別の値が入り、項目を足すと後ろが全部ずれるため。
	void readParams(const Io::ParamReader& reader, const juce::String& key);
	void writeParams(Io::ParamWriter& writer, const juce::String& key);

	// ノイズリダクションの値を、渡された束へ名前で読み書きする。
	// QUALITY のパラメータファイルと CH Params の両方から使う。
	// 書かれていない値は今のまま残る (3.5.0 までのファイル)。
	void readNrParams(const Io::ParamReader& reader);
	void writeNrParams(Io::ParamWriter& writer);

	// 写しと貼り付け (RHYTHM のパッド)
	void copyNr(CopyPcmQuality& q) const {
		q.nrResample = nrResampleToggle.getToggleState();
		q.nrGate = nrGateToggle.getToggleState();
		q.nrGateLevel = (float)nrGateLevelSlider.getValue();
		q.nrLpf = nrLpfSelector.getSelectedItemIndex();
	}
	void pasteNr(const CopyPcmQuality& q) {
		nrResampleToggle.setToggleState(q.nrResample, juce::sendNotification);
		nrGateToggle.setToggleState(q.nrGate, juce::sendNotification);
		nrGateLevelSlider.setValue(q.nrGateLevel, juce::sendNotification);
		nrLpfSelector.setSelectedItemIndex(q.nrLpf, juce::sendNotification);
	}
	juce::String getExportedParams();
};
