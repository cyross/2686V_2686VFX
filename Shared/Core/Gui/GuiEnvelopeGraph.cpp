#include "./GuiI18n.h"
#include <cmath>

#include "./GuiEnvelopeGraph.h"
#include "Shared/Advanced/Curve/AdvancedCurveParams.h"

GuiEnvelopeGraph::GuiEnvelopeGraph() {
    isBypass = false;
    setOpaque(false);
}

void GuiEnvelopeGraph::setEnvelope(EnvType type, const juce::String& title, const std::vector<PhaseDef>& phases)
{
    ++m_revision;

    currentType = type;
    currentTitle = title;
    currentPhases = phases;

    // KEEP のときは、行き先へ向かって斜めに引かず、その段の始まりの値で
    // 横一直線を引く。つなぎ目だけが縦に飛ぶ (階段状)。カーブも同じ理由で
    // 効かないので、曲げるはたらきごと外す。
    //
    // リリースも走り終えるまで前の値を保ち、終わりで行き先 (LR) へ移る。
    // 均すと行き先が消えてしまうので、終わりに縦の線を足す。
    if (keepLevels)
    {
        std::vector<PhaseDef> kept;

        for (auto phase : currentPhases)
        {
            if (phase.keepShape) {
                kept.push_back(phase);
                continue;
            }

            const float target = phase.endLevel;

            phase.endLevel = phase.startLevel;
            phase.curveFunc = [](float x) { return x; };
            phase.curveStartX = 0.0f;
            phase.curveEndX = 1.0f;

            kept.push_back(phase);

            if (phase.isRelease && target != phase.startLevel) {
                kept.push_back({
                    .widthPx = 0.01f,
                    .startLevel = phase.startLevel,
                    .endLevel = target,
                    .color = phase.color,
                    .drawPhaseLine = false,
                    .keepShape = true
                    });
            }
        }

        currentPhases = std::move(kept);
    }

    // 1 回きりの印。次に描く区分へ持ち越さない。グラフは区分ごとに
    // 使い回すので、立てっぱなしにすると関係のない区分まで横一直線に
    // なってしまう。
    keepLevels = false;

    // 鳴っている位置も同じ。付けるのは絵を組んだ側 (setLiveSource)
    setLiveSource(nullptr, nullptr);

    // STL の中抜きの丸とテンポの文字も同じ。付けるのは段のエンベロープ (setStepEnvelope)
    hollowStart = false;
    tempoText.clear();

    repaint();
}

void GuiEnvelopeGraph::setLiveSource(const LivePosition* position, std::function<int()> fps)
{
    ++m_revision;

    liveTable = position;
    liveFps = std::move(fps);

    if (liveTable == nullptr || !liveFps) {
        stopTimer();

        if (liveStep != LivePosition::idle) {
            liveStep = LivePosition::idle;
            repaint();
        }

        return;
    }

    timerCallback();
}

// 表に書かれた位置を読み、変わっていれば描き直す。切っているあいだも、
// 入れ直したときに追えるよう、ゆっくり見に行く
void GuiEnvelopeGraph::timerCallback()
{
    if (liveTable == nullptr || !liveFps) {
        stopTimer();
        return;
    }

    const int fps = liveFps();

    LiveGraph::pace(*this, fps > 0 ? fps : LiveGraph::idleHz);

    // 隠れたタブ・閉じた区分のグラフは読みに行かない (見えたときの次の回で追いつく)
    if (!isShowing()) return;

    const auto live = liveTable->read();
    const auto now = juce::Time::getMillisecondCounter();

    // 書く側は鳴っているあいだ毎回書く。書かれなくなったら (ボイスが止まった、
    // 指し先を変えたなど) 最後の位置が残るので、しばらく変わらなければ消す
    if (live.seq != liveSeq) {
        liveSeq = live.seq;
        liveSeqMs = now;
    }

    const bool stale = now - liveSeqMs > liveStaleMs;

    const int step = fps > 0 && !stale ? live.phase : LivePosition::idle;
    const float frac = step != LivePosition::idle ? live.frac : 0.0f;

    if (step == liveStep && std::abs(frac - liveFrac) < 0.001f) return;

    liveStep = step;
    liveFrac = frac;

    repaint();
}

namespace
{
    // 拡大のつまみの丈 (送り棒 + つまみとボタンの行)
    constexpr int zoomBarHeight = 8;
    constexpr int zoomRowHeight = 16;

    constexpr std::array<float, 6> zoomPresets = { 1.0f, 2.0f, 4.0f, 8.0f, 16.0f, 32.0f };

    // x1〜x32 のボタン 1 個の幅。広い枠ではこの幅、狭い枠では文字が収まる幅まで詰める
    constexpr int zoomButtonWidth = 30;

    // 文字の左右に要る余白 (JUCE がボタンの文字の左右へ空けるぶんと、並べるときの隙間)
    constexpr int zoomButtonTextPadding = 16;

    // 拡大率のつまみに残す幅の下限 (数値の枠と帯)
    constexpr int zoomSliderMinWidth = 100;
}

double GuiZoomSlider::align(double value, double step, double lo, double hi)
{
    if (step <= 0.0 || step > hi - lo) return value;

    return juce::jlimit(lo, hi, std::round(value / step) * step);
}

double GuiZoomSlider::snapValue(double attemptedValue, juce::Slider::DragMode dragMode)
{
    // ドラッグのときだけ。数字の打ち込みやボタン・ホイールでは、そろえない
    if (dragMode == juce::Slider::notDragging) return attemptedValue;

    const double step = GuiSlider::alignStepForKeys(juce::ModifierKeys::currentModifiers, getInterval());

    return align(attemptedValue, step, getMinimum(), getMaximum());
}

void GuiEnvelopeGraph::setZoomable(bool z)
{
    zoomable = z;

    if (zoomable) {
        scrollBar.setAutoHide(false);
        scrollBar.addListener(this);
        addAndMakeVisible(scrollBar);

        zoomSlider.setRange(zoomMin, zoomMax, 0.01);
        zoomSlider.setSkewFactorFromMidPoint(4.0);
        zoomSlider.setNumDecimalPlacesToDisplay(2);
        zoomSlider.setTextBoxStyle(juce::Slider::TextBoxRight, false, 44, zoomRowHeight);
        zoomSlider.setValue(zoom, juce::dontSendNotification);
        zoomSlider.onValueChange = [this] { setZoom((float)zoomSlider.getValue()); };
        addAndMakeVisible(zoomSlider);

        for (size_t i = 0; i < zoomButtons.size(); ++i) {
            auto& b = zoomButtons[i];
            const float value = zoomPresets[i];

            b.setButtonText("x" + juce::String((int)value));
            b.onClick = [this, value] { setZoom(value); };
            addAndMakeVisible(b);
        }
    }
    else {
        scrollBar.removeListener(this);
        removeChildComponent(&scrollBar);
        removeChildComponent(&zoomSlider);

        for (auto& b : zoomButtons) removeChildComponent(&b);

        zoom = 1.0f;
        scroll = 0.0f;
    }

    resized();
    repaint();
}

void GuiEnvelopeGraph::setZoom(float z)
{
    z = juce::jlimit(zoomMin, zoomMax, z);

    if (z == zoom) return;

    // 見えている真ん中 (中身の幅に対する割合) を保つ
    const float view = juce::jmax(1.0f, lastViewW);
    const float centre = (overflow() * scroll + view * 0.5f) / (view * zoom);

    zoom = z;

    const float newOverflow = overflow();

    scroll = newOverflow > 0.0f ? juce::jlimit(0.0f, 1.0f, (centre * view * zoom - view * 0.5f) / newOverflow) : 0.0f;

    zoomSlider.setValue(zoom, juce::dontSendNotification);

    repaint();
}

juce::Rectangle<int> GuiEnvelopeGraph::graphBounds() const
{
    auto r = getLocalBounds();

    if (zoomable) r.removeFromBottom(zoomBarHeight + zoomRowHeight + 2);

    return r;
}

void GuiEnvelopeGraph::resized()
{
    if (!zoomable) return;

    auto r = getLocalBounds();
    auto row = r.removeFromBottom(zoomRowHeight);

    scrollBar.setBounds(r.removeFromBottom(zoomBarHeight).reduced(5, 0));

    // 左に拡大率のつまみ、右に x1〜x32 のボタン。
    //
    // ボタンは文字が収まる幅を取る。行の半分を等分していたころは、狭い枠
    // (グローバルのグラフ) で x16 / x32 が「x...」と省略されていた。
    std::array<int, std::tuple_size_v<decltype(zoomButtons)>> widths{};

    int needed = 0;
    int preferred = 0;

    for (size_t i = 0; i < zoomButtons.size(); ++i) {
        auto& b = zoomButtons[i];
        const auto font = b.getLookAndFeel().getTextButtonFont(b, zoomRowHeight);

        widths[i] = (int)std::ceil(juce::GlyphArrangement::getStringWidth(font, b.getButtonText())) + zoomButtonTextPadding;

        needed += widths[i];
        preferred += juce::jmax(widths[i], zoomButtonWidth);
    }

    const int room = juce::jmax(0, row.getWidth() - zoomSliderMinWidth);

    // 場所があれば揃いの幅にする。無ければ文字が収まる幅、それでも足りなければ縮める
    if (preferred <= room) {
        for (auto& w : widths) w = juce::jmax(w, zoomButtonWidth);
    }
    else if (needed > room && needed > 0) {
        for (auto& w : widths) w = w * room / needed;
    }

    int total = 0;

    for (auto w : widths) total += w;

    auto buttons = row.removeFromRight(total);

    zoomSlider.setBounds(row.reduced(4, 0));

    for (size_t i = 0; i < zoomButtons.size(); ++i) zoomButtons[i].setBounds(buttons.removeFromLeft(widths[i]).reduced(1, 0));
}

void GuiEnvelopeGraph::syncScrollBar()
{
    if (!zoomable) return;

    const juce::ScopedValueSetter<bool> guard(isSyncingScrollBar, true);

    const double view = juce::jmax(1.0f, lastViewW);
    const double total = view * zoom;

    scrollBar.setRangeLimits(0.0, total, juce::dontSendNotification);
    scrollBar.setCurrentRange((total - view) * scroll, view, juce::dontSendNotification);
}

void GuiEnvelopeGraph::scrollBarMoved(juce::ScrollBar*, double newRangeStart)
{
    if (isSyncingScrollBar) return;

    const float over = overflow();

    scroll = over > 0.0f ? juce::jlimit(0.0f, 1.0f, (float)newRangeStart / over) : 0.0f;

    repaint();
}

void GuiEnvelopeGraph::mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel)
{
    if (!zoomable || !graphBounds().contains(e.getPosition())) {
        juce::Component::mouseWheelMove(e, wheel);
        return;
    }

    setZoom(zoom * std::pow(2.0f, wheel.deltaY * 2.0f));
}

void GuiEnvelopeGraph::mouseDown(const juce::MouseEvent& e)
{
    isScrollDrag = zoomable && e.mods.isRightButtonDown();

    if (isScrollDrag) {
        dragStartX = (float)e.position.x;
        dragStartScroll = scroll;
    }
}

void GuiEnvelopeGraph::mouseDrag(const juce::MouseEvent& e)
{
    if (!isScrollDrag) return;

    const float over = overflow();

    if (over <= 0.0f) return;

    scroll = juce::jlimit(0.0f, 1.0f, dragStartScroll - ((float)e.position.x - dragStartX) / over);

    repaint();
}

void GuiEnvelopeGraph::paint(juce::Graphics& g)
{
    auto bounds = graphBounds().toFloat();
    auto graphArea = bounds.reduced(5.0f);

    // 背景とグリッド
    g.setColour(juce::Colours::black.withAlpha(0.6f));
    g.fillRoundedRectangle(bounds, 5.0f);
    g.setColour(juce::Colours::grey.withAlpha(0.3f));
    g.drawRect(graphArea, 1.0f);

    if (!isEnabled()) {
        g.setColour(juce::Colours::white.withAlpha(0.8f));
        g.setFont(14.0f);
        g.drawText(I18n::pick(u8"利用不可", u8"Not available"), graphArea.withTrimmedRight(4).withTrimmedBottom(2), juce::Justification::bottomRight, false);

        return;
    }

    bool isPitch = currentType == EnvType::Pitch || currentType == EnvType::SsgSwP256;

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

    // T SYNC の刻みを描くのに使ったテンポ (4.3.0)
    if (tempoText.isNotEmpty()) {
        g.setColour(GuiColor::EnvelopeGraph::TempoText);
        g.setFont(11.0f);
        g.drawText(tempoText, graphArea.withTrimmedRight(4).withTrimmedTop(2), juce::Justification::topRight, false);
    }

    if (isBypass) {
        g.setColour(juce::Colours::white.withAlpha(0.8f));
        g.setFont(14.0f);
        g.drawText(I18n::pick(u8"バイパス中", u8"Bypassed"), graphArea.withTrimmedRight(4).withTrimmedBottom(2), juce::Justification::bottomRight, false);

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

    // 横方向の拡大。広げた分は送り具合で左右へずらし、枠の外は描かない。
    lastViewW = graphArea.getWidth();
    scaleX *= zoom;

    const float offsetX = overflow() * scroll;

    syncScrollBar();

    juce::Graphics::ScopedSaveState clipState(g);
    g.reduceClipRegion(graphArea.toNearestInt());

    float currentPixelX = graphArea.getX() - offsetX;
    float basePixelY = isPitch ? graphArea.getCentreY() : graphArea.getBottom();
    float heightScale = isPitch ? (graphArea.getHeight() / 2.0f) : graphArea.getHeight();

    bool hasMax = false;
    float maxX = 0.0f;
    float maxY = 0.0f;
    juce::Colour maxColour = juce::Colours::white;

    // 鳴っている位置。印の付いた最初の段に引く (ループで戻っても同じ段の上)
    bool hasLive = false;
    float liveX = 0.0f;

    // サスティンの区間 (保っているあいだは、線ではなく区間ごと塗る)
    float liveStartX = 0.0f;
    float liveWidth = 0.0f;

    for (const auto& phase : currentPhases) {
        if (phase.widthPx <= 0.0f && !phase.isMax) continue;

        // ジャンプ指定がある場合、X座標を移動させて新しい線を引く準備をする
        if (phase.moveToStart) {
            currentPixelX = graphArea.getX() - offsetX + (phase.startXOffsetPx * scaleX);
        }

        if (phase.isMax) {
            hasMax = true;
            maxX = currentPixelX;
            maxY = basePixelY - (phase.startLevel * heightScale);
            maxColour = phase.color;

            continue;
        }

        float actualWidth = phase.widthPx * scaleX;

        // 有限でない幅は描かない。無限や NaN の座標を点線にすると、
        // 長さを刻む処理が終わらなくなり、画面ごと固まってしまう。
        if (!std::isfinite(actualWidth)) continue;

        if (!hasLive && liveStep != LivePosition::idle && phase.liveStep == liveStep) {
            hasLive = true;
            liveX = currentPixelX + juce::jlimit(0.0f, 1.0f, liveFrac) * actualWidth;
            liveStartX = currentPixelX;
            liveWidth = actualWidth;
        }

        juce::Path envPath;
        float startY = basePixelY - (phase.startLevel * heightScale);

        if (!std::isfinite(startY)) continue;

        envPath.startNewSubPath(currentPixelX, startY);

        // 刻みで動く RATE の段 (4.3.0)。刻みごとに横へ引いてから縦に動かす (階段)。
        // 刻みが多すぎるときは、ふつうの線として描く
        if (phase.stairPeriod > 0.0f && phase.stairPeriod * 4096.0f >= 1.0f && phase.endLevel != phase.startLevel) {
            auto levelAt = [&phase](float t) {
                return phase.startLevel + (phase.endLevel - phase.startLevel) * phase.curveFunc(juce::jmin(1.0f, t));
            };

            float prevY = startY;

            for (int k = 1;; ++k) {
                const float t = (float)k * phase.stairPeriod;

                if (t >= 1.0f - 1.0e-4f) {
                    envPath.lineTo(currentPixelX + actualWidth, prevY);
                    envPath.lineTo(currentPixelX + actualWidth, basePixelY - phase.endLevel * heightScale);
                    break;
                }

                const float px = currentPixelX + t * actualWidth;

                // 刻みの位置ちょうどで引くと、丸めでその刻みを取りこぼすことがあるので、少し後ろで引く
                const float py = basePixelY - levelAt(t + phase.stairPeriod * 1.0e-3f) * heightScale;

                envPath.lineTo(px, prevY);
                envPath.lineTo(px, py);
                prevY = py;
            }

            g.setColour(phase.color);
            g.strokePath(envPath, juce::PathStrokeType(2.0f));

            currentPixelX += actualWidth;

            if (phase.drawPhaseLine) {
                g.setColour(phase.phaseLineColor.withAlpha(0.3f));
                g.drawLine(currentPixelX, graphArea.getY(), currentPixelX, graphArea.getBottom(), 2.0f);
            }

            continue;
        }

        int resolution = std::max(10, (int)actualWidth / 2);

        float yStart = phase.curveFunc(phase.curveStartX);
        float yEnd = phase.curveFunc(phase.curveEndX);

        // カーブが極端な値を返すことがある。そのまま使うと座標が
        // 無限になるので、その場合は直線として描く。
        if (!std::isfinite(yStart) || !std::isfinite(yEnd)) {
            yStart = 0.0f;
            yEnd = 0.0f;
        }

        float yRange = yEnd - yStart;

        for (int i = 1; i <= resolution; ++i) {
            float localProgress = (float)i / resolution; // 現在のフェーズ内での進行度 (0.0 〜 1.0)

            // 1. 本来のカーブX座標を算出 (0.5〜0.7など)
            float curveX = phase.curveStartX + localProgress * (phase.curveEndX - phase.curveStartX);

            // 2. カーブ関数を通した生のY値を取得
            float rawY = phase.curveFunc(curveX);

            if (std::isnan(rawY)) {
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

            if (!std::isfinite(px) || !std::isfinite(py)) continue;

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

    // START NOW (4.3.0)。押したときにまだ走っていれば STL からは始まらないので、STL の点を中抜きの丸にする
    if (hollowStart) {
        const auto& first = currentPhases.front();
        constexpr float radius = 4.0f;

        // 左端ちょうどに置くと枠で半分切れるので、半径ぶん内へ寄せる
        const float x = graphArea.getX() - offsetX + radius + 1.0f;
        const float y = basePixelY - first.startLevel * heightScale;

        if (std::isfinite(y)) {
            const juce::Rectangle<float> dot(x - radius, y - radius, radius * 2.0f, radius * 2.0f);

            g.setColour(GuiColor::EnvelopeGraph::MarkerFill);
            g.fillEllipse(dot);
            g.setColour(first.color);
            g.drawEllipse(dot, 1.5f);
        }
    }

    // 保っているあいだ (サスティン) は、区間を半透明の黄色で塗る。ほかは縦線
    if (hasLive && liveStep == LivePosition::sustain && std::isfinite(liveStartX) && std::isfinite(liveWidth)) {
        g.setColour(GuiColor::EnvelopeGraph::LiveFill);
        g.fillRect(juce::Rectangle<float>(liveStartX, graphArea.getY(), juce::jmax(2.0f, liveWidth), graphArea.getHeight()));
    }
    else if (hasLive && std::isfinite(liveX)) {
        g.setColour(GuiColor::EnvelopeGraph::LiveLine);
        g.drawLine(liveX, graphArea.getY(), liveX, graphArea.getBottom(), 2.0f);
    }
}

void GuiEnvelopeGraph::appendEndLevel(std::vector<PhaseDef>& phases, float releaseEnd, float endl)
{
    // 走り終えたところで ENDL へ移る (ほぼ縦の線)
    phases.push_back({
        .widthPx = 0.01f,
        .startLevel = releaseEnd,
        .endLevel = endl,
        .color = GuiColor::EnvelopeGraph::ReleaseLine,
        .drawPhaseLine = false,
        .keepShape = true
        });

    // ENDL を保つ
    phases.push_back({
        .widthPx = 40.0f,
        .startLevel = endl,
        .endLevel = endl,
        .isDashed = true,
        .color = GuiColor::EnvelopeGraph::ReleaseLine,
        .drawPhaseLine = false,
        .keepShape = true
        });
}

void GuiEnvelopeGraph::updatePitchEnv(
    const PitchEnvValues& v,
    CurveCore* p_curveCore,
    bool isCurveMode,
    int posIdx
) {
    // -------------------------------------------------------------
    // Helper: カーブ関数を生成する
    // -------------------------------------------------------------
    auto getCurveFunc = [this, isCurveMode, p_curveCore](int posIdx, int targetIdx, int prmIdx) {
        return [this, isCurveMode, posIdx, targetIdx, prmIdx, p_curveCore](float progress) -> float {
            if (!isCurveMode || p_curveCore == nullptr) return progress;
            return p_curveCore->process(posIdx, targetIdx, prmIdx, progress);
            };
        };

    // -------------------------------------------------------------
    // Helper: 幅の計算
    // -------------------------------------------------------------
    auto rateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
        if (maxRate <= 0.0001f) return 2.0f;

        if (rateValue <= 0.001f) return 2.0f; // 0なら左に詰める(一瞬)

        return (rateValue / maxRate) * maxWidth;
        };

    float ar = v.ar;
    float dr = v.dr;
    float rr = v.rr;

    float arMax = v.arMax;
    float drMax = v.drMax;
    float rrMax = v.rrMax;

    const float maxCents = 4800.0f; // 仮の最大値
    float stl = v.stl / maxCents;
    float atl = v.atl / maxCents;
    float ssl = v.ssl / maxCents;
    float rll = v.rll / maxCents;

    std::vector<GuiEnvelopeGraph::PhaseDef> phases;
    juce::Colour color = GuiColor::EnvelopeGraph::PitchLine;
    int targetIdx = (int)CurveParams::Target::PitchEnv;

    // Attack
    phases.push_back({
        .widthPx = rateToWidth(ar, arMax),
        .startLevel = stl, .endLevel = atl, .color = color,
        .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetPitchEnv::Ar),
        .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine,
        .liveStep = LivePosition::attack
        });

    // Decay
    phases.push_back({
        .widthPx = rateToWidth(dr, drMax),
        .startLevel = atl, .endLevel = ssl, .color = color,
        .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetPitchEnv::Dr),
        .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine,
        .liveStep = LivePosition::decay
        });

    // Sustain (決まった幅・別の色)
    phases.push_back({ .widthPx = sustainWidthPx, .startLevel = ssl, .endLevel = ssl, .color = GuiColor::EnvelopeGraph::SustainLine, .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine, .liveStep = LivePosition::sustain });

    // Release
    phases.push_back({
        .widthPx = rateToWidth(rr, rrMax),
        .startLevel = ssl, .endLevel = rll, .color = color,
        .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetPitchEnv::Rr),
        .liveStep = LivePosition::release,
        .isRelease = true
        });

    if (v.endlEnable) appendEndLevel(phases, rll, v.endl / maxCents);

    setEnvelope(GuiEnvelopeGraph::EnvType::Pitch, "Pitch Env", phases);
}

void GuiEnvelopeGraph::updateSsgSwEnv256(const StepEnvValues& v, CurveCore* p_curveCore, bool isCurveMode, int posIdx)
{
    updateStepEnv(GuiEnvelopeGraph::EnvType::SsgSw256, "SSG SW Env256", GuiColor::EnvelopeGraph::AmpLine,
        (int)CurveParams::Target::SsgSwEnv256, v, p_curveCore, isCurveMode, posIdx);
}

void GuiEnvelopeGraph::updateSsgSwPEnv256(const StepEnvValues& v, CurveCore* p_curveCore, bool isCurveMode, int posIdx)
{
    updateStepEnv(GuiEnvelopeGraph::EnvType::SsgSwP256, "SSG SW PitchEnv256", GuiColor::EnvelopeGraph::PitchLine,
        (int)CurveParams::Target::SsgSwPEnv256, v, p_curveCore, isCurveMode, posIdx);
}

void GuiEnvelopeGraph::updateParamEnv256(const StepEnvValues& v, const juce::String& title)
{
    updateStepEnv(GuiEnvelopeGraph::EnvType::ParamEnv256, title, GuiColor::EnvelopeGraph::ParamLine, -1, v, nullptr, false, 0);
}

// 段のエンベロープの絵。
//
// 段の幅は R に比例させる。段が多いと全体が長くなるが、絵の幅に収めるのは
// setEnvelope の側 (全体を縮めて入れる)。カーブは段 1〜10 が R1〜R10、
// 11 段目からは R10、リリースが R11、ループで戻る段が LoopTo を使う。
void GuiEnvelopeGraph::updateStepEnv(EnvType type, const juce::String& title, juce::Colour color, int targetIdx,
    const StepEnvValues& v, CurveCore* p_curveCore, bool isCurveMode, int posIdx)
{
    auto getCurveFunc = [isCurveMode, p_curveCore, posIdx, targetIdx](int prmIdx) {
        return [isCurveMode, posIdx, targetIdx, prmIdx, p_curveCore](float progress) -> float {
            if (!isCurveMode || p_curveCore == nullptr) return progress;
            return p_curveCore->process(posIdx, targetIdx, prmIdx, progress);
            };
        };

    auto rateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
        if (maxRate <= 0.0001f) return 2.0f;

        if (rateValue <= 0.001f) return 2.0f; // 0なら左に詰める(一瞬)

        return (rateValue / maxRate) * maxWidth;
        };

    constexpr int curveStepParams = 10;
    constexpr int curveRelease = 10;
    constexpr int curveLoopTo = 11;

    const int steps = juce::jlimit(1, (int)v.l.size() - 1, v.head.steps);
    const bool isLoop = v.head.loop;
    const int loopTo = juce::jlimit(0, steps - 1, v.head.loopTo);
    const int loopCount = v.head.loopCount;
    const float lMax = v.lMax <= 0.0f ? 1.0f : v.lMax;

    auto levelAt = [&](int i) { return v.l[(size_t)juce::jlimit(0, (int)v.l.size() - 1, i)] / lMax; };
    auto rateAt = [&](int i) { return v.r[(size_t)juce::jlimit(0, (int)v.r.size() - 1, i)]; };
    auto rateStepAt = [&](int s) { return s >= 1 && s < (int)v.rates.size() ? v.rates[(size_t)s] : StepRate::Step{}; };

    // RATE の段 (4.3.0)。長さと終わりの値は、音の側と同じ式 (StepRate) で start から出す。
    // 値は絵の値 (lMax で割ったもの) なので、D も同じく割る。刻みは階段で描く。
    // 動かず打ち切りもしない段は終わらないので、破線を引いて endless を立てる
    auto ratePhase = [&](const StepRate::Step& st, float start, float target, float limit, PhaseDef phase, bool& endless) {
        const double tick = StepRate::tickSeconds(st, v.tickSync, v.bpm);
        const float d = st.delta / lMax;
        const auto seg = StepRate::segment(start, target, d, tick, limit);

        endless = seg.endless;
        phase.startLevel = start;

        if (seg.endless) {
            phase.widthPx = 30.0f;
            phase.endLevel = start;
            phase.isDashed = true;
            return phase;
        }

        const double dur = seg.seconds;
        const float end = seg.end;

        phase.widthPx = rateToWidth((float)dur, v.rMax);
        phase.endLevel = end;
        phase.curveFunc = [start, target, d, tick, dur, end](float x) {
            if (end == start) return x;

            return (StepRate::valueAt(start, target, d, tick, (double)x * dur) - start) / (end - start);
        };

        if (tick > 0.0 && dur > 0.0) phase.stairPeriod = (float)(tick / dur);

        return phase;
    };

    // 段 s を start から target へ進める区間を足す。終わったときの値を返す
    auto addStep = [&](std::vector<PhaseDef>& phases, int s, float start, float target, int prmIdx, juce::Colour col, int liveStep, bool& endless) {
        PhaseDef phase{
            .widthPx = rateToWidth(rateAt(s), v.rMax), .startLevel = start, .endLevel = target, .color = col,
            .curveFunc = getCurveFunc(prmIdx),
            .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine,
            .liveStep = liveStep
        };

        endless = false;

        if (const auto st = rateStepAt(s); st.rate) phase = ratePhase(st, start, target, rateAt(s), phase, endless);

        phases.push_back(phase);

        return phase.endLevel;
    };

    // リリースの区間を足す。走り終えたときの値を返す
    auto addRelease = [&](std::vector<PhaseDef>& phases, float start, bool moveToStart, float startXOffsetPx) {
        PhaseDef phase{
            .widthPx = rateToWidth(v.rr, v.rMax),
            .startLevel = start,
            .endLevel = v.lr / lMax,
            .color = GuiColor::EnvelopeGraph::ReleaseLine,
            .curveFunc = getCurveFunc(curveRelease),
            .moveToStart = moveToStart,
            .startXOffsetPx = startXOffsetPx,
            .liveStep = LivePosition::release,
            .isRelease = true
        };

        // RATE のリリース。D が 0 なら音の側も TIME で進める
        if (v.releaseRateStep.rate && v.releaseRateStep.delta > 0.0f) {
            bool endless = false;

            phase = ratePhase(v.releaseRateStep, start, v.lr / lMax, v.rr, phase, endless);
        }

        phases.push_back(phase);

        return phase.endLevel;
    };

    std::vector<GuiEnvelopeGraph::PhaseDef> phases;

    // Rich Mode。ループの組で実際に通る段の順を、音の側と同じ決まりで並べる。
    // 無限のループで戻るところまで来たら、戻る先へ破線を引いてそこで止める
    // (Original Mode の無限ループと同じ見せ方)。回数が多すぎるときも同じ。
    if (isLoop && v.head.loopMode == StepLoop::Rich) {
        constexpr int maxPhases = 1024;

        const auto& loops = v.head.loops;
        const int num = (int)loops.size();

        StepLoop::State state;

        int s = 1;
        int visited = 0;
        float level = levelAt(0);
        bool endless = false;

        while (s <= steps) {
            level = addStep(phases, s, level, levelAt(s), std::min(s, curveStepParams) - 1, color, s, endless);

            // 動かず打ち切りもしない RATE の段。離すまでここにいる
            if (endless) break;

            const int nextStep = StepLoop::next(loops, num, steps, state, s);

            // 無限のループで戻るところか
            bool infinite = false;

            for (const auto& e : loops) {
                if (e.count == 0 && e.end == s && e.start == nextStep && StepLoop::isValid(e, steps)) infinite = true;
            }

            if (infinite || ++visited >= maxPhases) {
                phases.push_back({
                    .widthPx = 30.0f, .startLevel = level, .endLevel = levelAt(nextStep), .isDashed = true,
                    .color = color, .drawPhaseLine = false
                    });

                level = levelAt(nextStep);
                endless = true;
                break;
            }

            s = nextStep;
        }

        // 最後の段を走り終えたら、離すまで保つ (サスティン。決まった幅・別の色)
        if (!endless) {
            phases.push_back({
                .widthPx = sustainWidthPx, .startLevel = level, .endLevel = level,
                .color = GuiColor::EnvelopeGraph::SustainLine,
                .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine,
                .liveStep = LivePosition::sustain
                });
        }

        const float releaseEnd = addRelease(phases, level, false, 0.0f);

        if (v.endlEnable) appendEndLevel(phases, releaseEnd, v.endl / lMax);

        setStepEnvelope(type, title, phases, v);
        return;
    }

    float currentTotalWidth = 0.0f;
    const float loopToLevel = isLoop ? levelAt(loopTo) : 0.0f;
    float loopEndWidth = 0.0f;

    // 段の頭の値。TIME の段は行き先に着いて終わるが、RATE の段は打ち切られると手前で終わる
    float level = levelAt(0);

    // ループで戻る段を走り終えたときの値 (無限のループなら、離すとここから)
    float loopBackLevel = loopToLevel;
    bool endless = false;

    for (int s = 1; s <= steps; ++s) {
        const float startL = level;
        const int prmIdx = (isLoop && s == steps) ? curveLoopTo : std::min(s, curveStepParams) - 1;

        if (isLoop && s == steps) {
            loopEndWidth = currentTotalWidth;

            loopBackLevel = addStep(phases, s, startL, loopToLevel, prmIdx, color, s, endless);

            const float width = phases.back().widthPx;

            if (loopCount > 0 && !endless) {
                phases.push_back({
                    .widthPx = 30.0f,
                    .startLevel = loopBackLevel,
                    .endLevel = loopBackLevel,
                    .isDashed = true,
                    .color = color,
                    .drawPhaseLine = false
                    });

                // 回り終えたあとの最後の段。頭の値は 1 回目と同じとして描く
                bool lastEndless = false;

                level = addStep(phases, s, startL, levelAt(s), prmIdx, color.darker(0.3f), LivePosition::idle, lastEndless);
                phases.back().moveToStart = true;
                phases.back().startXOffsetPx = loopEndWidth;

                // 回り終えたら、離すまで最後の段を保つ (サスティン)
                if (!lastEndless) {
                    phases.push_back({
                        .widthPx = sustainWidthPx, .startLevel = level, .endLevel = level,
                        .color = GuiColor::EnvelopeGraph::SustainLine,
                        .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine,
                        .liveStep = LivePosition::sustain
                        });
                }
            }
            else {
                level = loopBackLevel;
            }

            loopEndWidth += width;

            if (loopCount > 0) loopEndWidth += sustainWidthPx;

            currentTotalWidth += width;
        }
        else {
            level = addStep(phases, s, startL, levelAt(s), prmIdx, color, s, endless);

            currentTotalWidth += phases.back().widthPx;
        }

        // 動かず打ち切りもしない RATE の段。離すまでここにいる
        if (endless) break;
    }

    // ループしないときは、最後の段を走り終えたら離すまで保つ (サスティン。決まった幅・別の色)
    if (!isLoop && !endless) {
        phases.push_back({
            .widthPx = sustainWidthPx, .startLevel = level, .endLevel = level,
            .color = GuiColor::EnvelopeGraph::SustainLine,
            .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine,
            .liveStep = LivePosition::sustain
            });
    }

    // Release。ループの段まで来ずに止まったときは、止まった値から続けて描く
    const bool jump = isLoop && !endless;
    const float releaseStart = jump && loopCount == 0 ? loopBackLevel : level;
    const float releaseEnd = addRelease(phases, releaseStart, jump, loopEndWidth);

    if (v.endlEnable) appendEndLevel(phases, releaseEnd, v.endl / lMax);

    setStepEnvelope(type, title, phases, v);
}

// 段のエンベロープの絵を置く。START NOW なら STL の点を中抜きの丸に、T SYNC で音符の
// 刻みを使っていれば、計算に使ったテンポを右上に出す (どちらも setEnvelope が外す)
void GuiEnvelopeGraph::setStepEnvelope(EnvType type, const juce::String& title, const std::vector<PhaseDef>& phases, const StepEnvValues& v)
{
    setEnvelope(type, title, phases);

    hollowStart = v.startNow;

    bool usesNotes = false;

    if (v.tickSync) {
        const int steps = juce::jmin(v.head.steps, (int)v.rates.size() - 1);

        for (int s = 1; s <= steps; ++s) usesNotes = usesNotes || (v.rates[(size_t)s].rate && v.rates[(size_t)s].tickNote > 0);

        usesNotes = usesNotes || (v.releaseRateStep.rate && v.releaseRateStep.tickNote > 0);
    }

    tempoText = usesNotes ? juce::String(v.bpm, 1) + " BPM" : juce::String();
}

void GuiEnvelopeGraph::updateAmpEnv(
    const AmpEnvValues& v,
    CurveCore* p_curveCore,
    bool isCurveMode,
    int posIdx
) {
    // -------------------------------------------------------------
    // Helper: カーブ関数を生成する
    // -------------------------------------------------------------
    auto getCurveFunc = [this, isCurveMode, p_curveCore](int posIdx, int targetIdx, int prmIdx) {
        return [this, isCurveMode, posIdx, targetIdx, prmIdx, p_curveCore](float progress) -> float {
            if (!isCurveMode || p_curveCore == nullptr) return progress;
            return p_curveCore->process(posIdx, targetIdx, prmIdx, progress);
            };
        };

    // -------------------------------------------------------------
    // Helper: 幅の計算
    // -------------------------------------------------------------
    auto rateToWidth = [](float rateValue, float maxRate, float maxWidth = 150.0f) {
        if (maxRate <= 0.0001f) return maxWidth;

        if (rateValue <= 0.0f) return maxWidth;

        float norm = rateValue / maxRate;

        return std::max(2.0f, maxWidth * norm);
        };

    float stlMax = v.stlMax;
    float arMax = v.arMax;
    float drMax = v.drMax;
    float slMax = v.slMax;
    float rrMax = v.rrMax;

    float stlVal = v.stl;
    float arVal = v.ar;
    float drVal = v.dr;
    float slVal = v.sl;
    float rrVal = v.rr;

    float sl = slVal / slMax;

    bool isKor = v.kor;

    std::vector<GuiEnvelopeGraph::PhaseDef> phases;
    juce::Colour color = GuiColor::EnvelopeGraph::AmpLine;
    int targetIdx = (int)CurveParams::Target::AmpEnv; // または RegValue

    float currentTotalWidth = 0.0f;

    // 1. Attack
    float attackWidth = rateToWidth(arVal, arMax);
    phases.push_back({
        .widthPx = attackWidth, .startLevel = stlVal / stlMax, .endLevel = 1.0f, .color = color,
        .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Ar),
        .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine,
        .liveStep = LivePosition::attack
        });
    currentTotalWidth += attackWidth;

    // 2. Decay 1
    float decayWidth = rateToWidth(drVal, drMax);
    phases.push_back({
        .widthPx = decayWidth, .startLevel = 1.0f, .endLevel = sl, .color = color,
        .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Dr),
        .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine,
        .liveStep = LivePosition::decay
        });
    currentTotalWidth += decayWidth;

    // 3. Sustain (決まった幅・別の色)
    float keyOnWidth = sustainWidthPx;

    phases.push_back({ .widthPx = keyOnWidth, .startLevel = sl, .endLevel = sl, .color = GuiColor::EnvelopeGraph::SustainLine, .phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine, .liveStep = LivePosition::sustain });

    currentTotalWidth += keyOnWidth;
    float noteOffPositionX = currentTotalWidth;

    // 4. Release (通常時のみカーブを適用)
    if (isKor) {
        // korでは、rrが終わるまでslをキープ
        phases.push_back({
            .widthPx = rateToWidth(rrVal, rrMax),
            .startLevel = sl,
            .endLevel = sl,
            .color = GuiColor::EnvelopeGraph::ReleaseLine,
            .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Rr),
            .liveStep = LivePosition::release,
            .isRelease = true
            });
    }
    else {
        phases.push_back({
            .widthPx = rateToWidth(rrVal, rrMax),
            .startLevel = sl,
            .endLevel = 0.0f,
            .color = color,
            .curveFunc = getCurveFunc(posIdx, targetIdx, (int)CurveParams::TargetAmpEnv::Rr),
            .liveStep = LivePosition::release,
            .isRelease = true
            });
    }

    if (v.endlEnable) appendEndLevel(phases, isKor ? sl : 0.0f, v.endl / slMax);

    setEnvelope(GuiEnvelopeGraph::EnvType::Amp, "Amp Env", phases);
}
