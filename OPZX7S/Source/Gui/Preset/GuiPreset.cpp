#include "../../Core/Editor/EditorGuiValues.h"
#include "./GuiPreset.h"

#include "../../Core/Processor/PluginProcessor.h"

#include "../../Core/Const/ConstGlobal.h"
#include "../../Core/Processor/ProcessorBase.h"
#include "../../Core/Processor/ProcessorValues.h"
#include "../../Core/Const/ConstFileValues.h"
#include "./PresetKeys.h"
#include "./PresetValues.h"

#include "./GuiPresetValues.h"
#include "./GuiPresetText.h"
#include "../../Core/Gui/GuiStructs.h"
#include "../../Core/Editor/EditorGuiText.h"

#include <algorithm>

namespace
{
    // 一覧に出すチャンネルの名前。タブの見出しと同じにする。
    //
    // プリセットの中では WAVETABLE や ADPCM という綴りで持っているが、
    // 画面では WT や PCM と出している。絞り込みの選択肢だけ別の綴りに
    // なっていると、同じものだと気づけない。
    juce::String channelLabel(OscMode mode)
    {
        switch (mode)
        {
        case OscMode::OPZX7: return EditorGuiText::Tab::opzx7;
        default: break;
        }

        return getModeName(mode);
    }
}

// ============================================================================
// お気に入りの印
// ============================================================================
// 押せるものは描くだけでは足りないので、セルへ部品を置く。入れ物を挟んで
// いるのは、ボタンをセルいっぱいに広げると窮屈なため。
class FavouriteCell : public juce::Component
{
public:
	GuiTextButton button;

	// どの行のものかは差し替わる。押されたときに読むので、値を捕まえず
	// ここを見ること。
	int row = -1;

	explicit FavouriteCell(const GuiContext& context) : button(context)
	{
		addAndMakeVisible(button);
	}

	// 色は部品へ写るので、行を差し替えるたびに写し直す
	void applyColours()
	{
		button.setColour(juce::TextButton::textColourOffId, GuiColor::TextButton::Text);
		button.setColour(juce::TextButton::textColourOnId, GuiColor::TextButton::TextOn);
		button.setColour(juce::TextButton::buttonColourId, GuiColor::TextButton::Bg);
	}

	void resized() override
	{
		button.setBounds(getLocalBounds().reduced(PresetGuiValue::Table::CellPadding));
	}
};

juce::File GuiPreset::getSelectedFile() const
{
    int row = table.getSelectedRow();
    if (row >= 0 && row < filteredItems.size()) {
        return filteredItems[row].file;
    }
    return {};
}

void GuiPreset::setup()
{
    int tabOrder = 1;

    /********************
    *
    * 1. Folder
    *
    *********************/

    // defaultPresetDirから取ってくる
    auto defaultPath = ctx.audioProcessor.getDefaultPresetDir();

    currentFolder = juce::File(defaultPath);

    if (!currentFolder.exists()) {
        currentFolder.createDirectory();
    }

    /********************
    *
    * 2. Path Label
    *
    *********************/

    pathLabel.setup({ .parent = *this, .title = currentFolder.getFullPathName()});
    pathLabel.setJustificationType(juce::Justification::left);
    pathLabel.setColour(juce::Label::backgroundColourId, juce::Colours::transparentBlack.withAlpha(0.2f));
    pathLabel.setWantsKeyboardFocus(false);

    /********************
    *
    * 3. Preset Table & Search
    *
    *********************/

    // 一覧の見方。お気に入りと履歴は今見ているフォルダの外も指すので、
    // 元になる並びごと切り替える。
    std::vector<SelectItem> viewItems = {
        {.name = PresetKey::View::all, .value = 1 },
        {.name = PresetKey::View::favourites, .value = 2 },
        {.name = PresetKey::View::history, .value = 3 },
    };

    viewSelector.setup({ .parent = *this, .id = "", .title = PresetKey::View::title,
        .items = viewItems, .isReset = false });
    viewSelector.setSelectedId(1, juce::dontSendNotification);
    viewSelector.setWantsKeyboardFocus(true);
    viewSelector.setExplicitFocusOrder(++tabOrder);

    viewSelector.onChange = [this] {
        view = (View)viewSelector.getSelectedItemIndex();

        // 履歴を消せるのは履歴を見ているときだけ。ほかの見方で出して
        // いると、何が消えるのか分かりにくい。
        clearHistoryButton.setVisible(view == View::history);

        applyFilter();
        };

    // 形式での絞り込み。同じフォルダに JSON と YAML と XML が混ざると
    // 目で追いにくいので、出すものを絞れるようにしてある。
    std::vector<SelectItem> formatItems = {
        {.name = PresetKey::Format::all, .value = 1 },
        {.name = PresetKey::Format::xml, .value = 2 },
        {.name = PresetKey::Format::json, .value = 3 },
        {.name = PresetKey::Format::yaml, .value = 4 },
    };

    formatSelector.setup({ .parent = *this, .id = "", .title = PresetKey::Format::title,
        .items = formatItems, .isReset = false });
    formatSelector.setSelectedId(1, juce::dontSendNotification);
    formatSelector.setWantsKeyboardFocus(true);
    formatSelector.setExplicitFocusOrder(++tabOrder);

    formatSelector.onChange = [this] {
        formatFilter = (Format)formatSelector.getSelectedItemIndex();

        applyFilter();
        };

    clearHistoryButton.setup({ .parent = *this, .title = PresetKey::View::clearHistory,
        .textColor = juce::Colours::white, .bgColor = juce::Colours::red.withAlpha(0.5f), .isReset = false });
    clearHistoryButton.setWantsKeyboardFocus(true);
    clearHistoryButton.setExplicitFocusOrder(++tabOrder);
    clearHistoryButton.setVisible(false);

    clearHistoryButton.onClick = [this] {
        // 戻せないので確かめる
        juce::AlertWindow::showAsync(juce::MessageBoxOptions()
            .withIconType(juce::MessageBoxIconType::WarningIcon)
            .withTitle(PresetKey::View::Dialog::clearTitle)
            .withMessage(PresetKey::View::Dialog::clearMessage)
            .withButton(PresetKey::View::Dialog::clearOk)
            .withButton(PresetKey::View::Dialog::clearCancel),
            [this](int result) {
                if (result != 1) return;

                ctx.editor.presetLibrary.clearHistory();

                applyFilter();
            });
        };

    searchBox.setup({ .parent = *this, .title = PresetKey::Search::title, .isMultiLine = false });
    searchBox.setWantsKeyboardFocus(true);
    searchBox.setExplicitFocusOrder(++tabOrder);

    searchBox.onTextChange = [this] { applyFilter(); };

    clearSearchButton.setup({ .parent = *this, .title = PresetKey::Search::clear, .textColor = juce::Colours::white, .bgColor = juce::Colours::red.withAlpha(0.5f), .isReset = false});
    clearSearchButton.setWantsKeyboardFocus(true);
    clearSearchButton.setExplicitFocusOrder(++tabOrder);

    clearSearchButton.onClick = [this] {
        searchBox.setText(""); // テキストボックスを空にする
        applyFilter();         // リストの絞り込みをリセット（全件表示）する
    };

    // ------------------------------------------------------------------
    // チャンネルでの絞り込み
    // ------------------------------------------------------------------
    // 選択肢に並ぶのは、このプラグインが積んでいる音源だけ。OscMode の
    // 並びがそのままタブの並びなので、そこから起こす。
    std::vector<SelectItem> channelItems;

    for (int i = 0; i < (int)OscMode::Count; ++i)
    {
        channelItems.push_back({ .name = channelLabel((OscMode)i), .value = i + 1 });
    }

    channelSelector.setup({ .parent = *this, .id = "", .title = PresetKey::Channel::title,
        .items = channelItems, .isReset = false });
    channelSelector.setSelectedId(1, juce::dontSendNotification);
    channelSelector.setWantsKeyboardFocus(true);
    channelSelector.setExplicitFocusOrder(++tabOrder);

    channelSelector.onChange = [this] { refreshChannelRow(); };

    channelCheck.setup({ .parent = *this, .title = PresetKey::Channel::use, .isReset = false });
    channelCheck.setWantsKeyboardFocus(true);
    channelCheck.setExplicitFocusOrder(++tabOrder);

    channelCheck.onClick = [this] {
        const int index = channelSelector.getSelectedItemIndex();

        if (index >= 0 && index < (int)channelFilter.size())
        {
            channelFilter[(size_t)index] = channelCheck.getToggleState();
        }

        refreshChannelRow();
        applyFilter();
        };

    channelAllOnButton.setup({ .parent = *this, .title = PresetKey::Channel::allOn, .isReset = false });
    channelAllOnButton.setWantsKeyboardFocus(true);
    channelAllOnButton.setExplicitFocusOrder(++tabOrder);

    channelAllOnButton.onClick = [this] {
        channelFilter.fill(true);

        refreshChannelRow();
        applyFilter();
        };

    channelAllOffButton.setup({ .parent = *this, .title = PresetKey::Channel::allOff, .isReset = false });
    channelAllOffButton.setWantsKeyboardFocus(true);
    channelAllOffButton.setExplicitFocusOrder(++tabOrder);

    channelAllOffButton.onClick = [this] {
        channelFilter.fill(false);

        refreshChannelRow();
        applyFilter();
        };

    channelSummary.setup({ .parent = *this, .title = "",
        .justification = juce::Justification::centredLeft });

    // 音源を絞ったプラグインは、積んでいるものを最初から対象にしておく。
    // 目当ての音へ早く着けるため。すべてを積んでいる 2686V と 2686VLight
    // は、初めから絞っていると却って探しにくいので切っておく。
    channelFilter.fill((int)OscMode::Count < PresetGuiValue::Channel::FullCount);

    refreshChannelRow();

	table.setup({ .parent = *this, .title = PresetKey::Table::title, .canMultipleSelection = false });
    table.setWantsKeyboardFocus(false);
    table.addColumn(PresetKey::Table::ColName::favourite, 1, PresetGuiValue::Table::ColWidth::Favourite);
    table.addColumn(PresetKey::Table::ColName::genre, 2, PresetGuiValue::Table::ColWidth::Genre);
    table.addColumn(PresetKey::Table::ColName::name, 3, PresetGuiValue::Table::ColWidth::PresetName);
    table.addColumn(PresetKey::Table::ColName::author, 4, PresetGuiValue::Table::ColWidth::Author);
    table.addColumn(PresetKey::Table::ColName::version, 5, PresetGuiValue::Table::ColWidth::Version);
    table.addColumn(PresetKey::Table::ColName::mode, 6, PresetGuiValue::Table::ColWidth::Mode);
    table.addColumn(PresetKey::Table::ColName::lastModified, 7, PresetGuiValue::Table::ColWidth::LastModified);
    table.addColumn(PresetKey::Table::ColName::format, 8, PresetGuiValue::Table::ColWidth::Format);

    table.onGetNumRows = [this]() {
        return (int)filteredItems.size();
        };

    table.onGetCellText = [this](int row, int columnId) {
        if (row >= filteredItems.size()) return juce::String();
        const auto& item = filteredItems[row];
        switch (columnId) {
        // 1 列目は押せる印なので、文字ではなく部品を置く
        case 2: return item.genre;
        case 3: return item.name;
        case 4: return item.author;
        case 5: return item.version;
        case 6: return item.modeName;
        case 7: return item.lastModificationTime.formatted("%Y-%m-%d %H:%M");
        case 8: return item.format;
        }
        return juce::String();
    };

    // お気に入りの印は押せるので、描くだけでは足りない
    table.onRefreshCellComponent = [this](int row, int columnId, bool, juce::Component* existing)
        -> juce::Component* {
        if (columnId != 1 || row < 0 || row >= (int)filteredItems.size()) {
            delete existing;

            return nullptr;
        }

        auto* cell = dynamic_cast<FavouriteCell*>(existing);

        if (cell == nullptr) {
            delete existing;

            cell = new FavouriteCell(ctx);

            cell->button.onClick = [this, cell] {
                if (cell->row < 0 || cell->row >= (int)filteredItems.size()) return;

                // 写しを取る。この後で並びを作り直すので、参照のままだと
                // 指す先が消える。
                auto target = filteredItems[cell->row];

                ctx.editor.presetLibrary.setFavourite(target,
                    !ctx.editor.presetLibrary.isFavourite(target.fullPath));

                // お気に入りを見ているときは、外したものが一覧から消える。
                // 作り直すと今押しているボタンごと片付けられてしまうので、
                // この処理から抜けてから行う。
                juce::Component::SafePointer<GuiPreset> safe(this);

                juce::MessageManager::callAsync([safe] {
                    if (safe != nullptr) safe->applyFilter();
                    });
                };
        }

        cell->row = row;
        cell->applyColours();
        cell->button.setButtonText(ctx.editor.presetLibrary.isFavourite(filteredItems[row].fullPath)
            ? PresetKey::View::Mark::on : PresetKey::View::Mark::off);

        return cell;
        };

    // ホバー時にコメント文字列をツールチップとして返す
    table.onGetCellTooltip = [this](int row, int columnId) {
        if (row >= 0 && row < filteredItems.size()) {
            juce::String comment = filteredItems[row].comment.trim(); // 前後の余白を削除

            // コメントが空欄でない場合のみ返す（空ならJUCE側で勝手に非表示にしてくれます）
            if (comment.isNotEmpty()) {
                bool isTruncated = false;

                // 1. 行数制限のチェック（改行で分割して配列にする）
                juce::StringArray lines = juce::StringArray::fromLines(comment);
                if (lines.size() > 3) {
                    // 3行目までを結合して上書き
                    comment = lines[0] + "\n" + lines[1] + "\n" + lines[2];
                    isTruncated = true;
                }

                // 2. 文字数制限のチェック（128文字を超える場合）
                if (comment.length() > 128) {
                    // 0文字目から128文字分だけを切り出す
                    comment = comment.substring(0, 128);
                    isTruncated = true;
                }

                // 3. どちらかの制限に引っかかっていたら末尾に "..." を追加
                if (isTruncated) {
                    comment += "...";
                }

                return comment;
            }
        }
        return juce::String();
    };

    table.onDoubleClicked = [this](int row) {
        juce::File file = getSelectedFile();
        if (file.existsAsFile()) {
            ctx.editor.loadPresetFile(file);

            // FXの順番は直に実行
            ctx.editor.updateFxOrder();
        }
    };

    table.onSelectionChanged = [this](int lastRow) {
        bool hasSelection = table.getNumSelectedRows() > 0;
        deleteButton.setEnabled(hasSelection);
        reflectButton.setEnabled(hasSelection);
        copyButton.setEnabled(hasSelection);
        loadButton.setEnabled(hasSelection);
    };

    table.onSortOrderChanged = [this](int newSortColumnId, bool isForwards) {
        sortColumnId = newSortColumnId;
        sortForwards = isForwards;

        // ここで並べても、次の絞り込みで作り直されて消えてしまう。
        // 並べ替えは絞り込みの最後で掛けるので、そちらへ任せる。
        applyFilter();
        };

    /********************
    *
    * 4. Metadata Group
    *
    *********************/

	metaGroup.setup(*this, PresetKey::MetaData::title);

    genreEditor.setup({ .parent = *this, .title = PresetKey::MetaData::Label::genre, .isMultiLine = false });
    genreEditor.setText(ctx.audioProcessor.presetGenre);
    genreEditor.setWantsKeyboardFocus(true);
    genreEditor.setExplicitFocusOrder(++tabOrder);
    genreEditor.onTextChange = [this] { ctx.audioProcessor.presetGenre = genreEditor.getText(); };

    filePathEditor.setup({ .parent = *this, .title = PresetKey::MetaData::Label::filePath, .isMultiLine = false });
    filePathEditor.setText(ctx.audioProcessor.presetFilePath);
    filePathEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colours::darkgrey.withAlpha(0.3f));
    filePathEditor.setReadOnly(true); // ユーザーには手打ちさせない
    filePathEditor.setWantsKeyboardFocus(false);

    // Name
	nameEditor.setup({ .parent = *this, .title = PresetKey::MetaData::Label::name, .isMultiLine = false });
    nameEditor.setText(ctx.audioProcessor.presetName);
    nameEditor.setWantsKeyboardFocus(true);
    nameEditor.setExplicitFocusOrder(++tabOrder);
    nameEditor.onTextChange = [this] {
        ctx.audioProcessor.presetName = nameEditor.getText();
        ctx.editor.updatePresetNameToTabs(ctx.audioProcessor.presetName);
    };

    // Author
    authorEditor.setup({ .parent = *this, .title = PresetKey::MetaData::Label::auther, .isMultiLine = false });
    authorEditor.setText(ctx.audioProcessor.presetAuthor);
    authorEditor.setWantsKeyboardFocus(true);
    authorEditor.setExplicitFocusOrder(++tabOrder);
    authorEditor.onTextChange = [this] { ctx.audioProcessor.presetAuthor = authorEditor.getText(); };

    // Version
    versionEditor.setup({ .parent = *this, .title = PresetKey::MetaData::Label::version, .isMultiLine = false });
    versionEditor.setText(ctx.audioProcessor.presetVersion);
    versionEditor.setWantsKeyboardFocus(true);
    versionEditor.setExplicitFocusOrder(++tabOrder);
    versionEditor.onTextChange = [this] { ctx.audioProcessor.presetVersion = versionEditor.getText(); };

    // Comment
    commentEditor.setup({ .parent = *this, .title = PresetKey::MetaData::Label::comment, .isMultiLine = true, .isReturnKeyStartsNewLine = true });
    commentEditor.setText(ctx.audioProcessor.presetComment);
    commentEditor.setWantsKeyboardFocus(true);
    commentEditor.setExplicitFocusOrder(++tabOrder);
    commentEditor.onTextChange = [this] { ctx.audioProcessor.presetComment = commentEditor.getText(); };

    /********************
    *
    * 5. Buttons
    *
    *********************/

    // --- Init Preset Button ---
	initButton.setup({ .parent = *this, .title = PresetKey::Button::initPreset, .font = buttonFont, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkblue.withAlpha(0.7f)});
    initButton.setWantsKeyboardFocus(true);
    initButton.setExplicitFocusOrder(++tabOrder);
    initButton.onClick = [this] {
        // 確認ダイアログを表示
        juce::AlertWindow::showAsync(juce::MessageBoxOptions()
            .withIconType(juce::MessageBoxIconType::WarningIcon)
            .withTitle(PresetGuiText::Preset::Dialog::initPreset)
            .withMessage(PresetGuiText::Preset::Dialog::initPresetNotice)
            .withButton(PresetGuiText::Preset::Dialog::initPresetSuccedBtn)
            .withButton(PresetGuiText::Preset::Dialog::initPresetCancelBtn),
            [this](int result) {
                if (result == 1) { // Initializeボタンが押された
                    // 1. プロセッサ側の初期化実行
                    ctx.audioProcessor.initPreset();

                    // 2. エディタの表示更新
                    // テキストエディタへの反映
                    nameEditor.setText(ctx.audioProcessor.presetName);
                    authorEditor.setText(ctx.audioProcessor.presetAuthor);
                    versionEditor.setText(ctx.audioProcessor.presetVersion);
                    commentEditor.setText(ctx.audioProcessor.presetComment);
                    genreEditor.setText(ctx.audioProcessor.presetGenre);
                    filePathEditor.setText(ctx.audioProcessor.presetFilePath);

                    // ファイル名表示のクリア
                    ctx.editor.updateOpzx7PcmFileNames(Io::empty);
                    ctx.editor.updateOpzx7WtFileNames(Io::empty);

                    // 各タブのプリセット名をリセット
                    ctx.editor.updatePresetNameToTabs(ctx.audioProcessor.presetName);
                }
            }
        );
    };

    // --- Load Preset Info Button ---
	loadButton.setup({ .parent = *this, .title = PresetKey::Button::loadPreset, .font = buttonFont,
		.textColor = juce::Colours::black, .disabledTextColor = juce::Colours::white, .isReset = false });
    loadButton.setEnabled(false);
    loadButton.setWantsKeyboardFocus(true);
    loadButton.setExplicitFocusOrder(++tabOrder);
    loadButton.onClick = [this] {
        auto file = getSelectedFile();

        if (file.existsAsFile()) {
            ctx.editor.loadPresetFile(file);

            // FXの順番は直に実行
            ctx.editor.updateFxOrder();
        }
    };

    // --- Save Preset Button ---
    saveButton.setup({ .parent = *this, .title = PresetKey::Button::savePreset, .font = buttonFont });
    saveButton.setWantsKeyboardFocus(true);
    saveButton.setExplicitFocusOrder(++tabOrder);
    saveButton.onClick = [this] { ctx.editor.saveCurrentPreset(); };

    // --- Save Preset As Button ---
    saveAsButton.setup({ .parent = *this, .title = PresetKey::Button::savePresetAs, .font = buttonFont, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkgreen.withAlpha(0.7f) });
    saveAsButton.setWantsKeyboardFocus(true);
    saveAsButton.setExplicitFocusOrder(++tabOrder);
    saveAsButton.onClick = [this] { ctx.editor.saveCurrentPresetAs(); };

    // --- Delete Preset Button ---
	deleteButton.setup({ .parent = *this, .title = PresetKey::Button::deletePreset, .font = buttonFont, .textColor = juce::Colours::white, .bgColor = juce::Colours::darkred.withAlpha(0.7f) });
    deleteButton.setWantsKeyboardFocus(true);
    deleteButton.setExplicitFocusOrder(++tabOrder);
    deleteButton.setEnabled(false);
    deleteButton.onClick = [this] {
        auto file = getSelectedFile();

        if (file.existsAsFile()) {
            // 確認ダイアログ
            juce::AlertWindow::showAsync(juce::MessageBoxOptions()
                .withIconType(juce::MessageBoxIconType::WarningIcon)
                .withTitle(PresetGuiText::Preset::Dialog::deletePreset)
                .withMessage(PresetGuiText::Preset::Dialog::deletePresetNotice + file.getFileName() + "?")
                .withButton(PresetGuiText::Preset::Dialog::deletePresetSuccedBtn)
                .withButton(PresetGuiText::Preset::Dialog::deletePresetCancelBtn),
                [this, file](int result) {
                    if (result == 1) { // Delete
                        file.deleteFile();
                        ctx.editor.scanPresets(); // リスト更新
                    }
                }
            );
        }
    };

    // --- Reflesh Preset List Button ---
	refreshButton.setup({ .parent = *this, .title = PresetKey::Button::refleshPresetList, .font = buttonFont });
    refreshButton.setWantsKeyboardFocus(true);
    refreshButton.setExplicitFocusOrder(++tabOrder);
    refreshButton.onClick = [this] { ctx.editor.scanPresets(); };

    // --- Reflect Preset Info Button ---
	reflectButton.setup({ .parent = *this, .title = PresetKey::Button::reflectPresetInfo, .font = buttonFont, .isReset = false });
    reflectButton.setWantsKeyboardFocus(true);
    reflectButton.setExplicitFocusOrder(++tabOrder);
    reflectButton.onClick = [this] {
        int row = table.getSelectedRow();

        if (row >= 0 && row < filteredItems.size()) {
            const auto& item = filteredItems[row];

            nameEditor.setText(item.name);
            authorEditor.setText(item.author);
            versionEditor.setText(item.version);
            commentEditor.setText(item.comment);
            genreEditor.setText(item.genre);
            filePathEditor.setText(item.fullPath);
        }
    };

    // --- Copy Preset Info to Clipboard Button ---
	copyButton.setup({ .parent = *this, .title = PresetKey::Button::copyPresetInfoToClipboard, .font = buttonFont,
		.textColor = juce::Colours::black, .disabledTextColor = juce::Colours::white, .isReset = false });
    copyButton.setWantsKeyboardFocus(true);
    copyButton.setExplicitFocusOrder(++tabOrder);
    copyButton.setEnabled(false);
    copyButton.onClick = [this] {
        int row = table.getSelectedRow();
        if (row >= 0 && row < filteredItems.size()) {
            const auto& item = filteredItems[row];
            juce::String info = PresetValue::MetaData::ClipBoardPrefix::name + item.name + "\n" +
                PresetValue::MetaData::ClipBoardPrefix::genre + item.genre + "\n" +
                PresetValue::MetaData::ClipBoardPrefix::auther + item.author + "\n" +
                PresetValue::MetaData::ClipBoardPrefix::version + item.version + "\n" +
                PresetValue::MetaData::ClipBoardPrefix::comment + item.comment + "\n" +
                PresetValue::MetaData::ClipBoardPrefix::mode + item.modeName +
                PresetValue::MetaData::ClipBoardPrefix::filePath + item.fullPath;
            juce::SystemClipboard::copyTextToClipboard(info);
        }
    };
}

void GuiPreset::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // タブの下辺とグループの見出しが詰まって見えるので、少しだけ離す。
    // ここで取るのは、上の withZeroOrigin() が渡された位置を捨てるため。
    pageArea.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

    // Path Label (Top)
    pathLabel.setBounds(pageArea.removeFromTop(PresetGuiValue::FileLabelHeight));

    // Left: List
    auto listArea = pageArea.removeFromLeft(pageArea.getWidth() * PresetGuiValue::Table::WidthRate);

    // リストのすぐ上に検索ボックスを配置する
    auto searchArea = listArea.removeFromTop(PresetGuiValue::Search::Row::Height).reduced(PresetGuiValue::Table::PaddingWidth, 0);

    // 見方の切り替えを左端に置く。検索はどの見方でも効く。
    viewSelector.label.setBounds(searchArea.removeFromLeft(PresetGuiValue::View::LabelWidth));
    viewSelector.setBounds(searchArea.removeFromLeft(PresetGuiValue::View::Width));

    searchArea.removeFromLeft(PresetGuiValue::View::PaddingRight);

    // 形式の絞り込みは見方の右隣。どちらも一覧の元を決めるものなので並べる。
    formatSelector.label.setBounds(searchArea.removeFromLeft(PresetGuiValue::Format::LabelWidth));
    formatSelector.setBounds(searchArea.removeFromLeft(PresetGuiValue::Format::Width));

    searchArea.removeFromLeft(PresetGuiValue::Format::PaddingRight);

    clearHistoryButton.setBounds(searchArea.removeFromRight(PresetGuiValue::View::ClearHistoryWidth));

    searchArea.removeFromRight(PresetGuiValue::Search::Row::Padding::Right);

    searchBox.label.setBounds(searchArea.removeFromLeft(PresetGuiValue::Search::Row::Button::Search::Width)); // "Search" というラベルの幅

    clearSearchButton.setBounds(searchArea.removeFromRight(PresetGuiValue::Search::Row::Button::Clear::Width));

    searchArea.removeFromRight(PresetGuiValue::Search::Row::Padding::Right);

    searchBox.setBounds(searchArea);

    listArea.removeFromTop(PresetGuiValue::Search::Padding::Botton); // 検索ボックスとリストの間の少しの余白

    // 検索の行と一覧の間へ、チャンネルの絞り込みを 1 行入れる
    auto channelArea = listArea.removeFromTop(PresetGuiValue::Channel::RowHeight)
        .reduced(PresetGuiValue::Table::PaddingWidth, 0);

    channelSelector.label.setBounds(channelArea.removeFromLeft(PresetGuiValue::Channel::LabelWidth));
    channelSelector.setBounds(channelArea.removeFromLeft(PresetGuiValue::Channel::Width));

    channelArea.removeFromLeft(PresetGuiValue::Channel::PaddingRight);

    channelCheck.setBounds(channelArea.removeFromLeft(PresetGuiValue::Channel::CheckWidth));

    channelArea.removeFromLeft(PresetGuiValue::Channel::PaddingRight);

    channelAllOnButton.setBounds(channelArea.removeFromLeft(PresetGuiValue::Channel::ButtonWidth));

    channelArea.removeFromLeft(PresetGuiValue::Channel::ButtonGap);

    channelAllOffButton.setBounds(channelArea.removeFromLeft(PresetGuiValue::Channel::ButtonWidth));

    channelArea.removeFromLeft(PresetGuiValue::Channel::SummaryPaddingLeft);

    channelSummary.setBounds(channelArea);

    listArea.removeFromTop(PresetGuiValue::Channel::PaddingBottom);

    table.setBounds(listArea.reduced(PresetGuiValue::Table::PaddingWidth, PresetGuiValue::Table::PaddingHeight));

    // Right: Info & Buttons
    auto rightArea = pageArea;

    // Metadata Group
    auto metaArea = rightArea.removeFromTop(PresetGuiValue::Meta::AreaHeight);

    metaGroup.setBounds(metaArea);

    auto mRect = metaArea.reduced(PresetGuiValue::Group::Padding::width, PresetGuiValue::Group::Padding::height);

    mRect.removeFromTop(PresetGuiValue::Group::TitlePaddingTop);

    // File Path
    auto row1 = mRect.removeFromTop(PresetGuiValue::Meta::RowHeight).reduced(PresetGuiValue::Meta::PaddingWidth, 0);
    filePathEditor.label.setBounds(row1.removeFromLeft(PresetGuiValue::Meta::LabelWidth));
    filePathEditor.setBounds(row1);

    mRect.removeFromTop(PresetGuiValue::Meta::PaddingHeight2);

    // Genre
    auto row2 = mRect.removeFromTop(PresetGuiValue::Meta::RowHeight).reduced(PresetGuiValue::Meta::PaddingWidth, 0);
    genreEditor.label.setBounds(row2.removeFromLeft(PresetGuiValue::Meta::LabelWidth));
    genreEditor.setBounds(row2);

    mRect.removeFromTop(PresetGuiValue::Meta::PaddingHeight);

    // Name
    auto row3 = mRect.removeFromTop(PresetGuiValue::Meta::RowHeight).reduced(PresetGuiValue::Meta::PaddingWidth, 0);
    nameEditor.label.setBounds(row3.removeFromLeft(PresetGuiValue::Meta::LabelWidth));
    nameEditor.setBounds(row3);

    mRect.removeFromTop(PresetGuiValue::Meta::PaddingHeight);

    // Author
    auto row4 = mRect.removeFromTop(PresetGuiValue::Meta::RowHeight).reduced(PresetGuiValue::Meta::PaddingWidth, 0);
    authorEditor.label.setBounds(row4.removeFromLeft(PresetGuiValue::Meta::LabelWidth));
    authorEditor.setBounds(row4);

    mRect.removeFromTop(PresetGuiValue::Meta::PaddingHeight);

    // Version
    auto row5 = mRect.removeFromTop(PresetGuiValue::Meta::RowHeight).reduced(PresetGuiValue::Meta::PaddingWidth, 0);
    versionEditor.label.setBounds(row5.removeFromLeft(PresetGuiValue::Meta::LabelWidth));
    versionEditor.setBounds(row5);

    mRect.removeFromTop(PresetGuiValue::Meta::PaddingHeight);

    // Comment
    auto row6 = mRect.removeFromTop(PresetGuiValue::Meta::RowHeight).reduced(PresetGuiValue::Meta::PaddingWidth, 0);
    commentEditor.label.setBounds(row6);

    mRect.removeFromTop(PresetGuiValue::Meta::PaddingHeight);

    auto row7 = mRect.reduced(PresetGuiValue::Meta::PaddingWidth, 0);
    commentEditor.setBounds(row7);

    rightArea.removeFromTop(PresetGuiValue::Button::PaddingTop);

    // Buttons
    initButton.setBounds(rightArea.removeFromTop(PresetGuiValue::Button::Height));

    rightArea.removeFromTop(PresetGuiValue::Button::PaddingHeight);

    loadButton.setBounds(rightArea.removeFromTop(PresetGuiValue::Button::Height));

    rightArea.removeFromTop(PresetGuiValue::Button::PaddingHeight);

    saveButton.setBounds(rightArea.removeFromTop(PresetGuiValue::Button::Height));

    rightArea.removeFromTop(PresetGuiValue::Button::PaddingHeight);

    saveAsButton.setBounds(rightArea.removeFromTop(PresetGuiValue::Button::Height));

    rightArea.removeFromTop(PresetGuiValue::Button::PaddingHeight);

    deleteButton.setBounds(rightArea.removeFromTop(PresetGuiValue::Button::Height));

    rightArea.removeFromTop(PresetGuiValue::Button::PaddingHeight);

    refreshButton.setBounds(rightArea.removeFromTop(PresetGuiValue::Button::Height));

    rightArea.removeFromTop(PresetGuiValue::Button::PaddingHeight);

    reflectButton.setBounds(rightArea.removeFromTop(PresetGuiValue::Button::Height));

    rightArea.removeFromTop(PresetGuiValue::Button::PaddingHeight);

    copyButton.setBounds(rightArea.removeFromTop(PresetGuiValue::Button::Height));
}

void GuiPreset::setMetaData(const juce::String& name, const juce::String& author, const juce::String& version, const juce::String& comment, const juce::String& genre, const juce::String& filePath)
{
    nameEditor.setText(name);
    authorEditor.setText(author);
    versionEditor.setText(version);
    commentEditor.setText(comment);
    genreEditor.setText(genre);
    filePathEditor.setText(filePath);
}

void GuiPreset::clearTable()
{
    items.clear();
}

// テーブル更新時にフィルター関数を呼び出すようにする
void GuiPreset::updateTableContent()
{
    applyFilter();
}

void GuiPreset::repaintTable()
{
    table.repaint();
}

void GuiPreset::updatePresetPath()
{
    pathLabel.setText(currentFolder.getFullPathName(), juce::dontSendNotification);
}

// チェックと、いま対象にしているものの文字を今の値へ合わせる
void GuiPreset::refreshChannelRow()
{
    const int index = channelSelector.getSelectedItemIndex();
    const bool on = index >= 0 && index < (int)channelFilter.size()
        && channelFilter[(size_t)index];

    channelCheck.setToggleState(on, juce::dontSendNotification);

    juce::StringArray names;

    for (int i = 0; i < (int)OscMode::Count; ++i)
    {
        if (channelFilter[(size_t)i]) names.add(channelLabel((OscMode)i));
    }

    channelSummary.setText(PresetKey::Channel::summary
        + (names.isEmpty() ? PresetKey::Channel::summaryNone : names.joinIntoString(" / ")),
        juce::dontSendNotification);
}

// 覚えている並べ替えを、今の一覧へ掛け直す
void GuiPreset::sortFiltered()
{
    if (sortColumnId <= 0) return;

    const int columnId = sortColumnId;
    const bool forwards = sortForwards;

    // 同じ値のものが入れ替わらないよう、安定な並べ替えを使う。
    // 押すたびに順番が変わると、目で追えなくなる。
    std::stable_sort(filteredItems.begin(), filteredItems.end(),
        [this, columnId, forwards](const PresetItem& a, const PresetItem& b) -> bool
        {
            int result = 0;

            switch (columnId)
            {
                // お気に入りが先へ来るように、入っているほうを小さく扱う
            case 1:
            {
                const int favouriteA = ctx.editor.presetLibrary.isFavourite(a.fullPath) ? 1 : 0;
                const int favouriteB = ctx.editor.presetLibrary.isFavourite(b.fullPath) ? 1 : 0;

                result = favouriteB - favouriteA;

                break;
            }
            case 2: result = a.genre.compareNatural(b.genre); break;
            case 3: result = a.name.compareNatural(b.name); break;
            case 4: result = a.author.compareNatural(b.author); break;
            case 5: result = a.version.compareNatural(b.version); break;
            case 6: result = a.modeName.compareNatural(b.modeName); break;
            case 7: result = a.lastModificationTime < b.lastModificationTime ? -1
                : (a.lastModificationTime > b.lastModificationTime ? 1 : 0); break;
            case 8: result = a.format.compareNatural(b.format); break;
            default: break;
            }

            return forwards ? (result < 0) : (result > 0);
        });
}

// 検索ボックスの文字列でリストを絞り込む関数
void GuiPreset::applyFilter()
{
    filteredItems.clear();

    // 見方によって元になる並びが変わる。お気に入りと履歴は今見ている
    // フォルダの外も指すので、覚えてあるものをそのまま使う。
    const std::vector<PresetItem>& source =
        view == View::favourites ? ctx.editor.presetLibrary.getFavourites() :
        view == View::history ? ctx.editor.presetLibrary.getHistory() :
        items;

    juce::String query = searchBox.getText().trim().toLowerCase();

    // PresetItem::format は拡張子を大文字にしたもの。空なら形式で絞らない。
    const juce::String wantFormat =
        formatFilter == Format::xml ? PresetKey::Format::xml :
        formatFilter == Format::json ? PresetKey::Format::json :
        formatFilter == Format::yaml ? PresetKey::Format::yaml : juce::String();

    // チャンネルの絞り込み。ひとつも入っていなければ絞らない。
    // 「すべて入っている」と同じ結果になるので、どちらでも困らない。
    const bool byChannel = std::any_of(channelFilter.begin(), channelFilter.end(),
        [](bool on) { return on; });

    // ファイル名、プリセット名、ジャンル、作者名、コメント、チャンネルの
    // どれかに合っていたら出す。検索窓が空ならすべて出す。
    for (const auto& item : source) {
        if (wantFormat.isNotEmpty() && item.format != wantFormat) continue;

        if (byChannel)
        {
            bool wanted = false;

            for (int i = 0; i < (int)OscMode::Count; ++i)
            {
                if (!channelFilter[(size_t)i]) continue;
                if (getModeName((OscMode)i) != item.modeName) continue;

                wanted = true;

                break;
            }

            if (!wanted) continue;
        }

        if (query.isNotEmpty()
            && !item.name.toLowerCase().contains(query)
            && !item.genre.toLowerCase().contains(query)
            && !item.author.toLowerCase().contains(query)
            && !item.comment.toLowerCase().contains(query)
            && !item.modeName.toLowerCase().contains(query)
            && !item.fileName.toLowerCase().contains(query))
        {
            continue;
        }

        filteredItems.push_back(item);
    }

    // 並びを作り直したので、覚えている並べ替えを掛け直す
    sortFiltered();

    // テーブルに更新を通知
    table.updateContent();
    table.repaint();
}