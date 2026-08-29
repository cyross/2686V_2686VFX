#include <cmath>

#include "./GuiEnvelopeGraph.h"

GuiEnvelopeGraph::GuiEnvelopeGraph() {
    isBypass = false;
    setOpaque(false);
}

void GuiEnvelopeGraph::setEnvelope(EnvType type, const juce::String& title, const std::vector<PhaseDef>& phases)
{
    currentType = type;
    currentTitle = title;
    currentPhases = phases;
    repaint();
}

void GuiEnvelopeGraph::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto graphArea = bounds.reduced(5.0f);

    // 背景とグリッド
    g.setColour(juce::Colours::black.withAlpha(0.6f));
    g.fillRoundedRectangle(bounds, 5.0f);
    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    g.drawRect(graphArea, 1.0f);

    if (!isEnabled()) {
        g.setColour(juce::Colours::white.withAlpha(0.8f));
        g.setFont(14.0f);
        g.drawText(juce::String("") + "利用不可", graphArea.withTrimmedRight(4).withTrimmedBottom(2), juce::Justification::bottomRight, false);

        return;
    }

    bool isPitch = currentType == EnvType::Pitch || currentType == EnvType::SsgSwP11;

    // センターライン (Pitch用)
    if (isPitch) {
        g.setColour(juce::Colours::grey.withAlpha(0.5f));
        g.drawLine(graphArea.getX(), graphArea.getCentreY(), graphArea.getRight(), graphArea.getCentreY(), 1.0f);
    }

    if (currentTitle.isNotEmpty()) {
        g.setColour(juce::Colours::white.withAlpha(0.8f));
        g.setFont(12.0f);
        g.drawText(currentTitle, graphArea.withTrimmedLeft(4).withTrimmedTop(2), juce::Justification::topLeft, false);
    }

    if (isBypass) {
        g.setColour(juce::Colours::white.withAlpha(0.8f));
        g.setFont(14.0f);
        g.drawText(juce::String("") + "バイパス中", graphArea.withTrimmedRight(4).withTrimmedBottom(2), juce::Justification::bottomRight, false);

        return;
    }

    if (currentPhases.empty()) return;

    // ========================================================
    // 分岐(ジャンプ)を考慮して「最も右側に到達するX座標」を最大幅として計算する
    // ========================================================
    float maxTotalWidthPx = 0.0f;
    float currentVirtualX = 0.0f; // スケール前の仮想X座標

    for (const auto& phase : currentPhases) {
        if (phase.moveToStart) {
            currentVirtualX = phase.startXOffsetPx; // 分岐点にX座標を巻き戻す
        }
        currentVirtualX += phase.widthPx;
        maxTotalWidthPx = std::max(maxTotalWidthPx, currentVirtualX);
    }

    if (maxTotalWidthPx <= 0.0f) return;

    // ========================================================
    // グラフが短すぎる場合に均等拡大されるのを防ぐ
    // ========================================================
    // 論理的な基準幅（例: 500.0f）を設定し、それを下回る場合は左寄せにする
    float referenceWidth = 500.0f;
    float effectiveTotalWidth = std::max(maxTotalWidthPx, referenceWidth);

    // この scaleX により、長いエンベロープは全体が収まるように縮小され、
    // 短いエンベロープは基準幅に対する比率で描画されるため左に詰まります。
    float scaleX = graphArea.getWidth() / effectiveTotalWidth;

    float currentPixelX = graphArea.getX();
    float basePixelY = isPitch ? graphArea.getCentreY() : graphArea.getBottom();
    float heightScale = isPitch ? (graphArea.getHeight() / 2.0f) : graphArea.getHeight();

    bool hasMax = false;
    float maxX = 0.0f;
    float maxY = 0.0f;
    juce::Colour maxColour = juce::Colours::white;

    for (const auto& phase : currentPhases) {
        if (phase.widthPx <= 0.0f && !phase.isMax) continue;

        // ジャンプ指定がある場合、X座標を移動させて新しい線を引く準備をする
        if (phase.moveToStart) {
            currentPixelX = graphArea.getX() + (phase.startXOffsetPx * scaleX);
        }

        if (phase.isMax) {
            hasMax = true;
            maxX = currentPixelX;
            maxY = basePixelY - (phase.startLevel * heightScale);
            maxColour = phase.color;

            continue;
        }

        float actualWidth = phase.widthPx * scaleX;

        juce::Path envPath;
        float startY = basePixelY - (phase.startLevel * heightScale);
        envPath.startNewSubPath(currentPixelX, startY);

        int resolution = std::max(10, (int)actualWidth / 2);

        for (int i = 1; i <= resolution; ++i) {
            float localProgress = (float)i / resolution; // 現在のフェーズ内での進行度 (0.0 〜 1.0)

            if (std::isnan(localProgress)) {
                localProgress = 0.0f; // NAN回避
            }

            float currentLevel = phase.startLevel + (phase.endLevel - phase.startLevel) * localProgress;

            float px = currentPixelX + localProgress * actualWidth;
            float py = basePixelY - (currentLevel * heightScale);

            envPath.lineTo(px, py);
        }

        // 線の描画 (実線か点線か)
        g.setColour(phase.color);
        if (phase.isDashed) {
            juce::PathStrokeType strokeType(2.0f);
            float dashLengths[] = { 4.0f, 4.0f };
            juce::Path dashedPath;
            strokeType.createDashedStroke(dashedPath, envPath, dashLengths, 2);
            g.fillPath(dashedPath);
        }
        else {
            g.strokePath(envPath, juce::PathStrokeType(2.0f));
        }

        currentPixelX += actualWidth;

        if (phase.drawPhaseLine) {
            // フェーズの区切り線
            g.setColour(phase.phaseLineColor.withAlpha(0.3f));
            g.drawLine(currentPixelX, graphArea.getY(), currentPixelX, graphArea.getBottom(), 2.0f);
        }
    }

    if (hasMax) {
        g.setColour(maxColour);
        g.drawLine(maxX, maxY, maxX, basePixelY, 2.0f);
    }
}

void GuiEnvelopeGraph::updatePitchEnv(
    const GuiSlider& pitchAttackSlider,
    const GuiSlider& pitchDecaySlider,
    const GuiSlider& pitchReleaseSlider,
    const GuiSlider& pitchStartLevelSlider,
    const GuiSlider& pitchAttackLevelSlider,
    const GuiSlider& pitchSustainLevelSlider,
    const GuiSlider& pitchReleaseLevelSlider
) {
    // -------------------------------------------------------------
    // Helper: 幅の計算
    // -------------------------------------------------------------
    auto rateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
        if (maxRate <= 0.0001f) return 2.0f;
        if (rateValue <= 0.001f) return 2.0f; // 0なら左に詰める(一瞬)
        return (rateValue / maxRate) * maxWidth;
        };

    float ar = pitchAttackSlider.getValue();
    float dr = pitchDecaySlider.getValue();
    float rr = pitchReleaseSlider.getValue();

    float arMax = pitchAttackSlider.getMaximum();
    float drMax = pitchDecaySlider.getMaximum();
    float rrMax = pitchReleaseSlider.getMaximum();

    const float maxCents = 4800.0f; // 仮の最大値
    float stl = pitchStartLevelSlider.getValue() / maxCents;
    float atl = pitchAttackLevelSlider.getValue() / maxCents;
    float ssl = pitchSustainLevelSlider.getValue() / maxCents;
    float rll = pitchReleaseLevelSlider.getValue() / maxCents;

    std::vector<GuiEnvelopeGraph::PhaseDef> phases;
    auto color = GuiColor::EnvelopeGraph::PitchLine;

    // Attack
    phases.push_back({
        .widthPx = rateToWidth(ar, arMax),
        .startLevel = stl, .endLevel = atl, .color = color,
        .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine
        });

    // Decay
    phases.push_back({
        .widthPx = rateToWidth(dr, drMax),
        .startLevel = atl, .endLevel = ssl, .color = color,
        .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine
        });

    // Sustain
    phases.push_back({ .widthPx = 60.0f, .startLevel = ssl, .endLevel = ssl, .color = color, .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine });

    // Release
    phases.push_back({
        .widthPx = rateToWidth(rr, rrMax),
        .startLevel = ssl, .endLevel = rll, .color = color
        });

    setEnvelope(GuiEnvelopeGraph::EnvType::Pitch, "Pitch Env", phases);
}

void GuiEnvelopeGraph::updateSsgSwEnv(
    const GuiSlider& ssgSwStepsSlider,
    const GuiToggleButton& ssgSwEnvLoopButton,
    const GuiSlider& ssgSwLoopToSlider,
    const GuiSlider& ssgSwLoopCountSlider,
    const std::array<juce::Slider*, 7>& rSl,
    const std::array<juce::Slider*, 7>& lSl
) {
    // -------------------------------------------------------------
    // Helper: 幅の計算
    // -------------------------------------------------------------
    auto rateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
        if (maxRate <= 0.0001f) return 2.0f;
        if (rateValue <= 0.001f) return 2.0f; // 0なら左に詰める(一瞬)
        return (rateValue / maxRate) * maxWidth;
        };

    int steps = (int)ssgSwStepsSlider.getValue();
    if (steps < 1) steps = 1;
    bool isLoop = ssgSwEnvLoopButton.getToggleState();
    int loopTo = (int)ssgSwLoopToSlider.getValue();
    int loopCount = (int)ssgSwLoopCountSlider.getValue();

    std::vector<GuiEnvelopeGraph::PhaseDef> phases;
    auto color = GuiColor::EnvelopeGraph::AmpLine;

    float currentTotalWidth = 0.0f;
    float loopToLevel = isLoop ? lSl[loopTo]->getValue() / lSl[loopTo]->getMaximum() : 0.0f;
    float loopEndWidth = 0.0f;

    for (int s = 1; s <= steps; ++s) {
        float rate = rSl[s]->getValue();
        float rateMax = rSl[s]->getMaximum();
        float startL = lSl[s - 1]->getValue() / lSl[s - 1]->getMaximum();
        float endL = lSl[s]->getValue() / lSl[s]->getMaximum();

        float width = rateToWidth(rate, rateMax);

        if (isLoop && s == steps) {
            phases.push_back({
                .widthPx = width, .startLevel = startL, .endLevel = loopToLevel, .color = color,
                .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine
                });

            loopEndWidth = currentTotalWidth;

            if (loopCount > 0) {
                phases.push_back({
                    .widthPx = 30.0f,
                    .startLevel = loopToLevel,
                    .endLevel = loopToLevel,
                    .isDashed = true,
                    .color = color,
                    .drawPhaseLine = false
                    });
                phases.push_back({
                    .widthPx = width,
                    .startLevel = startL,
                    .endLevel = endL,
                    .color = color.darker(0.3f),
                    .moveToStart = true,
                    .startXOffsetPx = loopEndWidth,
                    .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine
                    });
            }

            loopEndWidth += width;
        }
        else {
            phases.push_back({
                .widthPx = width, .startLevel = startL, .endLevel = endL, .color = color,
                .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine
                });
        }
        currentTotalWidth += width;
    }

    // Release
    float rr = rSl[6]->getValue();
    float rrMax = rSl[6]->getMaximum();
    float rStartL = lSl[steps]->getValue() / lSl[steps]->getMaximum();
    float rEndL = lSl[6]->getValue() / lSl[6]->getMaximum();

    phases.push_back({
        .widthPx = rateToWidth(rr, rrMax),
        .startLevel = (isLoop && loopCount == 0) ? loopToLevel : rStartL,
        .endLevel = rEndL,
        .color = GuiColor::EnvelopeGraph::ReleaseLine,
        .moveToStart = isLoop,
        .startXOffsetPx = loopEndWidth
        });

    setEnvelope(GuiEnvelopeGraph::EnvType::SsgSw, "SSG SW Env", phases);
}

void GuiEnvelopeGraph::updateSsgSwEnv11(
    const GuiSlider& ssgSwStepsSlider,
    const GuiToggleButton& ssgSwEnvLoopButton,
    const GuiSlider& ssgSwLoopToSlider,
    const GuiSlider& ssgSwLoopCountSlider,
    const std::array<juce::Slider*, 12>& rSl,
    const std::array<juce::Slider*, 12>& lSl
) {
    // -------------------------------------------------------------
    // Helper: 幅の計算
    // -------------------------------------------------------------
    auto rateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
        if (maxRate <= 0.0001f) return 2.0f;
        if (rateValue <= 0.001f) return 2.0f; // 0なら左に詰める(一瞬)
        return (rateValue / maxRate) * maxWidth;
        };

    int steps = (int)ssgSwStepsSlider.getValue();
    if (steps < 1) steps = 1;
    bool isLoop = ssgSwEnvLoopButton.getToggleState();
    int loopTo = (int)ssgSwLoopToSlider.getValue();
    int loopCount = (int)ssgSwLoopCountSlider.getValue();

    std::vector<GuiEnvelopeGraph::PhaseDef> phases;
    auto color = GuiColor::EnvelopeGraph::AmpLine;

    float currentTotalWidth = 0.0f;
    float loopToLevel = isLoop ? lSl[loopTo]->getValue() / lSl[loopTo]->getMaximum() : 0.0f;
    float loopEndWidth = 0.0f;

    for (int s = 1; s <= steps; ++s) {
        float rate = rSl[s]->getValue();
        float rateMax = rSl[s]->getMaximum();
        float startL = lSl[s - 1]->getValue() / lSl[s - 1]->getMaximum();
        float endL = lSl[s]->getValue() / lSl[s]->getMaximum();

        float width = rateToWidth(rate, rateMax);

        if (isLoop && s == steps) {
            phases.push_back({
                .widthPx = width, .startLevel = startL, .endLevel = loopToLevel, .color = color,
                .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine
                });

            loopEndWidth = currentTotalWidth;

            if (loopCount > 0) {
                phases.push_back({
                    .widthPx = 30.0f,
                    .startLevel = loopToLevel,
                    .endLevel = loopToLevel,
                    .isDashed = true,
                    .color = color,
                    .drawPhaseLine = false
                    });
                phases.push_back({
                    .widthPx = width,
                    .startLevel = startL,
                    .endLevel = endL,
                    .color = color.darker(0.3f),
                    .moveToStart = true,
                    .startXOffsetPx = loopEndWidth,
                    .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine
                    });
            }

            loopEndWidth += width;
        }
        else {
            phases.push_back({
                .widthPx = width, .startLevel = startL, .endLevel = endL, .color = color,
                .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine
                });
        }
        currentTotalWidth += width;
    }

    // Release
    float rr = rSl[11]->getValue();
    float rrMax = rSl[11]->getMaximum();
    float rStartL = lSl[steps]->getValue() / lSl[steps]->getMaximum();
    float rEndL = lSl[11]->getValue() / lSl[11]->getMaximum();

    phases.push_back({
        .widthPx = rateToWidth(rr, rrMax),
        .startLevel = (isLoop && loopCount == 0) ? loopToLevel : rStartL,
        .endLevel = rEndL,
        .color = GuiColor::EnvelopeGraph::ReleaseLine,
        .moveToStart = isLoop,
        .startXOffsetPx = loopEndWidth
        });

    setEnvelope(GuiEnvelopeGraph::EnvType::SsgSw11, "SSG SW Env11", phases);
}

void GuiEnvelopeGraph::updateSsgSwPEnv11(
    const GuiSlider& ssgSwStepsSlider,
    const GuiToggleButton& ssgSwEnvLoopButton,
    const GuiSlider& ssgSwLoopToSlider,
    const GuiSlider& ssgSwLoopCountSlider,
    const std::array<juce::Slider*, 12>& rSl,
    const std::array<juce::Slider*, 12>& lSl
) {
    // -------------------------------------------------------------
    // Helper: 幅の計算
    // -------------------------------------------------------------
    auto rateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
        if (maxRate <= 0.0001f) return 2.0f;
        if (rateValue <= 0.001f) return 2.0f; // 0なら左に詰める(一瞬)
        return (rateValue / maxRate) * maxWidth;
        };

    const float maxCents = 4800.0f; // 仮の最大値
    int steps = (int)ssgSwStepsSlider.getValue();
    if (steps < 1) steps = 1;
    bool isLoop = ssgSwEnvLoopButton.getToggleState();
    int loopTo = (int)ssgSwLoopToSlider.getValue();
    int loopCount = (int)ssgSwLoopCountSlider.getValue();

    std::vector<GuiEnvelopeGraph::PhaseDef> phases;
    auto color = GuiColor::EnvelopeGraph::PitchLine;

    float currentTotalWidth = 0.0f;
    float loopToLevel = isLoop ? lSl[loopTo]->getValue() / lSl[loopTo]->getMaximum() : 0.0f;
    float loopEndWidth = 0.0f;

    for (int s = 1; s <= steps; ++s) {
        float rate = rSl[s]->getValue();
        float rateMax = rSl[s]->getMaximum();
        float startL = lSl[s - 1]->getValue() / lSl[s - 1]->getMaximum();
        float endL = lSl[s]->getValue() / lSl[s]->getMaximum();
        float width = rateToWidth(rate, rateMax);

        if (isLoop && s == steps) {
            phases.push_back({
                .widthPx = width, .startLevel = startL, .endLevel = loopToLevel, .color = color,
                .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine
                });

            loopEndWidth = currentTotalWidth;

            if (loopCount > 0) {
                phases.push_back({
                    .widthPx = 30.0f,
                    .startLevel = loopToLevel,
                    .endLevel = loopToLevel,
                    .isDashed = true,
                    .color = color,
                    .drawPhaseLine = false
                    });
                phases.push_back({
                    .widthPx = width,
                    .startLevel = startL,
                    .endLevel = endL,
                    .color = color.darker(0.3f),
                    .moveToStart = true,
                    .startXOffsetPx = loopEndWidth,
                    .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine
                    });
            }

            loopEndWidth += width;
        }
        else {
            phases.push_back({
                .widthPx = width, .startLevel = startL, .endLevel = endL, .color = color,
                .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine
                });
        }
        currentTotalWidth += width;
    }

    // Release
    float rr = rSl[11]->getValue();
    float rrMax = rSl[11]->getMaximum();
    float rStartL = lSl[steps]->getValue() / lSl[steps]->getMaximum();
    float rEndL = lSl[11]->getValue() / lSl[11]->getMaximum();

    phases.push_back({
        .widthPx = rateToWidth(rr, rrMax),
        .startLevel = (isLoop && loopCount == 0) ? loopToLevel : rStartL,
        .endLevel = rEndL,
        .color = GuiColor::EnvelopeGraph::ReleaseLine,
        .moveToStart = isLoop,
        .startXOffsetPx = loopEndWidth
        });

    setEnvelope(GuiEnvelopeGraph::EnvType::SsgSwP11, "SSG SW PitchEnv11", phases);
}

void GuiEnvelopeGraph::updateAmpEnv(
    const GuiSlider& startLevelSlider,
    const GuiSlider& attackSlider,
    const GuiSlider& decaySlider,
    const GuiSlider& sustainSlider,
    const GuiSlider& releaseSlider,
    const GuiToggleButton& korButton
) {
    // -------------------------------------------------------------
    // Helper: 幅の計算
    // -------------------------------------------------------------
    auto rateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
        if (maxRate <= 0.0001f) return maxWidth;
        if (rateValue <= 0.0f) return maxWidth;
        float norm = rateValue / maxRate;
        return std::max(2.0f, maxWidth * norm);
        };

    float stlMax = (float)startLevelSlider.getMaximum();
    float arMax = (float)attackSlider.getMaximum();
    float drMax = (float)decaySlider.getMaximum();
    float slMax = (float)sustainSlider.getMaximum();
    float rrMax = (float)releaseSlider.getMaximum();

    float stlVal = (float)startLevelSlider.getValue();
    float arVal = (float)attackSlider.getValue();
    float drVal = (float)decaySlider.getValue();
    float slVal = (float)sustainSlider.getValue();
    float rrVal = (float)releaseSlider.getValue();

    float sl = slVal / slMax;

    bool isKor = korButton.getToggleState();

    std::vector<GuiEnvelopeGraph::PhaseDef> phases;
    auto color = GuiColor::EnvelopeGraph::AmpLine;

    float currentTotalWidth = 0.0f;

    // 1. Attack
    float attackWidth = rateToWidth(arVal, arMax);
    phases.push_back({
        .widthPx = attackWidth, .startLevel = stlVal / stlMax, .endLevel = 1.0f, .color = color,
        .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine
        });
    currentTotalWidth += attackWidth;

    // 2. Decay 1
    float decayWidth = rateToWidth(drVal, drMax);
    phases.push_back({
        .widthPx = decayWidth, .startLevel = 1.0f, .endLevel = sl, .color = color,
        .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine
        });
    currentTotalWidth += decayWidth;

    // 3. Sustain
    float keyOnWidth = 60.0f;

    phases.push_back({ .widthPx = keyOnWidth, .startLevel = sl, .endLevel = sl, .color = color, .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine });

    currentTotalWidth += keyOnWidth;
    float noteOffPositionX = currentTotalWidth;

    // 4. Release (通常時のみカーブを適用)
    if (isKor) {
        // korでは、rrが終わるまでslをキープ
        phases.push_back({
            .widthPx = rateToWidth(rrVal, rrMax),
            .startLevel = sl,
            .endLevel = sl,
            .color = GuiColor::EnvelopeGraph::ReleaseLine
            });
    }
    else {
        phases.push_back({
            .widthPx = rateToWidth(rrVal, rrMax),
            .startLevel = sl,
            .endLevel = 0.0f,
            .color = color
            });
    }

    setEnvelope(GuiEnvelopeGraph::EnvType::Amp, "Amp Env", phases);
}
