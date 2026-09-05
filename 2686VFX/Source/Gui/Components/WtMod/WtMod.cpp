#include "./WtMod.h"

#include "../../../Core/Gui/GuiRefresh.h"

#include "../../../Core/Io/ParamFile.h"

#include "../WavePreview/WavePreviewSource.h"

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorKeys.h"
#include "../../../Core/Editor/PluginEditor.h"
#include "../../../Core/Gui/GuiHelpers.h"
#include "../../../Core/Gui/GuiStructs.h"
#include "../../../Core/Const/ConstGlobal.h"
#include "../../../Core/Const/ConstFileValues.h"
#include "../../../Core/Gui/GuiColor.h"
#include "../../../Core/Gui/GuiValues.h"
#include "../../../Core/Synth/CommonParams.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat wtmodFormat{ "wtmod", 1 };
}

static std::vector<SelectItem> wtModShapeItems = {
    {.name = "0: Sine",            .value = 1 },
    {.name = "1: FDS Triangle",    .value = 2 },
    {.name = "2: FDS Saw",         .value = 3 },
    {.name = "3: FDS Reset",       .value = 4 },
    {.name = "4: FDS Pulse",       .value = 5 },
    {.name = "5: WS Sweep Up",     .value = 6 },
    {.name = "6: WS Sweep Down",   .value = 7 },
    {.name = "7: HuC6280 Wave",    .value = 8 },
    {.name = "8: FDS Table",       .value = 9 },
};

namespace WtModGuiValue
{
	// エディタの高さ。上段が増減値、下段が積算後の階段波。
	namespace Editor
	{
		static inline constexpr int height = 96;
	}
}

static const juce::String fdsPresetNames[FdsMod::tableCount] = { "Tri", "Saw", "Rst", "Pls" };

// ==========================================================
// FdsTableEditor
// ==========================================================
// 3bit のレジスタ値を、増減量の小さい順に並べたときの位置へ写す表。
// 4 (リセット) は増減量の軸に乗らないのでここには入れない。
static constexpr int fdsLevelToEntry[7] = { 5, 6, 7, 0, 1, 2, 3 };  // -4,-2,-1,0,+1,+2,+4
static constexpr int fdsEntryToLevel[8] = { 3, 4, 5, 6, -1, 0, 1, 2 };

void FdsTableEditor::setup(juce::Component& parent, const juce::String& idPrefix)
{
    parent.addAndMakeVisible(this);

    attachParams(idPrefix, 32);
}

std::array<int, 32> FdsTableEditor::currentTable() const
{
    std::array<int, 32> table = { 0 };

    for (int i = 0; i < 32 && i < (int)m_params.size(); ++i) {
        table[i] = (int)std::round(m_params[i]->convertFrom0to1(m_params[i]->getValue()));
    }

    return table;
}

void FdsTableEditor::loadTable(const std::array<int, 32>& table)
{
    ctx.audioProcessor.undoManager.beginNewTransaction();

    for (int i = 0; i < 32 && i < (int)m_params.size(); ++i) {
        m_params[i]->setValueNotifyingHost(m_params[i]->convertTo0to1((float)table[i]));
    }

    repaint();
}

void FdsTableEditor::updateSliderValue(const juce::MouseEvent& e)
{
    if (!isEnabledState || m_params.empty()) return;

    float stepWidth = (float)getWidth() / 32.0f;
    int index = std::clamp((int)(e.position.x / stepWidth), 0, (int)m_params.size() - 1);

    int entry;

    if (e.mods.isRightButtonDown()) {
        // 右クリックはリセットエントリの ON/OFF
        int current = (int)std::round(m_params[index]->convertFrom0to1(m_params[index]->getValue()));
        entry = (current == 4) ? 0 : 4;
    }
    else {
        // 増減量の段は 7 段。上が +4、下が -4。
        int barHeight = getHeight() / 2;
        float t = 1.0f - (e.position.y / (float)barHeight);
        int level = std::clamp((int)std::round(t * 6.0f), 0, 6);
        entry = fdsLevelToEntry[level];
    }

    m_params[index]->setValueNotifyingHost(m_params[index]->convertTo0to1((float)entry));
}

void FdsTableEditor::updateHoverState(const juce::MouseEvent& e)
{
    int index = std::clamp((int)(e.position.x / ((float)getWidth() / 32.0f)), 0, 31);

    lastMousePos = e.position.toInt();
    lastModifiers = e.mods;

    if (index != hoveredIndex) {
        hoveredIndex = index;
    }

    repaint();
}

void FdsTableEditor::mouseDown(const juce::MouseEvent& e)
{
    ctx.audioProcessor.undoManager.beginNewTransaction();

    updateSliderValue(e);
    updateHoverState(e);
}

void FdsTableEditor::mouseDrag(const juce::MouseEvent& e)
{
    updateSliderValue(e);
    updateHoverState(e);
}

void FdsTableEditor::paint(juce::Graphics& g)
{
    auto table = currentTable();

    int barHeight = getHeight() / 2;
    float stepWidth = (float)getWidth() / 32.0f;

    juce::Colour track = GuiColor::WaveformContainer::Track;
    juce::Colour thumb = GuiColor::WaveformContainer::Thumb;

    if (!isEnabledState) {
        track = track.withAlpha(0.2f);
        thumb = thumb.withAlpha(0.3f);
    }

    // ---------------- 上段 : 増減値 ----------------
    float zeroY = (float)barHeight * 0.5f;

    g.setColour(track);
    g.drawHorizontalLine((int)zeroY, 0.0f, (float)getWidth());

    for (int i = 0; i < 32; ++i)
    {
        float x = i * stepWidth;
        auto cell = juce::Rectangle<float>(x, 0.0f, stepWidth - 1.0f, (float)barHeight);

        if (i == hoveredIndex) {
            g.setColour(track.withAlpha(0.15f));
            g.fillRect(cell);
        }

        if (table[i] == 4) {
            // リセットは高さを持たないので、縦棒 1 本で区切りとして描く
            g.setColour(GuiColor::WaveformContainer::ResetBtn::To1);
            g.fillRect(x, 0.0f, stepWidth - 1.0f, (float)barHeight);
            continue;
        }

        int level = fdsEntryToLevel[table[i]];
        if (level < 0) continue;

        // level 3 が増減 0。そこから上下に伸ばす。
        float t = ((float)level - 3.0f) / 3.0f;
        float y = zeroY - t * zeroY;

        g.setColour(thumb);
        g.fillRect(juce::Rectangle<float>(x, std::min(y, zeroY), stepWidth - 1.0f,
                                          std::max(std::fabs(zeroY - y), 1.0f)));
    }

    // ---------------- 下段 : 積算後の階段波 ----------------
    auto steps = FdsMod::makeSteps(table);

    float baseY = (float)barHeight;
    float centerY = baseY + (float)barHeight * 0.5f;

    g.setColour(track);
    g.drawHorizontalLine((int)centerY, 0.0f, (float)getWidth());

    // 積算後の階段波は「変調のかかり方」を示すものなので、
    // 付いている変調のプレビューと同じ色で描く。
    g.setColour(stepColourEntry->get().withAlpha(isEnabledState ? 0.8f : 0.3f));

    for (int i = 0; i < 32; ++i)
    {
        float x = i * stepWidth;
        float y = centerY - steps[i] * ((float)barHeight * 0.45f);

        g.fillRect(x, y - 1.0f, stepWidth - 1.0f, 2.0f);
    }
}

void FdsTableEditor::paintOverChildren(juce::Graphics& g)
{
    if (hoveredIndex < 0 || !isEnabledState || m_params.empty()) return;

    auto table = currentTable();

    // [index] 増減量 で出す。リセットは RST。
    juce::String text = "[" + juce::String(hoveredIndex) + "] ";

    if (table[hoveredIndex] == 4) {
        text += "RST";
    }
    else {
        int inc = FdsMod::decodeIncrement(table[hoveredIndex]);
        text += (inc >= 0 ? "+" : "") + juce::String(inc);
    }

    paintHoverText(g, text);
}

void GuiComponentWtMod::setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder,
    juce::Colour categoryBg)
{
    // 変調波形はプロセッサが持っているので、引き当ての鍵だけ覚えておく
    m_code = code;

    cat.setupCategory({ .parent = parent, .title = juce::String("") + "WT PITCH MOD", .enableChangeDetailVisible = true }, categoryBg);

    enableButton.setup({ .parent = parent, .id = code + CPK::WtMod::enable, .title = "Enable", .isReset = true, .isResized = true });
    enableButton.setWantsKeyboardFocus(true);
    enableButton.setExplicitFocusOrder(++tabOrder);

    depthSlider.setup({ .parent = parent, .id = code + CPK::WtMod::depth, .title = "DPTH", .isReset = true });
    depthSlider.setWantsKeyboardFocus(true);
    depthSlider.setExplicitFocusOrder(++tabOrder);

    speedSlider.setup({ .parent = parent, .id = code + CPK::WtMod::speed, .title = "SPED", .isReset = true });
    speedSlider.setWantsKeyboardFocus(true);
    speedSlider.setExplicitFocusOrder(++tabOrder);

    shapeSelector.setup({ .parent = parent, .id = code + CPK::WtMod::shape, .title = "SHPE", .items = wtModShapeItems, .isReset = true, .isResized = true });
    shapeSelector.setWantsKeyboardFocus(true);
    shapeSelector.setExplicitFocusOrder(++tabOrder);

    // setup が .isResized で入れた再レイアウト用の処理を潰さないよう、
    // 元の呼び出しを拾ってから後ろへつなぐ。
    auto relayoutOnShapeChange = shapeSelector.onChange;

    shapeSelector.onChange = [this, relayoutOnShapeChange] {
        if (relayoutOnShapeChange) relayoutOnShapeChange();

        this->updateModPreview();
        };

    // 読み込み行とプレビューをスロットの数だけ作る
    // 並びは 対象 → 読み込み / 名前 / 消去 → 各スロットの波形。
    slotTarget.setup({ .parent = parent, .title = "TGT", .isReset = false });
    slotTarget.setRange(0.0, (double)(Global::WtMod::slots - 1), 1.0);
    slotTarget.setNumDecimalPlacesToDisplay(0);
    slotTarget.setWantsKeyboardFocus(true);
    slotTarget.setExplicitFocusOrder(++tabOrder);
    slotTarget.onValueChange = [this] { applySlotTarget(); };

    slotWtBtn.setup({ .parent = parent, .title = "WT", .bgColor = juce::Colours::darkgrey.brighter(0.2f), .isReset = false, .isResized = true });
    slotWtBtn.setWantsKeyboardFocus(true);
    slotWtBtn.setExplicitFocusOrder(++tabOrder);
    slotWtBtn.onClick = [this] { importWave(targetSlot(), false); };

    slotWt2Btn.setup({ .parent = parent, .title = "W2", .bgColor = juce::Colours::darkgrey.brighter(0.2f), .isReset = false, .isResized = true });
    slotWt2Btn.setWantsKeyboardFocus(true);
    slotWt2Btn.setExplicitFocusOrder(++tabOrder);
    slotWt2Btn.onClick = [this] { importWave(targetSlot(), true); };

    slotClearBtn.setup({ .parent = parent, .title = "Clear", .textColor = juce::Colours::white, .bgColor = juce::Colours::darkred.withAlpha(0.7f), .isReset = false, .isResized = true });
    slotClearBtn.setWantsKeyboardFocus(true);
    slotClearBtn.setExplicitFocusOrder(++tabOrder);
    slotClearBtn.onClick = [this] { clearWave(targetSlot()); };

    slotFileNameLabel.setup({ .parent = parent, .title = Io::empty });

    slotPreviews.setup(parent, GuiColor::WavePreview::WaveMemory, Global::WtMod::slots);

    for (int i = 0; i < Global::WtMod::slots; ++i) updateSlotPreview(i);

    applySlotTarget();

    slotPreviews.setActive(currentSlot());

    waveSmoothBtn.setup({ .parent = parent, .id = code + CPK::WtMod::waveSmooth, .title = "Smooth", .isReset = true, .isResized = true });
    waveSmoothBtn.setWantsKeyboardFocus(true);
    waveSmoothBtn.setExplicitFocusOrder(++tabOrder);

    // setup が .isResized で入れた再レイアウト用の処理を潰さないよう、
    // 元の呼び出しを拾ってから後ろへつなぐ。
    //
    // onStateChange ではなく onClick に付けているのは、パラメータの
    // 復元でスイッチが動いたときまで取り込み直してしまわないため。
    auto relayoutOnSmooth = waveSmoothBtn.onClick;

    waveSmoothBtn.onClick = [this, relayoutOnSmooth] {
        if (relayoutOnSmooth) relayoutOnSmooth();

        this->reapplyWaveFiles();
        };

    // 使うスロットの切り替え。パラメータなので演奏中でも動かせる。
    waveSlotSlider.setup({ .parent = parent, .id = code + CPK::WtMod::waveSlot, .title = "SLOT", .isReset = true });
    waveSlotSlider.setWantsKeyboardFocus(true);
    waveSlotSlider.setExplicitFocusOrder(++tabOrder);

    // 切り替えたら Shape のプレビューもその波形に合わせる
    waveSlotSlider.onValueChange = [this] {
        this->updateModPreview();

        // どれが鳴っているのかは並べた波形のほうにも印を付ける。
        this->slotPreviews.setActive(this->currentSlot());
        };


    modPreview.setup(parent, GuiColor::WavePreview::PitchEnv);
    updateModPreview();

    fdsCat.setupSwPitchCategory({ .parent = parent, .title = juce::String("") + "FDS PITCH TABLE", .enableChangeDetailVisible = true });

    fdsEditor.setup(parent, code + CPK::WtMod::fdsTable);
    fdsEditor.onParamChanged = [this] { this->updateModPreview(); };

    // 作り置きのテーブルを流し込むボタン
    for (int i = 0; i < FdsMod::tableCount; ++i)
    {
        fdsPresetBtn[i].setup({ .parent = parent, .title = fdsPresetNames[i], .bgColor = juce::Colours::darkgrey.brighter(0.2f), .isReset = false, .isResized = true });
        fdsPresetBtn[i].setWantsKeyboardFocus(true);
        fdsPresetBtn[i].setExplicitFocusOrder(++tabOrder);
        fdsPresetBtn[i].onClick = [this, i] { fdsEditor.loadTable(FdsMod::tables[i]); };
    }
}

void GuiComponentWtMod::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &cat });

    bool visible = cat.isDetailVisible();

    enableButton.setVisible(visible);
    depthSlider.setVisibleWithLabel(visible);
    speedSlider.setVisibleWithLabel(visible);
    shapeSelector.setVisibleWithLabel(visible);
    waveSmoothBtn.setVisible(visible);
    waveSlotSlider.setVisibleWithLabel(visible);
    modPreview.setVisible(visible);

    slotTarget.setVisibleWithLabel(visible);
    slotWtBtn.setVisible(visible);
    slotWt2Btn.setVisible(visible);
    slotClearBtn.setVisible(visible);
    slotFileNameLabel.setVisible(visible);
    slotPreviews.setVisible(visible);

    if (visible)
    {
        layoutMain({ .mainRect = rect, .component = &enableButton });
        layoutMain({ .mainRect = rect, .label = &depthSlider.label, .component = &depthSlider });
        layoutMain({ .mainRect = rect, .label = &speedSlider.label, .component = &speedSlider, });
        layoutMain({ .mainRect = rect, .label = &shapeSelector.label, .component = &shapeSelector, });

        // 選んでいる Shape の形。9 種すべてに効くので、読み込み行より前に置く。
        modPreview.setBounds(rect.removeFromTop(GuiWavePreview::defaultHeight));
        rect.removeFromTop(2);

        layoutMain({ .mainRect = rect, .component = &waveSmoothBtn });
        layoutMain({ .mainRect = rect, .label = &waveSlotSlider.label, .component = &waveSlotSlider });

        // 読み込み行とプレビューを 1 組にして、スロットの数だけ並べる
        layoutMain({ .mainRect = rect, .label = &slotTarget.label, .component = &slotTarget });

        layoutMainWtFiles({ .rect = rect,
                            .loadWtBtn = &slotWtBtn,
                            .loadWt2Btn = &slotWt2Btn,
                            .fileNameLabel = &slotFileNameLabel,
                            .clearBtn = &slotClearBtn });

        slotPreviews.setBounds(rect.removeFromTop(slotPreviews.getNaturalHeight()));
        rect.removeFromTop(2);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }

    // Enable が OFF のときは中身を触れなくする
    bool isMod = enableButton.getToggleState();
    depthSlider.setEnabledWithLabel(isMod);
    speedSlider.setEnabledWithLabel(isMod);
    shapeSelector.setEnabledWithLabel(isMod);
    waveSmoothBtn.setEnabled(isMod);
    waveSlotSlider.setEnabledWithLabel(isMod);

    slotTarget.setEnabledWithLabel(isMod);
    slotWtBtn.setEnabled(isMod);
    slotWt2Btn.setEnabled(isMod);
    slotClearBtn.setEnabled(isMod);
    slotFileNameLabel.setEnabled(isMod);
    // ---------------- FDS TABLE ----------------
    // MODULATION の中の小見出しなので、親を畳んだらこちらも隠す。
    fdsCat.setVisible(visible);

    if (visible) {
        layoutMainCategory({ .mainRect = rect, .label = &fdsCat });
    }

    bool fdsVisible = visible && fdsCat.isDetailVisible();

    fdsEditor.setVisible(fdsVisible);
    for (auto& btn : fdsPresetBtn) btn.setVisible(fdsVisible);

    if (fdsVisible)
    {
        // プリセット 4 つを 1 行に並べる
        auto btnRow = rect.removeFromTop(CoreGuiValue::MainGroup::Row::height);
        rect.removeFromTop(CoreGuiValue::MainGroup::Row::paddingTop);

        int btnWidth = (btnRow.getWidth() - 3 * 4) / 4;

        for (int i = 0; i < FdsMod::tableCount; ++i)
        {
            fdsPresetBtn[i].setBounds(btnRow.removeFromLeft(btnWidth));
            btnRow.removeFromLeft(4);
        }

        fdsEditor.setBounds(rect.removeFromTop(WtModGuiValue::Editor::height));
        rect.removeFromTop(CoreGuiValue::MainGroup::Row::paddingTop);

		rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }

    // 編集できるのは FdsUser を選んでいるときだけ
    bool isFdsUser = isMod && (shapeSelector.getSelectedItemIndex() == (int)WtModShape::FdsUser);

    fdsEditor.setCustomEnabled(isFdsUser);
    for (auto& btn : fdsPresetBtn) btn.setEnabled(isFdsUser);
}

void GuiComponentWtMod::importWave(int slot, bool isWt2)
{
    juce::File defaultDir(ctx.audioProcessor.defaultWavetableDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    ctx.editor.openFileChooser(
        isWt2 ? "Load Mod Wave (.wt2)" : "Load Mod Wave (.wt)",
        defaultDir,
        isWt2 ? "*.wt2" : "*.wt",
        [this, slot](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (!file.existsAsFile()) return;

            // 出すのは対象のスロットのときだけ。読み込み中は名前欄で示す。
            if (slot == targetSlot()) slotFileNameLabel.setText("Loading...", juce::dontSendNotification);

            // 発火するころには画面が消えているかもしれないので、弱い参照で見張る。
            juce::Component::SafePointer<std::remove_pointer_t<decltype(this)>> safe(this);

            juce::Timer::callAfterDelay(50, [this, safe, slot, file]()
                {
                    // 画面が閉じられていたら何もしない。callAfterDelay は取り消せず、
                    // メッセージが詰まっていれば 50ms よりずっと遅れて発火する。
                    if (safe == nullptr) return;

                    // 読み込みと 32 サンプルへの落とし込みはプロセッサが行う。
                    // 実データを持っているのがあちら側だから。
                    ctx.audioProcessor.loadWtModWaveFile(m_code, slot, file);

                    updateSlotFileName(slot);

                    ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();
                });
        }
    );
}


// Smooth は「読み込んだ波形を 32 サンプルへ落とす方法」を選ぶスイッチで、
// 取り込んだあとの値には掛からない。切り替えただけでは何も変わらないと
// 分かりにくいので、読み込んだファイルを覚えているうちは取り込み直す。


// 今どのスロットを触っているか。
// 演奏中に切り替えられるよう、パラメータとして持っている。
int GuiComponentWtMod::currentSlot() const
{
    if (auto* p = ctx.audioProcessor.apvts.getRawParameterValue(m_code + CPK::WtMod::waveSlot)) {
        return std::clamp((int)p->load(), 0, Global::WtMod::slots - 1);
    }

    return 0;
}
void GuiComponentWtMod::clearWave(int slot)
{
    ctx.audioProcessor.unloadWtModWaveFile(m_code, slot);

    updateSlotFileName(slot);
}

// Smooth は「読み込んだ波形を 32 サンプルへ落とす方法」を選ぶスイッチで、
// 取り込んだあとの値には掛からない。切り替えたら、覚えているファイルから
// 読み込み直す。
void GuiComponentWtMod::reapplyWaveFiles()
{
    for (int i = 0; i < Global::WtMod::slots; ++i)
    {
        juce::File file(wavePath(i));

        if (!file.existsAsFile()) continue;

        ctx.audioProcessor.loadWtModWaveFile(m_code, i, file);

        updateSlotPreview(i);
    }

    updateModPreview();
}

void GuiComponentWtMod::applySlotTarget()
{
    const int slot = targetSlot();

    slotPreviews.setSelected(slot);

    updateSlotFileName(slot);
}

void GuiComponentWtMod::updateSlotFileName(int slot)
{
    juce::String path = wavePath(slot);

    // 名前を出すのは対象のスロットだけ。他は帯の波形で分かる。
    if (slot == targetSlot()) {
        slotFileNameLabel.setText(
            juce::String(slot) + ": " + (path.isEmpty() ? Io::empty : juce::File(path).getFileName()),
            juce::dontSendNotification);
    }

    // 名前とプレビューは常に同じ波形を指していてほしいので、ここで揃える
    updateSlotPreview(slot);

    // 使っているスロットが変わったのなら、Shape のプレビューも合わせる
    if (slot == currentSlot()) updateModPreview();
}

// 読み込んだ 32 サンプルを折れ線にする。
// 音源側は補間せず index で拾うので、階段として描く。
void GuiComponentWtMod::updateSlotPreview(int slot)
{
    auto it = ctx.audioProcessor.modWaveSlots.find(m_code);

    if (it == ctx.audioProcessor.modWaveSlots.end() || !it->second[(size_t)slot].hasData) {
        slotPreviews.setPoints(slot, {});

        return;
    }

    // 1 サンプルを平らに伸ばして階段にする
    constexpr int holdWidth = 8;

    const auto& wave = it->second[(size_t)slot].data;

    std::vector<float> points;
    points.reserve(wave.size() * holdWidth);

    for (float v : wave) {
        for (int k = 0; k < holdWidth; ++k) points.push_back(v);
    }

    // 変調波形は -1.0〜1.0 の両振り
    slotPreviews.setPoints(slot, points);
}

juce::String GuiComponentWtMod::wavePath(int slot) const
{
    auto it = ctx.audioProcessor.modWavePaths.find(m_code);

    if (it == ctx.audioProcessor.modWavePaths.end()) return juce::String();

    return it->second[(size_t)slot];
}



// 選んでいる Shape の変調波形を折れ線にする。
// 描画のたびに計算すると重いので、形が変わったときだけここを通す。
void GuiComponentWtMod::updateModPreview()
{
    // 読み込み中は溜めておき、読み終えてから 1 度だけ作り直す
    if (GuiRefresh::defer(this, [this] { updateModPreview(); })) return;

    // HuC6280 モードで使う 32 サンプル。実データはプロセッサが持つ。
    std::array<float, Global::WtMod::waveSize> wave = { 0.0f };

    auto it = ctx.audioProcessor.modWaveSlots.find(m_code);

    if (it != ctx.audioProcessor.modWaveSlots.end()) {
        wave = it->second[(size_t)currentSlot()].data;
    }

    // 変調の向きは Shape によって上下どちらにも振れるので、両振りで描く
    modPreview.setPoints(
        WavePreviewSource::wtMod(shapeSelector.getSelectedItemIndex(), wave, fdsEditor.currentTable()),
        true);
}

void GuiComponentWtMod::setImportingBaseParams(juce::StringArray& lines, int& index)
{
    enableButton.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);
    depthSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
    speedSlider.setValue(lines[index++].getFloatValue(), juce::sendNotification);
}

juce::String GuiComponentWtMod::getExportedBaseParams()
{
    juce::String content = "";

    content += juce::String(enableButton.getToggleState() ? 1 : 0) + "\n";
    content += juce::String(depthSlider.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(speedSlider.getValue(), Global::floatDecimalPlaces) + "\n";

    return content;
}

// Modulation Shape は後から追加したパラメータなので、旧フォーマットのファイルと
// 互換を保つためファイル末尾に置いてある。行が無ければ既定(正弦波)のままにする。
void GuiComponentWtMod::setImportingShapeParam(juce::StringArray& lines, int& index)
{
    if (index < lines.size()) {
        shapeSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
    }
}

juce::String GuiComponentWtMod::getExportedShapeParam()
{
    return juce::String(shapeSelector.getSelectedItemIndex()) + "\n";
}

void GuiComponentWtMod::setImportingParams(juce::StringArray& lines, int& index)
{
    setImportingBaseParams(lines, index);

    shapeSelector.setSelectedItemIndex(lines[index++].getIntValue(), juce::sendNotification);
    waveSmoothBtn.setToggleState(lines[index++].getIntValue() == 1, juce::sendNotification);

    std::array<int, 32> table = { 0 };

    for (int i = 0; i < 32; ++i) {
        table[i] = std::clamp(lines[index++].getIntValue(), 0, 7);
    }

    fdsEditor.loadTable(table);
}

juce::String GuiComponentWtMod::getExportedParams()
{
    juce::String content = getExportedBaseParams();

    content += juce::String(shapeSelector.getSelectedItemIndex()) + "\n";
    content += juce::String(waveSmoothBtn.getToggleState() ? 1 : 0) + "\n";

    auto table = fdsEditor.currentTable();

    for (int i = 0; i < 32; ++i) {
        content += juce::String(table[i]) + "\n";
    }

    return content;
}

void GuiComponentWtMod::importParams()
{
    juce::File defaultDir(ctx.audioProcessor.defaultWtModParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importWtModParamFile, defaultDir, Io::ExtensionGlob::WtModParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultWtModParamDir = file.getParentDirectory().getFullPathName();

                // 3.0.0 より前のファイルは、当時の処理で読み込んでから
                // 新しい形式へ書き出す。並び順を写し直すと取り違えるので、
                // 読み込みは当時のものをそのまま使う。
                if (Io::isLegacyFile(file)) {
                    juce::StringArray lines;

                    file.readLines(lines);

                    int index = 0;

                    {
                        // 読み終えてからまとめて描き直す
                        GuiRefresh::Batch batch;

                        setImportingParams(lines, index);
                    }

                    // 単体のファイルは入れ子にせず、そのまま中身として書く
                    Io::ParamWriter writer(wtmodFormat);

                    writeParams(writer, Io::ParamKey::values);
                    writer.hoist(Io::ParamKey::values);

                    Io::writeConverted(file, writer);

                    return;
                }

                auto reader = Io::ParamReader::open(file, wtmodFormat);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す
                GuiRefresh::Batch batch;

                // チャンネルファイルの中に入る形と同じ中身にしてある
                readParams(*reader, "wtMod");
            }
        });
}

void GuiComponentWtMod::exportParams()
{
    juce::File defaultDir(ctx.audioProcessor.defaultWtModParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportWtModParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::WtModParam)), Io::saveGlob(Io::Extension::WtModParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultWtModParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(wtmodFormat);

                writeParams(writer, "wtMod");

                writer.writeTo(file);
            }
        });
}

// 名前で持つので、後から足した項目を末尾へ置く必要がなくなった。
// 基本の値と Shape を分けて渡していたのも、まとめて 1 つで足りる。
void GuiComponentWtMod::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

    enableButton.setToggleState(r.getBool("enable", enableButton.getToggleState()), juce::sendNotification);
    depthSlider.setValue(r.getFloat("depth", (float)depthSlider.getValue()), juce::sendNotification);
    speedSlider.setValue(r.getFloat("speed", (float)speedSlider.getValue()), juce::sendNotification);
    shapeSelector.setSelectedItemIndex(r.getInt("shape", shapeSelector.getSelectedItemIndex()), juce::sendNotification);
    waveSmoothBtn.setToggleState(r.getBool("waveSmooth", waveSmoothBtn.getToggleState()), juce::sendNotification);

    auto values = r.getIntArray("table");

    if (values.empty()) return;

    auto table = fdsEditor.currentTable();

    for (int i = 0; i < (int)table.size() && i < (int)values.size(); ++i) {
        table[(size_t)i] = std::clamp(values[(size_t)i], 0, 7);
    }

    fdsEditor.loadTable(table);
}

void GuiComponentWtMod::writeParams(Io::ParamWriter& writer, const juce::String& key)
{
    auto w = writer.child(key);

    w.set("enable", enableButton.getToggleState());
    w.set("depth", (float)depthSlider.getValue());
    w.set("speed", (float)speedSlider.getValue());
    w.set("shape", shapeSelector.getSelectedItemIndex());
    w.set("waveSmooth", waveSmoothBtn.getToggleState());

    auto table = fdsEditor.currentTable();

    w.setArray("table", std::vector<int>(table.begin(), table.end()));
}
