#include "./ParamBrowser.h"

#include <algorithm>
#include <cmath>
#include <cstring>
#include <memory>
#include <vector>

#include "../GenWave/GenWave.h"
#include "../../../Core/Editor/PluginEditor.h"
#include "../../../Core/Editor/EditorGuiText.h"
#include "../../../Core/Gui/GuiColor.h"
#include "../../../Core/Gui/GuiComponents.h"
#include "../../../Core/Const/ConstFileValues.h"

namespace
{
    // ファイル名の後ろと、画面に出す区分の名前。
    //
    // ".param.opl" は ".param.opl3" の一部、".ssgSwEnv" は
    // ".ssgSwEnv11" の一部になっている。前から順に見ると取り違えるので、
    // いちばん長く一致したものを採ること。
    // 表示名を切り出すための目印。パラメータファイルは
    // 「名前 + .param.〜」という形をしている。
    constexpr const char* paramMark = ".param.";

    struct KindRow { juce::String suffix; juce::String label; };

    const std::vector<KindRow>& kindTableRows()
    {
        static const std::vector<KindRow> table = {
            // チャンネル
        { ".param.opna", "OPNA" },
        { ".param.opn", "OPN" },
        { ".param.opl3", "OPL3" },
        { ".param.opl", "OPL" },
        { ".param.opm", "OPM" },
        { ".param.opzx7s", "OPZX7" },
        { ".param.ssg", "SSG" },
        { ".param.wtplus", "WT+" },
        { ".param.wt2", "WT2" },
        { ".param.wt", "WT" },
        { ".param.rhythm", "RHYTHM" },
        { ".param.adpcm", "PCM" },
        { ".param.beep", "BEEP" },

            // 波形そのもの
            { ".wt2", EditorGuiText::ParamBrowser::waveWt2 },
            { ".wt", EditorGuiText::ParamBrowser::waveWt },

            // 区分ごとのパラメータ
        { ".ccolor", EditorGuiText::ParamBrowser::kindColors },
        { ".fxo", EditorGuiText::ParamBrowser::kindFxOrder },
        { ".2fx", EditorGuiText::ParamBrowser::kindFxParam },
        { ".param.opna.op", EditorGuiText::ParamBrowser::kindOpnaOp },
        { ".param.opn.op", EditorGuiText::ParamBrowser::kindOpnOp },
        { ".param.opl.op", EditorGuiText::ParamBrowser::kindOplOp },
        { ".param.opl3.op", EditorGuiText::ParamBrowser::kindOpl3Op },
        { ".param.opm.op", EditorGuiText::ParamBrowser::kindOpmOp },
        { ".param.opzx7s.op", EditorGuiText::ParamBrowser::kindOpzx7Op },
        { ".param.rhythm.pad", EditorGuiText::ParamBrowser::kindRhythmPad },
        { ".opnahwlfo", EditorGuiText::ParamBrowser::kindHwLfo },
        { ".lfon88", EditorGuiText::ParamBrowser::kindLfoN88 },
        { ".lfoopm", EditorGuiText::ParamBrowser::kindLfoOpm },
        { ".lfoopl", EditorGuiText::ParamBrowser::kindLfoOpl },
        { ".lfoopzx7", EditorGuiText::ParamBrowser::kindLfoOpzx7 },
        { ".ampenv", EditorGuiText::ParamBrowser::kindAmpEnv },
        { ".ssghwenv", EditorGuiText::ParamBrowser::kindSsgHwEnv },
        { ".ssghwpenv", EditorGuiText::ParamBrowser::kindSsgHwPEnv },
        { ".ssgswenv", EditorGuiText::ParamBrowser::kindSsgSwEnv },
        { ".ssgswenv11", EditorGuiText::ParamBrowser::kindSsgSwEnv11 },
        { ".ssgswpenv11", EditorGuiText::ParamBrowser::kindSsgSwPEnv11 },
        { ".pitchenv", EditorGuiText::ParamBrowser::kindPitchEnv },
        { ".detune", EditorGuiText::ParamBrowser::kindDetune },
        { ".unison", EditorGuiText::ParamBrowser::kindUnison },
        { ".quality", EditorGuiText::ParamBrowser::kindQuality },
        { ".pcmquality", EditorGuiText::ParamBrowser::kindPcmQuality },
        { ".pcmplay", EditorGuiText::ParamBrowser::kindPcmPlay },
        { ".tonenoise", EditorGuiText::ParamBrowser::kindToneNoise },
        { ".wtmod", EditorGuiText::ParamBrowser::kindWtMod },
        { ".wtampmod", EditorGuiText::ParamBrowser::kindWtAmpMod },
        };

        return table;
    }
}

juce::String GuiParamBrowser::categoryOf(const juce::File& file)
{
    juce::String name = file.getFileName().toLowerCase();

    // JSON / YAML はどちらも同じ中身なので、後ろを落としてから見る
    if (name.endsWith(".json")) name = name.dropLastCharacters(5);
    else if (name.endsWith(".yaml")) name = name.dropLastCharacters(5);

    // CURVE は「curve_<置き場>_<対象>」と後ろが変わるので、表では
    // 拾えない。名前の中に印があるかどうかで見る。
    if (name.contains("curve_")) return EditorGuiText::ParamBrowser::kindCurve;

    const KindRow* best = nullptr;

    for (const auto& row : kindTableRows())
    {
        if (!name.endsWith(row.suffix.toLowerCase())) continue;
        if (best != nullptr && row.suffix.length() <= best->suffix.length()) continue;

        best = &row;
    }

    return best != nullptr ? best->label : juce::String();
}

juce::String GuiParamBrowser::formatOf(const juce::File& file)
{
    const juce::String name = file.getFileName().toLowerCase();

    if (name.endsWith(".json")) return EditorGuiText::ParamBrowser::formatJson;
    if (name.endsWith(".yaml")) return EditorGuiText::ParamBrowser::formatYaml;

    return EditorGuiText::ParamBrowser::formatPlain;
}

// ============================================================================
GuiParamBrowser::GuiParamBrowser(const GuiContext& context) : ctx(context)
{
    setInterceptsMouseClicks(true, true);
    setVisible(false);
}

GuiParamBrowser::~GuiParamBrowser()
{
    stopTimer();

    m_pool.removeAllJobs(true, 4000);
}

bool GuiParamBrowser::isAllowed(const Item& item) const
{
    if (item.isFolder) return true;

    if (m_nameMustContain.isNotEmpty()
        && !item.file.getFileName().containsIgnoreCase(m_nameMustContain)) return false;

    return m_allowed.isEmpty() || m_allowed.contains(item.category);
}

void GuiParamBrowser::open(juce::Component& parent, const Request& request)
{
    m_root = request.root;
    m_limit = request.limit;

    // 前に開いていた先が縛りの外だったときは、縛りの根から出し直す。
    // そのままだと、上へも下へも行けないところで開いてしまう。
    if (!canGoTo(m_root)) m_root = m_limit;

    m_allowed = request.allowed;
    m_mode = request.mode;
    m_extension = request.extension;
    m_nameMustContain = request.nameMustContain;
    m_onChoose = request.onChoose;
    m_cancelled = false;
    m_selected = -1;
    m_scroll = 0;

    // 前に打っていたキーワードはそのまま残す。開くたびに白紙へ戻ると、
    // 探し直すのに毎回打ち直すことになる。残っていることは欄を見れば
    // 分かるので、見当違いの結果が出ても迷わない。
    keyword.setTextToShowWhenEmpty(EditorGuiText::ParamBrowser::keywordHint,
        GuiColor::ParamBrowser::HintText);

    // キーワードを打つと、下のフォルダまで潜って探す形へ切り替わる。
    // 集め直しが要るので、絞り込みだけを掛け直す口とは別にしてある。
    keyword.onTextChange = [this] { m_scroll = 0; collect(); rebuildView(); repaint(); };
    keyword.setColour(juce::TextEditor::backgroundColourId, GuiColor::ParamBrowser::FieldBg);
    keyword.setColour(juce::TextEditor::textColourId, GuiColor::ParamBrowser::Text);
    keyword.setColour(juce::TextEditor::outlineColourId, GuiColor::ParamBrowser::Border);
    addAndMakeVisible(keyword);

    collect();
    refillCategoryFilter();

    categoryFilter.onChange = [this] { m_scroll = 0; rebuildView(); repaint(); };
    addAndMakeVisible(categoryFilter);

    // 形式の絞り込みも、前に選んでいたものを引き継ぐ。選択肢は毎回
    // 作り直すので、番号を控えてから入れ直す。
    const int keptFormat = juce::jlimit(1, 4, formatFilter.getSelectedId());

    formatFilter.clear(juce::dontSendNotification);
    formatFilter.addItem(EditorGuiText::ParamBrowser::filterAll, 1);
    formatFilter.addItem(EditorGuiText::ParamBrowser::formatJson, 2);
    formatFilter.addItem(EditorGuiText::ParamBrowser::formatYaml, 3);
    formatFilter.addItem(EditorGuiText::ParamBrowser::formatPlain, 4);
    formatFilter.setSelectedId(keptFormat, juce::dontSendNotification);
    formatFilter.onChange = [this] { m_scroll = 0; rebuildView(); repaint(); };
    addAndMakeVisible(formatFilter);

    // 遠くのフォルダへ一息で移りたいときのための逃げ道。
    // 隣り合ったフォルダは一覧から辿れる。
    folderBtn.setButtonText(EditorGuiText::ParamBrowser::folder);
    folderBtn.onClick = [this] { chooseRoot(); };
    addAndMakeVisible(folderBtn);

    newFolderBtn.setButtonText(EditorGuiText::ParamBrowser::newFolder);
    newFolderBtn.onClick = [this] { createFolder(); };
    addAndMakeVisible(newFolderBtn);

    deleteFolderBtn.setButtonText(EditorGuiText::ParamBrowser::deleteFolder);
    deleteFolderBtn.setColour(juce::TextButton::buttonColourId, GuiColor::GenWave::DeleteBg);
    deleteFolderBtn.setColour(juce::TextButton::textColourOffId, GuiColor::GenWave::DeleteText);
    deleteFolderBtn.setColour(juce::TextButton::textColourOnId, GuiColor::GenWave::DeleteText);
    deleteFolderBtn.onClick = [this] { deleteSelectedFolder(); };
    addAndMakeVisible(deleteFolderBtn);

    bulkGenerateBtn.setButtonText(EditorGuiText::ParamBrowser::bulkGenerate);
    bulkGenerateBtn.setColour(juce::TextButton::buttonColourId, GuiColor::GenWave::GenerateBg);
    bulkGenerateBtn.setColour(juce::TextButton::textColourOffId, GuiColor::GenWave::GenerateText);
    bulkGenerateBtn.setColour(juce::TextButton::textColourOnId, GuiColor::GenWave::GenerateText);
    bulkGenerateBtn.onClick = [this] { startBulkGenerate(); };
    addAndMakeVisible(bulkGenerateBtn);

    bulkDeleteBtn.setButtonText(EditorGuiText::ParamBrowser::bulkDelete);
    bulkDeleteBtn.setColour(juce::TextButton::buttonColourId, GuiColor::GenWave::DeleteBg);
    bulkDeleteBtn.setColour(juce::TextButton::textColourOffId, GuiColor::GenWave::DeleteText);
    bulkDeleteBtn.setColour(juce::TextButton::textColourOnId, GuiColor::GenWave::DeleteText);
    bulkDeleteBtn.onClick = [this] { deleteAllPreviews(); };
    addAndMakeVisible(bulkDeleteBtn);

    // 書き出す先を決めるときだけ、名前の欄と保存ボタンを出す
    nameEditor.setText(request.defaultName, juce::dontSendNotification);
    nameEditor.setColour(juce::TextEditor::backgroundColourId, GuiColor::ParamBrowser::FieldBg);
    nameEditor.setColour(juce::TextEditor::textColourId, GuiColor::ParamBrowser::Text);
    nameEditor.setColour(juce::TextEditor::outlineColourId, GuiColor::ParamBrowser::Border);
    nameEditor.onReturnKey = [this] { commitSave(); };
    nameEditor.setVisible(m_mode == Mode::save);
    addChildComponent(nameEditor);

    saveBtn.setButtonText(EditorGuiText::ParamBrowser::save);
    saveBtn.setColour(juce::TextButton::buttonColourId, GuiColor::GenWave::GenerateBg);
    saveBtn.setColour(juce::TextButton::textColourOffId, GuiColor::GenWave::GenerateText);
    saveBtn.setColour(juce::TextButton::textColourOnId, GuiColor::GenWave::GenerateText);
    saveBtn.onClick = [this] { commitSave(); };
    saveBtn.setVisible(m_mode == Mode::save);
    addChildComponent(saveBtn);

    rebuildView();

    parent.addAndMakeVisible(*this);

    setBounds(parent.getLocalBounds());
    toFront(true);
    setVisible(true);

    m_startMs = juce::Time::getMillisecondCounterHiRes();

    startTimer(frameMs);
}

void GuiParamBrowser::close()
{
    stopTimer();
    setVisible(false);

    m_items.clear();
    m_view.clear();
    m_onChoose = nullptr;
}

// ----------------------------------------------------------------------------
// 集める・絞る・並べる
// ----------------------------------------------------------------------------
void GuiParamBrowser::collect()
{
    m_items.clear();
    m_previewOrder.clear();
    m_selected = -1;

    if (!m_root.isDirectory()) return;

    // キーワードを打っているときだけ、下のフォルダまで潜って探す。
    // 打っていないときは、今いるフォルダの中身だけを出す。
    const bool deep = keyword.getText().trim().isNotEmpty();

    if (!deep)
    {
        const auto parent = m_root.getParentDirectory();

        if (parent != m_root && parent.isDirectory() && canGoTo(parent))
        {
            Item up;

            up.file = parent;
            up.name = EditorGuiText::ParamBrowser::parentFolder;
            up.isFolder = true;
            up.isParent = true;

            m_items.push_back(up);
        }

        for (const auto& dir : m_root.findChildFiles(juce::File::findDirectories, false))
        {
            Item item;

            item.file = dir;
            item.name = dir.getFileName();
            item.isFolder = true;

            m_items.push_back(item);
        }
    }

    for (const auto& file : m_root.findChildFiles(juce::File::findFiles, deep))
    {
        juce::String category = categoryOf(file);
        Kind kind = Kind::param;

        if (category == EditorGuiText::ParamBrowser::waveWt
            || category == EditorGuiText::ParamBrowser::waveWt2)
        {
            kind = Kind::wave;
        }
        else if (category.isEmpty())
        {
            // 音声ファイルかどうかは、読める形式かで決める。
            // 対応している形式は音源側の持ち物なので、そちらへ訊く。
            if (ctx.audioProcessor.formatManager.findFormatForFileExtension(
                file.getFileExtension()) == nullptr) continue;

            category = EditorGuiText::ParamBrowser::audioFile;
            kind = Kind::audio;
        }

        Item item;

        item.file = file;
        item.category = category;
        item.format = formatOf(file);
        item.kind = kind;

        // 表示名は「.param.〜」より前。形式と区分は別の列に出るので、
        // ここへ重ねて出しても読みにくくなるだけ。
        const int at = file.getFileName().toLowerCase().lastIndexOf(paramMark);

        item.name = (at > 0) ? file.getFileName().substring(0, at) : file.getFileNameWithoutExtension();

        m_items.push_back(std::move(item));
    }
}

void GuiParamBrowser::refillCategoryFilter()
{
    const juce::String kept = categoryFilter.getSelectedId() > 1 ? categoryFilter.getText() : juce::String();

    categoryFilter.clear(juce::dontSendNotification);
    categoryFilter.addItem(EditorGuiText::ParamBrowser::filterAll, 1);

    juce::StringArray kinds;

    for (const auto& item : m_items)
    {
        if (!item.isFolder) kinds.addIfNotAlreadyThere(item.category);
    }

    // 扱える区分は、そのフォルダに 1 件も無くても選択肢へ出す。
    // 出しておかないと絞り込みが「すべて」へ落ちて、選べない行が
    // ずらりと並ぶだけになる。空なら空と分かるほうがよい。
    for (const auto& allowedKind : m_allowed) kinds.addIfNotAlreadyThere(allowedKind);

    kinds.sort(true);

    for (int i = 0; i < kinds.size(); ++i) categoryFilter.addItem(kinds[i], i + 2);

    // 前に選んでいた区分があればそのまま。無ければ、扱える区分が
    // ひとつだけのときにそこへ絞る。
    juce::String want = kept;

    if (want.isEmpty() && m_allowed.size() == 1) want = m_allowed[0];

    int id = 1;

    if (want.isNotEmpty())
    {
        const int at = kinds.indexOf(want);

        if (at >= 0) id = at + 2;
    }

    categoryFilter.setSelectedId(id, juce::dontSendNotification);
}

void GuiParamBrowser::rebuildView()
{
    m_view.clear();

    const juce::String word = keyword.getText().trim().toLowerCase();
    const juce::String cat = categoryFilter.getSelectedId() <= 1 ? juce::String()
        : categoryFilter.getText();
    const juce::String fmt = formatFilter.getSelectedId() <= 1 ? juce::String()
        : formatFilter.getText();

    std::vector<int> folders;
    std::vector<int> files;

    int parentRow = -1;

    for (int i = 0; i < (int)m_items.size(); ++i)
    {
        const auto& item = m_items[(size_t)i];

        if (item.isParent)
        {
            // 「上へ」は絞り込みに関わらず出す。ここが消えると戻れない。
            parentRow = i;

            continue;
        }

        if (word.isNotEmpty() && !item.name.toLowerCase().contains(word)) continue;

        if (item.isFolder)
        {
            // 区分と形式はファイルの持ち物なので、フォルダは落とさない
            folders.push_back(i);

            continue;
        }

        if (cat.isNotEmpty() && item.category != cat) continue;
        if (fmt.isNotEmpty() && item.format != fmt) continue;

        files.push_back(i);
    }

    if (m_sort != Sort::none)
    {
        const bool up = (m_sort == Sort::ascending);
        const Column column = m_sortColumn;
        const auto* items = &m_items;

        auto byKey = [items, column, up](int a, int b) {
            const auto& x = (*items)[(size_t)a];
            const auto& y = (*items)[(size_t)b];

            auto key = [column](const Item& item) -> const juce::String& {
                switch (column)
                {
                case Column::category: return item.category;
                case Column::format: return item.format;
                default: return item.name;
                }
            };

            const int order = key(x).compareNatural(key(y));

            return up ? (order < 0) : (order > 0);
        };

        // フォルダは名前でしか並べようがないので、区分や形式で
        // 並べ替えているときも名前順のまま置く。
        std::stable_sort(folders.begin(), folders.end(), [items, up](int a, int b) {
            const int order = (*items)[(size_t)a].name.compareNatural((*items)[(size_t)b].name);

            return up ? (order < 0) : (order > 0);
        });

        std::stable_sort(files.begin(), files.end(), byKey);
    }

    // 「上へ」→ フォルダ → ファイル の順に置く
    if (parentRow >= 0) m_view.push_back(parentRow);

    m_view.insert(m_view.end(), folders.begin(), folders.end());
    m_view.insert(m_view.end(), files.begin(), files.end());

    m_selected = -1;

    const int maxScroll = juce::jmax(0, (int)m_view.size() - visibleRows());

    m_scroll = juce::jlimit(0, maxScroll, m_scroll);
}

// プレビューは行を出すときに用意する。作り方は種類ごとに違う。
void GuiParamBrowser::loadPreview(int itemIndex)
{
    auto& item = m_items[(size_t)itemIndex];

    if (item.previewLoaded || item.isFolder) return;

    item.previewLoaded = true;

    m_previewOrder.push_back(itemIndex);

    dropOldPreviews();

    switch (item.kind)
    {
    case Kind::wave: loadWavePoints(item); return;
    case Kind::audio: loadAudioShape(item); return;
    default: break;
    }

    const auto cache = GenWaveRender::cacheFileFor(
        GenWaveRender::cacheDirectory(ctx.audioProcessor), item.file);

    item.wave = GenWaveRender::read(cache);
    item.hasWave = !item.wave.isEmpty();
}

// 波形ファイルはそのまま線にする。
//
//   .wt  … 1 行目がサンプル数、その後ろに -1.0〜1.0 の値
//   .wt2 … 1 行目がサンプル数、2 行目が解像度、その後ろに 0〜解像度-1 の値
//
// 読み込み側 (GuiWt / GuiWt2) と同じ読み方をしている。
// 出した順に捨てていく。捨てた行はまた出したときに読み直す。
void GuiParamBrowser::dropOldPreviews()
{
    while ((int)m_previewOrder.size() > previewKeep)
    {
        const int index = m_previewOrder.front();

        m_previewOrder.pop_front();

        if (index < 0 || index >= (int)m_items.size()) continue;

        auto& old = m_items[(size_t)index];

        old.wave = {};
        old.hasWave = false;
        old.points.clear();
        old.points.shrink_to_fit();
        old.mins.clear();
        old.mins.shrink_to_fit();
        old.maxs.clear();
        old.maxs.shrink_to_fit();
        old.previewLoaded = false;
    }
}

void GuiParamBrowser::loadWavePoints(Item& item)
{
    juce::StringArray lines;

    item.file.readLines(lines);

    if (lines.size() < 2) return;

    const int count = lines[0].trim().getIntValue();

    if (count <= 0 || count > 256) return;

    const bool isWt2 = (item.category == EditorGuiText::ParamBrowser::waveWt2);
    const int first = isWt2 ? 2 : 1;
    const int resolution = isWt2 ? lines[1].trim().getIntValue() : 0;

    if (isWt2 && resolution <= 1) return;

    const float centre = isWt2 ? (float)(resolution >> 1) : 0.0f;

    item.points.reserve((size_t)count);

    for (int i = 0; i < count; ++i)
    {
        if (i + first >= lines.size()) break;

        const float value = isWt2
            ? (lines[i + first].getIntValue() - centre) / juce::jmax(1.0f, centre)
            : lines[i + first].getFloatValue();

        item.points.push_back(juce::jlimit(-1.0f, 1.0f, value));
    }
}

// 音声ファイルは 1 点ずつ拾っても形が分からないので、区間ごとの
// 最小値と最大値で出す。長いものでも一度に読み込まず、少しずつ舐める。
void GuiParamBrowser::loadAudioShape(Item& item)
{
    std::unique_ptr<juce::AudioFormatReader> reader(
        ctx.audioProcessor.formatManager.createReaderFor(item.file));

    if (reader == nullptr || reader->lengthInSamples <= 0) return;

    const int buckets = previewWidth;
    const juce::int64 total = reader->lengthInSamples;
    const juce::int64 perBucket = juce::jmax((juce::int64)1, total / buckets);

    item.mins.assign((size_t)buckets, 0.0f);
    item.maxs.assign((size_t)buckets, 0.0f);

    constexpr int blockSize = 8192;

    juce::AudioBuffer<float> block((int)juce::jmin((juce::uint32)2, reader->numChannels), blockSize);

    for (int b = 0; b < buckets; ++b)
    {
        const juce::int64 start = (juce::int64)b * perBucket;

        if (start >= total) break;

        const int num = (int)juce::jmin((juce::int64)blockSize, juce::jmin(perBucket, total - start));

        block.clear();

        if (!reader->read(&block, 0, num, start, true, block.getNumChannels() > 1)) break;

        float low = 0.0f;
        float high = 0.0f;

        for (int ch = 0; ch < block.getNumChannels(); ++ch)
        {
            const auto range = juce::FloatVectorOperations::findMinAndMax(block.getReadPointer(ch), num);

            low = juce::jmin(low, range.getStart());
            high = juce::jmax(high, range.getEnd());
        }

        item.mins[(size_t)b] = juce::jlimit(-1.0f, 1.0f, low);
        item.maxs[(size_t)b] = juce::jlimit(-1.0f, 1.0f, high);
    }
}

void GuiParamBrowser::toggleSort(Column column)
{
    if (m_sortColumn != column)
    {
        m_sortColumn = column;
        m_sort = Sort::ascending;
    }
    else
    {
        switch (m_sort)
        {
        case Sort::none: m_sort = Sort::ascending; break;
        case Sort::ascending: m_sort = Sort::descending; break;
        default: m_sort = Sort::none; break;
        }
    }

    rebuildView();
    repaint();
}

bool GuiParamBrowser::canGoTo(const juce::File& dir) const
{
    if (m_limit == juce::File{}) return true;

    return dir == m_limit || dir.isAChildOf(m_limit);
}

void GuiParamBrowser::goTo(const juce::File& dir)
{
    if (!dir.isDirectory()) return;
    if (!canGoTo(dir)) return;

    m_root = dir;
    m_scroll = 0;

    if (onRootChanged) onRootChanged(dir);

    collect();
    refillCategoryFilter();
    rebuildView();
    repaint();
}

void GuiParamBrowser::chooseAt(int viewIndex)
{
    if (viewIndex < 0 || viewIndex >= (int)m_view.size()) return;

    const auto& item = m_items[(size_t)m_view[(size_t)viewIndex]];

    if (item.isFolder)
    {
        goTo(item.file);

        return;
    }

    // 呼んだ側が扱えない区分は選ばせない
    if (!isAllowed(item)) return;

    if (m_mode == Mode::save)
    {
        // 書き出す先を決めているときは、名前を写すだけにする。
        // 押した拍子に上書きされては困る。
        nameEditor.setText(item.file.getFileNameWithoutExtension(), juce::dontSendNotification);

        return;
    }

    auto chosen = item.file;
    auto callback = m_onChoose;

    close();

    if (callback) callback(chosen);
}

// ----------------------------------------------------------------------------
// フォルダ
// ----------------------------------------------------------------------------
void GuiParamBrowser::chooseRoot()
{
    if (m_busy) return;

    m_fileChooser = std::make_unique<juce::FileChooser>(
        EditorGuiText::ParamBrowser::folderTitle, m_root);

    juce::Component::SafePointer<GuiParamBrowser> safe(this);

    m_fileChooser->launchAsync(
        juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectDirectories,
        [safe](const juce::FileChooser& fc) {
            if (safe == nullptr) return;

            safe->goTo(fc.getResult());
        });
}

void GuiParamBrowser::createFolder()
{
    if (m_busy || !m_root.isDirectory()) return;

    auto* window = new juce::AlertWindow(
        EditorGuiText::ParamBrowser::newFolderTitle,
        m_root.getFullPathName(),
        juce::MessageBoxIconType::NoIcon);

    window->addTextEditor("name", {}, EditorGuiText::ParamBrowser::newFolderPrompt);
    window->addButton(EditorGuiText::ParamBrowser::create, 1,
        juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
    window->addButton(EditorGuiText::ParamBrowser::cancel, 0,
        juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

    GuiDialog::styleButtons(*window);

    juce::Component::SafePointer<GuiParamBrowser> safe(this);

    window->enterModalState(true, juce::ModalCallbackFunction::create(
        [safe, window](int result) {
            if (safe == nullptr || result != 1) return;

            const juce::String name = window->getTextEditorContents("name").trim();

            if (name.isEmpty()) return;

            // 使えない文字はここで落とす。作れない名前をそのまま渡すと
            // 失敗の理由が分かりにくい。
            auto dir = safe->m_root.getChildFile(juce::File::createLegalFileName(name));

            dir.createDirectory();

            safe->collect();
            safe->rebuildView();
            safe->repaint();
        }), true);
}

void GuiParamBrowser::deleteSelectedFolder()
{
    if (m_busy) return;
    if (m_selected < 0 || m_selected >= (int)m_view.size()) return;

    const auto& item = m_items[(size_t)m_view[(size_t)m_selected]];

    if (!item.isFolder || item.isParent) return;

    const auto dir = item.file;
    const int count = dir.getNumberOfChildFiles(juce::File::findFilesAndDirectories, "*");

    auto* window = new juce::AlertWindow(
        EditorGuiText::ParamBrowser::deleteFolderTitle,
        dir.getFullPathName() + "\n\n"
        + EditorGuiText::ParamBrowser::deleteFolderCount.replace("%d", juce::String(count)),
        juce::MessageBoxIconType::NoIcon);

    window->addButton(EditorGuiText::ParamBrowser::moveToTrash, 1);
    window->addButton(EditorGuiText::ParamBrowser::cancel, 0,
        juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

    GuiDialog::styleButtons(*window);

    juce::Component::SafePointer<GuiParamBrowser> safe(this);

    window->enterModalState(true, juce::ModalCallbackFunction::create(
        [safe, dir](int result) {
            if (safe == nullptr || result != 1) return;

            // 消し切らずにごみ箱へ入れる。間違えても取り戻せるように。
            dir.moveToTrash();

            safe->collect();
            safe->rebuildView();
            safe->repaint();
        }), true);
}

void GuiParamBrowser::commitSave()
{
    if (m_mode != Mode::save || !m_root.isDirectory()) return;

    const juce::String name = nameEditor.getText().trim();

    if (name.isEmpty()) return;

    juce::String fileName = juce::File::createLegalFileName(name);

    if (m_extension.isNotEmpty() && !fileName.endsWithIgnoreCase(m_extension))
    {
        fileName += m_extension;
    }

    const auto target = m_root.getChildFile(fileName);

    auto finish = [this](const juce::File& file) {
        auto callback = m_onChoose;

        close();

        if (callback) callback(file);
    };

    if (!target.existsAsFile())
    {
        finish(target);

        return;
    }

    auto* window = new juce::AlertWindow(
        EditorGuiText::ParamBrowser::overwriteTitle,
        target.getFullPathName(),
        juce::MessageBoxIconType::NoIcon);

    window->addButton(EditorGuiText::ParamBrowser::overwrite, 1);
    window->addButton(EditorGuiText::ParamBrowser::cancel, 0,
        juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

    GuiDialog::styleButtons(*window);

    juce::Component::SafePointer<GuiParamBrowser> safe(this);

    window->enterModalState(true, juce::ModalCallbackFunction::create(
        [safe, target](int result) {
            if (safe == nullptr || result != 1) return;

            auto callback = safe->m_onChoose;

            safe->close();

            if (callback) callback(target);
        }), true);
}

// ----------------------------------------------------------------------------
// プレビューを作る
// ----------------------------------------------------------------------------
// 1 件ずつ「当てる → 計算する → 貯める」を繰り返す。当てるところは
// メッセージスレッドで、計算だけ別のスレッドへ回す。こうしておくと
// 待っている間もローディングの棒が動く。
//
// 当てると今の音が変わってしまうので、始める前に状態を控えておき、
// 終わったら戻す。
void GuiParamBrowser::startBulkGenerate()
{
    if (m_busy || m_view.empty()) return;

    m_queue.clear();
    m_cancelled = false;

    for (int index : m_view)
    {
        const auto& item = m_items[(size_t)index];

        if (item.isFolder || item.kind != Kind::param) continue;

        m_queue.push_back(index);
    }

    if (m_queue.empty()) return;

    m_queueAt = 0;
    m_busy = true;

    bulkGenerateBtn.setEnabled(false);
    bulkDeleteBtn.setEnabled(false);

    ctx.audioProcessor.getStateInformation(m_savedState);

    ctx.editor.showLoading(EditorGuiText::ParamBrowser::working,
        [this] { m_cancelled = true; });

    generateNext();
}

void GuiParamBrowser::generateOne(int itemIndex)
{
    if (m_busy) return;

    m_queue.assign(1, itemIndex);
    m_queueAt = 0;
    m_cancelled = false;
    m_busy = true;

    bulkGenerateBtn.setEnabled(false);
    bulkDeleteBtn.setEnabled(false);

    ctx.audioProcessor.getStateInformation(m_savedState);

    ctx.editor.showLoading(EditorGuiText::ParamBrowser::working,
        [this] { m_cancelled = true; });

    generateNext();
}

void GuiParamBrowser::generateNext()
{
    // 中止は次の 1 件へ進む前に見る。走っている計算は最後まで進むが、
    // せいぜい 1 件ぶんなのですぐ止まる。
    if (m_cancelled || m_queueAt >= m_queue.size())
    {
        finishBulk();

        return;
    }

    const int index = m_queue[m_queueAt];
    auto& item = m_items[(size_t)index];

    ctx.editor.updateLoading(EditorGuiText::ParamBrowser::working
        + " (" + juce::String((int)m_queueAt + 1) + " / " + juce::String((int)m_queue.size()) + ")");

    // 当てて、その設定で音源を組む。どちらもメッセージスレッド。
    if (!ctx.editor.applyChannelParamFile(item.file))
    {
        ++m_queueAt;

        juce::MessageManager::callAsync([safe = juce::Component::SafePointer<GuiParamBrowser>(this)] {
            if (safe != nullptr) safe->generateNext();
        });

        return;
    }

    auto rig = std::make_shared<GenWaveRender::Rig>();

    rig->build(ctx.audioProcessor);

    const auto cacheFile = GenWaveRender::cacheFileFor(
        GenWaveRender::cacheDirectory(ctx.audioProcessor), item.file);

    juce::Component::SafePointer<GuiParamBrowser> safe(this);

    m_pool.addJob([safe, rig, cacheFile, index] {
        auto wave = std::make_shared<GenWaveRender::Wave>(GenWaveRender::render(*rig));

        GenWaveRender::write(cacheFile, *wave);

        juce::MessageManager::callAsync([safe, wave, index] {
            if (safe == nullptr) return;

            safe->m_items[(size_t)index].wave = *wave;
            safe->m_items[(size_t)index].hasWave = !wave->isEmpty();
            safe->m_items[(size_t)index].previewLoaded = true;

            ++safe->m_queueAt;

            safe->generateNext();
        });
    });
}

void GuiParamBrowser::finishBulk()
{
    // 控えておいた状態へ戻す。当てたぶんはここで消える。
    if (m_savedState.getSize() > 0)
    {
        ctx.audioProcessor.setStateInformation(m_savedState.getData(), (int)m_savedState.getSize());
    }

    m_savedState.reset();
    m_queue.clear();
    m_queueAt = 0;
    m_busy = false;
    m_cancelled = false;

    bulkGenerateBtn.setEnabled(true);
    bulkDeleteBtn.setEnabled(true);

    ctx.editor.hideLoading();

    repaint();
}

void GuiParamBrowser::deleteAllPreviews()
{
    if (m_busy) return;

    const auto cacheDir = GenWaveRender::cacheDirectory(ctx.audioProcessor);

    for (int index : m_view)
    {
        auto& item = m_items[(size_t)index];

        if (item.isFolder) continue;

        GenWaveRender::cacheFileFor(cacheDir, item.file).deleteFile();

        item.wave = {};
        item.hasWave = false;
        item.previewLoaded = true;
    }

    repaint();
}

// ----------------------------------------------------------------------------
// 区画
// ----------------------------------------------------------------------------
juce::Rectangle<int> GuiParamBrowser::panelArea() const
{
    return getLocalBounds().reduced(margin);
}

juce::Rectangle<int> GuiParamBrowser::innerArea() const
{
    return panelArea().reduced(padding);
}

int GuiParamBrowser::topRowsHeight() const
{
    int height = searchRowHeight * 2 + padding * 2;

    if (m_mode == Mode::save) height += searchRowHeight + padding;

    return height + pathRowHeight;
}

juce::Rectangle<int> GuiParamBrowser::pathArea() const
{
    auto area = innerArea();

    area.removeFromTop(topRowsHeight() - pathRowHeight);

    return area.removeFromTop(pathRowHeight);
}

juce::Rectangle<int> GuiParamBrowser::headerArea() const
{
    auto area = innerArea();

    area.removeFromTop(topRowsHeight());

    return area.removeFromTop(headerHeight);
}

juce::Rectangle<int> GuiParamBrowser::listArea() const
{
    auto area = innerArea();

    area.removeFromTop(topRowsHeight() + headerHeight);

    return area;
}

int GuiParamBrowser::visibleRows() const
{
    return juce::jmax(1, listArea().getHeight() / rowHeight);
}

juce::Rectangle<int> GuiParamBrowser::rowArea(int viewIndex) const
{
    auto area = listArea().withTrimmedRight(scrollBarWidth);

    return { area.getX(), area.getY() + (viewIndex - m_scroll) * rowHeight,
             area.getWidth(), rowHeight };
}

juce::Rectangle<int> GuiParamBrowser::columnArea(juce::Rectangle<int> row, Column column) const
{
    const int nameWidth = juce::jmax(80,
        row.getWidth() - categoryWidth - formatWidth - previewWidth - actionWidth);

    switch (column)
    {
    case Column::category: return { row.getX() + nameWidth, row.getY(), categoryWidth, row.getHeight() };
    case Column::format: return { row.getX() + nameWidth + categoryWidth, row.getY(), formatWidth, row.getHeight() };
    default: return { row.getX(), row.getY(), nameWidth, row.getHeight() };
    }
}

juce::Rectangle<int> GuiParamBrowser::previewArea(juce::Rectangle<int> row) const
{
    return { row.getRight() - actionWidth - previewWidth, row.getY(), previewWidth, row.getHeight() };
}

juce::Rectangle<int> GuiParamBrowser::actionArea(juce::Rectangle<int> row) const
{
    return { row.getRight() - actionWidth, row.getY(), actionWidth, row.getHeight() };
}

juce::Rectangle<int> GuiParamBrowser::cycleCell(int index) const
{
    auto area = innerArea();

    area.removeFromTop(searchRowHeight + padding);

    auto row = area.removeFromTop(searchRowHeight);
    auto cells = row.removeFromRight((int)GuiGenWave::cycleChoices.size() * 34);

    const int count = (int)GuiGenWave::cycleChoices.size();
    const int left = cells.getX() + cells.getWidth() * index / count;
    const int right = cells.getX() + cells.getWidth() * (index + 1) / count;

    return { left, cells.getY(), right - left, cells.getHeight() };
}

void GuiParamBrowser::resized()
{
    auto area = innerArea();

    auto row1 = area.removeFromTop(searchRowHeight);

    formatFilter.setBounds(row1.removeFromRight(filterWidth));
    row1.removeFromRight(padding);
    categoryFilter.setBounds(row1.removeFromRight(filterWidth));
    row1.removeFromRight(padding);
    keyword.setBounds(row1);

    area.removeFromTop(padding);

    auto row2 = area.removeFromTop(searchRowHeight);

    row2.removeFromRight((int)GuiGenWave::cycleChoices.size() * 34 + padding);

    folderBtn.setBounds(row2.removeFromLeft(folderButtonWidth));
    row2.removeFromLeft(padding);
    newFolderBtn.setBounds(row2.removeFromLeft(newFolderButtonWidth));
    row2.removeFromLeft(padding);
    deleteFolderBtn.setBounds(row2.removeFromLeft(deleteFolderButtonWidth));
    row2.removeFromLeft(padding * 2);
    bulkGenerateBtn.setBounds(row2.removeFromLeft(bulkButtonWidth));
    row2.removeFromLeft(padding);
    bulkDeleteBtn.setBounds(row2.removeFromLeft(deleteButtonWidth));

    if (m_mode != Mode::save) return;

    area.removeFromTop(padding);

    auto row3 = area.removeFromTop(searchRowHeight);

    saveBtn.setBounds(row3.removeFromRight(saveButtonWidth));
    row3.removeFromRight(padding);
    row3.removeFromLeft(nameLabelWidth);
    nameEditor.setBounds(row3);
}

// ----------------------------------------------------------------------------
// マウス
// ----------------------------------------------------------------------------
void GuiParamBrowser::mouseDown(const juce::MouseEvent& event)
{
    const auto at = event.getPosition();

    // 外を押したら、選ばずに閉じる
    if (!panelArea().contains(at))
    {
        close();

        return;
    }

    if (m_busy) return;

    for (int i = 0; i < (int)GuiGenWave::cycleChoices.size(); ++i)
    {
        if (!cycleCell(i).contains(at)) continue;

        m_cycleIndex = i;

        repaint();

        return;
    }

    auto header = headerArea();

    if (header.contains(at))
    {
        for (Column column : { Column::name, Column::category, Column::format })
        {
            if (columnArea(header.withTrimmedRight(scrollBarWidth), column).contains(at))
            {
                toggleSort(column);

                return;
            }
        }

        return;
    }

    if (!listArea().contains(at)) return;

    const int viewIndex = m_scroll + (at.getY() - listArea().getY()) / rowHeight;

    if (viewIndex < 0 || viewIndex >= (int)m_view.size()) return;

    const auto& item = m_items[(size_t)m_view[(size_t)viewIndex]];

    if (!item.isFolder && item.kind == Kind::param && actionArea(rowArea(viewIndex)).contains(at))
    {
        generateOne(m_view[(size_t)viewIndex]);

        return;
    }

    // 扱えない区分は選ばせない。生成だけは押せるままにしてある
    // (どの行のプレビューも作れたほうが都合がよいため)。
    if (!isAllowed(item)) return;

    // フォルダも「上へ」も、押しただけでは移らない。ほかの行と同じく
    // ダブルクリックで移る。片方だけ一押しで動くと、選ぶつもりが移って
    // しまう。
    m_selected = viewIndex;

    repaint(listArea());
}

void GuiParamBrowser::mouseDoubleClick(const juce::MouseEvent& event)
{
    if (m_busy) return;
    if (!listArea().contains(event.getPosition())) return;

    const int viewIndex = m_scroll + (event.getPosition().getY() - listArea().getY()) / rowHeight;

    if (viewIndex < 0 || viewIndex >= (int)m_view.size()) return;

    const auto& item = m_items[(size_t)m_view[(size_t)viewIndex]];

    if (!item.isFolder && actionArea(rowArea(viewIndex)).contains(event.getPosition())) return;

    chooseAt(viewIndex);
}

void GuiParamBrowser::mouseWheelMove(const juce::MouseEvent&, const juce::MouseWheelDetails& wheel)
{
    const int maxScroll = juce::jmax(0, (int)m_view.size() - visibleRows());

    m_scroll = juce::jlimit(0, maxScroll, m_scroll - (int)std::lround(wheel.deltaY * 6.0f));

    repaint(listArea());
}

// ----------------------------------------------------------------------------
// 描く
// ----------------------------------------------------------------------------
void GuiParamBrowser::paint(juce::Graphics& g)
{
    // 下の画面は半透明の黒で覆う
    g.fillAll(GuiColor::ParamBrowser::Backdrop);

    auto panel = panelArea();

    g.setColour(GuiColor::ParamBrowser::PanelBg);
    g.fillRoundedRectangle(panel.toFloat(), 6.0f);

    g.setColour(GuiColor::ParamBrowser::Border);
    g.drawRoundedRectangle(panel.toFloat(), 6.0f, 1.0f);

    if (m_mode == Mode::save)
    {
        auto area = innerArea();

        area.removeFromTop(searchRowHeight * 2 + padding * 3);

        g.setColour(GuiColor::ParamBrowser::Text);
        g.setFont(juce::FontOptions(13.0f));
        g.drawText(EditorGuiText::ParamBrowser::saveName,
            area.removeFromTop(searchRowHeight).withWidth(nameLabelWidth),
            juce::Justification::centredLeft, false);
    }

    drawCycles(g);
    drawPath(g);
    drawHeader(g);
    drawRows(g);
    drawScrollBar(g);
}

void GuiParamBrowser::drawPath(juce::Graphics& g)
{
    g.setColour(GuiColor::ParamBrowser::HintText);
    g.setFont(juce::FontOptions(12.0f));
    g.drawText(m_root.getFullPathName(), pathArea(), juce::Justification::centredLeft, true);
}

void GuiParamBrowser::drawCycles(juce::Graphics& g)
{
    g.setFont(juce::FontOptions(12.0f));

    for (int i = 0; i < (int)GuiGenWave::cycleChoices.size(); ++i)
    {
        const auto cell = cycleCell(i).reduced(1);
        const bool on = (i == m_cycleIndex);

        g.setColour(on ? GuiColor::GenWave::CycleBgOn : GuiColor::GenWave::CycleBg);
        g.fillRect(cell);

        g.setColour(GuiColor::GenWave::Border);
        g.drawRect(cell, 1);

        g.setColour(on ? GuiColor::GenWave::CycleTextOn : GuiColor::GenWave::CycleText);
        g.drawText(juce::String(GuiGenWave::cycleChoices[(size_t)i]), cell,
            juce::Justification::centred, false);
    }
}

void GuiParamBrowser::drawHeader(juce::Graphics& g)
{
    auto header = headerArea();

    g.setColour(GuiColor::ParamBrowser::HeaderBg);
    g.fillRect(header);

    g.setFont(juce::FontOptions(13.0f));

    auto row = header.withTrimmedRight(scrollBarWidth);

    struct HeadRow { Column column; const juce::String& text; };

    const HeadRow heads[] = {
        { Column::name, EditorGuiText::ParamBrowser::columnName },
        { Column::category, EditorGuiText::ParamBrowser::columnCategory },
        { Column::format, EditorGuiText::ParamBrowser::columnFormat },
    };

    for (const auto& head : heads)
    {
        auto cell = columnArea(row, head.column).reduced(6, 0);

        g.setColour(GuiColor::ParamBrowser::HeaderText);
        g.drawText(head.text, cell, juce::Justification::centredLeft, false);

        if (m_sortColumn != head.column || m_sort == Sort::none) continue;

        // 並べ替えの向きは、列名の右へ三角で添える
        const juce::String mark = (m_sort == Sort::ascending)
            ? EditorGuiText::ParamBrowser::sortUp
            : EditorGuiText::ParamBrowser::sortDown;

        g.drawText(mark, cell, juce::Justification::centredRight, false);
    }

    g.setColour(GuiColor::ParamBrowser::HeaderText);
    g.drawText(EditorGuiText::ParamBrowser::columnPreview, previewArea(row).reduced(6, 0),
        juce::Justification::centredLeft, false);
    g.drawText(EditorGuiText::ParamBrowser::columnAction, actionArea(row).reduced(6, 0),
        juce::Justification::centredLeft, false);

    g.setColour(GuiColor::ParamBrowser::Border);
    g.drawHorizontalLine(header.getBottom() - 1, (float)header.getX(), (float)header.getRight());
}

void GuiParamBrowser::drawRows(juce::Graphics& g)
{
    auto list = listArea();

    juce::Graphics::ScopedSaveState clip(g);

    g.reduceClipRegion(list);

    if (m_view.empty())
    {
        g.setColour(GuiColor::ParamBrowser::HintText);
        g.setFont(juce::FontOptions(14.0f));
        g.drawText(EditorGuiText::ParamBrowser::empty, list, juce::Justification::centred, false);

        return;
    }

    const int last = juce::jmin((int)m_view.size(), m_scroll + visibleRows() + 1);

    // 貯めてあるものは、出す行のぶんだけ見に行く。開いた時点で全部を
    // 見に行くと、件数の多いフォルダで待たされる。
    for (int i = m_scroll; i < last; ++i) loadPreview(m_view[(size_t)i]);

    for (int i = m_scroll; i < last; ++i) drawRow(g, i);
}

void GuiParamBrowser::drawRow(juce::Graphics& g, int viewIndex)
{
    const auto& item = m_items[(size_t)m_view[(size_t)viewIndex]];
    const auto row = rowArea(viewIndex);
    const bool allowed = isAllowed(item);

    if (viewIndex == m_selected)
    {
        g.setColour(GuiColor::ParamBrowser::SelectedBg);
        g.fillRect(row);
    }
    else if ((viewIndex % 2) == 1)
    {
        g.setColour(GuiColor::ParamBrowser::StripeBg);
        g.fillRect(row);
    }

    g.setFont(juce::FontOptions(13.0f));

    auto nameCell = columnArea(row, Column::name).reduced(6, 0);

    if (item.isFolder)
    {
        // フォルダは名前だけ。区分も形式も持たない。
        g.setColour(GuiColor::ParamBrowser::FolderText);
        g.drawText(EditorGuiText::ParamBrowser::folderMark + item.name, nameCell,
            juce::Justification::centredLeft, true);

        g.setColour(GuiColor::ParamBrowser::Border);
        g.drawHorizontalLine(row.getBottom() - 1, (float)row.getX(), (float)row.getRight());

        return;
    }

    g.setColour(allowed ? GuiColor::ParamBrowser::Text.get()
                        : GuiColor::ParamBrowser::DisabledText.get());

    g.drawText(item.name, nameCell, juce::Justification::centredLeft, true);
    g.drawText(item.category, columnArea(row, Column::category).reduced(6, 0),
        juce::Justification::centredLeft, false);
    g.drawText(item.format, columnArea(row, Column::format).reduced(6, 0),
        juce::Justification::centredLeft, false);

    // 波形と音声は鳴らして見せられないので、中身をそのまま止め絵で出す。
    // 生成ボタンも要らない。
    if (item.kind != Kind::param)
    {
        drawStill(g, previewArea(row).reduced(4), item);

        g.setColour(GuiColor::ParamBrowser::Border);
        g.drawHorizontalLine(row.getBottom() - 1, (float)row.getX(), (float)row.getRight());

        return;
    }

    drawWave(g, previewArea(row).reduced(4), item);

    // 行ごとの生成・再生成。ボタンの形に描いてあるだけで、部品ではない。
    const auto action = actionArea(row).reduced(6, 8);

    g.setColour(GuiColor::GenWave::GenerateBg);
    g.fillRoundedRectangle(action.toFloat(), 3.0f);

    g.setColour(GuiColor::GenWave::GenerateText);
    g.setFont(juce::FontOptions(12.0f));
    g.drawText(item.hasWave ? EditorGuiText::GenWave::regenerate : EditorGuiText::GenWave::generate,
        action, juce::Justification::centred, false);

    g.setColour(GuiColor::ParamBrowser::Border);
    g.drawHorizontalLine(row.getBottom() - 1, (float)row.getX(), (float)row.getRight());
}

void GuiParamBrowser::drawWave(juce::Graphics& g, juce::Rectangle<int> area, const Item& item)
{
    g.setColour(GuiColor::GenWave::Bg);
    g.fillRect(area);

    if (!item.hasWave)
    {
        g.setColour(GuiColor::ParamBrowser::HintText);
        g.setFont(juce::FontOptions(11.0f));
        g.drawText(EditorGuiText::ParamBrowser::noPreview, area, juce::Justification::centred, false);

        return;
    }

    // 見せ方は「生成波形のプレビュー」と同じ。10 秒ぶんの上を窓が動き、
    // L / M / R を 3 段で出す。
    const double length = item.wave.lengthSeconds();
    const double elapsed = (juce::Time::getMillisecondCounterHiRes() - m_startMs) / 1000.0;
    const double at = (length > 0.0) ? std::fmod(elapsed, length) : 0.0;

    const double perCycle = item.wave.samplesPerCycle();
    const int total = (int)item.wave.size();
    const int window = juce::jlimit(2, total,
        (int)std::lround(perCycle * GuiGenWave::cycleChoices[(size_t)m_cycleIndex]));

    // 窓の頭は周期の切れ目へ合わせる。合わせないと、1 周期 9 ミリ秒の
    // 波形が 1 コマ (33 ミリ秒) で 3 周期ぶんも流れてしまい、形が読めない。
    // オシロスコープの同期と同じ考え方で、こうすると波形はその場で
    // 繰り返し、時間とともに音色と音量だけが変わって見える。
    const double cycles = (perCycle > 0.0)
        ? std::floor(at * item.wave.sampleRate / perCycle) : 0.0;

    const int start = juce::jlimit(0, juce::jmax(0, total - window),
        (int)std::lround(cycles * perCycle));

    GuiColor::Entry* const colours[] = {
        &GuiColor::GenWave::LineL, &GuiColor::GenWave::LineM, &GuiColor::GenWave::LineR,
    };

    const int laneHeight = area.getHeight() / 3;
    const int width = juce::jmax(1, area.getWidth());

    for (int lane = 0; lane < 3; ++lane)
    {
        auto strip = juce::Rectangle<int>(area.getX(), area.getY() + laneHeight * lane,
            area.getWidth(), laneHeight);

        juce::Path path;

        const float centreY = (float)strip.getCentreY();
        const float halfHeight = strip.getHeight() * 0.5f - 1.0f;

        for (int i = 0; i < width; ++i)
        {
            const size_t index = (size_t)start + (size_t)((juce::int64)window * i / width);

            if (index >= item.wave.size()) break;

            const float y = centreY - item.wave.at(lane, index) * halfHeight;

            if (i == 0) path.startNewSubPath((float)strip.getX(), y);
            else path.lineTo((float)(strip.getX() + i), y);
        }

        g.setColour(*colours[lane]);
        g.strokePath(path, juce::PathStrokeType(1.0f));
    }
}

// 波形ファイルと音声ファイルの止め絵。動かさないので、開いている間
// ずっと同じ形が出る。
void GuiParamBrowser::drawStill(juce::Graphics& g, juce::Rectangle<int> area, const Item& item)
{
    g.setColour(GuiColor::GenWave::Bg);
    g.fillRect(area);

    const float centreY = (float)area.getCentreY();
    const float halfHeight = area.getHeight() * 0.5f - 1.0f;
    const int width = juce::jmax(1, area.getWidth());

    g.setColour(GuiColor::GenWave::Axis);
    g.drawHorizontalLine(area.getCentreY(), (float)area.getX(), (float)area.getRight());

    if (item.kind == Kind::audio)
    {
        if (item.mins.empty())
        {
            g.setColour(GuiColor::ParamBrowser::HintText);
            g.setFont(juce::FontOptions(11.0f));
            g.drawText(EditorGuiText::ParamBrowser::noPreview, area,
                juce::Justification::centred, false);

            return;
        }

        g.setColour(GuiColor::WavePreview::AudioFile);

        for (int i = 0; i < width; ++i)
        {
            const size_t at = (size_t)((juce::int64)item.mins.size() * i / width);

            if (at >= item.mins.size()) break;

            const float top = centreY - item.maxs[at] * halfHeight;
            const float bottom = centreY - item.mins[at] * halfHeight;

            g.drawVerticalLine(area.getX() + i, juce::jmin(top, bottom), juce::jmax(top, bottom) + 1.0f);
        }

        return;
    }

    if (item.points.empty())
    {
        g.setColour(GuiColor::ParamBrowser::HintText);
        g.setFont(juce::FontOptions(11.0f));
        g.drawText(EditorGuiText::ParamBrowser::noPreview, area,
            juce::Justification::centred, false);

        return;
    }

    juce::Path path;

    for (int i = 0; i < width; ++i)
    {
        const size_t at = (size_t)((juce::int64)item.points.size() * i / width);

        if (at >= item.points.size()) break;

        const float y = centreY - item.points[at] * halfHeight;

        if (i == 0) path.startNewSubPath((float)area.getX(), y);
        else path.lineTo((float)(area.getX() + i), y);
    }

    g.setColour(GuiColor::WavePreview::WaveMemory);
    g.strokePath(path, juce::PathStrokeType(1.0f));
}

void GuiParamBrowser::drawScrollBar(juce::Graphics& g)
{
    const int rows = (int)m_view.size();
    const int shown = visibleRows();

    if (rows <= shown) return;

    auto track = listArea().removeFromRight(scrollBarWidth).reduced(2, 0);

    g.setColour(GuiColor::ParamBrowser::StripeBg);
    g.fillRect(track);

    const float ratio = (float)shown / (float)rows;
    const int thumbHeight = juce::jmax(20, (int)(track.getHeight() * ratio));
    const int span = track.getHeight() - thumbHeight;
    const int top = track.getY() + (int)(span * (float)m_scroll / (float)(rows - shown));

    g.setColour(GuiColor::ScrollBar::Thumb);
    g.fillRect(track.getX(), top, track.getWidth(), thumbHeight);
}
