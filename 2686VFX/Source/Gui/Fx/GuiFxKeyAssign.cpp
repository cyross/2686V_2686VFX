#include "./GuiFxKeyAssign.h"

#include "../../Core/Gui/GuiHelpers.h"
#include "../../Core/Gui/GuiStructs.h"
#include "../../Processor/Mod/ProcessorModValues.h"
#include "./GuiFxText.h"
#include "./GuiFxValues.h"

namespace
{
    // 鍵盤の名前。FIX の音名と同じく、60 を C3 とする。
    std::vector<SelectItem> keyItems()
    {
        static const char* names[] = { "C", "C#", "D", "D#", "E", "F", "F#", "G", "G#", "A", "A#", "B" };

        std::vector<SelectItem> items;

        for (int note = ModPrValue::KeyAssign::keyMin; note <= ModPrValue::KeyAssign::keyMax; ++note)
        {
            juce::String name = juce::String(names[note % 12]) + juce::String(note / 12 - 2)
                + " (" + juce::String(note) + ")";

            items.push_back({ .name = name, .value = note + 1 });
        }

        return items;
    }

    std::vector<SelectItem> modeItems()
    {
        return {
            {.name = FxGuiText::KeyAssign::single, .value = 1 },
            {.name = FxGuiText::KeyAssign::custom, .value = 2 },
        };
    }

    juce::String paramId(const juce::String& tail) { return ModPrKey::prefix + tail; }

    void setParam(juce::AudioProcessorValueTreeState& apvts, const juce::String& id, int value)
    {
        if (auto* p = apvts.getParameter(id))
        {
            p->setValueNotifyingHost(p->convertTo0to1((float)value));
        }
    }

    int getParam(juce::AudioProcessorValueTreeState& apvts, const juce::String& id, int fallback)
    {
        auto* v = apvts.getRawParameterValue(id);

        return v != nullptr ? (int)std::lround(v->load()) : fallback;
    }
}

GuiFxKeyAssign::GuiFxKeyAssign(const GuiContext& context) :
    GuiBase(context),
    separator(context),
    mode(context),
    keys{
        GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context),
        GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context),
        GuiComboBox(context), GuiComboBox(context), GuiComboBox(context), GuiComboBox(context),
        GuiComboBox(context),
    }
{
    static_assert(ModPrKey::KeyAssign::NumTargets == 13, "keys の初期化の数を合わせる");
}

void GuiFxKeyAssign::setup(juce::Component& parent, int& tabOrder)
{
    separator.setupComponent(parent);

    mode.setup({ .parent = parent, .id = paramId(ModPrKey::KeyAssign::mode), .title = FxGuiText::KeyAssign::title, .items = modeItems(), .isReset = true });
    mode.setWantsKeyboardFocus(true);
    mode.setExplicitFocusOrder(++tabOrder);
    mode.onChange = [this] {
        if (onModeChanged) onModeChanged();
    };

    const auto items = keyItems();

    for (size_t i = 0; i < keys.size(); ++i)
    {
        const auto& t = ModPrKey::KeyAssign::targets[i];

        keys[i].setup({ .parent = parent, .id = paramId(t.key), .title = t.name, .items = items, .isReset = true });
        keys[i].setWantsKeyboardFocus(true);
        keys[i].setExplicitFocusOrder(++tabOrder);
    }
}

bool GuiFxKeyAssign::isCustom() const
{
    return mode.getSelectedItemIndex() == ModPrValue::KeyAssign::custom;
}

void GuiFxKeyAssign::collectComponents(std::set<juce::Component*>& out)
{
    out.insert(&separator);
    out.insert(&mode);
    out.insert(&mode.label);

    for (auto& key : keys)
    {
        out.insert(&key);
        out.insert(&key.label);
    }
}

void GuiFxKeyAssign::layout(juce::Rectangle<int>& rect)
{
    separator.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .label = &mode.label, .component = &mode, .labelWidth = FxGuiValue::KeyAssign::modeLabelWidth });

    const bool custom = isCustom();

    for (auto& key : keys)
    {
        key.setVisibleWithLabel(custom);

        if (custom) layoutMain({ .mainRect = rect, .label = &key.label, .component = &key, .labelWidth = FxGuiValue::KeyAssign::labelWidth });
    }
}

void GuiFxKeyAssign::writeParams(juce::AudioProcessorValueTreeState& apvts, Io::ParamWriter& writer)
{
    auto w = writer.child("keyAssign");

    w.set("mode", getParam(apvts, paramId(ModPrKey::KeyAssign::mode), ModPrValue::KeyAssign::modeInitial));

    for (const auto& t : ModPrKey::KeyAssign::targets)
    {
        w.set(t.fileKey, getParam(apvts, paramId(t.key), ModPrValue::KeyAssign::keyInitial));
    }
}

void GuiFxKeyAssign::readParams(juce::AudioProcessorValueTreeState& apvts, const Io::ParamReader& reader)
{
    // キーアサインを持たない古いファイルでは、いまの値のまま触らない
    auto r = reader.child("keyAssign");

    const auto modeId = paramId(ModPrKey::KeyAssign::mode);

    setParam(apvts, modeId, r.getInt("mode", getParam(apvts, modeId, ModPrValue::KeyAssign::modeInitial)));

    for (const auto& t : ModPrKey::KeyAssign::targets)
    {
        const auto id = paramId(t.key);

        setParam(apvts, id, r.getInt(t.fileKey, getParam(apvts, id, ModPrValue::KeyAssign::keyInitial)));
    }
}
