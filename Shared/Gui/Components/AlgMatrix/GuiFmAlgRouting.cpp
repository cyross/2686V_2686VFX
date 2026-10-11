#include "./GuiFmAlgRouting.h"

#include "../../../Core/Io/ParamFile.h"

static const juce::Colour crrColor = juce::Colours::cyan.withAlpha(0.8f);
static const juce::Colour modColor = juce::Colours::white.withAlpha(0.8f);
static const juce::Colour disabledModColor = juce::Colours::grey.withAlpha(0.25f);
static const juce::Colour fbModColor = juce::Colours::orange;
static const juce::Colour permanentDisabledModColor = juce::Colours::black.withAlpha(0.6f);

static const int rectRadius = 2;
static const int chkW = 10;
static const int chkH = 10;
static const int margin = 2;
static const int cellW = chkW + margin * 2;
static const int cellH = chkH + margin * 2;
static const int opLabelW = 28;
static const int labelH = 12;

// ルーティングとフィードバックを切り替えるスイッチ
static const int switchW = 84;
static const int switchH = 16;
static const int switchGapH = 2;

// オペレータのテーマカラー（最大8色）
// OP8 は地の暗さに沈んで番号が読みにくかったので、白へ 0.3 寄せてある。
static const std::array<juce::Colour, 8> opColors = {
    juce::Colours::red.brighter(0.2f), juce::Colours::orange.brighter(0.2f),
    juce::Colours::yellow.brighter(0.2f), juce::Colours::green.brighter(0.2f),
    juce::Colours::cyan.brighter(0.2f), juce::Colours::dodgerblue.brighter(0.2f),
    juce::Colours::magenta.brighter(0.2f), juce::Colour(0xffe066ff).interpolatedWith(juce::Colours::white, 0.3f)
};

// ==============================================================================
// アルゴリズム図の寸法
// ==============================================================================
// 3.6.3 で全体を詰めた。オペレータの箱を上下左右 2px ずつ縮め、そのぶん
// 段と段、横に並ぶ箱どうしの間も縮めてある。
namespace AlgGraphSize
{
    // オペレータの箱の半分の大きさ (箱は 10 x 10)
    static constexpr float opHalf = 5.0f;

    // 段の間隔と、横に並ぶ箱の最小の間隔
    static constexpr float yStep = 24.0f;
    static constexpr float minSpacing = 28.0f;

    // 一番下の段 (キャリア) から下端までと、一番上の段から上端まで。
    // 下はキャリアから出る矢印の分、上は自分自身へのフィードバックの輪の分。
    static constexpr float bottomSpace = 16.0f;
    static constexpr float topSpace = 12.0f;

    // 横に並べるときに左右の端から空ける幅
    static constexpr float sideSpace = 15.0f;

    // 自分自身へのフィードバックの輪の半径
    static constexpr float selfFbRadius = 4.0f;

    // 他のオペレータへのフィードバックが横へ出る長さ。つなぐ先が
    // 離れているほど外へ出して、線どうしが重ならないようにする。
    static constexpr float fbRunBase = 4.0f;
    static constexpr float fbRunPerOp = 3.5f;
}

int GuiFmAlgGraph::preferredHeight(int numOps, int margin)
{
    const int maxDepth = juce::jmax(0, numOps - 1);

    return (int)std::ceil(AlgGraphSize::bottomSpace + AlgGraphSize::topSpace
        + maxDepth * AlgGraphSize::yStep) + margin * 2;
}

// ==============================================================================
// GuiFmAlgGraph の実装
// ==============================================================================
void GuiFmAlgGraph::paint(juce::Graphics& g) {
    namespace S = AlgGraphSize;

    g.fillAll(juce::Colours::black.withAlpha(0.3f));

    std::array<juce::Point<float>, 8> pos;
    std::array<int, 8> depths;
    depths.fill(-1);

    // 1. キャリア（出力ノード）を Depth 0 とする
    for (int i = 0; i < state.numOps; ++i) {
        if (state.isCarrier[i]) depths[i] = 0;
    }

    // 2. 出力から逆に辿って（dest -> src）深さを計算する
    bool changed = true;
    int maxIter = 10;
    while (changed && maxIter-- > 0) {
        changed = false;
        for (int dest = 0; dest < state.numOps; ++dest) {
            if (depths[dest] == -1) continue; // 自分がまだ未到達ならパス

            for (int src = 0; src < state.numOps; ++src) {
                // srcからdestへのモジュレーションがある場合、srcの深さはdestより1つ上
                if (state.mod[src][dest]) {
                    int newDepth = depths[dest] + 1;
                    // より深い(上の)階層に更新できる場合のみ更新
                    if (depths[src] < newDepth) {
                        depths[src] = newDepth;
                        changed = true;
                    }
                }
            }
        }
    }

    // 3. フィードバック「のみ」で繋がっている孤立ノードや、
    // まだ配置が決まっていない有効ノード（M->Mなどの特殊ケース）を最上段に配置する
    auto activeOps = state.getActiveOperators();
    int maxCurrentDepth = 0;
    for (int i = 0; i < state.numOps; ++i) {
        if (depths[i] > maxCurrentDepth) maxCurrentDepth = depths[i];
    }

    for (int i = 0; i < state.numOps; ++i) {
        if (activeOps[i] && depths[i] == -1) {
            // 出力に繋がっているはずなのにDepthが決まらなかったノードは最上段+1へ
            depths[i] = maxCurrentDepth + 1;
        }
    }

    // 4. 深さごとにノードを振り分け
    std::vector<int> nodesAtDepth[8];
    int maxDepth = 0;
    for (int i = 0; i < state.numOps; ++i) {
        int d = depths[i];
        if (d >= 0 && d < state.numOps) {
            nodesAtDepth[d].push_back(i);
            maxDepth = std::max(maxDepth, d);
        }
        else {
            pos[i] = juce::Point<float>(-100, -100);
        }
    }

    float w = getWidth();
    float h = getHeight();
    float yStep = S::yStep;
    const float vSpace = S::bottomSpace + S::topSpace;

    // 階層が深い場合(直列8段など)、yStepを動的に縮小して画面内に収める
    if (maxDepth > 0) {
        float requiredHeight = maxDepth * yStep + vSpace;
        if (requiredHeight > h) {
            yStep = (h - vSpace) / maxDepth;
        }
    }

    // 5. 座標計算
    for (int d = 0; d < state.numOps; ++d) {
        int count = nodesAtDepth[d].size();
        if (count == 0) continue;

        float y = h - S::bottomSpace - d * yStep; // Depth 0 が一番下、数字が大きいほど上

        if (d == 0) {
            // キャリア(Depth 0)は画面幅に対して均等配置
            float spacing = w / (count + 1);
            for (int i = 0; i < count; ++i) {
                int opIdx = nodesAtDepth[d][i];
                pos[opIdx] = juce::Point<float>(spacing * (i + 1), y);
            }
        }
        else {
            // モジュレータは「接続先のX座標の平均値」をターゲット位置として配置する
            std::vector<std::pair<int, float>> targetXs;
            for (int i = 0; i < count; ++i) {
                int src = nodesAtDepth[d][i];
                float sumX = 0.0f;
                int destCount = 0;
                for (int dest = 0; dest < state.numOps; ++dest) {
                    // 自分より下の階層にある接続先を探す
                    if (state.mod[src][dest] && depths[dest] < d && depths[dest] != -1) {
                        sumX += pos[dest].x;
                        destCount++;
                    }
                }
                // 接続先がない場合(自己FB等)は中央をターゲットにする
                float tx = (destCount > 0) ? (sumX / destCount) : (w / 2.0f);
                targetXs.push_back({ src, tx });
            }

            // 重なりを防ぐため、ターゲットのX座標順でソート (同じ場合はOP番号順)
            std::stable_sort(targetXs.begin(), targetXs.end(), [](const auto& a, const auto& b) {
                if (a.second == b.second) return a.first < b.first;
                return a.second < b.second;
                });

            std::vector<float> finalX(count);
            for (int i = 0; i < count; ++i) finalX[i] = targetXs[i].second;

            // Relaxation(緩和)法で、ノード同士が近すぎる場合は左右に押し広げる
            for (int iter = 0; iter < 50; ++iter) {
                for (int i = 0; i < count - 1; ++i) {
                    float dist = finalX[i + 1] - finalX[i];
                    if (dist < S::minSpacing) {
                        float push = (S::minSpacing - dist) * 0.5f;
                        finalX[i] -= push;
                        finalX[i + 1] += push;
                    }
                }
                // 画面端からはみ出ないように制限
                for (int i = 0; i < count; ++i) {
                    if (finalX[i] < S::sideSpace) finalX[i] = S::sideSpace;
                    if (finalX[i] > w - S::sideSpace) finalX[i] = w - S::sideSpace;
                }
            }

            // 計算された最終的なX座標を適用
            for (int i = 0; i < count; ++i) {
                int opIdx = targetXs[i].first;
                pos[opIdx] = juce::Point<float>(finalX[i], y);
            }
        }
    }

    // 6. 線の描画
    g.setColour(modColor);
    for (int src = 0; src < state.numOps; ++src) {
        // アクティブでない（描画対象外の）ノードからの線は引かない
        if (depths[src] == -1) continue;

        // キャリア出力矢印
        if (state.isCarrier[src]) {
            g.setColour(crrColor);
            const float bottom = pos[src].y + S::opHalf;
            g.drawLine(pos[src].x, bottom, pos[src].x, bottom + 9.0f, 1.5f);
            juce::Path p;
            p.addTriangle(pos[src].x, bottom + 11.0f, pos[src].x - 3.0f, bottom + 7.0f, pos[src].x + 3.0f, bottom + 7.0f);
            g.fillPath(p);
        }

        for (int dest = 0; dest < state.numOps; ++dest) {
            if (depths[dest] == -1) continue;

            // 通常モジュレーション (カギ線/エルボ結線)
            if (state.mod[src][dest]) {
                g.setColour(modColor);

                // 中心座標からスタート
                float x1 = pos[src].x;
                float y1 = pos[src].y + S::opHalf; // srcの下端
                float x2 = pos[dest].x;
                float y2 = pos[dest].y - S::opHalf; // destの上端

                juce::Path p;
                p.startNewSubPath(x1, y1);

                // X軸が異なる（斜めになる）場合は、直角に曲がるパスを作成
                if (std::abs(x1 - x2) > 1.0f) {
                    // dest(接続先)の1つ上の層との「中間位置」を曲がるポイント(横方向のバス)とする
                    float midY = pos[dest].y - yStep * 0.5f;

                    // y1 と y2 が近すぎる場合の安全対策
                    if (midY < y1 + 2.0f) midY = y1 + (y2 - y1) * 0.5f;

                    // 下へ降りて、横へ移動
                    p.lineTo(x1, midY);
                    p.lineTo(x2, midY);
                }

                // 最後に接続先へ降りる
                p.lineTo(x2, y2);

                // 角を少しだけ丸めて回路図・ブロック図らしさを出す (Visio風)
                p = p.createPathWithRoundedCorners(3.0f);

                g.strokePath(p, juce::PathStrokeType(1.5f));
            }

            // フィードバックモジュレーション (破線 / 自己FB)
            //
            // 「FB」の文字は 3.6.3 でやめた。色と破線で見分けられ、
            // 文字の分だけ横に場所を取っていた。
            if (state.fbMod[src][dest]) {
                g.setColour(fbModColor);
                if (src == dest) {
                    // 輪の中心は箱の左上の角に置く
                    const float r = S::selfFbRadius;
                    const float cx = pos[src].x - S::opHalf;
                    const float cy = pos[src].y - S::opHalf;
                    g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 1.0f);
                }
                else {
                    const float sX = pos[src].x - S::opHalf; const float sY = pos[src].y;
                    const float eX = pos[dest].x - S::opHalf; const float eY = pos[dest].y;
                    const float mX = sX - (S::fbRunBase + std::abs(src - dest) * S::fbRunPerOp);
                    const float dashLengths[] = { 2.0f, 2.0f };
                    g.drawDashedLine(juce::Line<float>(sX, sY, mX, sY), dashLengths, 2, 1.0f);
                    g.drawDashedLine(juce::Line<float>(mX, sY, mX, eY), dashLengths, 2, 1.0f);
                    g.drawDashedLine(juce::Line<float>(mX, eY, eX, eY), dashLengths, 2, 1.0f);
                }
            }
        }
    }

    // 7. オペレータボックスの描画
    for (int i = 0; i < state.numOps; ++i) {
        if (depths[i] == -1) continue;
        const float size = S::opHalf * 2.0f;
        g.setColour(opColors[i]);
        g.fillRect(pos[i].x - S::opHalf, pos[i].y - S::opHalf, size, size);
        g.setColour(juce::Colours::black);
        g.setFont(8.0f);
        g.drawText(juce::String(i + 1), pos[i].x - S::opHalf, pos[i].y - S::opHalf, size, size, juce::Justification::centred);
    }
}

// ==============================================================================
// GuiFmAlgMatrix の実装
// ==============================================================================
GuiFmAlgMatrix::GuiFmAlgMatrix(const GuiContext& context, int ops)
    : GuiBaseComponent(context), numOps(ops), m_opReachable(ops, false),
    routingSw(context), feedbackSw(context)
{
    // 上にスイッチの行、その下に 1 つぶんのマス目。
    // ルーティングは「→2〜→N」と「OUT」、フィードバックは「→1〜→N」で、
    // どちらも numOps 行になる。
    gridW = opLabelW + numOps * cellW + rectRadius * 2;
    naturalW = juce::jmax(gridW, switchW * 2);
    gridX = (naturalW - gridW) / 2;
    gridStartY = switchH + switchGapH;
    gridChkStartY = gridStartY + rectRadius + labelH;
    gridTotalH = rectRadius + labelH + numOps * cellH + rectRadius;

    // ラジオボタンのように、どちらか一方だけが点く
    for (auto* sw : { &routingSw, &feedbackSw })
    {
        sw->setup({ .parent = *this, .title = (sw == &routingSw) ? "ROUTING" : "FEEDBACK",
            .font = juce::Font(juce::FontOptions(11.0f)) });
        sw->setRadioGroupId(1, juce::dontSendNotification);
        sw->onClick = [this] { repaint(); };
    }

    routingSw.setToggleState(true, juce::dontSendNotification);

    m_state.numOps = numOps;

    m_modEnabled.assign((size_t)numOps, std::vector<bool>((size_t)numOps, false));
    m_fbEnabled.assign((size_t)numOps, std::vector<bool>((size_t)numOps, false));

    updateValidity();
}

void GuiFmAlgMatrix::resized() {
    auto row = juce::Rectangle<int>(0, 0, naturalW, switchH).withSizeKeepingCentre(switchW * 2, switchH);

    routingSw.setBounds(row.removeFromLeft(switchW));
    feedbackSw.setBounds(row);
}

void GuiFmAlgMatrix::paint(juce::Graphics& g) {
    // マス目の四角。トグルを置く代わりにここで描く。
    // 枠と中のランプ、消えているときの薄さは、これまでのトグルと同じ出方にしてある。
    auto drawCell = [&g](int x, int y, bool on, bool enabled) {
        const float radius = juce::jmin(guiCornerRadius, juce::jmin((float)chkW, (float)chkH) * 0.5f);
        const float alpha = enabled ? 1.0f : 0.5f;

        juce::Rectangle<float> box((float)(x + margin), (float)(y + margin), (float)chkW, (float)chkH);

        g.setColour(GuiColor::ToggleButton::Box.get().withMultipliedAlpha(alpha));
        g.drawRoundedRectangle(box, radius, 1.0f);

        juce::Colour lamp = on ? GuiColor::ToggleButton::LampOn.get()
                               : GuiColor::ToggleButton::LampOff.get();

        g.setColour(lamp.withMultipliedAlpha(alpha));
        g.fillRoundedRectangle(box, radius);
        };

    const bool feedback = isFeedbackShown();
    const int labelX = gridX + rectRadius;
    const int chkStartX = labelX + opLabelW;
    const int labelW = opLabelW - margin;

    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.fillRoundedRectangle((float)gridX, (float)gridStartY, (float)gridW, (float)gridTotalH, (float)rectRadius);

    g.setFont(11.0f);

    // 列の見出し (つなぐ元のオペレータ)
    for (int i = 0; i < numOps; ++i) {
        g.setColour(opColors[i]);
        g.drawText(juce::String(i + 1), chkStartX + i * cellW, gridStartY + rectRadius, cellW, labelH, juce::Justification::centred);
    }

    if (!feedback) {
        for (int dest = 1; dest < numOps; ++dest) {
            int y = gridChkStartY + cellH * (dest - 1);
            for (int src = 0; src < numOps; ++src) {
                int x = chkStartX + src * cellW;
                const bool permDisabled = (src >= dest);
                const bool enabled = !permDisabled && m_modEnabled[(size_t)src][(size_t)dest];

                if (permDisabled) {
                    g.setColour(permanentDisabledModColor); g.fillRect(x, y, cellW, cellH);
                }
                else if (!enabled) {
                    g.setColour(disabledModColor); g.fillRect(x, y, cellW, cellH);
                }

                if (!permDisabled) drawCell(x, y, m_state.mod[(size_t)src][(size_t)dest], enabled);
            }

            g.setColour(opColors[dest]);
            g.drawText("->" + juce::String(dest + 1), labelX, y, labelW, cellH, juce::Justification::centredRight);
        }

        // 一番下の行は「出力へ出すか」。
        int outY = gridChkStartY + cellH * (numOps - 1);

        for (int src = 0; src < numOps; ++src) {
            drawCell(chkStartX + src * cellW, outY, m_state.isCarrier[(size_t)src], true);
        }

        g.setColour(juce::Colours::white);
        g.drawText("OUT", labelX, outY, labelW, cellH, juce::Justification::centredRight);

        return;
    }

    for (int dest = 0; dest < numOps; ++dest) {
        int y = gridChkStartY + cellH * dest;
        for (int src = 0; src < numOps; ++src) {
            int x = chkStartX + src * cellW;
            const bool permDisabled = (src < dest);
            const bool enabled = !permDisabled && m_fbEnabled[(size_t)src][(size_t)dest];

            if (permDisabled) {
                g.setColour(permanentDisabledModColor); g.fillRect(x, y, cellW, cellH);
            }
            else if (!enabled) {
                g.setColour(disabledModColor); g.fillRect(x, y, cellW, cellH);
            }

            if (!permDisabled) drawCell(x, y, m_state.fbMod[(size_t)src][(size_t)dest], enabled);
        }

        g.setColour(opColors[dest]);
        g.drawText("->" + juce::String(dest + 1), labelX, y, labelW, cellH, juce::Justification::centredRight);
    }
}

void GuiFmAlgMatrix::updateValidity() {
    std::vector<bool> canReach(numOps, false);
    for (int i = 0; i < numOps; ++i) canReach[i] = m_state.isCarrier[(size_t)i];

    for (int dest = numOps - 1; dest >= 1; --dest) {
        if (!canReach[dest]) continue;
        for (int src = dest - 1; src >= 0; --src) {
            if (m_state.mod[(size_t)src][(size_t)dest]) canReach[src] = true;
        }
    }

    auto isInSameChain = [&](int a, int b) {
        if (a == b) return true;
        std::vector<std::vector<bool>> undir(numOps, std::vector<bool>(numOps, false));
        for (int s = 0; s < numOps; ++s) {
            for (int d = 1; d < numOps; ++d) {
                if (m_state.mod[(size_t)s][(size_t)d]) {
                    undir[s][d] = true;
                    undir[d][s] = true;
                }
            }
        }
        std::vector<bool> visited(numOps, false);
        std::vector<int> q;
        q.push_back(a);
        visited[a] = true;
        while (!q.empty()) {
            int curr = q.back();
            q.pop_back();
            if (curr == b) return true;
            for (int next = 0; next < numOps; ++next) {
                if (undir[curr][next] && !visited[next]) {
                    visited[next] = true;
                    q.push_back(next);
                }
            }
        }
        return false;
        };

    // 通らなくなった経路は落とす
    for (int src = 0; src < numOps; ++src) {
        for (int dest = 1; dest < numOps; ++dest) {
            bool isModPermDisabled = (src >= dest);
            if (m_state.mod[(size_t)src][(size_t)dest] && (!canReach[dest] || isModPermDisabled)) {
                m_state.mod[(size_t)src][(size_t)dest] = false;
            }
        }
        for (int dest = 0; dest < numOps; ++dest) {
            bool isFbPermDisabled = (src < dest) || !isInSameChain(src, dest);
            if (m_state.fbMod[(size_t)src][(size_t)dest] && (!canReach[src] || !canReach[dest] || isFbPermDisabled)) {
                m_state.fbMod[(size_t)src][(size_t)dest] = false;
            }
        }
    }

    // 押せるマスを組み立て直す。描くときと押されたときの両方で使う。
    for (int src = 0; src < numOps; ++src) {
        for (int dest = 0; dest < numOps; ++dest) {
            const bool modPerm = (src >= dest) || dest == 0;
            m_modEnabled[(size_t)src][(size_t)dest] = !modPerm && canReach[dest];

            const bool fbPerm = (src < dest) || !isInSameChain(src, dest);
            m_fbEnabled[(size_t)src][(size_t)dest] = !fbPerm && canReach[src] && canReach[dest];
        }
    }

    repaint();

    if (onMatrixChanged) onMatrixChanged(getState());
}

void GuiFmAlgMatrix::mouseDown(const juce::MouseEvent& e) {
    const auto pos = e.getPosition();
    const int chkStartX = gridX + rectRadius + opLabelW;

    if (pos.getX() < chkStartX) return;
    if (pos.getY() < gridChkStartY || pos.getY() >= gridChkStartY + cellH * numOps) return;

    // 横位置からどのオペレータの列かを出す
    const int src = (pos.getX() - chkStartX) / cellW;

    if (src < 0 || src >= numOps) return;

    const int row = (pos.getY() - gridChkStartY) / cellH;

    // フィードバックの升目
    if (isFeedbackShown()) {
        const int dest = row;

        if (!m_fbEnabled[(size_t)src][(size_t)dest]) return;

        m_state.fbMod[(size_t)src][(size_t)dest] = !m_state.fbMod[(size_t)src][(size_t)dest];

        updateValidity();

        return;
    }

    // モジュレーションの升目 (最後の 1 行は出力へ出すかどうか)
    if (row == numOps - 1) {
        m_state.isCarrier[(size_t)src] = !m_state.isCarrier[(size_t)src];

        updateValidity();

        return;
    }

    const int dest = row + 1;

    if (!m_modEnabled[(size_t)src][(size_t)dest]) return;

    m_state.mod[(size_t)src][(size_t)dest] = !m_state.mod[(size_t)src][(size_t)dest];

    updateValidity();
}

FmAlgState GuiFmAlgMatrix::getState() const {
    return m_state;
}

void GuiFmAlgMatrix::setState(const FmAlgState& s) {
    m_state = s;
    m_state.numOps = numOps;

    updateValidity();
}

void GuiFmAlgMatrix::setImportingParams(juce::StringArray& lines, int& index) {
    FmAlgState s;
    s.numOps = numOps;
    for (int i = 0; i < numOps; ++i) {
        if (index < lines.size()) s.isCarrier[i] = (lines[index++].getIntValue() == 1);
        for (int j = 0; j < numOps; ++j) {
            if (index < lines.size()) s.mod[i][j] = (lines[index++].getIntValue() == 1);
            if (index < lines.size()) s.fbMod[i][j] = (lines[index++].getIntValue() == 1);
        }
    }
    setState(s);
}

juce::String GuiFmAlgMatrix::getExportedParams() {
    juce::String content = "";
    FmAlgState s = getState();
    for (int i = 0; i < numOps; ++i) {
        content += juce::String(s.isCarrier[i] ? 1 : 0) + "\n";
        for (int j = 0; j < numOps; ++j) {
            content += juce::String(s.mod[i][j] ? 1 : 0) + "\n";
            content += juce::String(s.fbMod[i][j] ? 1 : 0) + "\n";
        }
    }
    return content;
}

// つながり方はオペレータごとの並びとして持つ。行の順番で持つと、
// オペレータ数の違う音源のあいだで意味がずれるため。
void GuiFmAlgMatrix::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

    // 書かれていないものは今の状態のままにする
    FmAlgState s = getState();

    s.numOps = numOps;

    for (int i = 0; i < numOps; ++i) {
        auto op = r.arrayItem("ops", i);

        s.isCarrier[i] = op.getBool("isCarrier", s.isCarrier[i]);

        auto mod = op.getIntArray("mod");
        auto fbMod = op.getIntArray("fbMod");

        for (int j = 0; j < numOps; ++j) {
            if (j < (int)mod.size()) s.mod[i][j] = mod[(size_t)j] != 0;
            if (j < (int)fbMod.size()) s.fbMod[i][j] = fbMod[(size_t)j] != 0;
        }
    }

    setState(s);
}

void GuiFmAlgMatrix::writeParams(Io::ParamWriter& writer, const juce::String& key)
{
    auto w = writer.child(key);

    FmAlgState s = getState();

    for (int i = 0; i < numOps; ++i) {
        auto op = w.arrayItem("ops", i);

        op.set("isCarrier", s.isCarrier[i]);

        std::vector<int> mod;
        std::vector<int> fbMod;

        for (int j = 0; j < numOps; ++j) {
            mod.push_back(s.mod[i][j] ? 1 : 0);
            fbMod.push_back(s.fbMod[i][j] ? 1 : 0);
        }

        op.setArray("mod", mod);
        op.setArray("fbMod", fbMod);
    }
}
