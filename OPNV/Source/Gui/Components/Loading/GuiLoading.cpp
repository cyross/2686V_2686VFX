#include "./GuiLoading.h"

#include <cmath>

#include "../../../Core/Editor/EditorGuiText.h"
#include "../../../Core/Gui/GuiColor.h"

GuiLoading::GuiLoading()
{
    // 覆っている間は下を触らせない。自分で受け止めて、素通りさせない。
    setInterceptsMouseClicks(true, false);
    setVisible(false);
}

void GuiLoading::show(juce::Component& parent, const juce::String& message,
    std::function<void()> onCancel)
{
    m_message = message;
    m_onCancel = std::move(onCancel);

    m_cancelBtn.setButtonText(EditorGuiText::Loading::cancel);
    m_cancelBtn.setColour(juce::TextButton::buttonColourId, GuiColor::Loading::CancelBg);
    m_cancelBtn.setColour(juce::TextButton::textColourOffId, GuiColor::Loading::CancelText);
    m_cancelBtn.setColour(juce::TextButton::textColourOnId, GuiColor::Loading::CancelText);
    m_cancelBtn.onClick = [this] {
        // 押せるのは 1 度だけ。止まるまでの間に何度も呼ばせない。
        m_cancelBtn.setEnabled(false);
        setMessage(EditorGuiText::Loading::cancelling);

        if (m_onCancel) m_onCancel();
    };
    m_cancelBtn.setEnabled(true);
    m_cancelBtn.setVisible(m_onCancel != nullptr);

    addAndMakeVisible(m_cancelBtn);

    parent.addAndMakeVisible(*this);

    setBounds(parent.getLocalBounds());
    toFront(false);
    setVisible(true);
}

void GuiLoading::resized()
{
    const float height = barHeight * sizeScale;

    int y = getHeight() / 2 + (int)(height * 0.5f) + messageGap;

    if (m_message.isNotEmpty()) y += (int)(messageFontHeight * 1.6f);

    m_cancelBtn.setBounds((getWidth() - cancelWidth) / 2, y + cancelGap,
        cancelWidth, cancelHeight);
}

void GuiLoading::setMessage(const juce::String& message)
{
    if (m_message == message) return;

    m_message = message;

    repaint();
}

void GuiLoading::hide()
{
    setVisible(false);

    m_cancelBtn.setVisible(false);
    m_onCancel = nullptr;
}

void GuiLoading::visibilityChanged()
{
    if (isVisible())
    {
        m_startMs = juce::Time::getMillisecondCounterHiRes();

        startTimer(frameMs);
    }
    else
    {
        stopTimer();
    }
}

// x から t をニュートン法で求め、その t での y を返す。
// 5 回も回せば、目で見て分かる差は残らない。
float GuiLoading::easeInOut(float x)
{
    constexpr float x1 = 0.42f;
    constexpr float x2 = 0.58f;

    // 制御点の y は 0 と 1 なので、y の式は 3(1-t)t^2 + t^3 になる
    auto valueX = [](float t) {
        const float u = 1.0f - t;

        return 3.0f * u * u * t * x1 + 3.0f * u * t * t * x2 + t * t * t;
    };

    auto slopeX = [](float t) {
        const float u = 1.0f - t;

        return 3.0f * u * u * x1 + 6.0f * u * t * (x2 - x1) + 3.0f * t * t * (1.0f - x2);
    };

    float t = juce::jlimit(0.0f, 1.0f, x);

    for (int i = 0; i < 5; ++i)
    {
        const float slope = slopeX(t);

        if (std::abs(slope) < 1.0e-6f) break;

        t = juce::jlimit(0.0f, 1.0f, t - (valueX(t) - x) / slope);
    }

    const float u = 1.0f - t;

    return 3.0f * u * t * t + t * t * t;
}

float GuiLoading::barScale(double elapsedSeconds, int index)
{
    // animation-delay のぶん遅らせる。始まる前は 0% の姿で待つ。
    const double shifted = elapsedSeconds - barDelaySeconds * index;

    if (shifted <= 0.0) return minScaleY;

    const double phase = std::fmod(shifted, cycleSeconds) / cycleSeconds;

    // 0% -> 50% で伸び、50% -> 100% で戻る
    const bool rising = phase < 0.5;
    const float local = (float)(rising ? phase / 0.5 : (phase - 0.5) / 0.5);
    const float eased = easeInOut(local);

    return rising ? minScaleY + (1.0f - minScaleY) * eased
                  : 1.0f - (1.0f - minScaleY) * eased;
}

void GuiLoading::paint(juce::Graphics& g)
{
    g.fillAll(GuiColor::Loading::Backdrop);

    const float scale = sizeScale;
    const float width = barWidth * scale;
    const float gap = barGap * scale;
    const float height = barHeight * scale;
    const float radius = barRadius * scale;

    const float totalWidth = width * barCount + gap * (barCount - 1);

    auto bounds = getLocalBounds().toFloat();

    const float centreX = bounds.getCentreX();
    const float centreY = bounds.getCentreY();

    const double elapsed = (juce::Time::getMillisecondCounterHiRes() - m_startMs) / 1000.0;

    g.setColour(GuiColor::Loading::Bar);

    for (int i = 0; i < barCount; ++i)
    {
        // transform: scaleY は真ん中を軸に伸び縮みする
        const float barH = height * barScale(elapsed, i);
        const float x = centreX - totalWidth * 0.5f + (width + gap) * i;

        g.fillRoundedRectangle(x, centreY - barH * 0.5f, width, barH, radius);
    }

    if (m_message.isEmpty()) return;

    g.setColour(GuiColor::Loading::Text);
    g.setFont(juce::FontOptions(messageFontHeight));

    const float top = centreY + height * 0.5f + messageGap;

    g.drawText(m_message,
        bounds.withTop(top).withHeight(messageFontHeight * 1.6f),
        juce::Justification::centredTop, true);
}
