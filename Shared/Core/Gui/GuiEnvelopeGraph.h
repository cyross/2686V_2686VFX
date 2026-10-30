#pragma once
#include <JuceHeader.h>
#include <array>
#include <vector>
#include <functional>

#include "./GuiComponents.h"
#include "./GuiColor.h"
#include "Shared/Advanced/Curve/AdvancedCurve.h"
#include "Shared/Core/Synth/StepLoop.h"
#include "Shared/Core/Synth/LivePosition.h"
#include "./GuiLiveGraph.h"

// 拡大率のつまみ。帯をドラッグしているあいだ、押しているキーで刻みへそろえる
// (Shift で 0.1、Ctrl で 1、Ctrl + Shift で 10。つまみのラベルのドラッグと同じ決まり)
class GuiZoomSlider : public juce::Slider
{
public:
    GuiZoomSlider() : juce::Slider(juce::Slider::LinearHorizontal, juce::Slider::TextBoxRight)
    {
        // JUCE の既定では、Ctrl などを押すとドラッグが速度で動かす形に切り替わる。
        // Ctrl は刻みに使うので、切り替えは止める
        setVelocityModeParameters(1.0, 1, 0.0, false);
    }

    double snapValue(double attemptedValue, juce::Slider::DragMode dragMode) override;

    // value を step の刻みへそろえ、[lo, hi] へ収める。step が 0 か範囲より粗ければそのまま
    static double align(double value, double step, double lo, double hi);
};

class GuiEnvelopeGraph : public juce::Component, private juce::ScrollBar::Listener, private juce::Timer
{
public:
    GuiEnvelopeGraph();

    void paint(juce::Graphics& g) override;
    void resized() override;

    // ---- 横方向の拡大 (4.0.0) ----
    //
    // 入れると、絵の下に横の送り棒と、拡大率のつまみ・ボタン (x1〜x32) を置く。
    // 絵の中でホイールを回すと拡大率が変わり、右ボタンで引きずると横へ送る。
    // 小さな枠の絵には入れない。
    void setZoomable(bool zoomable);

    static constexpr float zoomMin = 1.0f;
    static constexpr float zoomMax = 32.0f;

    void setZoom(float zoom);
    float getZoom() const { return zoom; }

    void mouseWheelMove(const juce::MouseEvent& e, const juce::MouseWheelDetails& wheel) override;
    void mouseDown(const juce::MouseEvent& e) override;
    void mouseDrag(const juce::MouseEvent& e) override;

    enum class EnvType { Amp, Pitch, SsgSw256, SsgSwP256, ParamEnv256 };

    struct PhaseDef {
        float widthPx;
        float startLevel;
        float endLevel;
        bool isDashed = false;
        juce::Colour color = GuiColor::EnvelopeGraph::AmpLine;
        std::function<float(float)> curveFunc = [](float x) { return x; };

        // カーブのどの部分を切り出すか（0.0 〜 1.0）
        float curveStartX = 0.0f;
        float curveEndX = 1.0f;

        // 線の描画を前の終点から繋げず、指定位置から新しく開始するための設定
        bool moveToStart = false;
        float startXOffsetPx = 0.0f; // グラフ左端からの相対X座標 (moveToStart が true の場合のみ使用)
        bool drawPhaseLine = true;
        juce::Colour phaseLineColor = GuiColor::EnvelopeGraph::PhaseLine;
        bool isMax = false; // 値が最大値 = 縦の直線

        // 鳴っている位置の印 (4.3.0。LivePosition の区間)。段のエンベロープの段なら段の番号 (1〜)、
        // ADSR のアタック・ディケイなら LivePosition::attack / decay、リリース・サスティンは
        // LivePosition::release / sustain。0 は印を付けない (ループの破線など)
        int liveStep = LivePosition::idle;

        // リリースの区間か。KEEP のときは、走り終えるまで前の値を保ち、終わりで行き先へ移る
        bool isRelease = false;

        // KEEP のときも形を変えない (行き先へ移る縦の線など)
        bool keepShape = false;
    };

    // サスティン (鍵を押しているあいだ保つ区間) の幅。時間ではなく、決まった長さで描く
    static constexpr float sustainWidthPx = 60.0f;


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

        // リリースを走り終えたあとに保つ値 (Use Endl)
        bool endlEnable = false;
        float endl = 0.0f;
    };

    struct PitchEnvValues {
        float ar = 0.0f, dr = 0.0f, rr = 0.0f;
        float arMax = 1.0f, drMax = 1.0f, rrMax = 1.0f;
        float stl = 0.0f, atl = 0.0f, ssl = 0.0f, rll = 0.0f;

        // リリースを走り終えたあとに保つ値 (Use Endl)
        bool endlEnable = false;
        float endl = 0.0f;
    };

    // 段のエンベロープの、段そのもの以外。段の値は別に配列で渡す。
    struct StepEnvHead {
        int steps = 1;
        bool loop = false;
        int loopTo = 0;
        int loopCount = 0;

        // LOOP MODE (StepLoop::Mode) と、Rich Mode のループの組
        int loopMode = 0;
        std::vector<StepLoop::Entry> loops;
    };

    void setEnvelope(EnvType type, const juce::String& title, const std::vector<PhaseDef>& phases);
    void updatePitchEnv(
        const PitchEnvValues& v,
        CurveCore* p_curveCore = nullptr,
        bool isCurveMode = false,
        int posIdx = 0
    );
    // 256 段のエンベロープ (SSG SW AMP / PITCH ENV[256] など) の値。
    // r / l の [0] は STL のぶんで、r[0] は使わない。段の数 (head.steps) + 1 個。
    struct StepEnvValues {
        StepEnvHead head;
        std::vector<float> r;
        std::vector<float> l;
        float rr = 0.0f;   // リリースの秒
        float lr = 0.0f;   // リリースの行き先
        float rMax = 1.0f;
        float lMax = 1.0f;

        // リリースを走り終えたあとに保つ値 (Use Endl)
        bool endlEnable = false;
        float endl = 0.0f;
    };

    void updateSsgSwEnv256(
        const StepEnvValues& v,
        CurveCore* p_curveCore = nullptr,
        bool isCurveMode = false,
        int posIdx = 0
    );
    void updateSsgSwPEnv256(
        const StepEnvValues& v,
        CurveCore* p_curveCore = nullptr,
        bool isCurveMode = false,
        int posIdx = 0
    );

    // PARAM ENV[256] (4.3.0)。値は指し先の範囲の割合 (0〜1)。カーブは無い
    void updateParamEnv256(const StepEnvValues& v, const juce::String& title);

    // ---- 鳴っている位置 (4.3.0)
    //
    // 音の側が書いた位置 (LivePosition) を読み、区間の印 (PhaseDef::liveStep) を
    // 持つ絵の上へ縦線を引く。fps は読みに行く速さで、0 を返すあいだは引かない
    // (SETTINGS で切っているとき)。
    //
    // setEnvelope で外れるので、絵を組んだあとに付けること。グラフは区分ごとに
    // 使い回すため、関係のない区分の絵に線を残さない。
    void setLiveSource(const LivePosition* position, std::function<int()> fps);
    void updateAmpEnv(
        const AmpEnvValues& v,
        CurveCore* p_curveCore = nullptr,
        bool isCurveMode = false,
        int posIdx = 0
    );
    void updateBypass(bool bypass) { this->isBypass = bypass; }

    // ほかの絵と同じものを描く (TARGET の大きな枠が、指している小さな枠を写す)
    void copyFrom(const GuiEnvelopeGraph& other)
    {
        currentType = other.currentType;
        currentTitle = other.currentTitle;
        currentPhases = other.currentPhases;
        isBypass = other.isBypass;
        keepLevels = other.keepLevels;
        setEnabled(other.isEnabled());
        setLiveSource(other.liveTable, other.liveFps);
        repaint();
    }

    // KEEP のとき、段のあいだレベルを保つ絵にする。
    //
    // setEnvelope へ渡す前に立てておくこと。渡された段の並びを
    // そこで横一直線へ均し、印は使い捨てにする (区分ごとにグラフを
    // 使い回すため)。
    void setKeepLevels(bool keep) { this->keepLevels = keep; }

    // リリースのあとに ENDL を描く。リリースの終わりから ENDL へ移り、そこで保つ。
    static void appendEndLevel(std::vector<PhaseDef>& phases, float releaseEnd, float endl);
private:
    EnvType currentType = EnvType::Amp;
    juce::String currentTitle;
    std::vector<PhaseDef> currentPhases;
    bool isBypass;

    // 段のエンベロープの並びを組む。種類ごとに違うのは色・見出し・カーブの的だけ。
    void updateStepEnv(EnvType type, const juce::String& title, juce::Colour color, int curveTarget,
        const StepEnvValues& v, CurveCore* p_curveCore, bool isCurveMode, int posIdx);

    // 段のあいだレベルを保つか。KEEP を入れた区分から立てられる。
    bool keepLevels = false;

    // ---- 横方向の拡大 ----
    bool zoomable = false;
    float zoom = 1.0f;

    // 送り具合 (0 = 左端、1 = 右端)
    float scroll = 0.0f;

    // 前に描いたときの見えている幅。拡大した中身の幅は、これの zoom 倍になる
    float lastViewW = 0.0f;

    // 右ボタンで引きずり始めたところ
    float dragStartX = 0.0f;
    float dragStartScroll = 0.0f;
    bool isScrollDrag = false;

    bool isSyncingScrollBar = false;

    // ---- 鳴っている位置
    const LivePosition* liveTable = nullptr;
    std::function<int()> liveFps;
    int liveStep = LivePosition::idle;
    float liveFrac = 0.0f;
    std::uint32_t liveSeq = 0;
    juce::uint32 liveSeqMs = 0;

    // 位置がこの間 (ms) 書かれなければ、鳴っていないとみなす
    static constexpr juce::uint32 liveStaleMs = 200;

    void timerCallback() override;

    juce::ScrollBar scrollBar{ false };
    GuiZoomSlider zoomSlider;
    std::array<juce::TextButton, 6> zoomButtons;

    // 絵を描く場所 (拡大のつまみの分を除いたもの)
    juce::Rectangle<int> graphBounds() const;

    // 見えている幅からはみ出した分 (送れる長さ)
    float overflow() const { return lastViewW * (zoom - 1.0f); }

    void syncScrollBar();
    void scrollBarMoved(juce::ScrollBar* bar, double newRangeStart) override;
};