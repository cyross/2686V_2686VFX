#include "./GenWavePlayer.h"

#include <cmath>

#include "Shared/Core/Editor/EditorGenWaveText.h"
#include "Shared/Core/Gui/GuiColor.h"

namespace
{
    // コマの番号を打ち込む欄。シークバーを右クリックすると出る。
    //
    // 「フレーム [   ] / 全コマ数」の 1 行だけ。Enter で当て、Esc か
    // 外を押すと当てずに閉じる。
    class FrameInput : public juce::Component
    {
    public:
        FrameInput(int current, int count, std::function<void(int)> apply)
            : m_count(count), m_apply(std::move(apply))
        {
            title.setText(EditorGuiText::GenWave::frame, juce::dontSendNotification);
            title.setJustificationType(juce::Justification::centredRight);
            title.setColour(juce::Label::textColourId, GuiColor::GenWave::SeekText);
            addAndMakeVisible(title);

            editor.setText(juce::String(current), juce::dontSendNotification);
            editor.setInputRestrictions(6, "0123456789");
            editor.setJustification(juce::Justification::centredRight);
            editor.setSelectAllWhenFocused(true);
            editor.onReturnKey = [this] { commit(); };
            editor.onEscapeKey = [this] { dismiss(); };
            addAndMakeVisible(editor);

            total.setText("/ " + juce::String(count), juce::dontSendNotification);
            total.setColour(juce::Label::textColourId, GuiColor::GenWave::SeekText);
            addAndMakeVisible(total);

            setSize(titleWidth + editorWidth + totalWidth + gap * 2, rowHeight);
        }

        void resized() override
        {
            auto area = getLocalBounds();

            title.setBounds(area.removeFromLeft(titleWidth));
            area.removeFromLeft(gap);
            editor.setBounds(area.removeFromLeft(editorWidth));
            area.removeFromLeft(gap);
            total.setBounds(area);
        }

        void parentHierarchyChanged() override
        {
            // 出たらすぐ打てるように。出る前に頼んでも焦点は移らない。
            if (isShowing()) editor.grabKeyboardFocus();
        }

    private:
        static constexpr int titleWidth = 64;
        static constexpr int editorWidth = 64;
        static constexpr int totalWidth = 64;
        static constexpr int gap = 4;
        static constexpr int rowHeight = 24;

        juce::Label title;
        juce::TextEditor editor;
        juce::Label total;

        int m_count;
        std::function<void(int)> m_apply;

        void commit()
        {
            const int value = editor.getText().trim().getIntValue();

            // 範囲の外は端へ寄せる。打ち間違いで何も起きないより分かりやすい。
            if (m_apply && editor.getText().trim().isNotEmpty())
                m_apply(juce::jlimit(1, juce::jmax(1, m_count), value));

            dismiss();
        }

        void dismiss()
        {
            if (auto* box = findParentComponentOfClass<juce::CallOutBox>()) box->dismiss();
        }
    };

    constexpr int playWidth = 24;
    constexpr int zoomWidth = 44;
    constexpr int partGap = 4;

    // 周期の分からない波形のひとコマ (秒)
    constexpr double fallbackFrameSeconds = 1.0 / 30.0;

    // 押せないときの薄さ
    constexpr float disabledAlpha = 0.35f;
}

// ----------------------------------------------------------------------------
// コマ
// ----------------------------------------------------------------------------
double GenWavePlayer::frameSamples(const GenWaveRender::Wave& wave)
{
    const double perCycle = wave.samplesPerCycle();

    if (perCycle > 0.0) return perCycle;

    return juce::jmax(1.0, wave.sampleRate * fallbackFrameSeconds);
}

int GenWavePlayer::frameCount(const GenWaveRender::Wave& wave)
{
    if (wave.isEmpty()) return 1;

    return juce::jmax(1, (int)std::ceil((double)wave.size() / frameSamples(wave)));
}

void GenWavePlayer::restart(double nowMs)
{
    m_startMs = nowMs;
    m_paused = false;
    m_pausedFrame = 0;
    m_dragging = false;
}

int GenWavePlayer::frame(const GenWaveRender::Wave& wave, double nowMs) const
{
    const int count = frameCount(wave);

    if (m_paused) return juce::jlimit(0, count - 1, m_pausedFrame);

    // 経過秒を、作ったものの中の位置として読む。最後まで行ったら頭へ戻る。
    const double length = wave.lengthSeconds();

    if (length <= 0.0) return 0;

    const double at = std::fmod(juce::jmax(0.0, (nowMs - m_startMs) / 1000.0), length);

    return juce::jlimit(0, count - 1, (int)std::floor(at * wave.sampleRate / frameSamples(wave)));
}

int GenWavePlayer::windowStart(const GenWaveRender::Wave& wave, int window, double nowMs) const
{
    const int total = (int)wave.size();

    // 窓の頭は周期の切れ目へ合わせる。合わせないと 1 周期が 1 コマの
    // 間に何周も流れてしまい、形が読めない。オシロの同期と同じ考え方。
    return juce::jlimit(0, juce::jmax(0, total - window),
        (int)std::lround(frame(wave, nowMs) * frameSamples(wave)));
}

float GenWavePlayer::progress(const GenWaveRender::Wave& wave, double nowMs) const
{
    const int count = frameCount(wave);

    if (count <= 1) return 0.0f;

    return (float)frame(wave, nowMs) / (float)(count - 1);
}

void GenWavePlayer::togglePause(const GenWaveRender::Wave& wave, double nowMs)
{
    if (m_paused)
    {
        // 止めたコマから動かし直す。コマの真ん中から始めて、
        // 切り捨てで前のコマへ戻らないようにする。
        m_startMs = nowMs - (m_pausedFrame + 0.5) * frameSamples(wave) / wave.sampleRate * 1000.0;
        m_paused = false;

        return;
    }

    m_pausedFrame = frame(wave, nowMs);
    m_paused = true;
}

void GenWavePlayer::pause(const GenWaveRender::Wave* wave, double nowMs)
{
    if (m_paused) return;

    m_pausedFrame = (wave != nullptr && !wave->isEmpty()) ? frame(*wave, nowMs) : 0;
    m_paused = true;
}

void GenWavePlayer::resume(const GenWaveRender::Wave* wave, double nowMs)
{
    if (!m_paused) return;

    if (wave != nullptr && !wave->isEmpty())
    {
        togglePause(*wave, nowMs);

        return;
    }

    m_paused = false;
    m_startMs = nowMs;
}

void GenWavePlayer::showFrame(const GenWaveRender::Wave& wave, int target, double nowMs)
{
    // 打ち込んでまで見たいコマなので、流れていかないよう止めて出す
    m_paused = true;
    m_pausedFrame = juce::jlimit(0, frameCount(wave) - 1, target);
    m_dragging = false;

    juce::ignoreUnused(nowMs);
}

void GenWavePlayer::seekTo(const GenWaveRender::Wave& wave, int target, double nowMs)
{
    const int clamped = juce::jlimit(0, frameCount(wave) - 1, target);

    if (m_paused)
    {
        m_pausedFrame = clamped;

        return;
    }

    // 動かしているときは、そのコマから続けて動く
    m_startMs = nowMs - (clamped + 0.5) * frameSamples(wave) / wave.sampleRate * 1000.0;
}

// ----------------------------------------------------------------------------
// 区画
// ----------------------------------------------------------------------------
GenWavePlayer::Parts GenWavePlayer::partsOf(juce::Rectangle<int> area)
{
    Parts parts;

    parts.play = area.removeFromLeft(playWidth);
    area.removeFromLeft(partGap);
    parts.zoom = area.removeFromRight(zoomWidth);
    area.removeFromRight(partGap);
    parts.seek = area;

    return parts;
}

juce::Rectangle<int> GenWavePlayer::zoomArea(juce::Rectangle<int> area)
{
    return partsOf(area).zoom;
}

int GenWavePlayer::frameAtX(juce::Rectangle<int> seek, int x, const GenWaveRender::Wave& wave) const
{
    const int count = frameCount(wave);

    if (count <= 1 || seek.getWidth() <= 1) return 0;

    const double ratio = juce::jlimit(0.0, 1.0,
        (double)(x - seek.getX()) / (double)(seek.getWidth() - 1));

    return (int)std::lround(ratio * (count - 1));
}

// ----------------------------------------------------------------------------
// 描く
// ----------------------------------------------------------------------------
void GenWavePlayer::draw(juce::Graphics& g, juce::Rectangle<int> area,
    const GenWaveRender::Wave* wave, double nowMs) const
{
    const auto parts = partsOf(area);
    const bool timed = (wave != nullptr && !wave->isEmpty());
    const float alpha = timed ? 1.0f : disabledAlpha;

    // --- 一時停止 / 再生 ---
    // 今の状態ではなく、押したらどうなるかを描く。止まっていれば ▶、
    // 動いていれば ❚❚。字形はフォントによって崩れるので線で描く。
    {
        const auto box = parts.play.toFloat();

        g.setColour(GuiColor::GenWave::CycleBg.get().withMultipliedAlpha(alpha));
        g.fillRect(box);
        g.setColour(GuiColor::GenWave::Border.get().withMultipliedAlpha(alpha));
        g.drawRect(box, 1.0f);

        const auto icon = box.withSizeKeepingCentre(10.0f, 10.0f);

        g.setColour(GuiColor::GenWave::ControlIcon.get().withMultipliedAlpha(alpha));

        if (!timed || m_paused)
        {
            juce::Path triangle;

            triangle.addTriangle(icon.getX() + 1.0f, icon.getY(),
                icon.getX() + 1.0f, icon.getBottom(),
                icon.getRight(), icon.getCentreY());

            g.fillPath(triangle);
        }
        else
        {
            g.fillRect(icon.withWidth(3.5f));
            g.fillRect(icon.withTrimmedLeft(icon.getWidth() - 3.5f));
        }
    }

    // --- シークバー ---
    // 全高の帯にして、進んだぶんを塗り、今のコマを縦線で示す。
    // コマの番号は帯の真ん中へ重ねて出す。
    {
        const auto box = parts.seek.toFloat();

        g.setColour(GuiColor::GenWave::CycleBg.get().withMultipliedAlpha(alpha));
        g.fillRect(box);

        if (timed)
        {
            const int count = frameCount(*wave);
            const int now = frame(*wave, nowMs);
            const float ratio = (count > 1) ? (float)now / (float)(count - 1) : 0.0f;
            const float x = box.getX() + (box.getWidth() - 1.0f) * ratio;

            g.setColour(GuiColor::GenWave::SeekFill);
            g.fillRect(box.withRight(x));

            g.setColour(GuiColor::GenWave::Progress);
            g.fillRect(x - 1.0f, box.getY(), 2.0f, box.getHeight());

            g.setColour(GuiColor::GenWave::SeekText);
            g.setFont(juce::FontOptions(10.0f));
            g.drawText(juce::String(now + 1) + " / " + juce::String(count),
                parts.seek, juce::Justification::centred, false);
        }

        g.setColour(GuiColor::GenWave::Border.get().withMultipliedAlpha(alpha));
        g.drawRect(box, 1.0f);
    }

    // --- 縦の拡大率 ---
    // 止め絵でも効くので、いつでも押せる。
    {
        const auto box = parts.zoom;

        g.setColour(GuiColor::GenWave::CycleBg);
        g.fillRect(box);
        g.setColour(GuiColor::GenWave::Border);
        g.drawRect(box, 1);

        g.setColour(m_zoom > zoomMin ? GuiColor::GenWave::Progress.get()
                                     : GuiColor::GenWave::ControlIcon.get());
        g.setFont(juce::FontOptions(11.0f, juce::Font::bold));
        g.drawText(EditorGuiText::GenWave::zoomPrefix + juce::String(m_zoom),
            box.withTrimmedRight(10), juce::Justification::centred, false);

        // 選択肢があることを示す小さな ▼
        const auto mark = box.withTrimmedLeft(box.getWidth() - 12).toFloat().withSizeKeepingCentre(6.0f, 4.0f);

        juce::Path down;

        down.addTriangle(mark.getX(), mark.getY(), mark.getRight(), mark.getY(),
            mark.getCentreX(), mark.getBottom());

        g.setColour(GuiColor::GenWave::ControlIcon);
        g.fillPath(down);
    }
}

// ----------------------------------------------------------------------------
// マウス
// ----------------------------------------------------------------------------
GenWavePlayer::Hit GenWavePlayer::mouseDown(juce::Rectangle<int> area, juce::Point<int> at,
    const GenWaveRender::Wave* wave, double nowMs, bool popup)
{
    if (!area.contains(at)) return Hit::none;

    const auto parts = partsOf(area);

    if (parts.zoom.contains(at)) return Hit::zoomMenu;

    const bool timed = (wave != nullptr && !wave->isEmpty());

    // 時間の軸が無いときも、段の上で押したものは受け取ったことにする。
    // 下の行が選ばれたり、別のものが動いたりしないように。
    if (!timed) return Hit::handled;

    if (parts.play.contains(at))
    {
        togglePause(*wave, nowMs);

        return Hit::handled;
    }

    if (parts.seek.contains(at))
    {
        if (popup) return Hit::frameInput;

        m_dragging = true;

        seekTo(*wave, frameAtX(parts.seek, at.getX(), *wave), nowMs);
    }

    return Hit::handled;
}

bool GenWavePlayer::mouseDrag(juce::Rectangle<int> area, juce::Point<int> at,
    const GenWaveRender::Wave* wave, double nowMs)
{
    if (!m_dragging) return false;
    if (wave == nullptr || wave->isEmpty()) return false;

    seekTo(*wave, frameAtX(partsOf(area).seek, at.getX(), *wave), nowMs);

    return true;
}

bool GenWavePlayer::mouseWheel(juce::Rectangle<int> area, juce::Point<int> at,
    const GenWaveRender::Wave* wave, double nowMs, float deltaY)
{
    if (!area.contains(at) || deltaY == 0.0f) return false;

    const int step = (deltaY > 0.0f) ? 1 : -1;
    const auto parts = partsOf(area);

    if (parts.zoom.contains(at))
    {
        setZoom(m_zoom + step);

        return true;
    }

    if (!parts.seek.contains(at)) return true;
    if (wave == nullptr || wave->isEmpty()) return true;

    // ホイールはコマ送り。1 コマずつ見たいときの操作なので、止めてから送る。
    if (!m_paused)
    {
        m_pausedFrame = frame(*wave, nowMs);
        m_paused = true;
    }

    seekTo(*wave, m_pausedFrame + step, nowMs);

    return true;
}

void GenWavePlayer::showZoomMenu(juce::Component& owner, juce::Rectangle<int> area,
    int current, std::function<void(int)> apply)
{
    juce::PopupMenu menu;

    for (int value = zoomMin; value <= zoomMax; ++value)
    {
        menu.addItem(value, EditorGuiText::GenWave::zoomPrefix + juce::String(value), true, value == current);
    }

    menu.showMenuAsync(juce::PopupMenu::Options()
        .withTargetComponent(&owner)
        .withTargetScreenArea(owner.localAreaToGlobal(zoomArea(area))),
        [apply = std::move(apply)](int chosen) {
            if (chosen >= zoomMin && chosen <= zoomMax && apply) apply(chosen);
        });
}

void GenWavePlayer::showFrameInput(juce::Component& owner, juce::Rectangle<int> area,
    int current, int count, std::function<void(int)> apply)
{
    // 吹き出しは画面のいちばん上の部品へ載せる。プレビューの部品は小さく、
    // その中へ出すと切れてしまう。
    auto* parent = owner.getTopLevelComponent();

    if (parent == nullptr) return;

    const auto seek = partsOf(area).seek;

    juce::CallOutBox::launchAsynchronously(
        std::make_unique<FrameInput>(current, count, std::move(apply)),
        parent->getLocalArea(&owner, seek), parent);
}
