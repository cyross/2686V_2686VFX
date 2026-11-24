#include "./Pattern.h"

#include "Shared/Core/Gui/GuiHost.h"
#include "Shared/Core/Gui/GuiHelpers.h"

GuiComponentPattern::GuiComponentPattern(const GuiContext& context) :
    GuiBase(context),
    separator(context),
    patterns(context),
    pattern(context),
    reset(context)
{
}

void GuiComponentPattern::setupComponent(juce::Component& parent, const juce::String& b, int& tabOrder)
{
    base = b;
    available = ctx.apvts.getParameter(base + EnvPattern::patternsKey) != nullptr;

    if (!available) return;

    separator.setupComponent(parent);

    patterns.setup({ .parent = parent, .id = base + EnvPattern::patternsKey, .title = "PATTERNS", .isReset = true, .labelFont = labelFont });
    patterns.setWantsKeyboardFocus(true);
    patterns.setExplicitFocusOrder(++tabOrder);
    patterns.onValueChange = [this] { clampPattern(); };

    pattern.setup({ .parent = parent, .id = base + EnvPattern::patternKey, .title = "PATTERN", .isReset = true, .labelFont = labelFont });
    pattern.setWantsKeyboardFocus(true);
    pattern.setExplicitFocusOrder(++tabOrder);

    reset.setup({ .parent = parent, .id = base + EnvPattern::resetKey, .title = "RESET TO CHANGE", .isReset = true });
    reset.setWantsKeyboardFocus(true);
    reset.setExplicitFocusOrder(++tabOrder);

    seen = ctx.audioProcessor.patterns.editingPattern(base);

    clampPattern();

    // 組の切り替えはプロセッサがメッセージスレッドで APVTS へ戻す。戻ったのを見て描き直す
    startTimerHz(15);
}

void GuiComponentPattern::rebind(const juce::String& b)
{
    if (!available) return;

    base = b;

    patterns.rebind(base + EnvPattern::patternsKey);
    pattern.rebind(base + EnvPattern::patternKey);
    reset.rebind(base + EnvPattern::resetKey);

    seen = ctx.audioProcessor.patterns.editingPattern(base);

    clampPattern();
}

void GuiComponentPattern::clampPattern()
{
    const int n = juce::jlimit(1, EnvPattern::maxPatterns, (int)patterns.getValue());

    pattern.setRange(1.0, (double)n, 1.0);

    if ((int)pattern.getValue() > n) pattern.setValue((double)n, juce::sendNotification);

    // 組が 1 つなら選ぶものが無い
    pattern.setEnabledWithLabel(partsEnabled && n > 1);
}

void GuiComponentPattern::timerCallback()
{
    if (!available) return;

    const int now = ctx.audioProcessor.patterns.editingPattern(base);

    if (now == seen) return;

    seen = now;

    if (onPatternChange) onPatternChange();
}

void GuiComponentPattern::setPartsVisible(bool visible)
{
    if (!available) return;

    separator.setVisible(visible);
    patterns.setVisibleWithLabel(visible);
    pattern.setVisibleWithLabel(visible);
    reset.setVisible(visible);
}

void GuiComponentPattern::setPartsEnabled(bool enabled)
{
    partsEnabled = enabled;

    if (!available) return;

    patterns.setEnabledWithLabel(enabled);
    reset.setEnabled(enabled);

    clampPattern();
}

void GuiComponentPattern::layoutComponent(juce::Rectangle<int>& rect)
{
    if (!available) return;

    separator.layoutComponent(rect);

    layoutMain({ .mainRect = rect, .label = &patterns.label, .component = &patterns });
    layoutMain({ .mainRect = rect, .label = &pattern.label, .component = &pattern });
    layoutMain({ .mainRect = rect, .component = &reset });
}
