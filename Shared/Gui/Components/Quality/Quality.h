#pragma once

#include <JuceHeader.h>

#include "../../../Core/Io/ParamFile.h"
#include <array>
#include <vector>
#include <functional>

#include "Shared/Core/Const/ConstGlobal.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiContext.h"
#include "Shared/Core/Gui/GuiValues.h"
#include "Shared/Core/Gui/GuiEnvelopeGraph.h"
#include "Shared/Advanced/Curve/AdvancedCurve.h"
#include "../../../Gui/Components/Separator/NormalSeparator.h"
#include "../../../Gui/Components/Separator/ShortSeparator.h"
#include "./QualityDac.h"

class Quality : public GuiBase {
    GuiCategoryLabel qualityCat;

    // 実機に合わせて BIT・RATE・INTERP をまとめて入れる
    GuiComponentDac dac;

    GuiComboBox bitSelector;
    GuiComboBox rateSelector;
    GuiComboBox interpSelector;

    // CHIP DAC。実機の DAC が分かっている音源 (FM 音源と SSG) だけが持つ。
    // 持たない音源では作るだけで、並べも見せもしない。
    GuiToggleButton chipDacToggle;
    bool hasChipDac = false;
public:
    // 区分の見出し。右クリックの IMPORT / EXPORT をつなぐのに使う
    GuiCategoryLabel& category() { return qualityCat; }

    Quality(const GuiContext& context) :
        GuiBase(context),
        qualityCat(context),
        dac(context),
        bitSelector(context),
        rateSelector(context),
        interpSelector(context),
        chipDacToggle(context)
    {
    }

	static std::vector<SelectItem> bdItems;
	static std::vector<SelectItem> rateItems;

    // withChipDac は CHIP DAC の行を置くか。パラメータを登録した音源だけが渡す。
    void setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder, bool withChipDac = false);
    // 束縛先を丸ごと差し替える。TARGET で指し先を切り替えるときに使う。
    void rebind(const juce::String& code);
    void layoutComponent(juce::Rectangle<int>& rect);
    void layoutComponentRow(juce::Rectangle<int>& rect);
    int getBit() const { return bitSelector.getSelectedItemIndex(); }
    int getRate() const { return rateSelector.getSelectedItemIndex(); }
    int getInterp() const { return interpSelector.getSelectedItemIndex(); }
    void setBit(int index) { bitSelector.setSelectedItemIndex(index, juce::sendNotification); }
    void setRate(int index) { rateSelector.setSelectedItemIndex(index, juce::sendNotification); }
    void setInterp(int index) { interpSelector.setSelectedItemIndex(index, juce::sendNotification); }
	void setVisibles(bool visible) {
		qualityCat.setVisible(visible);
		dac.setVisibles(visible);
		bitSelector.setVisibleWithLabel(visible);
		rateSelector.setVisibleWithLabel(visible);
		interpSelector.setVisibleWithLabel(visible);
		chipDacToggle.setVisible(visible && hasChipDac);
	}
    void setEnableds(bool enabled) {
        qualityCat.setEnabled(enabled);
        dac.setEnableds(enabled);
        bitSelector.setEnabled(enabled);
        rateSelector.setEnabled(enabled);
        interpSelector.setEnabled(enabled);
        chipDacToggle.setEnabled(enabled);
    }
    void setImportingParams(juce::StringArray& lines, int& index);

    // 名前で受け渡す。行の並びに頼ると、呼ぶ順番を間違えたときに
    // 黙って別の値が入り、項目を足すと後ろが全部ずれるため。
    void readParams(const Io::ParamReader& reader, const juce::String& key);
    void writeParams(Io::ParamWriter& writer, const juce::String& key);
    juce::String getExportedParams();
};
