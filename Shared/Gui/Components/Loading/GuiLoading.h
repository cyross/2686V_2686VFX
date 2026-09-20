#pragma once

#include <functional>

#include <JuceHeader.h>

// ============================================================================
// ローディング画面
// ============================================================================
// 時間の掛かる処理の間、画面全体を覆って「待ち」を示す。
//
// 覆っている間は下の画面を触らせない。マウスはこの部品で受け止め、
// 素通りさせないこと。
//
// 見た目は次の CSS をそのまま写したもの。
//
//   .wave { display: flex; align-items: center; justify-content: center; gap: 3px; }
//   .wave .bar-item {
//       width: 3px; height: 30px;
//       border-radius: 1.5px;
//       animation: wave-animation 1.2s ease-in-out infinite;
//   }
//   .wave .bar-item:nth-child(n) { animation-delay: (n-1) * 0.1s; }
//   @keyframes wave-animation {
//       0%, 100% { transform: scaleY(0.5); }
//       50%      { transform: scaleY(1); }
//   }
//
// 棒の色と地の色は CSS の値ではなく GuiColor::Loading から取る。
// 地は半透明の黒で、下の画面がうっすら透ける。
class GuiLoading : public juce::Component, private juce::Timer
{
    // いつ動き始めたか。経過時間から棒の伸び縮みを決める。
    double m_startMs = 0.0;

    juce::String m_message;

    // 中止ボタン。渡されたときだけ出す。
    juce::TextButton m_cancelBtn;
    std::function<void()> m_onCancel;
public:
    // --- CSS から写した寸法 ---
    static constexpr int barCount = 5;
    static constexpr int barWidth = 3;
    static constexpr int barHeight = 30;
    static constexpr int barGap = 3;
    static constexpr float barRadius = 1.5f;

    // --- CSS から写した動き ---
    static constexpr double cycleSeconds = 1.2;   // animation-duration
    static constexpr double barDelaySeconds = 0.1; // 1 本ごとの animation-delay
    static constexpr float minScaleY = 0.5f;      // scaleY(0.5) 〜 scaleY(1)

    // 描き直しの間隔。60fps 相当。
    static constexpr int frameMs = 16;

    // 棒ひと組の大きさの倍率。
    //
    // CSS の寸法どおりだと 27x30 しかなく、画面全体を覆った中では
    // 小さく見える。大きくしたいときはここだけ変えれば、棒の太さ・
    // 高さ・間隔・角の丸みが同じ比のまま揃って変わる。
    static constexpr float sizeScale = 1.0f;

    // 文字を添えるときの、棒との間隔と文字の高さ
    static constexpr int messageGap = 12;
    static constexpr float messageFontHeight = 14.0f;

    // 中止ボタン
    static constexpr int cancelGap = 14;
    static constexpr int cancelWidth = 120;
    static constexpr int cancelHeight = 26;

    GuiLoading();

    // 親いっぱいに広げて出す。message は空なら文字を出さない。
    //
    // onCancel を渡すと中止ボタンが出る。押されたときに呼ばれるだけで、
    // 覆いはこちらでは閉じない。止めた側が片付けること。
    void show(juce::Component& parent, const juce::String& message = {},
        std::function<void()> onCancel = nullptr);

    // 出している間に文字だけ差し替える (「12 / 340 件」のような進み具合)
    void setMessage(const juce::String& message);

    void hide();

    void paint(juce::Graphics& g) override;
    void resized() override;
private:
    void visibilityChanged() override;
    void timerCallback() override { repaint(); }

    // CSS の ease-in-out (cubic-bezier(0.42, 0, 0.58, 1))
    static float easeInOut(float x);

    // 経過秒から、i 本目の棒の縦の伸び (minScaleY 〜 1.0) を返す
    static float barScale(double elapsedSeconds, int index);
};
