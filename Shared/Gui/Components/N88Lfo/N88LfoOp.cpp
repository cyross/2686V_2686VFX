#include "./N88LfoOp.h"

#include "Shared/Core/Gui/GuiHelpers.h"
#include "Shared/Core/Gui/GuiStructs.h"
#include "Shared/Core/Processor/ProcessorKeys.h"

void GuiComponentN88LfoOp::setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder)
{
    cat.setupSwLfoCategory({ .parent = parent, .title = u8"N88 LFO", .enableChangeDetailVisible = true });

    ams.setup(GuiSlider::Config{ .parent = parent, .id = code + CPK::N88Lfo::ams, .title = u8"AMS", .isReset = true });
    ams.setWantsKeyboardFocus(true);
    ams.setExplicitFocusOrder(++tabOrder);
}

void GuiComponentN88LfoOp::rebind(const juce::String& code)
{
    ams.rebind(code + CPK::N88Lfo::ams);
}

void GuiComponentN88LfoOp::layoutComponentRow(juce::Rectangle<int>& rect)
{
    layoutRowCategory({ .rowRect = rect, .component = &cat });

    bool visible = cat.isDetailVisible();

    ams.setVisibleWithLabel(visible);

    if (visible)
    {
        layoutRow({ .rowRect = rect, .label = &ams.label, .component = &ams });

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }
}

void GuiComponentN88LfoOp::copyParams(CopyLfoN88Op& copyObj)
{
    copyObj.ams = (float)ams.getValue();
}

void GuiComponentN88LfoOp::pasteParams(CopyLfoN88Op& copyObj)
{
    ams.setValue(copyObj.ams, juce::sendNotification);
}

void GuiComponentN88LfoOp::readParams(const Io::ParamReader& reader)
{
    ams.setValue(reader.getFloat("n88Ams", (float)ams.getValue()), juce::sendNotification);
}

void GuiComponentN88LfoOp::writeParams(Io::ParamWriter& writer)
{
    writer.set("n88Ams", (float)ams.getValue());
}

void GuiComponentN88LfoOp::setImportingParams(juce::StringArray& lines, int& index)
{
    ams.setValue(lines[index++].getFloatValue(), juce::sendNotification);
}
