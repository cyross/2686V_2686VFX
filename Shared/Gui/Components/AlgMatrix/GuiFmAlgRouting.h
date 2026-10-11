#pragma once
#include <JuceHeader.h>

#include "../../../Core/Io/ParamFile.h"
#include <array>
#include <vector>
#include <functional>

#include "./FmAlgState.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiComponents.h"

// ==============================================================================
// アルゴリズムを可視化する汎用グラフコンポーネント
// ==============================================================================
class GuiFmAlgGraph : public juce::Component {
public:
    FmAlgState state;

    // オペレータ数 numOps の図を描くのに要る高さ。置く側が上下に取る
    // 余白 (reduced(margin)) の分も含める。直列につないだときがいちばん
    // 縦に伸びるので、それが収まる高さにしてある。
    static int preferredHeight(int numOps, int margin = 10);

    void paint(juce::Graphics& g) override;
    void updateState(const FmAlgState& s) {
        state = s;
        repaint();
    }
};

// ==============================================================================
// NxN マトリックス入力汎用コンポーネント
// ==============================================================================
class GuiFmAlgMatrix : public juce::Component, public GuiBaseComponent {
public:
    int numOps;
    GuiFmAlgMatrix(const GuiContext& context, int ops = 8);

    // マトリックスが必要とする高さ。余白の取りすぎを防ぐために使う。
    int getNaturalHeight() const { return gridStartY + gridTotalH; }

    // マトリックスは固定寸法なので、必要な幅も外へ出す。
    // 部品ごと中央へ寄せるために親が使う。
    int getNaturalWidth() const { return naturalW; }
    void paint(juce::Graphics& g) override;
    void resized() override;

    // マス目をクリックしたら、そこだけ入切する。
    void mouseDown(const juce::MouseEvent& e) override;

    // 入切はここに持ち、自分で描いてクリックを拾う。
    //
    // 以前はマス目ごとにトグルを置いていた。8 オペレータならキャリア 8 +
    // モジュレーション 64 + フィードバック 64 で 136 個になり、部品の数が
    // そのまま画面の重さになっていた。枠と地の色は元から paint で描いていて、
    // トグルが描いていたのは小さな四角だけだった。
    FmAlgState m_state;

    // 押せるマスかどうか。updateValidity が組み立てる。
    std::vector<std::vector<bool>> m_modEnabled;
    std::vector<std::vector<bool>> m_fbEnabled;

    std::function<void(const FmAlgState&)> onMatrixChanged;

    void updateValidity();
    FmAlgState getState() const;
    void setState(const FmAlgState& s);
    void setImportingParams(juce::StringArray& lines, int& index);

    // 名前で受け渡す。行の並びに頼ると、呼ぶ順番を間違えたときに
    // 黙って別の値が入り、項目を足すと後ろが全部ずれるため。
    void readParams(const Io::ParamReader& reader, const juce::String& key);
    void writeParams(Io::ParamWriter& writer, const juce::String& key);
    juce::String getExportedParams();

private:
    std::vector<bool> m_opReachable;

    // ルーティングとフィードバックは同じ大きさのマス目なので、1 つの場所に
    // 切り替えて出す。以前は上下に 2 つ並べていて、縦に場所を取っていた。
    // どちらを出しているかは画面だけの状態で、保存はしない。
    GuiToggleButton routingSw;
    GuiToggleButton feedbackSw;

    bool isFeedbackShown() const { return feedbackSw.getToggleState(); }

    // マス目の左端。切り替えのスイッチの方が幅を取るので、マス目はその中央へ寄せる。
    int gridX = 0;
    int naturalW, gridW, gridStartY, gridChkStartY, gridTotalH;
};
