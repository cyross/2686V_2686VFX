#include "./ParamBarEditor.h"

#include "../../../Core/Processor/PluginProcessor.h"

ParamBarEditorBase::~ParamBarEditorBase()
{
    // PluginProcessor の完全な定義を読み込んでいるので、エラーなくアクセスできる
    for (const auto& id : m_paramIds) {
        ctx.audioProcessor.apvts.removeParameterListener(id, this);
    }
}

void ParamBarEditorBase::attachParams(const juce::String& idPrefix, int count)
{
    m_params.clear();
    m_params.reserve((size_t)count);

    for (int i = 0; i < count; ++i)
    {
        juce::String paramId = idPrefix + juce::String(i);

        auto* param = ctx.audioProcessor.apvts.getParameter(paramId);

        if (param != nullptr) {
            m_params.push_back(param);
            m_paramIds.add(paramId);

            ctx.audioProcessor.apvts.addParameterListener(paramId, this);
        }
    }
}

// パラメータが外部(Undo など)から変更されたときに呼ばれる
void ParamBarEditorBase::parameterChanged(const juce::String&, float)
{
    // 描画を予約 (メッセージスレッドで実行される)
    juce::MessageManager::callAsync([this] { repaint(); });
}

void ParamBarEditorBase::setCustomEnabled(bool shouldBeEnabled)
{
    isEnabledState = shouldBeEnabled;

    repaint();
}

void ParamBarEditorBase::paintHoverText(juce::Graphics& g, const juce::String& text)
{
    g.setFont(14.0f);

    int textW = (int)juce::GlyphArrangement::getStringWidth(g.getCurrentFont(), juce::StringRef(text)) + 12;
    int textH = 22;

    int drawX = lastMousePos.x + 12;
    int drawY = lastMousePos.y - 24;

    if (drawX + textW > getWidth()) drawX = getWidth() - textW;
    if (drawY < 0) drawY = lastMousePos.y + 12;

    g.setColour(juce::Colours::black.withAlpha(0.8f));
    g.fillRoundedRectangle((float)drawX, (float)drawY, (float)textW, (float)textH, 4.0f);

    g.setColour(juce::Colours::white);
    g.drawText(text, drawX, drawY, textW, textH, juce::Justification::centred, false);
}

// =======================================================
// マウス操作の受け口
// =======================================================
void ParamBarEditorBase::mouseMove(const juce::MouseEvent& e) {
    updateHoverState(e);
}

void ParamBarEditorBase::mouseDown(const juce::MouseEvent& e) {
    if (!e.mods.isLeftButtonDown()) return; // 左クリック以外は無視する

    // マウスをクリックした瞬間に「新しい Undo の区切り」を作る
    ctx.audioProcessor.undoManager.beginNewTransaction();

    updateSliderValue(e);
    updateHoverState(e);
}

void ParamBarEditorBase::mouseDrag(const juce::MouseEvent& e) {
    if (!e.mods.isLeftButtonDown()) return; // 左クリック以外は無視する

    updateSliderValue(e);
    updateHoverState(e);
}

void ParamBarEditorBase::mouseExit(const juce::MouseEvent&) {
    hoveredIndex = -1;

    repaint();
}
