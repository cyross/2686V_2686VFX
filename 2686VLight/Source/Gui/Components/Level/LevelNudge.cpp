#include "./LevelNudge.h"

#include "../../../Core/Gui/GuiHelpers.h"

namespace
{
    // 押したときに、いまの値へ足すか、その値へ置くか
    enum class Action { Add, Set };

    struct Spec
    {
        const char* title;
        Action action;
        float value;
        juce::Colour (*colour)();
    };

    juce::Colour stepColour() { return juce::Colours::lightblue.brighter(0.5f); }
    juce::Colour oneColour() { return juce::Colours::yellow.brighter(0.5f); }
    juce::Colour quarterColour() { return juce::Colours::lightgreen.brighter(0.5f); }
    juce::Colour fifthColour() { return juce::Colours::lightsalmon.brighter(0.5f); }
    juce::Colour fractionColour() { return juce::Colours::lightseagreen.brighter(0.3f); }

    // 並びは作る順 (Tab で送る順) で、置き場所は下の行の表で決める。
    // 送る順は、1 つの部品にまとめる前の LEVEL と同じにしてある。
    constexpr int pm1 = 0, pm01 = 1, pm001 = 2, to1 = 3, p001 = 4, p01 = 5, p1 = 6,
        to025 = 7, to05 = 8, to075 = 9,
        to02 = 10, to04 = 11, to06 = 12, to08 = 13,
        to0125 = 14, to0142 = 15, to016 = 16, to033 = 17, to067 = 18, to083 = 19;

    const Spec specs[GuiComponentLevelNudge::count] = {
        { "-1.0",  Action::Add, -1.0f,   stepColour },
        { "-0.1",  Action::Add, -0.1f,   stepColour },
        { "-0.01", Action::Add, -0.01f,  stepColour },
        { "1.0",   Action::Set, 1.0f,    oneColour },
        { "+0.01", Action::Add, 0.01f,   stepColour },
        { "+0.1",  Action::Add, 0.1f,    stepColour },
        { "+1.0",  Action::Add, 1.0f,    stepColour },
        { "0.25",  Action::Set, 0.25f,   quarterColour },
        { "0.5",   Action::Set, 0.5f,    quarterColour },
        { "0.75",  Action::Set, 0.75f,   quarterColour },
        { "0.2",   Action::Set, 0.2f,    fifthColour },
        { "0.4",   Action::Set, 0.4f,    fifthColour },
        { "0.6",   Action::Set, 0.6f,    fifthColour },
        { "0.8",   Action::Set, 0.8f,    fifthColour },
        { "0.125", Action::Set, 0.125f,  fractionColour },
        { "0.142", Action::Set, 0.142f,  fractionColour },
        { "0.16",  Action::Set, 0.16f,   fractionColour },
        { "0.33",  Action::Set, 0.33f,   fractionColour },
        { "0.67",  Action::Set, 0.67f,   fractionColour },
        { "0.83",  Action::Set, 0.83f,   fractionColour },
    };
}

GuiComponentLevelNudge::GuiComponentLevelNudge(const GuiContext& context) :
    GuiBase(context)
{
    for (auto& b : buttons) b = std::make_unique<GuiTextButton>(context);
}

void GuiComponentLevelNudge::setupComponent(juce::Component& parent, GuiSlider& slider, int& tabOrder)
{
    for (int i = 0; i < count; ++i) {
        const Spec& s = specs[i];
        auto& b = *buttons[(size_t)i];

        b.setup(GuiTextButton::Config{ .parent = parent, .id = "", .title = s.title, .bgColor = s.colour(), .isReset = false });
        b.setWantsKeyboardFocus(true);
        b.setExplicitFocusOrder(++tabOrder);

        b.onClick = [&slider, s]() {
            const float next = (s.action == Action::Add) ? (float)slider.getValue() + s.value : s.value;

            // 範囲の外はつまみが丸める。
            slider.setValue(next, juce::sendNotification);
            };
    }
}

void GuiComponentLevelNudge::layoutComponent(juce::Rectangle<int>& rect)
{
    auto b = [this](int i) { return buttons[(size_t)i].get(); };

    layoutMainThreeComps({ .rect = rect, .comp1 = b(pm1), .comp2 = b(to1), .comp3 = b(p1) });
    layoutMainThreeComps({ .rect = rect, .comp1 = b(to025), .comp2 = b(to05), .comp3 = b(to075) });
    layoutMainFourComps({ .rect = rect, .comp1 = b(to02), .comp2 = b(to04), .comp3 = b(to06), .comp4 = b(to08) });
    layoutMainSixComps({ .rect = rect, .comp1 = b(to0125), .comp2 = b(to0142), .comp3 = b(to016), .comp4 = b(to033), .comp5 = b(to067), .comp6 = b(to083) });
    layoutMainFourComps({ .rect = rect, .comp1 = b(pm001), .comp2 = b(pm01), .comp3 = b(p01), .comp4 = b(p001) });
}

void GuiComponentLevelNudge::layoutComponentRow(juce::Rectangle<int>& rect)
{
    auto b = [this](int i) { return buttons[(size_t)i].get(); };

    layoutRowThreeComps({ .rect = rect, .comp1 = b(pm1), .comp2 = b(to1), .comp3 = b(p1) });
    layoutRowThreeComps({ .rect = rect, .comp1 = b(to025), .comp2 = b(to05), .comp3 = b(to075) });
    layoutRowFourComps({ .rect = rect, .comp1 = b(to02), .comp2 = b(to04), .comp3 = b(to06), .comp4 = b(to08) });
    layoutRowSixComps({ .rect = rect, .comp1 = b(to0125), .comp2 = b(to0142), .comp3 = b(to016), .comp4 = b(to033), .comp5 = b(to067), .comp6 = b(to083) });
    layoutRowFourComps({ .rect = rect, .comp1 = b(pm001), .comp2 = b(pm01), .comp3 = b(p01), .comp4 = b(p001) });
}

void GuiComponentLevelNudge::setVisibles(bool visible)
{
    for (auto& b : buttons) b->setVisible(visible);
}

void GuiComponentLevelNudge::setEnables(bool enabled)
{
    for (auto& b : buttons) b->setEnabled(enabled);
}
