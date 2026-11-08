#pragma once

#include <functional>

#include <JuceHeader.h>

#include "./GenWaveRender.h"

// ============================================================================
// 生成波形の再生の操作
// ============================================================================
// 生成波形のプレビューと、ブラウザの行のプレビューで同じものを使う。
// 一時停止・コマ送りのシークバー・縦の拡大率の 3 つを、プレビューの下の
// 1 段へ並べる。
//
// 部品 (juce::Component) にはしていない。ブラウザは何百行も並ぶので、
// 1 行ごとに部品を作ると開くだけで重くなる。ここは描くのと、押された
// 位置の読み取りだけを受け持ち、マウスは持ち主から回してもらう。
//
// シークバーを右クリックすると、見たいコマの番号を打ち込める。
//
// 1 コマは 1 周期。窓の頭は周期の切れ目へ合わせて動くので、画面の絵が
// 変わるのは周期ごとになる。そのひとつずつを「コマ」として送れるように
// してある。周期の分からない波形では 1/30 秒をひとコマにする。
class GenWavePlayer
{
public:
    // 縦の拡大率。x1〜x8。
    static constexpr int zoomMin = 1;
    static constexpr int zoomMax = 8;

    // 操作の段の高さ
    static constexpr int height = 20;

    // 拡大率の欄の幅
    static constexpr int zoomBoxWidth = 44;

    // 押された先
    enum class Hit { none = 0, handled = 1, zoomMenu = 2, frameInput = 3 };

    // 1 コマの点数と、コマの数
    double frameSamples(const GenWaveRender::Wave& wave) const;
    int frameCount(const GenWaveRender::Wave& wave) const;

    // 1 秒あたりのコマ数。0 (既定) なら 1 周期を 1 コマにする。
    // 決めると 1/fps 秒が 1 コマになり、コマの数は長さ × fps になる。
    // 窓の頭はそれでも周期の切れ目へ合わせる (形が流れないように)。
    void setFramesPerSecond(double fps) { m_fps = juce::jmax(0.0, fps); }
    double framesPerSecond() const { return m_fps; }

    // 頭から再生し直す。波形を作り直したときに呼ぶ。
    void restart(double nowMs);

    // 今のコマ
    int frame(const GenWaveRender::Wave& wave, double nowMs) const;

    // 窓の頭の位置 (点の番号)
    int windowStart(const GenWaveRender::Wave& wave, int window, double nowMs) const;

    // 全体のうち今どこか (0.0〜1.0)
    float progress(const GenWaveRender::Wave& wave, double nowMs) const;

    bool isPaused() const { return m_paused; }

    // まとめて止める・動かす。ブラウザの「すべて停止 / すべて再生」から使う。
    // wave が nullptr (まだ作っていない) でも、止めた・動かした状態は覚える。
    void pause(const GenWaveRender::Wave* wave, double nowMs);
    void resume(const GenWaveRender::Wave* wave, double nowMs);

    // そのコマで止める。番号は 0 から。打ち込んだ番号を当てるときに使う。
    void showFrame(const GenWaveRender::Wave& wave, int target, double nowMs);

    int zoom() const { return m_zoom; }
    void setZoom(int value) { m_zoom = juce::jlimit(zoomMin, zoomMax, value); }

    // 拡大率の欄を段の右端に置くか (既定は置く)。置かないときは、持ち主が
    // 好きな場所へ drawZoom で描き、押されたら showZoomMenuAt で選択肢を出す。
    // 段からは拡大率の欄が抜け、シークバーがそのぶん広がる。
    void setZoomInRow(bool inRow) { m_zoomInRow = inRow; }
    bool isZoomInRow() const { return m_zoomInRow; }

    // 拡大率の欄だけを描く
    void drawZoom(juce::Graphics& g, juce::Rectangle<int> box) const;

    // 描く。wave が nullptr なら時間の軸が無いものとして、一時停止と
    // シークバーを薄く出して押させない (止め絵のプレビュー用)。
    void draw(juce::Graphics& g, juce::Rectangle<int> area,
        const GenWaveRender::Wave* wave, double nowMs) const;

    // マウス。area は draw に渡したのと同じ区画。
    // popup は右クリック (コマの番号を打ち込む口を開く) かどうか。
    Hit mouseDown(juce::Rectangle<int> area, juce::Point<int> at,
        const GenWaveRender::Wave* wave, double nowMs, bool popup);
    bool mouseDrag(juce::Rectangle<int> area, juce::Point<int> at,
        const GenWaveRender::Wave* wave, double nowMs);
    void mouseUp() { m_dragging = false; }
    bool mouseWheel(juce::Rectangle<int> area, juce::Point<int> at,
        const GenWaveRender::Wave* wave, double nowMs, float deltaY);

    bool isDragging() const { return m_dragging; }

    // 拡大率の選択肢を出す。
    //
    // 選んだ値は apply で返す。メニューは閉じるまで待たないので、その間に
    // この再生の持ち主が消えていることがある。値を当てる先は呼ぶ側が
    // 探し直すこと。
    static void showZoomMenu(juce::Component& owner, juce::Rectangle<int> area,
        int current, std::function<void(int)> apply);

    // 拡大率の欄そのもの (box) の下へ選択肢を出す
    static void showZoomMenuAt(juce::Component& owner, juce::Rectangle<int> box,
        int current, std::function<void(int)> apply);

    // コマの番号を打ち込む欄を、シークバーの下へ出す。横に全コマ数を添える。
    //
    // 打ち込んだ番号は、画面に出している番号 (1 から) のまま apply で返す。
    // 当てる先の探し直しは、拡大率のメニューと同じく呼ぶ側で行うこと。
    // zoomInRow には、その段に拡大率の欄を置いているか (setZoomInRow と同じ値) を渡す。
    static void showFrameInput(juce::Component& owner, juce::Rectangle<int> area,
        int current, int count, std::function<void(int)> apply, bool zoomInRow = true);

    // 拡大率の欄の区画。メニューを出す位置に使う。
    static juce::Rectangle<int> zoomArea(juce::Rectangle<int> area);
private:
    double m_startMs = 0.0;
    bool m_paused = false;
    int m_pausedFrame = 0;
    int m_zoom = zoomMin;
    bool m_dragging = false;
    bool m_zoomInRow = true;
    double m_fps = 0.0;

    struct Parts
    {
        juce::Rectangle<int> play;
        juce::Rectangle<int> seek;
        juce::Rectangle<int> zoom;
    };

    // withZoom が false なら zoom は空で、シークバーが右端まで伸びる
    static Parts partsOf(juce::Rectangle<int> area, bool withZoom = true);

    void togglePause(const GenWaveRender::Wave& wave, double nowMs);
    void seekTo(const GenWaveRender::Wave& wave, int frame, double nowMs);
    int frameAtX(juce::Rectangle<int> seek, int x, const GenWaveRender::Wave& wave) const;
};
