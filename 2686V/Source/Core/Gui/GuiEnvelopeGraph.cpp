#include "GuiEnvelopeGraph.h"

GuiEnvelopeGraph::GuiEnvelopeGraph() { setOpaque(false); }

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

    // センターライン (Pitch用)
    if (currentType == EnvType::Pitch) {
        g.setColour(juce::Colours::grey.withAlpha(0.5f));
        g.drawLine(graphArea.getX(), graphArea.getCentreY(), graphArea.getRight(), graphArea.getCentreY(), 1.0f);
    }

    if (currentTitle.isNotEmpty()) {
        g.setColour(juce::Colours::white.withAlpha(0.8f));
        g.setFont(12.0f);
        g.drawText(currentTitle, graphArea.withTrimmedLeft(4).withTrimmedTop(2), juce::Justification::topLeft, false);
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
    float basePixelY = (currentType == EnvType::Pitch) ? graphArea.getCentreY() : graphArea.getBottom();
    float heightScale = (currentType == EnvType::Pitch) ? (graphArea.getHeight() / 2.0f) : graphArea.getHeight();

    for (const auto& phase : currentPhases) {
        if (phase.widthPx <= 0.0f) continue;

        // ジャンプ指定がある場合、X座標を移動させて新しい線を引く準備をする
        if (phase.moveToStart) {
            currentPixelX = graphArea.getX() + (phase.startXOffsetPx * scaleX);
        }

        float actualWidth = phase.widthPx * scaleX;

        juce::Path envPath;
        float startY = basePixelY - (phase.startLevel * heightScale);
        envPath.startNewSubPath(currentPixelX, startY);

        int resolution = std::max(10, (int)actualWidth / 2);

        float yStart = phase.curveFunc(phase.curveStartX);
        float yEnd = phase.curveFunc(phase.curveEndX);
        float yRange = yEnd - yStart;

        for (int i = 1; i <= resolution; ++i) {
            float localProgress = (float)i / resolution; // 現在のフェーズ内での進行度 (0.0 〜 1.0)

            // 1. 本来のカーブX座標を算出 (0.5〜0.7など)
            float curveX = phase.curveStartX + localProgress * (phase.curveEndX - phase.curveStartX);

            // 2. カーブ関数を通した生のY値を取得
            float rawY = phase.curveFunc(curveX);

            if (isnan(rawY)) {
                rawY = yStart; // NAN回避
            }

            // 3. 切り出した区間(yStart〜yEnd)の中で、現在のY値がどの割合(0.0〜1.0)かを正規化
            float yRatio = localProgress; // デフォルトは線形(0除算回避)
            if (std::abs(yRange) > 0.00001f) {
                yRatio = (rawY - yStart) / yRange;
            }

            // 4. GUI上の描画レベル(startLevel〜endLevel)にマッピング
            float currentLevel = phase.startLevel + (phase.endLevel - phase.startLevel) * yRatio;

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
}
