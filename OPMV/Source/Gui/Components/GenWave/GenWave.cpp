#include "./GenWave.h"

#include <cmath>
#include <memory>

#include "../../../Core/Editor/PluginEditor.h"
#include "../../../Core/Editor/EditorGuiText.h"
#include "../../../Core/Gui/GuiColor.h"

GuiGenWave::GuiGenWave(const GuiContext& context) : ctx(context)
{
}

GuiGenWave::~GuiGenWave()
{
    stopTimer();

    // 走っている計算が終わるまで待つ。待たずに壊すと、
    // 出来上がりを書き戻す先が無くなる。
    m_pool.removeAllJobs(true, 4000);
}

void GuiGenWave::setup(juce::Component& parent)
{
    parent.addAndMakeVisible(*this);

    title.setText(EditorGuiText::GenWave::title, juce::dontSendNotification);
    title.setJustificationType(juce::Justification::centred);
    title.setColour(juce::Label::textColourId, GuiColor::Label::Text);
    addAndMakeVisible(title);

    generateBtn.setButtonText(EditorGuiText::GenWave::generate);
    generateBtn.setColour(juce::TextButton::buttonColourId, GuiColor::GenWave::GenerateBg);
    generateBtn.setColour(juce::TextButton::textColourOffId, GuiColor::GenWave::GenerateText);
    generateBtn.setColour(juce::TextButton::textColourOnId, GuiColor::GenWave::GenerateText);
    generateBtn.setTooltip(EditorGuiText::GenWave::generateTooltip);
    generateBtn.onClick = [this] { startGenerate(); };
    addAndMakeVisible(generateBtn);

    deleteBtn.setButtonText(EditorGuiText::GenWave::remove);
    deleteBtn.setColour(juce::TextButton::buttonColourId, GuiColor::GenWave::DeleteBg);
    deleteBtn.setColour(juce::TextButton::textColourOffId, GuiColor::GenWave::DeleteText);
    deleteBtn.setColour(juce::TextButton::textColourOnId, GuiColor::GenWave::DeleteText);
    deleteBtn.setTooltip(EditorGuiText::GenWave::removeTooltip);
    deleteBtn.onClick = [this] { clearWave(); };
    deleteBtn.setEnabled(false);
    addAndMakeVisible(deleteBtn);
}

void GuiGenWave::layout(int x, int y, int width)
{
    setBounds(x, y, width, totalHeight);
}

void GuiGenWave::resized()
{
    title.setBounds(0, 0, getWidth(), labelHeight);

    const int buttonY = totalHeight - buttonRowHeight;

    generateBtn.setBounds(0, buttonY, getWidth() - deleteWidth - gap, buttonRowHeight);
    deleteBtn.setBounds(getWidth() - deleteWidth, buttonY, deleteWidth, buttonRowHeight);
}

juce::Rectangle<int> GuiGenWave::waveArea() const
{
    return { 0, labelHeight + gap, getWidth(), waveHeight };
}

juce::Rectangle<int> GuiGenWave::cycleArea() const
{
    return { 0, labelHeight + gap + waveHeight + gap, getWidth(), cycleRowHeight };
}

juce::Rectangle<int> GuiGenWave::cycleCell(int index) const
{
    auto area = cycleArea().withTrimmedLeft(cycleLabelWidth);

    const int count = (int)cycleChoices.size();
    const int left = area.getX() + area.getWidth() * index / count;
    const int right = area.getX() + area.getWidth() * (index + 1) / count;

    return { left, area.getY(), right - left, area.getHeight() };
}

void GuiGenWave::mouseDown(const juce::MouseEvent& event)
{
    for (int i = 0; i < (int)cycleChoices.size(); ++i)
    {
        if (!cycleCell(i).contains(event.getPosition())) continue;

        m_cycleIndex = i;

        repaint();

        return;
    }
}

// ----------------------------------------------------------------------------
// 作る
// ----------------------------------------------------------------------------
void GuiGenWave::startGenerate()
{
    if (m_busy) return;

    m_busy = true;

    generateBtn.setEnabled(false);
    deleteBtn.setEnabled(false);
    stopTimer();

    ctx.editor.showLoading(EditorGuiText::GenWave::working);

    // 音源の組み立てはメッセージスレッドで行う。プロセッサが持っている
    // 波形メモリや PCM への参照を受け取るため。回すのはこの後の仕事で。
    auto rig = std::make_shared<GenWaveRender::Rig>();

    rig->build(ctx.audioProcessor);

    // 画面が先に閉じることがあるので、書き戻す前に生きているかを確かめる
    juce::Component::SafePointer<GuiGenWave> safe(this);

    m_pool.addJob([safe, rig] {
        auto wave = std::make_shared<GenWaveRender::Wave>(GenWaveRender::render(*rig));

        juce::MessageManager::callAsync([safe, wave] {
            if (safe == nullptr) return;

            safe->finishGenerate(*wave);
        });
    });
}

void GuiGenWave::finishGenerate(const GenWaveRender::Wave& wave)
{
    m_wave = wave;
    m_busy = false;
    m_startMs = juce::Time::getMillisecondCounterHiRes();

    generateBtn.setEnabled(true);
    generateBtn.setButtonText(EditorGuiText::GenWave::regenerate);
    deleteBtn.setEnabled(!m_wave.isEmpty());

    ctx.editor.hideLoading();

    if (!m_wave.isEmpty()) startTimer(frameMs);

    repaint();
}

void GuiGenWave::clearWave()
{
    stopTimer();

    m_wave = {};

    generateBtn.setButtonText(EditorGuiText::GenWave::generate);
    deleteBtn.setEnabled(false);

    repaint();
}

// ----------------------------------------------------------------------------
// 描く
// ----------------------------------------------------------------------------
void GuiGenWave::paint(juce::Graphics& g)
{
    drawWave(g, waveArea());
    drawCycles(g);
}

void GuiGenWave::drawWave(juce::Graphics& g, juce::Rectangle<int> area)
{
    g.setColour(GuiColor::GenWave::Bg);
    g.fillRect(area);

    g.setColour(GuiColor::GenWave::Border);
    g.drawRect(area, 1);

    const auto inner = area.reduced(1);

    if (m_wave.isEmpty())
    {
        g.setColour(GuiColor::GenWave::Axis);
        g.drawHorizontalLine(inner.getCentreY(), (float)inner.getX(), (float)inner.getRight());

        g.setColour(GuiColor::GenWave::HintText);
        g.setFont(juce::FontOptions(12.0f));
        g.drawText(EditorGuiText::GenWave::empty, inner, juce::Justification::centred, true);

        return;
    }

    // 経過秒を、作ったものの中の位置として読む。10 秒で一巡する。
    const double length = m_wave.lengthSeconds();
    const double elapsed = (juce::Time::getMillisecondCounterHiRes() - m_startMs) / 1000.0;
    const double at = (length > 0.0) ? std::fmod(elapsed, length) : 0.0;

    const double perCycle = m_wave.samplesPerCycle();
    const int total = (int)m_wave.size();
    const int window = juce::jlimit(2, total,
        (int)std::lround(perCycle * cycleChoices[(size_t)m_cycleIndex]));

    // 窓の頭は周期の切れ目へ合わせる。合わせないと 1 周期が 1 コマの
    // 間に何周も流れてしまい、形が読めない。オシロの同期と同じ考え方。
    const double cycles = (perCycle > 0.0)
        ? std::floor(at * m_wave.sampleRate / perCycle) : 0.0;

    const int start = juce::jlimit(0, juce::jmax(0, total - window),
        (int)std::lround(cycles * perCycle));

    // L / M / R の 3 段。チャンネルによっては左右で違う音が出る。
    const juce::String labels[] = {
        EditorGuiText::GenWave::channelL,
        EditorGuiText::GenWave::channelM,
        EditorGuiText::GenWave::channelR,
    };

    GuiColor::Entry* const colours[] = {
        &GuiColor::GenWave::LineL, &GuiColor::GenWave::LineM, &GuiColor::GenWave::LineR,
    };

    const int laneHeight = inner.getHeight() / 3;

    g.setFont(juce::FontOptions(10.0f));

    for (int lane = 0; lane < 3; ++lane)
    {
        auto strip = juce::Rectangle<int>(inner.getX(), inner.getY() + laneHeight * lane,
            inner.getWidth(), laneHeight);

        g.setColour(GuiColor::GenWave::Axis);
        g.drawHorizontalLine(strip.getCentreY(), (float)strip.getX(), (float)strip.getRight());

        juce::Path path;

        const float centreY = (float)strip.getCentreY();
        const float halfHeight = strip.getHeight() * 0.5f - 1.0f;
        const int width = juce::jmax(1, strip.getWidth());

        for (int i = 0; i < width; ++i)
        {
            const size_t index = (size_t)start + (size_t)((juce::int64)window * i / width);

            if (index >= m_wave.size()) break;

            const float y = centreY - m_wave.at(lane, index) * halfHeight;

            if (i == 0) path.startNewSubPath((float)strip.getX(), y);
            else path.lineTo((float)(strip.getX() + i), y);
        }

        g.setColour(*colours[lane]);
        g.strokePath(path, juce::PathStrokeType(1.2f));

        g.setColour(GuiColor::GenWave::HintText);
        g.drawText(labels[lane], strip.withWidth(14).translated(2, 0),
            juce::Justification::centredLeft, false);
    }

    // 10 秒のうち今どこかを、下辺の帯で示す
    const float ratio = (length > 0.0) ? (float)(at / length) : 0.0f;

    g.setColour(GuiColor::GenWave::Progress);
    g.fillRect((float)inner.getX(), (float)inner.getBottom() - 2.0f,
        inner.getWidth() * ratio, 2.0f);
}

void GuiGenWave::drawCycles(juce::Graphics& g)
{
    g.setFont(juce::FontOptions(12.0f));

    auto labelArea = cycleArea().withWidth(cycleLabelWidth);

    g.setColour(GuiColor::Label::Text);
    g.drawText(EditorGuiText::GenWave::cycles, labelArea, juce::Justification::centredLeft, false);

    for (int i = 0; i < (int)cycleChoices.size(); ++i)
    {
        const auto cell = cycleCell(i).reduced(1);
        const bool on = (i == m_cycleIndex);

        g.setColour(on ? GuiColor::GenWave::CycleBgOn : GuiColor::GenWave::CycleBg);
        g.fillRect(cell);

        g.setColour(GuiColor::GenWave::Border);
        g.drawRect(cell, 1);

        g.setColour(on ? GuiColor::GenWave::CycleTextOn : GuiColor::GenWave::CycleText);
        g.drawText(juce::String(cycleChoices[(size_t)i]), cell, juce::Justification::centred, false);
    }
}
