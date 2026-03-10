#include "GuiPreset.h"
#include "../processor/PluginProcessor.h"

#include "../core/Global.h"
#include "../core/PrKeys.h"
#include "../core/PrValues.h"
#include "../core/FileValues.h"
#include "../core/PresetKeys.h"
#include "../core/PresetValues.h"

#include "../gui/GuiValues.h"
#include "../gui/GuiText.h"
#include "../gui/GuiStructs.h"

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

    /********************
    *
    * 3. Preset Table & Search
    *
    *********************/

    searchBox.setup({ .parent = *this, .title = PresetKey::Search::title, .isMultiLine = false });
    searchBox.onTextChange = [this] { applyFilter(); };

    clearSearchButton.setup({ .parent = *this, .title = PresetKey::Search::clear, .bgColor = juce::Colours::red.withAlpha(0.5f), .isReset = false});
    clearSearchButton.onClick = [this] {
        searchBox.setText(""); // テキストボックスを空にする
        applyFilter();         // リストの絞り込みをリセット（全件表示）する
    };

	table.setup({ .parent = *this, .title = PresetKey::Table::title, .canMultipleSelection = false });
    table.addColumn(PresetKey::Table::ColName::genre, 1, GuiValue::Preset::Table::ColWidth::Genre);
    table.addColumn(PresetKey::Table::ColName::name, 2, GuiValue::Preset::Table::ColWidth::PresetName);
    table.addColumn(PresetKey::Table::ColName::author, 3, GuiValue::Preset::Table::ColWidth::Author);
    table.addColumn(PresetKey::Table::ColName::version, 4, GuiValue::Preset::Table::ColWidth::Version);
    table.addColumn(PresetKey::Table::ColName::mode, 5, GuiValue::Preset::Table::ColWidth::Mode);
    table.addColumn(PresetKey::Table::ColName::lastModified, 6, GuiValue::Preset::Table::ColWidth::LastModified);
    table.onGetNumRows = [this]() {
        return (int)filteredItems.size();
        };
    table.onGetCellText = [this](int row, int columnId) {
        if (row >= filteredItems.size()) return juce::String();
        const auto& item = filteredItems[row];
        switch (columnId) {
        case 1: return item.genre;
        case 2: return item.name;
        case 3: return item.author;
        case 4: return item.version;
        case 5: return item.modeName;
        case 6: return item.lastModificationTime.formatted("%Y-%m-%d %H:%M");
        }
        return juce::String();
    };
    table.onDoubleClicked = [this](int row) {
        juce::File file = getSelectedFile();
        if (file.existsAsFile()) {
            ctx.editor.loadPresetFile(file);
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
        // 並び替え処理
        std::sort(filteredItems.begin(), filteredItems.end(),
            [newSortColumnId, isForwards](const PresetItem& a, const PresetItem& b) -> bool
            {
                int result = 0;
                switch (newSortColumnId)
                {
                case 1: result = a.fileName.compareNatural(b.fileName); break;
                case 2: result = a.name.compareNatural(b.name); break;
                case 3: result = a.author.compareNatural(b.author); break;
                case 4: result = a.version.compareNatural(b.version); break;
                case 5: result = a.modeName.compareNatural(b.modeName); break;
                    // 日時の比較
                case 6: result = (a.lastModificationTime < b.lastModificationTime) ? -1 : (a.lastModificationTime > b.lastModificationTime ? 1 : 0); break;
                default: break;
                }

                // isForwards (昇順) / !isForwards (降順) に応じて true/false を返す
                if (isForwards) return result < 0;
                else            return result > 0;
            });

        // 絞り込み元の元リスト(items)も同じようにソートしておくと、
        // 検索枠をクリアした時にソート順が維持されるので親切です。
        std::sort(items.begin(), items.end(),
            [newSortColumnId, isForwards](const PresetItem& a, const PresetItem& b) -> bool
            {
                // ... (上と全く同じロジックをコピー) ...
                int result = 0;
                switch (newSortColumnId) {
                case 1: result = a.fileName.compareNatural(b.fileName); break;
                case 2: result = a.name.compareNatural(b.name); break;
                case 3: result = a.author.compareNatural(b.author); break;
                case 4: result = a.version.compareNatural(b.version); break;
                case 5: result = a.modeName.compareNatural(b.modeName); break;
                case 6: result = (a.lastModificationTime < b.lastModificationTime) ? -1 : (a.lastModificationTime > b.lastModificationTime ? 1 : 0); break;
                }
                if (isForwards) return result < 0; else return result > 0;
            });

        // テーブルを再描画
        table.updateContent();
    };

    /********************
    *
    * 4. Metadata Group
    *
    *********************/

	metaGroup.setup(*this, PresetKey::MetaData::title);

    genreEditor.setup({ .parent = *this, .title = PresetKey::MetaData::Label::genre, .isMultiLine = false });
    genreEditor.setText(ctx.audioProcessor.presetGenre);
    genreEditor.onTextChange = [this] { ctx.audioProcessor.presetGenre = genreEditor.getText(); };

    filePathEditor.setup({ .parent = *this, .title = PresetKey::MetaData::Label::filePath, .isMultiLine = false });
    filePathEditor.setText(ctx.audioProcessor.presetFilePath);
    filePathEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colours::darkgrey.withAlpha(0.3f));
    filePathEditor.setReadOnly(true); // ユーザーには手打ちさせない

    // Name
	nameEditor.setup({ .parent = *this, .title = PresetKey::MetaData::Label::name, .isMultiLine = false });
    nameEditor.setText(ctx.audioProcessor.presetName);
    nameEditor.onTextChange = [this] { ctx.audioProcessor.presetName = nameEditor.getText(); };

    // Author
    authorEditor.setup({ .parent = *this, .title = PresetKey::MetaData::Label::auther, .isMultiLine = false });
    authorEditor.setText(ctx.audioProcessor.presetAuthor);
    authorEditor.onTextChange = [this] { ctx.audioProcessor.presetAuthor = authorEditor.getText(); };

    // Version
    versionEditor.setup({ .parent = *this, .title = PresetKey::MetaData::Label::version, .isMultiLine = false });
    versionEditor.setText(ctx.audioProcessor.presetVersion);
    versionEditor.onTextChange = [this] { ctx.audioProcessor.presetVersion = versionEditor.getText(); };

    // Comment
    commentEditor.setup({ .parent = *this, .title = PresetKey::MetaData::Label::comment, .isMultiLine = true, .isReturnKeyStartsNewLine = true });
    commentEditor.setText(ctx.audioProcessor.presetComment);
    commentEditor.onTextChange = [this] { ctx.audioProcessor.presetComment = commentEditor.getText(); };

    /********************
    *
    * 5. Buttons
    *
    *********************/

    // --- Init Preset Button ---
	initButton.setup({ .parent = *this, .title = PresetKey::Button::initPreset, .bgColor = juce::Colours::darkblue.withAlpha(0.7f) });
    initButton.onClick = [this] {
        // 確認ダイアログを表示
        juce::AlertWindow::showAsync(juce::MessageBoxOptions()
            .withIconType(juce::MessageBoxIconType::WarningIcon)
            .withTitle(GuiText::Preset::Dialog::initPreset)
            .withMessage(GuiText::Preset::Dialog::initPresetNotice)
            .withButton(GuiText::Preset::Dialog::initPresetSuccedBtn)
            .withButton(GuiText::Preset::Dialog::initPresetCancelBtn),
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
                    ctx.editor.updateRhythmFileNames(Io::empty);
                    ctx.editor.updateAdpcmFileName(Io::empty);
                    ctx.editor.updateOpzx3FileNames(Io::empty);

                    // 各タブのプリセット名をリセット
                    ctx.editor.updatePresetNameToTabs(ctx.audioProcessor.presetName);
                }
            }
        );
    };

    // --- Load Preset Info Button ---
	loadButton.setup({ .parent = *this, .title = PresetKey::Button::loadPreset, .isReset = false });
    loadButton.setEnabled(false);
    loadButton.onClick = [this] {
        auto file = getSelectedFile();

        if (file.existsAsFile()) ctx.editor.loadPresetFile(file);
    };

    // --- Save Preset Button ---
    saveButton.setup({ .parent = *this, .title = PresetKey::Button::savePreset });
    saveButton.onClick = [this] { ctx.editor.saveCurrentPreset(); };

    // --- Save Preset As Button ---
    saveAsButton.setup({ .parent = *this, .title = PresetKey::Button::savePresetAs, .bgColor = juce::Colours::darkgreen.withAlpha(0.7f) });
    saveAsButton.onClick = [this] { ctx.editor.saveCurrentPresetAs(); };

    // --- Delete Preset Button ---
	deleteButton.setup({ .parent = *this, .title = PresetKey::Button::deletePreset, .bgColor = juce::Colours::darkred.withAlpha(0.7f) });
    deleteButton.setEnabled(false);
    deleteButton.onClick = [this] {
        auto file = getSelectedFile();

        if (file.existsAsFile()) {
            // 確認ダイアログ
            juce::AlertWindow::showAsync(juce::MessageBoxOptions()
                .withIconType(juce::MessageBoxIconType::WarningIcon)
                .withTitle(GuiText::Preset::Dialog::deletePreset)
                .withMessage(GuiText::Preset::Dialog::deletePresetNotice + file.getFileName() + "?")
                .withButton(GuiText::Preset::Dialog::deletePresetSuccedBtn)
                .withButton(GuiText::Preset::Dialog::deletePresetCancelBtn),
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
	refreshButton.setup({ .parent = *this, .title = PresetKey::Button::refleshPresetList });
    refreshButton.onClick = [this] { ctx.editor.scanPresets(); };

    // --- Reflect Preset Info Button ---
	reflectButton.setup({ .parent = *this, .title = PresetKey::Button::reflectPresetInfo, .isReset = false });
    reflectButton.setTooltip(GuiText::Preset::Dialog::reflectPresetToolTipMessage);
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
	copyButton.setup({ .parent = *this, .title = PresetKey::Button::copyPresetInfoToClipboard, .isReset = false });
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

    // Path Label (Top)
    pathLabel.setBounds(pageArea.removeFromTop(GuiValue::Preset::FileLabelHeight));

    // Left: List
    auto listArea = pageArea.removeFromLeft(pageArea.getWidth() * GuiValue::Preset::Table::WidthRate);

    // リストのすぐ上に検索ボックスを配置する
    auto searchArea = listArea.removeFromTop(GuiValue::Preset::Search::Row::Height).reduced(GuiValue::Preset::Table::PaddingWidth, 0);

    searchBox.label.setBounds(searchArea.removeFromLeft(GuiValue::Preset::Search::Row::Button::Search::Width)); // "Search" というラベルの幅

    clearSearchButton.setBounds(searchArea.removeFromRight(GuiValue::Preset::Search::Row::Button::Clear::Width));

    searchArea.removeFromRight(GuiValue::Preset::Search::Row::Padding::Right);

    searchBox.setBounds(searchArea);

    listArea.removeFromTop(GuiValue::Preset::Search::Padding::Botton); // 検索ボックスとリストの間の少しの余白

    table.setBounds(listArea.reduced(GuiValue::Preset::Table::PaddingWidth, GuiValue::Preset::Table::PaddingHeight));

    // Right: Info & Buttons
    auto rightArea = pageArea;

    // Metadata Group
    auto metaArea = rightArea.removeFromTop(GuiValue::Preset::Meta::AreaHeight);

    metaGroup.setBounds(metaArea);

    auto mRect = metaArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mRect.removeFromTop(GuiValue::Group::TitlePaddingTop);

    // File Path
    auto row1 = mRect.removeFromTop(GuiValue::Preset::Meta::RowHeight).reduced(GuiValue::Preset::Meta::PaddingWidth, 0);
    filePathEditor.label.setBounds(row1.removeFromLeft(GuiValue::Preset::Meta::LabelWidth));
    filePathEditor.setBounds(row1);

    mRect.removeFromTop(GuiValue::Preset::Meta::PaddingHeight2);

    // Genre
    auto row2 = mRect.removeFromTop(GuiValue::Preset::Meta::RowHeight).reduced(GuiValue::Preset::Meta::PaddingWidth, 0);
    genreEditor.label.setBounds(row2.removeFromLeft(GuiValue::Preset::Meta::LabelWidth));
    genreEditor.setBounds(row2);

    mRect.removeFromTop(GuiValue::Preset::Meta::PaddingHeight);

    // Name
    auto row3 = mRect.removeFromTop(GuiValue::Preset::Meta::RowHeight).reduced(GuiValue::Preset::Meta::PaddingWidth, 0);
    nameEditor.label.setBounds(row3.removeFromLeft(GuiValue::Preset::Meta::LabelWidth));
    nameEditor.setBounds(row3);

    mRect.removeFromTop(GuiValue::Preset::Meta::PaddingHeight);

    // Author
    auto row4 = mRect.removeFromTop(GuiValue::Preset::Meta::RowHeight).reduced(GuiValue::Preset::Meta::PaddingWidth, 0);
    authorEditor.label.setBounds(row4.removeFromLeft(GuiValue::Preset::Meta::LabelWidth));
    authorEditor.setBounds(row4);

    mRect.removeFromTop(GuiValue::Preset::Meta::PaddingHeight);

    // Version
    auto row5 = mRect.removeFromTop(GuiValue::Preset::Meta::RowHeight).reduced(GuiValue::Preset::Meta::PaddingWidth, 0);
    versionEditor.label.setBounds(row5.removeFromLeft(GuiValue::Preset::Meta::LabelWidth));
    versionEditor.setBounds(row5);

    mRect.removeFromTop(GuiValue::Preset::Meta::PaddingHeight);

    // Comment
    auto row6 = mRect.removeFromTop(GuiValue::Preset::Meta::RowHeight).reduced(GuiValue::Preset::Meta::PaddingWidth, 0);
    commentEditor.label.setBounds(row6);

    mRect.removeFromTop(GuiValue::Preset::Meta::PaddingHeight);

    auto row7 = mRect.reduced(GuiValue::Preset::Meta::PaddingWidth, 0);
    commentEditor.setBounds(row7);

    rightArea.removeFromTop(GuiValue::Preset::Button::PaddingTop);

    // Buttons
    initButton.setBounds(rightArea.removeFromTop(GuiValue::Preset::Button::Height));

    rightArea.removeFromTop(GuiValue::Preset::Button::PaddingHeight);

    loadButton.setBounds(rightArea.removeFromTop(GuiValue::Preset::Button::Height));

    rightArea.removeFromTop(GuiValue::Preset::Button::PaddingHeight);

    saveButton.setBounds(rightArea.removeFromTop(GuiValue::Preset::Button::Height));

    rightArea.removeFromTop(GuiValue::Preset::Button::PaddingHeight);

    saveAsButton.setBounds(rightArea.removeFromTop(GuiValue::Preset::Button::Height));

    rightArea.removeFromTop(GuiValue::Preset::Button::PaddingHeight);

    deleteButton.setBounds(rightArea.removeFromTop(GuiValue::Preset::Button::Height));

    rightArea.removeFromTop(GuiValue::Preset::Button::PaddingHeight);

    refreshButton.setBounds(rightArea.removeFromTop(GuiValue::Preset::Button::Height));

    rightArea.removeFromTop(GuiValue::Preset::Button::PaddingHeight);

    reflectButton.setBounds(rightArea.removeFromTop(GuiValue::Preset::Button::Height));

    rightArea.removeFromTop(GuiValue::Preset::Button::PaddingHeight);

    copyButton.setBounds(rightArea.removeFromTop(GuiValue::Preset::Button::Height));
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

// 検索ボックスの文字列でリストを絞り込む関数
void GuiPreset::applyFilter()
{
    filteredItems.clear();
    juce::String query = searchBox.getText().trim().toLowerCase();

    // 検索窓が空なら全件表示
    if (query.isEmpty()) {
        filteredItems = items;
    }
    else {
        // ファイル名、プリセット名、作者名、コメント、モード名のどれかに合致したら表示
        for (const auto& item : items) {
            if (item.name.toLowerCase().contains(query) ||
                item.genre.toLowerCase().contains(query) ||
                item.author.toLowerCase().contains(query) ||
                item.comment.toLowerCase().contains(query) ||
                item.modeName.toLowerCase().contains(query))
            {
                filteredItems.push_back(item);
            }
        }
    }

    // テーブルに更新を通知
    table.updateContent();
}