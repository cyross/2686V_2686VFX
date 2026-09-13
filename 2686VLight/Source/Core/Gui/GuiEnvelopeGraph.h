#pragma once
#include <JuceHeader.h>
#include <vector>
#include <functional>

#include "./GuiComponents.h"
#include "./GuiColor.h"

class GuiEnvelopeGraph : public juce::Component
{
public:
    GuiEnvelopeGraph();

    void paint(juce::Graphics& g) override;

    enum class EnvType { Amp, Pitch, SsgSw, SsgSw11, SsgSwP11 };

    struct PhaseDef {
        float widthPx;
        float startLevel;
        float endLevel;
        bool isDashed = false;
        juce::Colour color = GuiColor::EnvelopeGraph::AmpLine;

        // 線の描画を前の終点から繋げず、指定位置から新しく開始するための設定
        bool moveToStart = false;
        float startXOffsetPx = 0.0f; // グラフ左端からの相対X座標 (moveToStart が true の場合のみ使用)
        bool drawPhaseLine = true;
        juce::Colour phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine;
        bool isMax = false; // 値が最大値 = 縦の直線
    };


    // つまみではなく値で受け取るための束。
    //
    // 区分の部品を 1 組しか置かない造りでは、上に並べる小さなグラフは
    // 自分のつまみを持たない。つまみから読む形だと、グラフの数だけ
    // つまみを用意することになってしまう。
    //
    // 上限も一緒に受ける。もとはつまみの幅を見ていたが、幅を決めて
    // いるのはパラメータの範囲なので、そちらから採っても同じになる。
    struct AmpEnvValues {
        float stl = 0.0f, ar = 0.0f, dr = 0.0f, sl = 0.0f, rr = 0.0f;
        float stlMax = 1.0f, arMax = 1.0f, drMax = 1.0f, slMax = 1.0f, rrMax = 1.0f;
        bool kor = false;
    };

    struct PitchEnvValues {
        float ar = 0.0f, dr = 0.0f, rr = 0.0f;
        float arMax = 1.0f, drMax = 1.0f, rrMax = 1.0f;
        float stl = 0.0f, atl = 0.0f, ssl = 0.0f, rll = 0.0f;
    };

    // 段のエンベロープの、段そのもの以外。段の値は別に配列で渡す。
    struct StepEnvHead {
        int steps = 1;
        bool loop = false;
        int loopTo = 0;
        int loopCount = 0;
    };

    void setEnvelope(EnvType type, const juce::String& title, const std::vector<PhaseDef>& phases);
    void updatePitchEnv(
        const PitchEnvValues& v
    );
    void updateSsgSwEnv(
        const StepEnvHead& head,
        // つまみを段の数だけ置かなくなったので、値と上限だけを受け取る。
        // 並びは元のつまみ配列と同じ (R 側の [0] は使わない)。
        const std::array<float, 7>& rVal, float rMax,
        const std::array<float, 7>& lVal, float lMax
    );
    void updateSsgSwEnv11(
        const StepEnvHead& head,
        // つまみを段の数だけ置かなくなったので、値と上限だけを受け取る。
        // 並びは元のつまみ配列と同じ (R 側の [0] は使わない)。
        const std::array<float, 12>& rVal, float rMax,
        const std::array<float, 12>& lVal, float lMax
    );
    void updateSsgSwPEnv11(
        const StepEnvHead& head,
        // つまみを段の数だけ置かなくなったので、値と上限だけを受け取る。
        // 並びは元のつまみ配列と同じ (R 側の [0] は使わない)。
        const std::array<float, 12>& rVal, float rMax,
        const std::array<float, 12>& lVal, float lMax
    );
    void updateAmpEnv(
        const AmpEnvValues& v
    );
    void updateBypass(bool bypass) { this->isBypass = bypass; }

    // KEEP のとき、段のあいだレベルを保つ絵にする。
    //
    // setEnvelope へ渡す前に立てておくこと。渡された段の並びを
    // そこで横一直線へ均し、印は使い捨てにする (区分ごとにグラフを
    // 使い回すため)。
    void setKeepLevels(bool keep) { this->keepLevels = keep; }
private:
    EnvType currentType = EnvType::Amp;
    juce::String currentTitle;
    std::vector<PhaseDef> currentPhases;
    bool isBypass;

    // 段のあいだレベルを保つか。KEEP を入れた区分から立てられる。
    bool keepLevels = false;
};