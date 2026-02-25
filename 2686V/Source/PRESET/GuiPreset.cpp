#include "GuiPreset.h"
#include "../processor/PluginProcessor.h"

#include "../core/GuiConstants.h"
#include "../core/LabelConstants.h"
#include "../core/OpConstants.h"
#include "../core/GlobalConstants.h"
#include "../core/FileConstants.h"
#include "../core/PresetConstants.h"

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

    /********************
    *
    * 3. Preset Table & Search
    *
    *********************/

    searchBox.setup({ .parent = *this, .title = "Search", .isMultiLine = false });
    searchBox.onTextChange = [this] { applyFilter(); };

    clearSearchButton.setup({ .parent = *this, .title = "X", .bgColor = juce::Colours::red.withAlpha(0.5f), .isReset = false});
    clearSearchButton.onClick = [this] {
        searchBox.setText(""); // テキストボックスを空にする
        applyFilter();         // リストの絞り込みをリセット（全件表示）する
    };

	table.setup({ .parent = *this, .title = "Presets", .canMultipleSelection = false });
    table.addColumn(presetTableFileNameColTitle, 1, PresetTableFileNameColTitleWidth);
    table.addColumn(presetTablePresetNameColTitle, 2, PresetTablePresetNameColTitleWidth);
    table.addColumn(presetTableAuthorColTitle, 3, PresetTableAuthorColTitleWidth);
    table.addColumn(presetTableVersionColTitle, 4, PresetTableVersionColTitleWidth);
    table.addColumn(presetTableModeColTitle, 5, PresetTableModeColTitleWidth);
    table.onGetNumRows = [this]() {
        return (int)filteredItems.size();
        };
    table.onGetCellText = [this](int row, int columnId) {
        if (row >= filteredItems.size()) return juce::String();
        const auto& item = filteredItems[row];
        switch (columnId) {
        case 1: return item.fileName;
        case 2: return item.name;
        case 3: return item.author;
        case 4: return item.version;
        case 5: return item.modeName;
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
        
    /********************
    *
    * 4. Metadata Group
    *
    *********************/

	metaGroup.setup(*this, "Preset Info");

    // Name
	nameEditor.setup({ .parent = *this, .title = presetCbName, .isMultiLine = false });
    nameEditor.setText(ctx.audioProcessor.presetName);
    nameEditor.onTextChange = [this] { ctx.audioProcessor.presetName = nameEditor.getText(); };

    // Author
    authorEditor.setup({ .parent = *this, .title = presetCbAuther, .isMultiLine = false });
    authorEditor.setText(ctx.audioProcessor.presetAuthor);
    authorEditor.onTextChange = [this] { ctx.audioProcessor.presetAuthor = authorEditor.getText(); };

    // Version
    versionEditor.setup({ .parent = *this, .title = presetCbVersion, .isMultiLine = false });
    versionEditor.setText(ctx.audioProcessor.presetVersion);
    versionEditor.onTextChange = [this] { ctx.audioProcessor.presetVersion = versionEditor.getText(); };

    // Comment
    commentEditor.setup({ .parent = *this, .title = presetCbComment, .isMultiLine = true, .isReturnKeyStartsNewLine = true });
    commentEditor.setText(ctx.audioProcessor.presetVersion);
    commentEditor.onTextChange = [this] { ctx.audioProcessor.presetComment = commentEditor.getText(); };

    /********************
    *
    * 5. Buttons
    *
    *********************/

    // --- Init Preset Button ---
	initButton.setup({ .parent = *this, .title = initPresetBtnLabel, .bgColor = juce::Colours::darkblue.withAlpha(0.7f) });
    initButton.onClick = [this] {
        // 確認ダイアログを表示
        juce::AlertWindow::showAsync(juce::MessageBoxOptions()
            .withIconType(juce::MessageBoxIconType::WarningIcon)
            .withTitle("Initialize Preset")
            .withMessage("Are you sure you want to initialize all parameters and unload samples?")
            .withButton("Initialize")
            .withButton("Cancel"),
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

                    // ファイル名表示のクリア
                    ctx.editor.updateRhythmFileNames(emptyFilename);
                    ctx.editor.updateAdpcmFileName(emptyFilename);
                }
            }
        );
    };

    // --- Load Preset Info Button ---
	loadButton.setup({ .parent = *this, .title = "Load Preset", .isReset = false });
    loadButton.setEnabled(false);
    loadButton.onClick = [this] {
        auto file = getSelectedFile();

        if (file.existsAsFile()) ctx.editor.loadPresetFile(file);
    };

    // --- Save Preset Button ---
    saveButton.setup({ .parent = *this, .title = "Save Preset" });
    saveButton.onClick = [this] { ctx.editor.saveCurrentPreset(); };

    // --- Delete Preset Button ---
	deleteButton.setup({ .parent = *this, .title = "Delete Preset", .bgColor = juce::Colours::darkred.withAlpha(0.7f) });
    deleteButton.setEnabled(false);
    deleteButton.onClick = [this] {
        auto file = getSelectedFile();

        if (file.existsAsFile()) {
            // 確認ダイアログ
            juce::AlertWindow::showAsync(juce::MessageBoxOptions()
                .withIconType(juce::MessageBoxIconType::WarningIcon)
                .withTitle("Delete Preset")
                .withMessage("Are you sure you want to delete " + file.getFileName() + "?")
                .withButton("Delete")
                .withButton("Cancel"),
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
	refreshButton.setup({ .parent = *this, .title = "Refresh Preset List" });
    refreshButton.onClick = [this] { ctx.editor.scanPresets(); };

    // --- Reflect Preset Info Button ---
	reflectButton.setup({ .parent = *this, .title = "Reflect Preset Info", .isReset = false });
    reflectButton.setTooltip("Reflect selected preset info to text editors without loading");
    reflectButton.onClick = [this] {
        int row = table.getSelectedRow();

        if (row >= 0 && row < filteredItems.size()) {
            const auto& item = filteredItems[row];

            nameEditor.setText(item.name);
            authorEditor.setText(item.author);
            versionEditor.setText(item.version);
            commentEditor.setText(item.comment);
        }
    };

    // --- Copy Preset Info to Clipboard Button ---
	copyButton.setup({ .parent = *this, .title = "Copy Preset Info to Clipboard", .isReset = false });
    copyButton.setEnabled(false);
    copyButton.onClick = [this] {
        int row = table.getSelectedRow();
        if (row >= 0 && row < filteredItems.size()) {
            const auto& item = filteredItems[row];
            juce::String info = presetCbName + item.name + "\n" +
                presetCbAuther + item.author + "\n" +
                presetCbVersion + item.version + "\n" +
                presetCbComment + item.comment + "\n" +
                presetCbMode + item.modeName;
            juce::SystemClipboard::copyTextToClipboard(info);
        }
    };
}

void GuiPreset::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();

    // Path Label (Top)
    pathLabel.setBounds(pageArea.removeFromTop(PresetFileLabelHeight));

    // Left: List
    auto listArea = pageArea.removeFromLeft(pageArea.getWidth() * PresetTableWidthRate);

    // リストのすぐ上に検索ボックスを配置する
    auto searchArea = listArea.removeFromTop(PresetMetaHeight).reduced(PresetTablePaddingWidth, 0);

    searchBox.label.setBounds(searchArea.removeFromLeft(60)); // "Search" というラベルの幅

    clearSearchButton.setBounds(searchArea.removeFromRight(30));

    searchArea.removeFromRight(5);

    searchBox.setBounds(searchArea);

    listArea.removeFromTop(5); // 検索ボックスとリストの間の少しの余白

    table.setBounds(listArea.reduced(PresetTablePaddingWidth, PresetTablePaddingHeight));

    // Right: Info & Buttons
    auto rightArea = pageArea;

    // Metadata Group
    auto metaArea = rightArea.removeFromTop(PresetMetaAreaHeight);

    metaGroup.setBounds(metaArea);

    auto mRect = metaArea.reduced(GroupPaddingWidth, GroupPaddingHeight);

    mRect.removeFromTop(TitlePaddingTop);

    // Name
    auto row1 = mRect.removeFromTop(PresetMetaHeight).reduced(PresetMetaPaddingWidth, 0);
    nameEditor.label.setBounds(row1.removeFromLeft(PresetMetaLabelWidth));
    nameEditor.setBounds(row1);

    mRect.removeFromTop(PresetMetaPaddingHeight);

    // Author
    auto row2 = mRect.removeFromTop(PresetMetaHeight).reduced(PresetMetaPaddingWidth, 0);
    authorEditor.label.setBounds(row2.removeFromLeft(PresetMetaLabelWidth));
    authorEditor.setBounds(row2);

    mRect.removeFromTop(PresetMetaPaddingHeight);

    // Version
    auto row3 = mRect.removeFromTop(PresetMetaHeight).reduced(PresetMetaPaddingWidth, 0);
    versionEditor.label.setBounds(row3.removeFromLeft(PresetMetaLabelWidth));
    versionEditor.setBounds(row3);

    mRect.removeFromTop(PresetMetaPaddingHeight);

    // Comment
    auto row4 = mRect.removeFromTop(PresetMetaHeight).reduced(PresetMetaPaddingWidth, 0);
    commentEditor.label.setBounds(row4);
    mRect.removeFromTop(PresetMetaPaddingHeight);
    auto row5 = mRect.reduced(PresetMetaPaddingWidth, 0);
    commentEditor.setBounds(row5);

    rightArea.removeFromTop(PresetButtonsPaddingTop);

    // Buttons
    initButton.setBounds(rightArea.removeFromTop(PresetButtonHeight));
    rightArea.removeFromTop(PresetButtonPaddingHeight);
    loadButton.setBounds(rightArea.removeFromTop(PresetButtonHeight));
    rightArea.removeFromTop(PresetButtonPaddingHeight);
    saveButton.setBounds(rightArea.removeFromTop(PresetButtonHeight));
    rightArea.removeFromTop(PresetButtonPaddingHeight);
    deleteButton.setBounds(rightArea.removeFromTop(PresetButtonHeight));
    rightArea.removeFromTop(PresetButtonPaddingHeight);
    refreshButton.setBounds(rightArea.removeFromTop(PresetButtonHeight));
    rightArea.removeFromTop(PresetButtonPaddingHeight);
    reflectButton.setBounds(rightArea.removeFromTop(PresetButtonHeight));
    rightArea.removeFromTop(PresetButtonPaddingHeight);
    copyButton.setBounds(rightArea.removeFromTop(PresetButtonHeight));
}

void GuiPreset::setMetaData(const juce::String& name, const juce::String& author, const juce::String& version, const juce::String& comment)
{
    nameEditor.setText(name);
    authorEditor.setText(author);
    versionEditor.setText(version);
	commentEditor.setText(comment);
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
                item.fileName.toLowerCase().contains(query) ||
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