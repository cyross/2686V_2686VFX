#include "./GuiOpzx7AlgMatrix.h"

static const juce::Colour crrColor = juce::Colours::white.withAlpha(0.8f);
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
static const int totalW = chkStartX + Opzx7PrValue::ops * cellW + rectRadius;

// NormalはOP2〜OP8(7行) + OUT(1行) = 8行
static const int modRows = 8;
static const int fbRows = 8;

static const int modStartY = startY;
static const int modChkStartY = modStartY + labelH * 2;
static const int modTotalH = rectRadius + labelH * 2 + modRows * cellH + rectRadius;
static const int fbStartY = modChkStartY + modRows * cellH + rectRadius + fbMarginH + rectRadius;
static const int fbChkStartY = fbStartY + labelH * 2;
static const int fbTotalH = rectRadius + labelH * 2 + fbRows * cellH + rectRadius;

// オペレータのテーマカラー（1〜8）
static const std::array<juce::Colour, Opzx7PrValue::ops> opColors = {
    juce::Colours::red.brighter(0.2f), juce::Colours::orange.brighter(0.2f),
    juce::Colours::yellow.brighter(0.2f), juce::Colours::green.brighter(0.2f),
    juce::Colours::cyan.brighter(0.2f), juce::Colours::dodgerblue.brighter(0.2f),
    juce::Colours::magenta.brighter(0.2f), juce::Colour(0xffe066ff)
};

// ==============================================================================
// GuiOpzx7AlgGraph の実装
// ==============================================================================
void GuiOpzx7AlgGraph::paint(juce::Graphics& g) {
    g.fillAll(juce::Colours::black.withAlpha(0.3f));

    std::array<juce::Point<float>, Opzx7PrValue::ops> pos;
    std::array<int, Opzx7PrValue::ops> depths;
    depths.fill(-1);

    for (int i = 0; i < Opzx7PrValue::ops; ++i) if (state.isCarrier[i]) depths[i] = 0;

    bool changed = true;
    int maxIter = 10;
    while (changed && maxIter-- > 0) {
        changed = false;
        for (int src = 0; src < Opzx7PrValue::ops; ++src) {
            for (int dest = 0; dest < Opzx7PrValue::ops; ++dest) {
                if (state.mod[src][dest] && depths[dest] != -1) {
                    int newDepth = depths[dest] + 1;
                    if (depths[src] < newDepth) {
                        depths[src] = newDepth;
                        changed = true;
                    }
                }
            }
        }
    }

    std::vector<int> nodesAtDepth[Opzx7PrValue::ops];
    for (int i = 0; i < Opzx7PrValue::ops; ++i) {
        int d = depths[i];
        if (d >= 0 && d < Opzx7PrValue::ops) nodesAtDepth[d].push_back(i);
        else pos[i] = juce::Point<float>(-100, -100);
    }

    float w = getWidth();
    float h = getHeight();
    float yStep = 19.0f;

    for (int d = 0; d < Opzx7PrValue::ops; ++d) {
        int count = nodesAtDepth[d].size();
        if (count == 0) continue;

        float spacing = w / (count + 1);
        float y = h - 12.0f - d * yStep;

        for (int i = 0; i < count; ++i) {
            int opIdx = nodesAtDepth[d][i];
            pos[opIdx] = juce::Point<float>(spacing * (i + 1), y);
        }
    }

    // 線の描画
    g.setColour(modColor);
    for (int src = 0; src < Opzx7PrValue::ops; ++src) {
        if (state.isCarrier[src] && depths[src] != -1) {
            g.setColour(modColor);
            g.drawLine(pos[src].x, pos[src].y + 7.0f, pos[src].x, pos[src].y + 16.0f, 1.5f);
            juce::Path p;
            p.addTriangle(pos[src].x, pos[src].y + 18.0f, pos[src].x - 3.0f, pos[src].y + 14.0f, pos[src].x + 3.0f, pos[src].y + 14.0f);
            g.fillPath(p);
        }

        for (int dest = 0; dest < Opzx7PrValue::ops; ++dest) {
            // 通常モジュレーション (実線)
            if (state.mod[src][dest]) {
                g.setColour(modColor);
                g.drawLine(pos[src].x + 2.0f, pos[src].y + 7.0f, pos[dest].x + 2.0f, pos[dest].y - 7.0f, 1.5f);
            }
            // フィードバックモジュレーション
            if (state.fbMod[src][dest]) {
                g.setColour(fbModColor);
                if (src == dest) {
                    // 自己FB円を小さくし、オペレータの左上に配置（右下が角に4分の1程度被る）
                    float r = 5.0f;
                    float cx = pos[src].x - 7.0f;
                    float cy = pos[src].y - 7.0f;
                    g.drawEllipse(cx - r, cy - r, r * 2.0f, r * 2.0f, 1.0f);
                    g.setFont(7.0f);
                    g.drawText("FB", cx - r - 12.0f, cy - r - 4.0f, 12.0f, 8.0f, juce::Justification::centredRight);
                }
                else {
                    // 他OPへのフィードバック（逆コの字で横に飛び出して描画）
                    float offsetX = -14.0f - std::abs(src - dest) * 3.5f;
                    float sX = pos[src].x - 7.0f;
                    float sY = pos[src].y;
                    float eX = pos[dest].x - 7.0f;
                    float eY = pos[dest].y;
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

    // オペレータボックスの描画
    for (int i = 0; i < Opzx7PrValue::ops; ++i) {
        if (depths[i] == -1) continue;
        g.setColour(opColors[i]);
        g.fillRect(pos[i].x - 7.0f, pos[i].y - 7.0f, 14.0f, 14.0f);

        g.setColour(juce::Colours::black);
        g.setFont(9.0f);
        g.drawText(juce::String(i + 1), pos[i].x - 7.0f, pos[i].y - 7.0f, 14.0f, 14.0f, juce::Justification::centred);
    }
}

// ==============================================================================
// GuiOpzx7AlgMatrix の実装
// ==============================================================================
GuiOpzx7AlgMatrix::GuiOpzx7AlgMatrix(const GuiContext& context) : GuiBaseComponent(context) {
    for (int src = 0; src < Opzx7PrValue::ops; ++src) {
        carrierBtns[src] = std::make_unique<GuiToggleButton>(context);
        carrierBtns[src]->setSize(chkW, chkH);
        carrierBtns[src]->setBoxSize(chkW, chkH);
        carrierBtns[src]->setBoxGap(0.0f, 0.0f);
        carrierBtns[src]->setLabelGap(0.0f);
        carrierBtns[src]->setColour(juce::ToggleButton::tickColourId, crrColor);
        carrierBtns[src]->setColour(juce::ToggleButton::textColourId, crrColor);

        addAndMakeVisible(carrierBtns[src].get());
        carrierBtns[src]->onClick = [this] { updateValidity(); };

        for (int dest = 0; dest < Opzx7PrValue::ops; ++dest) {
            modBtns[src][dest] = std::make_unique<GuiToggleButton>(context);
            addAndMakeVisible(modBtns[src][dest].get());
            modBtns[src][dest]->setSize(chkW, chkH);
            modBtns[src][dest]->setBoxSize(chkW, chkH);
            modBtns[src][dest]->setBoxGap(0.0f, 0.0f);
            modBtns[src][dest]->setLabelGap(0.0f);
            modBtns[src][dest]->setColour(juce::ToggleButton::tickColourId, modColor);
            modBtns[src][dest]->setColour(juce::ToggleButton::textColourId, modColor);
            modBtns[src][dest]->onClick = [this] { updateValidity(); };

            if (dest == 0) {
                modBtns[src][dest]->setVisible(false);
            }

            fbBtns[src][dest] = std::make_unique<GuiToggleButton>(context);
            addAndMakeVisible(fbBtns[src][dest].get());
            fbBtns[src][dest]->setSize(chkW, chkH);
            fbBtns[src][dest]->setBoxSize(chkW, chkH);
            fbBtns[src][dest]->setBoxGap(0.0f, 0.0f);
            fbBtns[src][dest]->setLabelGap(0.0f);
            fbBtns[src][dest]->setColour(juce::ToggleButton::tickColourId, fbModColor);
            fbBtns[src][dest]->setColour(juce::ToggleButton::textColourId, fbModColor);
            fbBtns[src][dest]->onClick = [this] { updateValidity(); };
        }
    }
}

void GuiOpzx7AlgMatrix::resized() {
    // --- Normal Matrix (上段: OP2〜8 + 最下段OUT) ---
    for (int src = 0; src < Opzx7PrValue::ops; ++src) {
        int x = chkStartX + src * cellW + margin;

        // OP2〜OP8 (dest = 1〜7)
        for (int dest = 1; dest < Opzx7PrValue::ops; ++dest) {
            int y = modChkStartY + cellH * (dest - 1) + margin;
            modBtns[src][dest]->setBounds(x, y, chkW, chkH);
        }

        // OUTを一番下に配置
        int outY = modChkStartY + cellH * 7 + margin;
        carrierBtns[src]->setBounds(x, outY, chkW, chkH);
    }

    // --- Feedback Matrix (下段) ---
    for (int src = 0; src < Opzx7PrValue::ops; ++src) {
        int x = chkStartX + src * cellW + margin;
        for (int dest = 0; dest < Opzx7PrValue::ops; ++dest) {
            int y = fbChkStartY + cellH * dest + margin;
            fbBtns[src][dest]->setBounds(x, y, chkW, chkH);
        }
    }
}

void GuiOpzx7AlgMatrix::paint(juce::Graphics& g) {
    g.setColour(juce::Colours::black.withAlpha(0.5f));
    g.fillRoundedRectangle(rectRadius, rectRadius, totalW, modTotalH, rectRadius);
    g.fillRoundedRectangle(rectRadius, fbStartY + rectRadius, totalW, fbTotalH, rectRadius);

    // ==========================================
    // セル単位のグレーアウト描画 (Normal)
    // ==========================================
    for (int dest = 1; dest < Opzx7PrValue::ops; ++dest) {
        int y = modChkStartY + cellH * (dest - 1);
        for (int src = 0; src < Opzx7PrValue::ops; ++src) {
            int x = chkStartX + src * cellW;
            if (src >= dest) {
                g.setColour(permanentDisabledModColor);
                g.fillRect(x, y, cellW, cellH);
            }
            else if (!modBtns[src][dest]->isEnabled()) {
                g.setColour(disabledModColor);
                g.fillRect(x, y, cellW, cellH);
            }
        }
    }

    // ==========================================
    // セル単位のグレーアウト描画 (Feedback)
    // ==========================================
    for (int dest = 0; dest < Opzx7PrValue::ops; ++dest) {
        int y = fbChkStartY + cellH * dest;
        for (int src = 0; src < Opzx7PrValue::ops; ++src) {
            int x = chkStartX + src * cellW;
            if (src < dest) {
                g.setColour(permanentDisabledModColor);
                g.fillRect(x, y, cellW, cellH);
            }
            else if (!fbBtns[src][dest]->isEnabled()) {
                g.setColour(disabledModColor);
                g.fillRect(x, y, cellW, cellH);
            }
        }
    }

    g.setFont(11.0f);

    // ==========================================
    // NORMAL MODULATION 描画
    // ==========================================
    g.setColour(juce::Colours::white.withAlpha(0.6f));
    g.drawText("NORMAL MODULATION", startX, startY, totalW, labelH, juce::Justification::centred);

    for (int i = 0; i < Opzx7PrValue::ops; ++i) {
        g.setColour(opColors[i]);
        g.drawText(juce::String(i + 1), chkStartX + i * cellW, startY + labelH, cellW, labelH, juce::Justification::centred);
    }

    // OP2〜OP8 の行ラベル
    for (int dest = 1; dest < Opzx7PrValue::ops; ++dest) {
        int y = modChkStartY + cellH * (dest - 1);
        g.setColour(opColors[dest]);
        g.drawText("->" + juce::String(dest + 1), startX, y, chkStartX - startX - margin, cellH, juce::Justification::centredRight);
    }

    // OUT行ラベルを一番下に
    int outY = modChkStartY + cellH * 7;
    g.setColour(juce::Colours::white);
    g.drawText("OUT", startX, outY, chkStartX - startX - margin, cellH, juce::Justification::centredRight);

    // ==========================================
    // FEEDBACK MODULATION 描画
    // ==========================================
    g.setColour(fbModColor.withAlpha(0.8f));
    g.drawText("FEEDBACK MODULATION", startX, fbStartY, totalW, labelH, juce::Justification::centred);

    for (int i = 0; i < Opzx7PrValue::ops; ++i) {
        g.setColour(opColors[i]);
        g.drawText(juce::String(i + 1), chkStartX + i * cellW, fbStartY + labelH, cellW, labelH, juce::Justification::centred);
    }

    for (int dest = 0; dest < Opzx7PrValue::ops; ++dest) {
        int y = fbChkStartY + cellH * dest;
        g.setColour(opColors[dest]);
        g.drawText("->" + juce::String(dest + 1), startX, y, chkStartX - startX - margin, cellH, juce::Justification::centredRight);
    }
}

// 到達可能性を評価し、矛盾する接続を無効化する
void GuiOpzx7AlgMatrix::updateValidity() {
    std::array<bool, Opzx7PrValue::ops> canReach = { false };
    for (int i = 0; i < Opzx7PrValue::ops; ++i) canReach[i] = carrierBtns[i]->getToggleState();

    // 1. 通常モジュレーションの到達可能性計算
    for (int dest = Opzx7PrValue::ops - 1; dest >= 1; --dest) {
        if (!canReach[dest]) continue;
        for (int src = dest - 1; src >= 0; --src) {
            if (modBtns[src][dest]->getToggleState()) canReach[src] = true;
        }
    }

    // ヘルパー: 指定された2つのオペレータが同じモジュレーションチェーン（連結成分）に属しているか判定
    auto isInSameChain = [&](int a, int b) {
        if (a == b) return true;
        std::array<std::array<bool, Opzx7PrValue::ops>, Opzx7PrValue::ops> undir = { false };
        for (int s = 0; s < Opzx7PrValue::ops; ++s) {
            for (int d = 1; d < Opzx7PrValue::ops; ++d) {
                if (modBtns[s][d]->getToggleState()) {
                    undir[s][d] = true;
                    undir[d][s] = true;
                }
            }
        }
        std::array<bool, Opzx7PrValue::ops> visited = { false };
        std::vector<int> q;
        q.push_back(a);
        visited[a] = true;
        while (!q.empty()) {
            int curr = q.back();
            q.pop_back();
            if (curr == b) return true;
            for (int next = 0; next < Opzx7PrValue::ops; ++next) {
                if (undir[curr][next] && !visited[next]) {
                    visited[next] = true;
                    q.push_back(next);
                }
            }
        }
        return false;
        };

    // 2. 無効なパスがONになっていたら自動解除
    for (int src = 0; src < Opzx7PrValue::ops; ++src) {
        for (int dest = 1; dest < Opzx7PrValue::ops; ++dest) {
            bool isModPermDisabled = (src >= dest);
            if (modBtns[src][dest]->getToggleState() && (!canReach[dest] || isModPermDisabled)) {
                modBtns[src][dest]->setToggleState(false, juce::dontSendNotification);
            }
        }
        for (int dest = 0; dest < Opzx7PrValue::ops; ++dest) {
            bool isFbPermDisabled = (src < dest) || !isInSameChain(src, dest); // ★要望3: チェーン外のFBを禁止
            if (fbBtns[src][dest]->getToggleState() && (!canReach[src] || !canReach[dest] || isFbPermDisabled)) {
                fbBtns[src][dest]->setToggleState(false, juce::dontSendNotification);
            }
        }
    }

    // 3. GUIボタンの Enabled 状態と色を更新する
    auto updateBtnColor = [](GuiToggleButton* btn, bool permDisabled, bool enabled, juce::Colour activeColor) {
        if (permDisabled) {
            btn->setEnabled(false);
            btn->setColour(juce::ToggleButton::textColourId, permanentDisabledModColor);
            btn->setColour(juce::ToggleButton::tickColourId, permanentDisabledModColor);
        }
        else if (!enabled) {
            btn->setEnabled(false);
            btn->setColour(juce::ToggleButton::textColourId, disabledModColor);
            btn->setColour(juce::ToggleButton::tickColourId, disabledModColor);
        }
        else {
            btn->setEnabled(true);
            btn->setColour(juce::ToggleButton::textColourId, activeColor);
            btn->setColour(juce::ToggleButton::tickColourId, activeColor);
        }
        };

    for (int src = 0; src < Opzx7PrValue::ops; ++src) {
        for (int dest = 1; dest < Opzx7PrValue::ops; ++dest) {
            bool isModPermDisabled = (src >= dest);
            bool modEnabled = canReach[dest] && !isModPermDisabled;
            updateBtnColor(modBtns[src][dest].get(), isModPermDisabled, modEnabled, modColor);
        }
        for (int dest = 0; dest < Opzx7PrValue::ops; ++dest) {
            bool isFbPermDisabled = (src < dest) || !isInSameChain(src, dest); // ★要望3: チェーン外のFBをDisabledに
            bool fbEnabled = canReach[src] && canReach[dest] && !isFbPermDisabled;
            updateBtnColor(fbBtns[src][dest].get(), isFbPermDisabled, fbEnabled, fbModColor);
        }
    }

    repaint();

    if (onMatrixChanged) onMatrixChanged(getState());
}

AlgMatrixState GuiOpzx7AlgMatrix::getState() const {
    AlgMatrixState s;
    for (int i = 0; i < Opzx7PrValue::ops; ++i) {
        s.isCarrier[i] = carrierBtns[i]->getToggleState();
        for (int j = 0; j < Opzx7PrValue::ops; ++j) {
            s.mod[i][j] = modBtns[i][j]->getToggleState();
            s.fbMod[i][j] = fbBtns[i][j]->getToggleState();
        }
    }
    return s;
}

void GuiOpzx7AlgMatrix::setState(const AlgMatrixState& s) {
    for (int i = 0; i < Opzx7PrValue::ops; ++i) {
        carrierBtns[i]->setToggleState(s.isCarrier[i], juce::dontSendNotification);
        for (int j = 0; j < 8; ++j) {
            modBtns[i][j]->setToggleState(s.mod[i][j], juce::dontSendNotification);
            fbBtns[i][j]->setToggleState(s.fbMod[i][j], juce::dontSendNotification);
        }
    }
    updateValidity();
}
