#include "./GenWave.h"

#include "Shared/Core/Editor/EditorStageWaveText.h"
#include "Shared/Core/Gui/GuiColor.h"

GuiGenWave::GuiGenWave(const GuiContext& context) : ctx(context),
    m_touchWatch(context.apvts.processor, context.audioProcessor.undoManager, context.audioProcessor.paramTouches)
{
}

GuiGenWave::~GuiGenWave()
{
    m_window = nullptr;
}

void GuiGenWave::setup(juce::Component& parent)
{
    parent.addAndMakeVisible(*this);

    title.setText(EditorGuiText::StageWave::windowTitle, juce::dontSendNotification);
    title.setJustificationType(juce::Justification::centred);
    title.setColour(juce::Label::textColourId, GuiColor::Label::Text);
    addAndMakeVisible(title);

    openBtn.setColour(juce::TextButton::buttonColourId, GuiColor::GenWave::GenerateBg);
    openBtn.setColour(juce::TextButton::textColourOffId, GuiColor::GenWave::GenerateText);
    openBtn.setColour(juce::TextButton::textColourOnId, GuiColor::GenWave::GenerateText);
    openBtn.setTooltip(EditorGuiText::StageWave::openTooltip);
    openBtn.onClick = [this] { setWindowOpen(!isWindowOpen()); };
    addAndMakeVisible(openBtn);

    updateButton();
}

void GuiGenWave::layout(int x, int y, int width)
{
    setBounds(x, y, width, totalHeight);
}

void GuiGenWave::resized()
{
    title.setBounds(0, 0, getWidth(), labelHeight);
    openBtn.setBounds(0, labelHeight + gap, getWidth(), buttonHeight);
}

void GuiGenWave::setWindowOpen(bool open)
{
    if (open == isWindowOpen()) {
        if (open) m_window->toFront(true);

        return;
    }

    if (open)
    {
        juce::Component::SafePointer<GuiGenWave> safe(this);

        // 閉じるボタンからは、窓を捨てる前に呼び出し元を抜けるよう、後で閉じる
        m_window = std::make_unique<GuiStageWaveWindow>(ctx, [safe] {
            juce::MessageManager::callAsync([safe] {
                if (safe != nullptr) safe->setWindowOpen(false);
            });
        });

        m_window->setVisible(true);
        m_window->toFront(true);

        // 開いた瞬間、触った履歴があればすぐ作る (自動生成が入っているとき)
        m_window->content().generateOnOpen();
    }
    else
    {
        m_window = nullptr;
    }

    updateButton();
}

void GuiGenWave::requestGenerate()
{
    setWindowOpen(true);

    if (auto* wave = stageWave()) wave->requestGenerate();
}

void GuiGenWave::updateButton()
{
    openBtn.setButtonText(isWindowOpen() ? EditorGuiText::StageWave::close : EditorGuiText::StageWave::open);
}
