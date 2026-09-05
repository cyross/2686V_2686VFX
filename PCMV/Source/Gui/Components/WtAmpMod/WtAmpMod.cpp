#include "./WtAmpMod.h"

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
	const Io::ParamFormat wtAmpModFormat{ "wtampmod", 1 };

	// 変調方式の一覧。ピッチ版と同じ WtModShape を使う。
	std::vector<SelectItem> wtAmpModShapeItems = {
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

	const juce::String fdsPresetNames[FdsMod::tableCount] = { "Tri", "Saw", "Rst", "Pls" };

	// エディタの高さ。上段が増減値、下段が積算後の階段波。
	constexpr int fdsEditorHeight = 96;
}

void GuiComponentWtAmpMod::setupComponent(juce::Component& parent, const juce::String& code, int& tabOrder,
    juce::Colour categoryBg)
{
    m_code = code;

    // 変調波形はプロセッサが持っている。ピッチ版と分けるため尻尾を付ける。
    m_waveKey = code + CPK::WtAmpMod::waveStoreSuffix;

    cat.setupCategory({ .parent = parent, .title = juce::String("") + "WT AMP MOD", .enableChangeDetailVisible = true }, categoryBg);

    enableButton.setup({ .parent = parent, .id = code + CPK::WtAmpMod::enable, .title = "Enable", .isReset = true, .isResized = true });
    enableButton.setWantsKeyboardFocus(true);
    enableButton.setExplicitFocusOrder(++tabOrder);

    depthSlider.setup({ .parent = parent, .id = code + CPK::WtAmpMod::depth, .title = "DPTH", .isReset = true });
    depthSlider.setWantsKeyboardFocus(true);
    depthSlider.setExplicitFocusOrder(++tabOrder);

    speedSlider.setup({ .parent = parent, .id = code + CPK::WtAmpMod::speed, .title = "SPED", .isReset = true });
    speedSlider.setWantsKeyboardFocus(true);
    speedSlider.setExplicitFocusOrder(++tabOrder);

    shapeSelector.setup({ .parent = parent, .id = code + CPK::WtAmpMod::shape, .title = "SHPE", .items = wtAmpModShapeItems, .isReset = true, .isResized = true });
    shapeSelector.setWantsKeyboardFocus(true);
    shapeSelector.setExplicitFocusOrder(++tabOrder);

    // setup が .isResized で入れた再レイアウト用の処理を潰さないよう、
    // 元の呼び出しを拾ってから後ろへつなぐ。
    auto relayoutOnShapeChange = shapeSelector.onChange;

    shapeSelector.onChange = [this, relayoutOnShapeChange] {
        if (relayoutOnShapeChange) relayoutOnShapeChange();

        this->updateModPreview();
        };

    // MIN と MAX は追い越せない。追い越したときは「相手を押す」形にする。
    // 自分へ書き戻すとドラッグ中にマウスと値の押し合いになり、
    // つまみが動かせなくなる。
    minSlider.setupComponent(parent, code + CPK::WtAmpMod::min, "MIN", tabOrder, [this] {
        if (isClampingRange) return;

        const juce::ScopedValueSetter<bool> guard(isClampingRange, true);

        if (minSlider.getValue() > maxSlider.getValue()) {
            maxSlider.setValue(minSlider.getValue(), juce::sendNotification);
        }

        updateModPreview();
        });

    minNudge.setupComponent(parent, minSlider.getSlider(), tabOrder);

    maxSlider.setupComponent(parent, code + CPK::WtAmpMod::max, "MAX", tabOrder, [this] {
        if (isClampingRange) return;

        const juce::ScopedValueSetter<bool> guard(isClampingRange, true);

        if (maxSlider.getValue() < minSlider.getValue()) {
            minSlider.setValue(maxSlider.getValue(), juce::sendNotification);
        }

        updateModPreview();
        });

    maxNudge.setupComponent(parent, maxSlider.getSlider(), tabOrder);

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

    waveSmoothBtn.setup({ .parent = parent, .id = code + CPK::WtAmpMod::waveSmooth, .title = "Smooth", .isReset = true, .isResized = true });
    waveSmoothBtn.setWantsKeyboardFocus(true);
    waveSmoothBtn.setExplicitFocusOrder(++tabOrder);

    // onStateChange ではなく onClick に付けているのは、パラメータの
    // 復元でスイッチが動いたときまで取り込み直してしまわないため。
    auto relayoutOnSmooth = waveSmoothBtn.onClick;

    waveSmoothBtn.onClick = [this, relayoutOnSmooth] {
        if (relayoutOnSmooth) relayoutOnSmooth();

        this->reapplyWaveFiles();
        };

    // 使うスロットの切り替え。パラメータなので演奏中でも動かせる。
    waveSlotSlider.setup({ .parent = parent, .id = code + CPK::WtAmpMod::waveSlot, .title = "SLOT", .isReset = true });
    waveSlotSlider.setWantsKeyboardFocus(true);
    waveSlotSlider.setExplicitFocusOrder(++tabOrder);

    // 切り替えたら Shape のプレビューもその波形に合わせる
    waveSlotSlider.onValueChange = [this] {
        this->updateModPreview();

        // どれが鳴っているのかは並べた波形のほうにも印を付ける。
        this->slotPreviews.setActive(this->currentSlot());
        };

    modPreview.setup(parent, GuiColor::WavePreview::AmpEnv);
    updateModPreview();

    fdsCat.setupSwAmpCategory({ .parent = parent, .title = juce::String("") + "FDS AMP TABLE", .enableChangeDetailVisible = true });

    // 積算後の階段波は音量側の色で描く
    fdsEditor.stepColourEntry = &GuiColor::WavePreview::AmpEnv;
    fdsEditor.setup(parent, code + CPK::WtAmpMod::fdsTable);
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

void GuiComponentWtAmpMod::layoutComponent(juce::Rectangle<int>& rect)
{
    layoutMainCategory({ .mainRect = rect, .label = &cat });

    bool visible = cat.isDetailVisible();

    enableButton.setVisible(visible);
    depthSlider.setVisibleWithLabel(visible);
    speedSlider.setVisibleWithLabel(visible);
    shapeSelector.setVisibleWithLabel(visible);
    minSlider.setVisibleWithLabel(visible);
    minNudge.setVisibles(visible && minSlider.isVisibleNudge());
    maxSlider.setVisibleWithLabel(visible);
    maxNudge.setVisibles(visible && maxSlider.isVisibleNudge());
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

        minSlider.layoutComponent(rect);
        if (minSlider.isVisibleNudge()) minNudge.layoutComponent(rect);
        maxSlider.layoutComponent(rect);
        if (maxSlider.isVisibleNudge()) maxNudge.layoutComponent(rect);

        // 選んでいる Shape のかかり方。9 種すべてに効くので、読み込み行より前に置く。
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
    minSlider.setEnabled(isMod);
    minNudge.setEnables(isMod);
    maxSlider.setEnabled(isMod);
    maxNudge.setEnables(isMod);
    waveSmoothBtn.setEnabled(isMod);
    waveSlotSlider.setEnabledWithLabel(isMod);

    slotTarget.setEnabledWithLabel(isMod);
    slotWtBtn.setEnabled(isMod);
    slotWt2Btn.setEnabled(isMod);
    slotClearBtn.setEnabled(isMod);
    slotFileNameLabel.setEnabled(isMod);

    // ---------------- FDS AMP TABLE ----------------
    // WT AMP MOD の中の小見出しなので、親を畳んだらこちらも隠す。
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

        fdsEditor.setBounds(rect.removeFromTop(fdsEditorHeight));
        rect.removeFromTop(CoreGuiValue::MainGroup::Row::paddingTop);

        rect.removeFromTop(CoreGuiValue::Category::gapBelow);
    }

    // 編集できるのは FdsUser を選んでいるときだけ
    bool isFdsUser = isMod && (shapeSelector.getSelectedItemIndex() == (int)WtModShape::FdsUser);

    fdsEditor.setCustomEnabled(isFdsUser);
    for (auto& btn : fdsPresetBtn) btn.setEnabled(isFdsUser);
}

void GuiComponentWtAmpMod::importWave(int slot, bool isWt2)
{
    juce::File defaultDir(ctx.audioProcessor.defaultWavetableDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    ctx.editor.openFileChooser(
        isWt2 ? "Load Amp Mod Wave (.wt2)" : "Load Amp Mod Wave (.wt)",
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
                    ctx.audioProcessor.loadWtModWaveFile(m_waveKey, slot, file);

                    updateSlotFileName(slot);

                    ctx.audioProcessor.defaultWavetableDir = file.getParentDirectory().getFullPathName();
                });
        }
    );
}

// 今どのスロットを触っているか。
// 演奏中に切り替えられるよう、パラメータとして持っている。
int GuiComponentWtAmpMod::currentSlot() const
{
    if (auto* p = ctx.audioProcessor.apvts.getRawParameterValue(m_code + CPK::WtAmpMod::waveSlot)) {
        return std::clamp((int)p->load(), 0, Global::WtMod::slots - 1);
    }

    return 0;
}

void GuiComponentWtAmpMod::clearWave(int slot)
{
    ctx.audioProcessor.unloadWtModWaveFile(m_waveKey, slot);

    updateSlotFileName(slot);
}

// Smooth は「読み込んだ波形を 32 サンプルへ落とす方法」を選ぶスイッチで、
// 取り込んだあとの値には掛からない。切り替えたら、覚えているファイルから
// 読み込み直す。
void GuiComponentWtAmpMod::reapplyWaveFiles()
{
    for (int i = 0; i < Global::WtMod::slots; ++i)
    {
        juce::File file(wavePath(i));

        if (!file.existsAsFile()) continue;

        ctx.audioProcessor.loadWtModWaveFile(m_waveKey, i, file);

        updateSlotPreview(i);
    }

    updateModPreview();
}

void GuiComponentWtAmpMod::applySlotTarget()
{
    const int slot = targetSlot();

    slotPreviews.setSelected(slot);

    updateSlotFileName(slot);
}

void GuiComponentWtAmpMod::updateSlotFileName(int slot)
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
void GuiComponentWtAmpMod::updateSlotPreview(int slot)
{
    auto it = ctx.audioProcessor.modWaveSlots.find(m_waveKey);

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

juce::String GuiComponentWtAmpMod::wavePath(int slot) const
{
    auto it = ctx.audioProcessor.modWavePaths.find(m_waveKey);

    if (it == ctx.audioProcessor.modWavePaths.end()) return juce::String();

    return it->second[(size_t)slot];
}

// 選んでいる Shape のかかり方を折れ線にする。
// 描画のたびに計算すると重いので、形が変わったときだけここを通す。
void GuiComponentWtAmpMod::updateModPreview()
{
    // 読み込み中は溜めておき、読み終えてから 1 度だけ作り直す
    if (GuiRefresh::defer(this, [this] { updateModPreview(); })) return;

    // HuC6280 モードで使う 32 サンプル。実データはプロセッサが持つ。
    std::array<float, Global::WtMod::waveSize> wave = { 0.0f };

    auto it = ctx.audioProcessor.modWaveSlots.find(m_waveKey);

    if (it != ctx.audioProcessor.modWaveSlots.end()) {
        wave = it->second[(size_t)currentSlot()].data;
    }

    // 返るのは音量の倍率なので、下端を 0 として片側で描く
    modPreview.setPoints(
        WavePreviewSource::wtAmpMod(shapeSelector.getSelectedItemIndex(), wave, fdsEditor.currentTable(),
            (float)minSlider.getValue(), (float)maxSlider.getValue()),
        false);
}

// 3.1.0 で足したものなので、3.0.0 より前の並び順のファイルには入っていない。
// 読み込みの位置がずれないよう、ここでは何も進めずに既定値のままにする。
void GuiComponentWtAmpMod::setImportingParams(juce::StringArray& lines, int& index)
{
    juce::ignoreUnused(lines, index);
}

juce::String GuiComponentWtAmpMod::getExportedParams()
{
    juce::String content = "";

    content += juce::String(enableButton.getToggleState() ? 1 : 0) + "\n";
    content += juce::String(depthSlider.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(speedSlider.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(shapeSelector.getSelectedItemIndex()) + "\n";
    content += juce::String(minSlider.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(maxSlider.getValue(), Global::floatDecimalPlaces) + "\n";
    content += juce::String(waveSmoothBtn.getToggleState() ? 1 : 0) + "\n";

    auto table = fdsEditor.currentTable();

    for (int i = 0; i < 32; ++i) {
        content += juce::String(table[i]) + "\n";
    }

    return content;
}

void GuiComponentWtAmpMod::importParams()
{
    juce::File defaultDir(ctx.audioProcessor.defaultWtModParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::importWtAmpModParamFile, defaultDir, Io::ExtensionGlob::WtAmpModParam);
    fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file.existsAsFile()) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultWtModParamDir = file.getParentDirectory().getFullPathName();

                auto reader = Io::ParamReader::open(file, wtAmpModFormat);

                if (!reader.has_value()) return;

                // 読み終えてからまとめて描き直す
                GuiRefresh::Batch batch;

                // チャンネルファイルの中に入る形と同じ中身にしてある
                readParams(*reader, "wtAmpMod");
            }
        });
}

void GuiComponentWtAmpMod::exportParams()
{
    juce::File defaultDir(ctx.audioProcessor.defaultWtModParamDir);
    if (!defaultDir.isDirectory()) {
        defaultDir = ctx.audioProcessor.getPluginDirectory();
    }

    fileChooser = std::make_unique<juce::FileChooser>(Io::Dialog::Title::exportWtAmpModParamFile, defaultDir.getChildFile(Io::defaultFileName(Io::Extension::WtAmpModParam)), Io::saveGlob(Io::Extension::WtAmpModParam));
    fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
        [this](const juce::FileChooser& fc) {
            auto file = fc.getResult();
            if (file != juce::File{}) {

                // 次回のダイアログ用にディレクトリを保存
                ctx.audioProcessor.defaultWtModParamDir = file.getParentDirectory().getFullPathName();

                Io::ParamWriter writer(wtAmpModFormat);

                writeParams(writer, "wtAmpMod");

                writer.writeTo(file);
            }
        });
}

void GuiComponentWtAmpMod::readParams(const Io::ParamReader& reader, const juce::String& key)
{
    auto r = reader.child(key);

    enableButton.setToggleState(r.getBool("enable", enableButton.getToggleState()), juce::sendNotification);
    depthSlider.setValue(r.getFloat("depth", (float)depthSlider.getValue()), juce::sendNotification);
    speedSlider.setValue(r.getFloat("speed", (float)speedSlider.getValue()), juce::sendNotification);
    shapeSelector.setSelectedItemIndex(r.getInt("shape", shapeSelector.getSelectedItemIndex()), juce::sendNotification);
    minSlider.setValue(r.getFloat("min", (float)minSlider.getValue()), juce::sendNotification);
    maxSlider.setValue(r.getFloat("max", (float)maxSlider.getValue()), juce::sendNotification);
    waveSmoothBtn.setToggleState(r.getBool("waveSmooth", waveSmoothBtn.getToggleState()), juce::sendNotification);

    auto values = r.getIntArray("table");

    if (values.empty()) return;

    auto table = fdsEditor.currentTable();

    for (int i = 0; i < (int)table.size() && i < (int)values.size(); ++i) {
        table[(size_t)i] = std::clamp(values[(size_t)i], 0, 7);
    }

    fdsEditor.loadTable(table);
}

void GuiComponentWtAmpMod::writeParams(Io::ParamWriter& writer, const juce::String& key)
{
    auto w = writer.child(key);

    w.set("enable", enableButton.getToggleState());
    w.set("depth", (float)depthSlider.getValue());
    w.set("speed", (float)speedSlider.getValue());
    w.set("shape", shapeSelector.getSelectedItemIndex());
    w.set("min", (float)minSlider.getValue());
    w.set("max", (float)maxSlider.getValue());
    w.set("waveSmooth", waveSmoothBtn.getToggleState());

    auto table = fdsEditor.currentTable();

    w.setArray("table", std::vector<int>(table.begin(), table.end()));
}
