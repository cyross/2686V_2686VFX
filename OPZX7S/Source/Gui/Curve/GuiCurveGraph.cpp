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
// 操作ハンドルのマッピング (固定長リストで高速化)
// ------------------------------------------------------------------
GuiCurveGraph::HandleList GuiCurveGraph::getActiveHandles() const
{
    HandleList handles;
    auto colorVia1 = juce::Colours::cyan;
    auto colorVia2 = juce::Colours::blue;
    auto colorCp1 = juce::Colours::green;
    auto colorCp2 = juce::Colours::yellow;
    auto colorCp3 = juce::Colours::orange;
    auto colorCp4 = juce::Colours::red;
    auto colorCp5 = juce::Colours::pink;
    auto colorCp6 = juce::Colours::magenta;

    switch (currentLogic) {
    case 5: // Sp1
        handles.push_back({ 0, 1, colorCp1 });
        break;
    case 6: // Sp2
        handles.push_back({ 0, 1, colorCp1 });
        handles.push_back({ 2, 3, colorCp2 });
        break;
    case 7:
    case 8:
    case 9:
    case 10: // Linear + Arc/Exp/Log
        handles.push_back({ 0, 1, colorVia1 });
        break;
    case 11: // Linear + Sp1
        handles.push_back({ 0, 1, colorVia1 });
        handles.push_back({ 2, 3, colorCp1 });
        break;
    case 12: // Linear + Sp2
        handles.push_back({ 0, 1, colorVia1 });
        handles.push_back({ 2, 3, colorCp1 });
        handles.push_back({ 4, 5, colorCp2 });
        break;
    case 13:
    case 14: // Arc + Linear
        handles.push_back({ 0, 1, colorVia1 }); 
        break;
    case 15:
    case 16: // Exp/Log + Linear (Rateが先頭のため1,2)
        handles.push_back({ 1, 2, colorVia1 });
        break;
    case 17: // Sp1 + Linear
        handles.push_back({ 0, 1, colorCp1 });
        handles.push_back({ 2, 3, colorVia1 });
        break;
    case 18: // Sp2 + Linear
        handles.push_back({ 0, 1, colorCp1 });
        handles.push_back({ 2, 3, colorCp2 });
        handles.push_back({ 4, 5, colorVia1 });
        break;
    case 19:
    case 20:
    case 21:
    case 22: // Linear2 + Arc/Exp/Log
        handles.push_back({ 0, 1, colorVia1 });
        handles.push_back({ 2, 3, colorVia1 });
        break;
    case 23: // Linear2 + Sp1
        handles.push_back({ 0, 1, colorVia1 });
        handles.push_back({ 2, 3, colorVia2 }); 
        handles.push_back({ 4, 5, colorCp1 });
        break;
    case 24: // Linear2 + Sp2
        handles.push_back({ 0, 1, colorVia1 });
        handles.push_back({ 2, 3, colorVia2 }); 
        handles.push_back({ 4, 5, colorCp1 }); 
        handles.push_back({ 6, 7, colorCp2 });
        break;
    case 27: // Sp1 + Sp1
        handles.push_back({ 0, 1, colorVia1 });
        handles.push_back({ 2, 3, colorCp1 }); 
        handles.push_back({ 4, 5, colorCp2 });
        break;
	case 28: // Sp2 + Sp2
        handles.push_back({ 0, 1, colorVia1 });
        handles.push_back({ 2, 3, colorCp1 });
        handles.push_back({ 4, 5, colorCp2 });
        handles.push_back({ 6, 7, colorCp3 });
        handles.push_back({ 8, 9, colorCp4 });
        break;
    case 29: // Sp1 + Sp1 + Sp1
        handles.push_back({ 0, 1, colorVia1 });
        handles.push_back({ 2, 3, colorVia2 });
        handles.push_back({ 4, 5, colorCp1 });
        handles.push_back({ 6, 7, colorCp2 });
        handles.push_back({ 8, 9, colorCp3 });
        break;
    case 30: // Sp2 + Sp2 + Sp2
        handles.push_back({ 0, 1, colorVia1 });
        handles.push_back({ 2, 3, colorVia2 });
        handles.push_back({ 4, 5, colorCp1 });
        handles.push_back({ 6, 7, colorCp2 });
        handles.push_back({ 8, 9, colorCp3 });
        handles.push_back({ 10, 11, colorCp4 });
        handles.push_back({ 12, 13, colorCp5 });
        handles.push_back({ 14, 15, colorCp6 });
        break;
    }
    return handles;
}

// ------------------------------------------------------------------
// 動的スケーリング (はみ出た分だけ描画領域を広げる)
// ------------------------------------------------------------------
void GuiCurveGraph::updateDisplayRange()
{
    float minX = 0.0f; float maxX = 1.0f;
    float minY = 0.0f; float maxY = 1.0f;

    auto val = [&](int idx) -> float { return (idx < boundSliders.size()) ? (float)boundSliders[idx]->getValue() : 0.0f; };
    auto handles = getActiveHandles();

    // ハンドルの位置をチェック
    for (const auto& h : handles) {
        if (h.xIndex < boundSliders.size()) {
            float hx = val(h.xIndex);
            minX = std::min(minX, hx);
            maxX = std::max(maxX, hx);
        }
        if (h.yIndex < boundSliders.size()) {
            float hy = val(h.yIndex);
            minY = std::min(minY, hy);
            maxY = std::max(maxY, hy);
        }
    }

    // 0.0〜1.0 の曲線評価値をチェック
    int resolution = 100;
    for (int i = 0; i <= resolution; ++i) {
        float nx = (float)i / resolution;
        float ny = evaluateCurve(nx);
        if (!std::isnan(ny)) {
            minY = std::min(minY, ny);
            maxY = std::max(maxY, ny);
        }
    }

    // はみ出ている場合のみ、操作しやすいように10%の余白を足す
    if (minX < 0.0f) minX -= std::abs(minX) * 0.1f;
    if (maxX > 1.0f) maxX += std::abs(maxX) * 0.1f;
    if (minY < 0.0f) minY -= std::abs(minY) * 0.1f;
    if (maxY > 1.0f) maxY += std::abs(maxY) * 0.1f;

    displayMinX = minX;
    displayMaxX = maxX;
    displayMinY = minY;
    displayMaxY = maxY;
}

juce::Point<float> GuiCurveGraph::getPixelFromNorm(float normX, float normY) const {
    auto bounds = getLocalBounds().toFloat().reduced(10.0f);
    float rangeX = displayMaxX - displayMinX;
    float rangeY = displayMaxY - displayMinY;

    // スケールに合わせて座標を算出
    float px = bounds.getX() + ((normX - displayMinX) / rangeX) * bounds.getWidth();
    float py = bounds.getBottom() - ((normY - displayMinY) / rangeY) * bounds.getHeight();

    return { px, py };
}

juce::Point<float> GuiCurveGraph::getNormFromPixel(float px, float py) const {
    auto bounds = getLocalBounds().toFloat().reduced(10.0f);
    float rangeX = displayMaxX - displayMinX;
    float rangeY = displayMaxY - displayMinY;

    // ピクセルからスケーリングされた値に復元
    float nx = displayMinX + ((px - bounds.getX()) / bounds.getWidth()) * rangeX;
    float ny = displayMinY + ((bounds.getBottom() - py) / bounds.getHeight()) * rangeY;

    return { nx, ny };
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

    // 動的スケーリングの計算
    updateDisplayRange();

    // 背景
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.fillRoundedRectangle(bounds, 5.0f);

    // 0.0 ~ 1.0 の「基準枠」の描画 (はみ出ている時の目安になる)
    auto pt00 = getPixelFromNorm(0.0f, 0.0f);
    auto pt11 = getPixelFromNorm(1.0f, 1.0f);
    juce::Rectangle<float> normBox(pt00.x, pt11.y, pt11.x - pt00.x, pt00.y - pt11.y);

    g.setColour(juce::Colours::grey.withAlpha(0.6f));
    g.drawRect(normBox, 1.0f);

    // 0.0 ~ 1.0 の枠内のグリッド線
    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    for (int i = 1; i < 4; ++i) {
        float nx = i / 4.0f;
        float px = getPixelFromNorm(nx, 0.0f).x;
        g.drawLine(px, graphArea.getY(), px, graphArea.getBottom(), 0.5f);

        float ny = i / 4.0f;
        float py = getPixelFromNorm(0.0f, ny).y;
        g.drawLine(graphArea.getX(), py, graphArea.getRight(), py, 0.5f);
    }

    // 0.0と1.0の基準軸を強調
    g.setColour(juce::Colours::grey.withAlpha(0.8f));
    g.drawLine(pt00.x, graphArea.getY(), pt00.x, graphArea.getBottom(), 1.0f); // X=0
    g.drawLine(pt11.x, graphArea.getY(), pt11.x, graphArea.getBottom(), 1.0f); // X=1
    g.drawLine(graphArea.getX(), pt00.y, graphArea.getRight(), pt00.y, 1.0f);  // Y=0
    g.drawLine(graphArea.getX(), pt11.y, graphArea.getRight(), pt11.y, 1.0f);  // Y=1

    if (title.isNotEmpty()) {
        g.setColour(juce::Colours::white.withAlpha(0.7f));
        g.setFont(12.0f);
        g.drawText(title, graphArea.withTrimmedLeft(4).withTrimmedTop(2), juce::Justification::topLeft, false);
    }

    // 補助線の描画
    g.setColour(juce::Colours::white.withAlpha(0.3f));
    auto val = [&](int idx) -> float { return (idx < boundSliders.size()) ? (float)boundSliders[idx]->getValue() : 0.0f; };
    auto pt = [&](int ix, int iy) { return getPixelFromNorm(val(ix), val(iy)); };
    auto drawLine = [&](juce::Point<float> p1, juce::Point<float> p2) { g.drawLine(p1.x, p1.y, p2.x, p2.y, 1.0f); };

    switch (currentLogic) {
    case 5: drawLine(pt00, pt(0, 1)); drawLine(pt(0, 1), pt11); break;
    case 6: drawLine(pt00, pt(0, 1)); drawLine(pt(0, 1), pt(2, 3)); drawLine(pt(2, 3), pt11); break;
    case 7: case 8: case 9: case 10: drawLine(pt00, pt(0, 1)); break;
    case 11: drawLine(pt00, pt(0, 1)); drawLine(pt(0, 1), pt(2, 3)); drawLine(pt(2, 3), pt11); break;
    case 12: drawLine(pt00, pt(0, 1)); drawLine(pt(0, 1), pt(2, 3)); drawLine(pt(2, 3), pt(4, 5)); drawLine(pt(4, 5), pt11); break;
    case 13: case 14: drawLine(pt(0, 1), pt11); break;
    case 15: case 16: drawLine(pt(1, 2), pt11); break; // Rateが先頭のためVx,Vyは1,2
    case 17: drawLine(pt00, pt(0, 1)); drawLine(pt(0, 1), pt(2, 3)); drawLine(pt(2, 3), pt11); break;
    case 18: drawLine(pt00, pt(0, 1)); drawLine(pt(0, 1), pt(2, 3)); drawLine(pt(2, 3), pt(4, 5)); drawLine(pt(4, 5), pt11); break;
    case 19: case 20: case 21: case 22: drawLine(pt00, pt(0, 1)); drawLine(pt(2, 3), pt11); break;
    case 23: drawLine(pt00, pt(0, 1)); drawLine(pt(0, 1), pt(4, 5)); drawLine(pt(4, 5), pt(2, 3)); drawLine(pt(2, 3), pt11); break;
    case 24: drawLine(pt00, pt(0, 1)); drawLine(pt(0, 1), pt(4, 5)); drawLine(pt(4, 5), pt(6, 7)); drawLine(pt(6, 7), pt(2, 3)); drawLine(pt(2, 3), pt11); break;
    case 27:
        drawLine(pt00, pt(2, 3));
        drawLine(pt(2, 3), pt(0, 1));
        drawLine(pt(0, 1), pt(4, 5));
        drawLine(pt(4, 5), pt11);
        break;
    case 28:
        drawLine(pt00, pt(2, 3));
        drawLine(pt(2, 3), pt(4, 5));
        drawLine(pt(4, 5), pt(0, 1));
        drawLine(pt(0, 1), pt(6, 7));
        drawLine(pt(6, 7), pt(8, 9));
        drawLine(pt(8, 9), pt11);
        break;
    case 29:
        drawLine(pt00, pt(4, 5));
        drawLine(pt(4, 5), pt(0, 1));
        drawLine(pt(0, 1), pt(6, 7));
        drawLine(pt(6, 7), pt(2, 3));
        drawLine(pt(2, 3), pt(8, 9));
        drawLine(pt(8, 9), pt11);
        break;
    case 30:
        drawLine(pt00, pt(4, 5));
        drawLine(pt(4, 5), pt(6, 7));
        drawLine(pt(6, 7), pt(0, 1));
        drawLine(pt(0, 1), pt(8, 9));
        drawLine(pt(8, 9), pt(10, 11));
        drawLine(pt(10, 11), pt(2, 3));
        drawLine(pt(2, 3), pt(12, 13));
        drawLine(pt(12, 13), pt(14, 15));
        drawLine(pt(14, 15), pt11);
        break;
    }

    // 曲線の描画 (X=0.0 〜 1.0 の間のみを描画)
    juce::Path curvePath;
    int resolution = 100;
    for (int i = 0; i <= resolution; ++i) {
        float nx = (float)i / resolution;
        float ny = evaluateCurve(nx);

        if (std::isnan(ny)) ny = 0.0f;

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
    case 0:
        return cLin(x);
    case 1:
        return cAEx(x);
    case 2:
        return cALg(x);
    case 3:
        return cExp(x, val(0));
    case 4:
        return cLog(x, val(0));
    case 5:
        return cSp1(x, val(0), val(1));
    case 6:
        return cSp2(x, val(0), val(1), val(2), val(3));
    case 7:
    {
        float px = val(0), py = val(1);

        if (x <= px) return mapR(cLin(mapR(x, 0.0f, px, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py);

        float lx = mapR(x, px, 1.0f, 0.0f, 1.0f);

        return mapR(cAEx(lx), 0.0f, 1.0f, py, 1.0f);
    }
    case 8:
    {
        float px = val(0), py = val(1);

        if (x <= px) return mapR(cLin(mapR(x, 0.0f, px, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py);

        float lx = mapR(x, px, 1.0f, 0.0f, 1.0f);

        return mapR(cALg(lx), 0.0f, 1.0f, py, 1.0f);
    }
    case 9:
    {
        float px = val(0), py = val(1);

        if (x <= px) return mapR(cLin(mapR(x, 0.0f, px, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py);

        float lx = mapR(x, px, 1.0f, 0.0f, 1.0f);

        return mapR(cExp(lx, val(2)), 0.0f, 1.0f, py, 1.0f);
    }
    case 10:
    {
        float px = val(0), py = val(1);

        if (x <= px) return mapR(cLin(mapR(x, 0.0f, px, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py);

        float lx = mapR(x, px, 1.0f, 0.0f, 1.0f);

        return mapR(cLog(lx, val(2)), 0.0f, 1.0f, py, 1.0f);
    }
    case 11:
    {
        float px = val(0), py = val(1);

        if (x <= px) return mapR(cLin(mapR(x, 0.0f, px, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py);

        float lx = mapR(x, px, 1.0f, 0.0f, 1.0f);

        return mapR(cSp1(lx, val(2), val(3)), 0.0f, 1.0f, py, 1.0f);
    }
    case 12:
    {
        float px = val(0), py = val(1);

        if (x <= px) return mapR(cLin(mapR(x, 0.0f, px, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py);

        float lx = mapR(x, px, 1.0f, 0.0f, 1.0f);

        return mapR(cSp2(lx, val(2), val(3), val(4), val(5)), 0.0f, 1.0f, py, 1.0f);
    }
    case 13:
    {
        float px, py;

        if (currentLogic == 15 || currentLogic == 16) { px = val(1); py = val(2); } // Rateが先頭
        else if (currentLogic == 17) { px = val(2); py = val(3); } // CPが先頭
        else if (currentLogic == 18) { px = val(4); py = val(5); } // CP1,2が先頭
        else { px = val(0); py = val(1); }

        if (x > px) return mapR(cLin(mapR(x, px, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py, 1.0f);

        float lx = mapR(x, 0.0f, px, 0.0f, 1.0f);

        return mapR(cAEx(lx), 0.0f, 1.0f, 0.0f, py);
    }
    case 14:
    {
        float px, py;

        if (currentLogic == 15 || currentLogic == 16) { px = val(1); py = val(2); } // Rateが先頭
        else if (currentLogic == 17) { px = val(2); py = val(3); } // CPが先頭
        else if (currentLogic == 18) { px = val(4); py = val(5); } // CP1,2が先頭
        else { px = val(0); py = val(1); }

        if (x > px) return mapR(cLin(mapR(x, px, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py, 1.0f);

        float lx = mapR(x, 0.0f, px, 0.0f, 1.0f);

        return mapR(cALg(lx), 0.0f, 1.0f, 0.0f, py);
    }
    case 15:
    {
        float px, py;

        if (currentLogic == 15 || currentLogic == 16) { px = val(1); py = val(2); } // Rateが先頭
        else if (currentLogic == 17) { px = val(2); py = val(3); } // CPが先頭
        else if (currentLogic == 18) { px = val(4); py = val(5); } // CP1,2が先頭
        else { px = val(0); py = val(1); }

        if (x > px) return mapR(cLin(mapR(x, px, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py, 1.0f);

        float lx = mapR(x, 0.0f, px, 0.0f, 1.0f);

        return mapR(cExp(lx, val(0)), 0.0f, 1.0f, 0.0f, py);
    }
    case 16:
    {
        float px, py;

        if (currentLogic == 15 || currentLogic == 16) { px = val(1); py = val(2); } // Rateが先頭
        else if (currentLogic == 17) { px = val(2); py = val(3); } // CPが先頭
        else if (currentLogic == 18) { px = val(4); py = val(5); } // CP1,2が先頭
        else { px = val(0); py = val(1); }

        if (x > px) return mapR(cLin(mapR(x, px, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py, 1.0f);

        float lx = mapR(x, 0.0f, px, 0.0f, 1.0f);

        return mapR(cLog(lx, val(0)), 0.0f, 1.0f, 0.0f, py);
    }
    case 17:
    {
        float px, py;

        if (currentLogic == 15 || currentLogic == 16) { px = val(1); py = val(2); } // Rateが先頭
        else if (currentLogic == 17) { px = val(2); py = val(3); } // CPが先頭
        else if (currentLogic == 18) { px = val(4); py = val(5); } // CP1,2が先頭
        else { px = val(0); py = val(1); }

        if (x > px) return mapR(cLin(mapR(x, px, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py, 1.0f);

        float lx = mapR(x, 0.0f, px, 0.0f, 1.0f);

        return mapR(cSp1(lx, val(0), val(1)), 0.0f, 1.0f, 0.0f, py);
    }
    case 18:
    {
        float px, py;

        if (currentLogic == 15 || currentLogic == 16) { px = val(1); py = val(2); } // Rateが先頭
        else if (currentLogic == 17) { px = val(2); py = val(3); } // CPが先頭
        else if (currentLogic == 18) { px = val(4); py = val(5); } // CP1,2が先頭
        else { px = val(0); py = val(1); }

        if (x > px) return mapR(cLin(mapR(x, px, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py, 1.0f);

        float lx = mapR(x, 0.0f, px, 0.0f, 1.0f);

        return mapR(cSp2(lx, val(0), val(1), val(2), val(3)), 0.0f, 1.0f, 0.0f, py);
    }
    case 19:
    {
        float px1 = val(0), py1 = val(1);
        float px2 = val(2), py2 = val(3);

        if (x <= px1) return mapR(cLin(mapR(x, 0.0f, px1, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py1);
        if (x > px2)  return mapR(cLin(mapR(x, px2, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py2, 1.0f);

        float lx = mapR(x, px1, px2, 0.0f, 1.0f);

        return mapR(cAEx(lx), 0.0f, 1.0f, py1, py2);
    }
    case 20:
    {
        float px1 = val(0), py1 = val(1);
        float px2 = val(2), py2 = val(3);

        if (x <= px1) return mapR(cLin(mapR(x, 0.0f, px1, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py1);
        if (x > px2)  return mapR(cLin(mapR(x, px2, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py2, 1.0f);

        float lx = mapR(x, px1, px2, 0.0f, 1.0f);

        return mapR(cALg(lx), 0.0f, 1.0f, py1, py2);
    }
    case 21:
    {
        float px1 = val(0), py1 = val(1);
        float px2 = val(2), py2 = val(3);

        if (x <= px1) return mapR(cLin(mapR(x, 0.0f, px1, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py1);
        if (x > px2)  return mapR(cLin(mapR(x, px2, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py2, 1.0f);

        float lx = mapR(x, px1, px2, 0.0f, 1.0f);

        return mapR(cExp(lx, val(4)), 0.0f, 1.0f, py1, py2);
    }
    case 22:
    {
        float px1 = val(0), py1 = val(1);
        float px2 = val(2), py2 = val(3);

        if (x <= px1) return mapR(cLin(mapR(x, 0.0f, px1, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py1);
        if (x > px2)  return mapR(cLin(mapR(x, px2, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py2, 1.0f);

        float lx = mapR(x, px1, px2, 0.0f, 1.0f);

        return mapR(cLog(lx, val(4)), 0.0f, 1.0f, py1, py2);
    }
    case 23:
    {
        float px1 = val(0), py1 = val(1);
        float px2 = val(2), py2 = val(3);

        if (x <= px1) return mapR(cLin(mapR(x, 0.0f, px1, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py1);
        if (x > px2)  return mapR(cLin(mapR(x, px2, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py2, 1.0f);

        float lx = mapR(x, px1, px2, 0.0f, 1.0f);

        return mapR(cSp1(lx, val(4), val(5)), 0.0f, 1.0f, py1, py2);
    }
    case 24:
    {
        float px1 = val(0), py1 = val(1);
        float px2 = val(2), py2 = val(3);

        if (x <= px1) return mapR(cLin(mapR(x, 0.0f, px1, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py1);
        if (x > px2)  return mapR(cLin(mapR(x, px2, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py2, 1.0f);

        float lx = mapR(x, px1, px2, 0.0f, 1.0f);

        return mapR(cSp2(lx, val(4), val(5), val(6), val(7)), 0.0f, 1.0f, py1, py2);
    }
    case 25:
    {
        float px1 = val(0), py1 = val(1);

        if (x <= px1) return mapR(cLin(mapR(x, 0.0f, px1, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py1);

        return mapR(cLin(mapR(x, px1, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py1, 1.0f);
    }
    case 26:
    {
        float px1 = val(0), py1 = val(1);
        float px2 = val(2), py2 = val(3);

        if (x <= px1) return mapR(cLin(mapR(x, 0.0f, px1, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py1);
        if (x > px2)  return mapR(cLin(mapR(x, px2, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py2, 1.0f);

        return mapR(cLin(mapR(x, px1, px2, 0.0f, 1.0f)), 0.0f, 1.0f, py1, py2);
    }
    case 27:
    {
        float px1 = val(0), py1 = val(1);

        float lx1 = mapR(x, 0.0f, px1, 0.0f, 1.0f);

        if (x <= px1) return mapR(cSp1(lx1, val(2), val(3)), 0.0f, 1.0f, 0.0f, py1);

        float lx2 = mapR(x, px1, 1.0, 0.0f, 1.0f);

        return mapR(cSp1(lx2, val(4), val(5)), 0.0f, 1.0f, py1, 1.0);
    }
    case 28:
    {
        float px1 = val(0), py1 = val(1);

        float lx1 = mapR(x, 0.0f, px1, 0.0f, 1.0f);

        if (x <= px1) return mapR(cSp2(lx1, val(2), val(3), val(4), val(5)), 0.0f, 1.0f, 0.0f, py1);

        float lx2 = mapR(x, px1, 1.0, 0.0f, 1.0f);

        return mapR(cSp2(lx2, val(6), val(7), val(8), val(9)), 0.0f, 1.0f, py1, 1.0f);
    }
    case 29:
    {
        float px1 = val(0), py1 = val(1);
        float px2 = val(2), py2 = val(3);

        float lx1 = mapR(x, 0.0f, px1, 0.0f, 1.0f);

        if (x <= px1) return mapR(cSp1(lx1, val(4), val(5)), 0.0f, 1.0f, 0.0f, py1);

        float lx2 = mapR(x, px1, px2, 0.0f, 1.0f);

        if (x <= px2)  return mapR(cSp1(lx2, val(6), val(7)), 0.0f, 1.0f, py1, py2);

        float lx3 = mapR(x, px2, 1.0, 0.0f, 1.0f);

        return mapR(cSp1(lx3, val(8), val(9)), 0.0f, 1.0f, py2, 1.0f);
    }
    case 30:
    {
        float px1 = val(0), py1 = val(1);
        float px2 = val(2), py2 = val(3);

        float lx1 = mapR(x, 0.0f, px1, 0.0f, 1.0f);

        if (x <= px1) return mapR(cSp2(lx1, val(4), val(5), val(6), val(7)), 0.0f, 1.0f, 0.0f, py1);

        float lx2 = mapR(x, px1, px2, 0.0f, 1.0f);

        if (x <= px2)  return mapR(cSp2(lx2, val(8), val(9), val(10), val(11)), 0.0f, 1.0f, py1, py2);

        float lx3 = mapR(x, px2, 1.0, 0.0f, 1.0f);

        return mapR(cSp2(lx3, val(12), val(13), val(14), val(15)), 0.0f, 1.0f, py2, 1.0f);
    }
    }
    return x;
}
