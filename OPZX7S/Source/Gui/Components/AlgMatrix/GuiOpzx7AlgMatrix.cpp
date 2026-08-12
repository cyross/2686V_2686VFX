#include "./GuiOpzx7AlgMatrix.h"

// オペレータのテーマカラー（1〜8）
static const std::array<juce::Colour, 8> opColors = {
    juce::Colours::red.brighter(0.2f), juce::Colours::orange.brighter(0.2f),
    juce::Colours::yellow.brighter(0.2f), juce::Colours::green.brighter(0.2f),
    juce::Colours::cyan.brighter(0.2f), juce::Colours::dodgerblue.brighter(0.2f),
    juce::Colours::magenta.brighter(0.2f), juce::Colours::purple.brighter(0.2f)
};

// ==============================================================================
// GuiOpzx7AlgGraph の実装
// ==============================================================================
void GuiOpzx7AlgGraph::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black.withAlpha(0.3f));

    std::array<juce::Point<float>, 8> pos;
    std::array<int, 8> depths;
    depths.fill(-1);

    // 深さの計算 (キャリアーを Depth 0 とする)
    for (int i = 0; i < 8; ++i) if (state.isCarrier[i]) depths[i] = 0;

    // 順方向接続のみを辿って深さを決定（無限ループ回避）
    bool changed = true;
    int maxIter = 10;
    while (changed && maxIter-- > 0) {
        changed = false;
        for (int src = 0; src < 8; ++src) {
            for (int dest = 0; dest < 8; ++dest) {
                if (src < dest && state.mod[src][dest] && depths[dest] != -1) {
                    int newDepth = depths[dest] + 1;
                    if (depths[src] < newDepth) {
                        depths[src] = newDepth;
                        changed = true;
                    }
                }
            }
        }
    }

    // ノードの座標決定
    std::vector<int> nodesAtDepth[8];
    for (int i = 0; i < 8; ++i) {
        int d = depths[i];
        if (d >= 0 && d < 8) nodesAtDepth[d].push_back(i);
        else pos[i] = juce::Point<float>(-100, -100); // 孤立ノードは画面外へ
    }

    float w = getWidth();
    float h = getHeight();

    for (int d = 0; d < 8; ++d) {
        int count = nodesAtDepth[d].size();
        if (count == 0) continue;

        float spacing = w / (count + 1);
        float y = h - 30.0f - d * 40.0f; // 下から上へ積み上げる

        for (int i = 0; i < count; ++i) {
            int opIdx = nodesAtDepth[d][i];
            pos[opIdx] = juce::Point<float>(spacing * (i + 1), y);
        }
    }

    // 線の描画
    g.setColour(juce::Colours::white.withAlpha(0.8f));
    for (int src = 0; src < 8; ++src) {
        // キャリア出力矢印
        if (state.isCarrier[src] && depths[src] != -1) {
            g.drawLine(pos[src].x, pos[src].y + 12, pos[src].x, pos[src].y + 25, 2.0f);
            juce::Path p;
            p.addTriangle(pos[src].x, pos[src].y + 28, pos[src].x - 5, pos[src].y + 23, pos[src].x + 5, pos[src].y + 23);
            g.fillPath(p);
        }

        for (int dest = 0; dest < 8; ++dest) {
            if (state.mod[src][dest]) {
                if (src == dest) { // 自己フィードバック
                    g.drawEllipse(pos[src].x + 5, pos[src].y - 25, 15, 15, 2.0f);
                }
                else if (src < dest) { // 順方向モジュレーション
                    g.drawLine(pos[src].x, pos[src].y + 12, pos[dest].x, pos[dest].y - 12, 2.0f);
                }
                else { // 逆方向(別OPへのFB)
                    const float dashLengths[] = { 3.0f, 3.0f };
                    g.drawDashedLine(juce::Line<float>(pos[src].x - 12, pos[src].y, pos[dest].x - 12, pos[dest].y), dashLengths, 2, 1.5f);
                }
            }
        }
    }

    // オペレータボックスの描画
    for (int i = 0; i < 8; ++i) {
        if (depths[i] == -1) continue; // 未接続ノードは描画しない

        g.setColour(opColors[i]);
        g.fillRect(pos[i].x - 12.0f, pos[i].y - 12.0f, 24.0f, 24.0f);

        g.setColour(juce::Colours::black);
        g.drawText(juce::String(i + 1), pos[i].x - 12, pos[i].y - 12, 24, 24, juce::Justification::centred);
    }
}

// ==============================================================================
// GuiOpzx7AlgMatrix の実装
// ==============================================================================
GuiOpzx7AlgMatrix::GuiOpzx7AlgMatrix() {
    for (int src = 0; src < 8; ++src) {
        carrierBtns[src] = std::make_unique<juce::ToggleButton>();
        addAndMakeVisible(carrierBtns[src].get());
        carrierBtns[src]->onClick = [this] { updateValidity(); };

        for (int dest = 0; dest < 8; ++dest) {
            modBtns[src][dest] = std::make_unique<juce::ToggleButton>();
            addAndMakeVisible(modBtns[src][dest].get());
            modBtns[src][dest]->onClick = [this] { updateValidity(); };
        }
    }
}

void GuiOpzx7AlgMatrix::resized() {
    int cellW = 30;
    int cellH = 20;
    int startX = 60;
    int startY = 25;

    for (int src = 0; src < 8; ++src) {
        carrierBtns[src]->setBounds(startX, startY + src * cellH, cellW, cellH);
        for (int dest = 0; dest < 8; ++dest) {
            modBtns[src][dest]->setBounds(startX + (dest + 1) * cellW, startY + src * cellH, cellW, cellH);
        }
    }
}

void GuiOpzx7AlgMatrix::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::white);
    g.setFont(12.0f);

    int cellW = 30;
    int cellH = 20;
    int startX = 60;
    int startY = 25;

    // ヘッダー描画
    g.drawText("OUT", startX, 5, cellW, cellH, juce::Justification::centred);
    for (int i = 0; i < 8; ++i) {
        g.setColour(opColors[i]);
        g.drawText(juce::String(i + 1), startX + (i + 1) * cellW, 5, cellW, cellH, juce::Justification::centred);
    }

    // 行ラベル描画
    for (int i = 0; i < 8; ++i) {
        g.setColour(opColors[i]);
        g.drawText("OP" + juce::String(i + 1), 5, startY + i * cellH, 50, cellH, juce::Justification::centredRight);
    }
}

// 到達可能性を評価し、矛盾する接続を無効化する
void GuiOpzx7AlgMatrix::updateValidity() {
    bool needsRecheck = true;
    while (needsRecheck) {
        needsRecheck = false;

        std::array<bool, 8> canReach = { false };
        for (int i = 0; i < 8; ++i) canReach[i] = carrierBtns[i]->getToggleState();

        bool changed = true;
        while (changed) {
            changed = false;
            for (int src = 0; src < 8; ++src) {
                if (canReach[src]) continue;
                for (int dest = 0; dest < 8; ++dest) {
                    if (modBtns[src][dest]->getToggleState() && canReach[dest]) {
                        canReach[src] = true;
                        changed = true;
                        break;
                    }
                }
            }
        }

        // キャリアに到達できないパスがONになっていたら強制解除し、再計算を要求する
        for (int src = 0; src < 8; ++src) {
            for (int dest = 0; dest < 8; ++dest) {
                if (modBtns[src][dest]->getToggleState() && !canReach[dest]) {
                    modBtns[src][dest]->setToggleState(false, juce::dontSendNotification);
                    needsRecheck = true;
                }
            }
        }
    }

    // 最終的な到達可能性で Enable/Disable を決定
    std::array<bool, 8> finalReach = { false };
    for (int i = 0; i < 8; ++i) finalReach[i] = carrierBtns[i]->getToggleState();
    bool changed = true;
    while (changed) {
        changed = false;
        for (int src = 0; src < 8; ++src) {
            if (finalReach[src]) continue;
            for (int dest = 0; dest < 8; ++dest) {
                if (modBtns[src][dest]->getToggleState() && finalReach[dest]) {
                    finalReach[src] = true;
                    changed = true;
                    break;
                }
            }
        }
    }

    for (int src = 0; src < 8; ++src) {
        for (int dest = 0; dest < 8; ++dest) {
            modBtns[src][dest]->setEnabled(finalReach[dest]);
        }
    }

    if (onMatrixChanged) onMatrixChanged(getState());
}

AlgMatrixState GuiOpzx7AlgMatrix::getState() const {
    AlgMatrixState s;
    for (int i = 0; i < 8; ++i) {
        s.isCarrier[i] = carrierBtns[i]->getToggleState();
        for (int j = 0; j < 8; ++j) {
            s.mod[i][j] = modBtns[i][j]->getToggleState();
        }
    }
    return s;
}

void GuiOpzx7AlgMatrix::setState(const AlgMatrixState& s) {
    for (int i = 0; i < 8; ++i) {
        carrierBtns[i]->setToggleState(s.isCarrier[i], juce::dontSendNotification);
        for (int j = 0; j < 8; ++j) {
            modBtns[i][j]->setToggleState(s.mod[i][j], juce::dontSendNotification);
        }
    }
    updateValidity();
}
