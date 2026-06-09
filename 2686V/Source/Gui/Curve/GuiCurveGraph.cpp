#include <cmath>

#include "./GuiCurveGraph.h"

GuiCurveGraph::GuiCurveGraph()
{
    setOpaque(false);
}

void GuiCurveGraph::bindActiveParameters(const juce::String& graphTitle, int logicIndex, std::vector<GuiSlider*> activeSliders, GuiSlider* kSlider)
{
    title = graphTitle;
    currentLogic = logicIndex;
    boundSliders = activeSliders;
    boundKSlider = kSlider;
    repaint();
}

void GuiCurveGraph::setEnabled(bool enabled) {
    this->enabled = enabled;
}

// ------------------------------------------------------------------
// 操作ハンドルのマッピング (ユーザー様の valueLabels に完全同期)
// ------------------------------------------------------------------
std::vector<GuiCurveGraph::HandleDef> GuiCurveGraph::getActiveHandles() const
{
    std::vector<HandleDef> handles;
    auto colorVia = juce::Colours::cyan;
    auto colorCp1 = juce::Colours::orange;
    auto colorCp2 = juce::Colours::yellow;

    switch (currentLogic) {
    case 5: // Sp1
        handles.push_back({ 0, 1, colorCp1 }); break;
    case 6: // Sp2
        handles.push_back({ 0, 1, colorCp1 }); handles.push_back({ 2, 3, colorCp2 }); break;
    case 7: case 8: case 9: case 10: // Linear + Arc/Exp/Log
        handles.push_back({ 0, 1, colorVia }); break;
    case 11: // Linear + Sp1
        handles.push_back({ 0, 1, colorVia }); handles.push_back({ 2, 3, colorCp1 }); break;
    case 12: // Linear + Sp2
        handles.push_back({ 0, 1, colorVia }); handles.push_back({ 2, 3, colorCp1 }); handles.push_back({ 4, 5, colorCp2 }); break;
    case 13: case 14: // Arc + Linear
        handles.push_back({ 0, 1, colorVia }); break;
    case 15: case 16: // Exp/Log + Linear (Rateが先頭のため1,2)
        handles.push_back({ 1, 2, colorVia }); break;
    case 17: // Sp1 + Linear
        handles.push_back({ 0, 1, colorCp1 }); handles.push_back({ 2, 3, colorVia }); break;
    case 18: // Sp2 + Linear
        handles.push_back({ 0, 1, colorCp1 }); handles.push_back({ 2, 3, colorCp2 }); handles.push_back({ 4, 5, colorVia }); break;
    case 19: case 20: case 21: case 22: // Linear2 + Arc/Exp/Log
        handles.push_back({ 0, 1, colorVia }); handles.push_back({ 2, 3, colorVia }); break;
    case 23: // Linear2 + Sp1
        handles.push_back({ 0, 1, colorVia }); handles.push_back({ 2, 3, colorVia }); handles.push_back({ 4, 5, colorCp1 }); break;
    case 24: // Linear2 + Sp2
        handles.push_back({ 0, 1, colorVia }); handles.push_back({ 2, 3, colorVia }); handles.push_back({ 4, 5, colorCp1 }); handles.push_back({ 6, 7, colorCp2 }); break;
    }
    return handles;
}

juce::Point<float> GuiCurveGraph::getPixelFromNorm(float normX, float normY) const {
    auto bounds = getLocalBounds().toFloat().reduced(10.0f);
    return { bounds.getX() + normX * bounds.getWidth(), bounds.getBottom() - normY * bounds.getHeight() };
}

juce::Point<float> GuiCurveGraph::getNormFromPixel(float px, float py) const {
    auto bounds = getLocalBounds().toFloat().reduced(10.0f);
    float nx = (px - bounds.getX()) / bounds.getWidth();
    float ny = (bounds.getBottom() - py) / bounds.getHeight();
    return { std::clamp(nx, 0.0f, 1.0f), std::clamp(ny, 0.0f, 1.0f) };
}

void GuiCurveGraph::paint(juce::Graphics& g)
{
    auto bounds = getLocalBounds().toFloat();
    auto graphArea = bounds.reduced(10.0f);

    if (!enabled) {
        g.setColour(juce::Colours::transparentBlack);
        g.fillRoundedRectangle(bounds, 5.0f);

        return;
    }

    // 背景とグリッド
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.fillRoundedRectangle(bounds, 5.0f);

    g.setColour(juce::Colours::grey.withAlpha(0.4f));
    g.drawRect(graphArea, 1.0f);
    for (int i = 1; i < 4; ++i) {
        float x = graphArea.getX() + graphArea.getWidth() * (i / 4.0f);
        float y = graphArea.getY() + graphArea.getHeight() * (i / 4.0f);
        g.drawLine(x, graphArea.getY(), x, graphArea.getBottom(), 0.5f);
        g.drawLine(graphArea.getX(), y, graphArea.getRight(), y, 0.5f);
    }

    if (title.isNotEmpty()) {
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.setFont(12.0f);
        g.drawText(title, graphArea.withTrimmedLeft(4).withTrimmedTop(2), juce::Justification::topLeft, false);
    }

    // 補助線の描画 (曲線より下に描画する)
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    auto val = [&](int idx) -> float { return (idx < boundSliders.size()) ? (float)boundSliders[idx]->getValue() : 0.0f; };
    auto pt = [&](int ix, int iy) { return getPixelFromNorm(val(ix), val(iy)); };
    auto pt0 = getPixelFromNorm(0.0f, 0.0f);
    auto pt1 = getPixelFromNorm(1.0f, 1.0f);
    auto drawLine = [&](juce::Point<float> p1, juce::Point<float> p2) { g.drawLine(p1.x, p1.y, p2.x, p2.y, 1.0f); };

    switch (currentLogic) {
    case 5: drawLine(pt0, pt(0, 1)); drawLine(pt(0, 1), pt1); break;
    case 6: drawLine(pt0, pt(0, 1)); drawLine(pt(0, 1), pt(2, 3)); drawLine(pt(2, 3), pt1); break;
    case 7: case 8: case 9: case 10: drawLine(pt0, pt(0, 1)); break;
    case 11: drawLine(pt0, pt(0, 1)); drawLine(pt(0, 1), pt(2, 3)); drawLine(pt(2, 3), pt1); break;
    case 12: drawLine(pt0, pt(0, 1)); drawLine(pt(0, 1), pt(2, 3)); drawLine(pt(2, 3), pt(4, 5)); drawLine(pt(4, 5), pt1); break;
    case 13: case 14: drawLine(pt(0, 1), pt1); break;
    case 15: case 16: drawLine(pt(1, 2), pt1); break; // Rateが先頭のためVx,Vyは1,2
    case 17: drawLine(pt0, pt(0, 1)); drawLine(pt(0, 1), pt(2, 3)); drawLine(pt(2, 3), pt1); break;
    case 18: drawLine(pt0, pt(0, 1)); drawLine(pt(0, 1), pt(2, 3)); drawLine(pt(2, 3), pt(4, 5)); drawLine(pt(4, 5), pt1); break;
    case 19: case 20: case 21: case 22: drawLine(pt0, pt(0, 1)); drawLine(pt(2, 3), pt1); break;
    case 23: drawLine(pt0, pt(0, 1)); drawLine(pt(0, 1), pt(4, 5)); drawLine(pt(4, 5), pt(2, 3)); drawLine(pt(2, 3), pt1); break;
    case 24: drawLine(pt0, pt(0, 1)); drawLine(pt(0, 1), pt(4, 5)); drawLine(pt(4, 5), pt(6, 7)); drawLine(pt(6, 7), pt(2, 3)); drawLine(pt(2, 3), pt1); break;
    }

    // 曲線の描画
    juce::Path curvePath;
    int resolution = 100;
    for (int i = 0; i <= resolution; ++i) {
        float nx = (float)i / resolution;
        float ny = evaluateCurve(nx);

        // NaN のときは 0.0 にする
        if (std::isnan(ny)) {
            ny = 0.0f;
        }

        auto currentPt = getPixelFromNorm(nx, ny);
        if (i == 0) curvePath.startNewSubPath(currentPt);
        else        curvePath.lineTo(currentPt);
    }
    g.setColour(juce::Colours::white);
    g.strokePath(curvePath, juce::PathStrokeType(2.0f, juce::PathStrokeType::curved));

    // ハンドルの点描画
    auto handles = getActiveHandles();
    for (const auto& h : handles) {
        if (h.xIndex >= boundSliders.size() || h.yIndex >= boundSliders.size()) continue;
        auto hPt = pt(h.xIndex, h.yIndex);

        g.setColour(h.color);
        g.fillEllipse(hPt.x - 5.0f, hPt.y - 5.0f, 10.0f, 10.0f);
        g.setColour(juce::Colours::white);
        g.drawEllipse(hPt.x - 5.0f, hPt.y - 5.0f, 10.0f, 10.0f, 1.0f);
    }
}

void GuiCurveGraph::mouseDown(const juce::MouseEvent& e) {
    draggingHandleIndex = -1;
    auto handles = getActiveHandles();
    for (size_t i = 0; i < handles.size(); ++i) {
        if (handles[i].xIndex >= boundSliders.size()) continue;
        float nx = (float)boundSliders[handles[i].xIndex]->getValue();
        float ny = (float)boundSliders[handles[i].yIndex]->getValue();
        auto pt = getPixelFromNorm(nx, ny);
        if (e.position.getDistanceFrom(pt) < 12.0f) {
            draggingHandleIndex = (int)i;
            break;
        }
    }
}

void GuiCurveGraph::mouseDrag(const juce::MouseEvent& e) {
    if (draggingHandleIndex < 0) return;
    auto handles = getActiveHandles();
    const auto& h = handles[draggingHandleIndex];

    auto newNorm = getNormFromPixel(e.position.x, e.position.y);
    boundSliders[h.xIndex]->setValue(newNorm.x, juce::sendNotificationSync);
    boundSliders[h.yIndex]->setValue(newNorm.y, juce::sendNotificationSync);
    repaint();
}

void GuiCurveGraph::mouseUp(const juce::MouseEvent&) { draggingHandleIndex = -1; }

// ------------------------------------------------------------------
// UIプレビュー用 数学エンジン
// ------------------------------------------------------------------
float GuiCurveGraph::evaluateCurve(float x) const
{
    // k を取得して乗算に使用する
    float kVal = boundKSlider ? boundKSlider->getValue() : 1.0f;

    auto mapR = [](float v, float inM, float inMx, float oM, float oMx) {
        if (inMx - inM == 0.0f) return oM;
        return oM + (v - inM) * (oMx - oM) / (inMx - inM);
        };

    auto val = [&](int idx) -> float { return (idx < boundSliders.size()) ? (float)boundSliders[idx]->getValue() : 0.0f; };

    auto cLin = [](float x) { return x; };
    auto cAEx = [](float x) { return 1.0f - std::sqrt(1.0f - std::pow(x, 2.0f)); };
    auto cALg = [](float x) { return std::sqrt(1.0f - std::pow(1.0f - x, 2.0f)); };
    auto cExp = [&](float x, float r) { float rate = r * kVal; return std::abs(rate) < 0.001f ? x : (std::exp(rate * x) - 1.0f) / (std::exp(rate) - 1.0f); };
    auto cLog = [&](float x, float r) { float rate = r * kVal; return std::abs(rate) < 0.001f ? x : std::log(1.0f + rate * x) / std::log(1.0f + rate); };
    auto cSp1 = [](float x, float cx, float cy) { float t = x; return (1.0f - t) * (1.0f - t) * 0.0f + 2.0f * (1.0f - t) * t * cy + t * t * 1.0f; };
    auto cSp2 = [](float x, float cx1, float cy1, float cx2, float cy2) {
        float t = x; float mt = 1.0f - t;
        return 3.0f * mt * mt * t * cy1 + 3.0f * mt * t * t * cy2 + t * t * t;
        };

    switch (currentLogic) {
    case 0: return cLin(x);
    case 1: return cAEx(x);
    case 2: return cALg(x);
    case 3: return cExp(x, val(0));
    case 4: return cLog(x, val(0));
    case 5: return cSp1(x, val(0), val(1));
    case 6: return cSp2(x, val(0), val(1), val(2), val(3));

    case 7: case 8: case 9: case 10: case 11: case 12: {
        float px = val(0), py = val(1);
        if (x <= px) return mapR(cLin(mapR(x, 0.0f, px, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py);
        float lx = mapR(x, px, 1.0f, 0.0f, 1.0f);
        if (currentLogic == 7) return mapR(cAEx(lx), 0.0f, 1.0f, py, 1.0f);
        if (currentLogic == 8) return mapR(cALg(lx), 0.0f, 1.0f, py, 1.0f);
        if (currentLogic == 9) return mapR(cExp(lx, val(2)), 0.0f, 1.0f, py, 1.0f);
        if (currentLogic == 10) return mapR(cLog(lx, val(2)), 0.0f, 1.0f, py, 1.0f);
        if (currentLogic == 11) return mapR(cSp1(lx, val(2), val(3)), 0.0f, 1.0f, py, 1.0f);
        if (currentLogic == 12) return mapR(cSp2(lx, val(2), val(3), val(4), val(5)), 0.0f, 1.0f, py, 1.0f);
    }
    case 13: case 14: case 15: case 16: case 17: case 18: {
        float px, py;
        if (currentLogic == 15 || currentLogic == 16) { px = val(1); py = val(2); } // Rateが先頭
        else if (currentLogic == 17) { px = val(2); py = val(3); } // CPが先頭
        else if (currentLogic == 18) { px = val(4); py = val(5); } // CP1,2が先頭
        else { px = val(0); py = val(1); }

        if (x > px) return mapR(cLin(mapR(x, px, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py, 1.0f);
        float lx = mapR(x, 0.0f, px, 0.0f, 1.0f);
        if (currentLogic == 13) return mapR(cAEx(lx), 0.0f, 1.0f, 0.0f, py);
        if (currentLogic == 14) return mapR(cALg(lx), 0.0f, 1.0f, 0.0f, py);
        if (currentLogic == 15) return mapR(cExp(lx, val(0)), 0.0f, 1.0f, 0.0f, py);
        if (currentLogic == 16) return mapR(cLog(lx, val(0)), 0.0f, 1.0f, 0.0f, py);
        if (currentLogic == 17) return mapR(cSp1(lx, val(0), val(1)), 0.0f, 1.0f, 0.0f, py);
        if (currentLogic == 18) return mapR(cSp2(lx, val(0), val(1), val(2), val(3)), 0.0f, 1.0f, 0.0f, py);
    }
    case 19: case 20: case 21: case 22: case 23: case 24: {
        float px1 = val(0), py1 = val(1);
        float px2 = val(2), py2 = val(3);
        if (x <= px1) return mapR(cLin(mapR(x, 0.0f, px1, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py1);
        if (x > px2)  return mapR(cLin(mapR(x, px2, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py2, 1.0f);

        float lx = mapR(x, px1, px2, 0.0f, 1.0f);
        if (currentLogic == 19) return mapR(cAEx(lx), 0.0f, 1.0f, py1, py2);
        if (currentLogic == 20) return mapR(cALg(lx), 0.0f, 1.0f, py1, py2);
        if (currentLogic == 21) return mapR(cExp(lx, val(4)), 0.0f, 1.0f, py1, py2);
        if (currentLogic == 22) return mapR(cLog(lx, val(4)), 0.0f, 1.0f, py1, py2);
        if (currentLogic == 23) return mapR(cSp1(lx, val(4), val(5)), 0.0f, 1.0f, py1, py2);
        if (currentLogic == 24) return mapR(cSp2(lx, val(4), val(5), val(6), val(7)), 0.0f, 1.0f, py1, py2);
    }
    }
    return x;
}
