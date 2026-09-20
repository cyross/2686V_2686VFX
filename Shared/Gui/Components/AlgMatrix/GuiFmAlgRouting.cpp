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
static const int startX = rectRadius;
static const int opLabelW = 28;
static const int chkStartX = startX + opLabelW;
static const int startY = rectRadius;
static const int labelH = 12;
static const int fbMarginH = 12;

// オペレータのテーマカラー（最大8色）
static const std::array<juce::Colour, 8> opColors = {
    juce::Colours::red.brighter(0.2f), juce::Colours::orange.brighter(0.2f),
    juce::Colours::yellow.brighter(0.2f), juce::Colours::green.brighter(0.2f),
    juce::Colours::cyan.brighter(0.2f), juce::Colours::dodgerblue.brighter(0.2f),
    juce::Colours::magenta.brighter(0.2f), juce::Colour(0xffe066ff)
};

// ==============================================================================
// GuiFmAlgGraph の実装
// ==============================================================================
void GuiFmAlgGraph::paint(juce::Graphics& g) {
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
    float yStep = 28.0f;
    float minSpacing = 32.0f; // 横方向の最小間隔 (ノードが重ならないための距離)

    // 階層が深い場合(直列8段など)、yStepを動的に縮小して画面内に収める
    if (maxDepth > 0) {
        float requiredHeight = maxDepth * yStep + 30.0f; // 30.0fは上下の余白
        if (requiredHeight > h) {
            yStep = (h - 30.0f) / maxDepth;
        }
    }

    // 5. 座標計算
    for (int d = 0; d < state.numOps; ++d) {
        int count = nodesAtDepth[d].size();
        if (count == 0) continue;

        float y = h - 20.0f - d * yStep; // Depth 0 が一番下、数字が大きいほど上

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
                    if (dist < minSpacing) {
                        float push = (minSpacing - dist) * 0.5f;
                        finalX[i] -= push;
                        finalX[i + 1] += push;
                    }
                }
                // 画面端からはみ出ないように制限
                for (int i = 0; i < count; ++i) {
                    if (finalX[i] < 15.0f) finalX[i] = 15.0f;
                    if (finalX[i] > w - 15.0f) finalX[i] = w - 15.0f;
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
            g.drawLine(pos[src].x, pos[src].y + 7.0f, pos[src].x, pos[src].y + 16.0f, 1.5f);
            juce::Path p;
            p.addTriangle(pos[src].x, pos[src].y + 18.0f, pos[src].x - 3.0f, pos[src].y + 14.0f, pos[src].x + 3.0f, pos[src].y + 14.0f);
            g.fillPath(p);
        }

        for (int dest = 0; dest < state.numOps; ++dest) {
            if (depths[dest] == -1) continue;

            // 通常モジュレーション (カギ線/エルボ結線)
            if (state.mod[src][dest]) {
                g.setColour(modColor);

                // 中心座標からスタート
                float x1 = pos[src].x;
                float y1 = pos[src].y + 7.0f; // srcの下端
                float x2 = pos[dest].x;
                float y2 = pos[dest].y - 7.0f; // destの上端

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
            if (state.fbMod[src][dest]) {
                g.setColour(fbModColor);
                if (src == dest) {
                    float r = 5.0f;
                    float cx = pos[src].x - 7.0f;
                    float cy = pos[src].y - 7.0f;
                    g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 1.0f);
                    g.setFont(7.0f);
                    g.drawText("FB", cx - r - 12.0f, cy - r - 4.0f, 12.0f, 8.0f, juce::Justification::centredRight);
                }
                else {
                    float offsetX = -14.0f - std::abs(src - dest) * 3.5f;
                    float sX = pos[src].x - 7.0f; float sY = pos[src].y;
                    float eX = pos[dest].x - 7.0f; float eY = pos[dest].y;
                    float mX = pos[src].x + offsetX;
                    const float dashLengths[] = { 2.0f, 2.0f };
                    g.drawDashedLine(juce::Line<float>(sX, sY, mX, sY), dashLengths, 2, 1.0f);
                    g.drawDashedLine(juce::Line<float>(mX, sY, mX, eY), dashLengths, 2, 1.0f);
                    g.drawDashedLine(juce::Line<float>(mX, eY, eX, eY), dashLengths, 2, 1.0f);
                    float mY = (sY + eY) / 2.0f;
                    g.setFont(7.0f);
                    g.drawText("FB", mX - 14.0f, mY - 4.0f, 12.0f, 8.0f, juce::Justification::centredRight);
                }
            }
        }
    }

    // 7. オペレータボックスの描画
    for (int i = 0; i < state.numOps; ++i) {
        if (depths[i] == -1) continue;
        g.setColour(opColors[i]);
        g.fillRect(pos[i].x - 7.0f, pos[i].y - 7.0f, 14.0f, 14.0f);
        g.setColour(juce::Colours::black);
        g.setFont(9.0f);
        g.drawText(juce::String(i + 1), pos[i].x - 7.0f, pos[i].y - 7.0f, 14.0f, 14.0f, juce::Justification::centred);
    }
}

// ==============================================================================
// GuiFmAlgMatrix の実装
// ==============================================================================
GuiFmAlgMatrix::GuiFmAlgMatrix(const GuiContext& context, int ops)
    : GuiBaseComponent(context), numOps(ops), m_opReachable(ops, false)
{
    totalW = chkStartX + numOps * cellW + rectRadius;
    modChkStartY = startY + labelH * 2;
    modTotalH = rectRadius + labelH * 2 + numOps * cellH + rectRadius;
    fbStartY = modChkStartY + numOps * cellH + rectRadius + fbMarginH + rectRadius;
    fbChkStartY = fbStartY + labelH * 2;
    fbTotalH = rectRadius + labelH * 2 + numOps * cellH + rectRadius;

    m_state.numOps = numOps;

    m_modEnabled.assign((size_t)numOps, std::vector<bool>((size_t)numOps, false));
    m_fbEnabled.assign((size_t)numOps, std::vector<bool>((size_t)numOps, false));

    updateValidity();
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

    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.fillRoundedRectangle(rectRadius, rectRadius, totalW, modTotalH, rectRadius);
    g.fillRoundedRectangle(rectRadius, fbStartY + rectRadius, totalW, fbTotalH, rectRadius);

    for (int dest = 1; dest < numOps; ++dest) {
        int y = modChkStartY + cellH * (dest - 1);
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
    }

    // 一番下の行は「出力へ出すか」。
    {
        int y = modChkStartY + cellH * (numOps - 1);

        for (int src = 0; src < numOps; ++src) {
            drawCell(chkStartX + src * cellW, y, m_state.isCarrier[(size_t)src], true);
        }
    }

    for (int dest = 0; dest < numOps; ++dest) {
        int y = fbChkStartY + cellH * dest;
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
    }

    g.setFont(11.0f);
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawText("NORMAL MODULATION", startX, startY, totalW, labelH, juce::Justification::centred);

    for (int i = 0; i < numOps; ++i) {
        g.setColour(opColors[i]);
        g.drawText(juce::String(i + 1), chkStartX + i * cellW, startY + labelH, cellW, labelH, juce::Justification::centred);
    }

    for (int dest = 1; dest < numOps; ++dest) {
        int y = modChkStartY + cellH * (dest - 1);
        g.setColour(opColors[dest]);
        g.drawText("->" + juce::String(dest + 1), startX, y, chkStartX - startX - margin, cellH, juce::Justification::centredRight);
    }

    int outY = modChkStartY + cellH * (numOps - 1);
    g.setColour(juce::Colours::white);
    g.drawText("OUT", startX, outY, chkStartX - startX - margin, cellH, juce::Justification::centredRight);

    g.setColour(fbModColor.withAlpha(0.8f));
    g.drawText("FEEDBACK MODULATION", startX, fbStartY, totalW, labelH, juce::Justification::centred);

    for (int i = 0; i < numOps; ++i) {
        g.setColour(opColors[i]);
        g.drawText(juce::String(i + 1), chkStartX + i * cellW, fbStartY + labelH, cellW, labelH, juce::Justification::centred);
    }

    for (int dest = 0; dest < numOps; ++dest) {
        int y = fbChkStartY + cellH * dest;
        g.setColour(opColors[dest]);
        g.drawText("->" + juce::String(dest + 1), startX, y, chkStartX - startX - margin, cellH, juce::Justification::centredRight);
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

    // 横位置からどのオペレータの列かを出す
    const int src = (pos.getX() - chkStartX) / cellW;

    if (src < 0 || src >= numOps) return;
    if (pos.getX() < chkStartX) return;

    // モジュレーションの升目 (最後の 1 行は出力へ出すかどうか)
    if (pos.getY() >= modChkStartY && pos.getY() < modChkStartY + cellH * numOps) {
        const int row = (pos.getY() - modChkStartY) / cellH;

        if (row == numOps - 1) {
            m_state.isCarrier[(size_t)src] = !m_state.isCarrier[(size_t)src];

            updateValidity();

            return;
        }

        const int dest = row + 1;

        if (!m_modEnabled[(size_t)src][(size_t)dest]) return;

        m_state.mod[(size_t)src][(size_t)dest] = !m_state.mod[(size_t)src][(size_t)dest];

        updateValidity();

        return;
    }

    // フィードバックの升目
    if (pos.getY() >= fbChkStartY && pos.getY() < fbChkStartY + cellH * numOps) {
        const int dest = (pos.getY() - fbChkStartY) / cellH;

        if (dest < 0 || dest >= numOps) return;
        if (!m_fbEnabled[(size_t)src][(size_t)dest]) return;

        m_state.fbMod[(size_t)src][(size_t)dest] = !m_state.fbMod[(size_t)src][(size_t)dest];

        updateValidity();
    }
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
