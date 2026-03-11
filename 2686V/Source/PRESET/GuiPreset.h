#pragma once

#include <JuceHeader.h>
#include <array>

#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiPreset : public GuiBase
{
    GuiGroup mainGroup;
    GuiGroup metaGroup;

    // UI Components
    GuiLabel pathLabel; // パス表示用

    GuiTextEditor searchBox; // 検索ボックス
    GuiTextButton clearSearchButton; // 検索クリアボタン

    GuiTableList table; // メタデータ付きリスト

    GuiTextEditor nameEditor;
    GuiTextEditor authorEditor;
    GuiTextEditor versionEditor;
    GuiTextEditor commentEditor;
    GuiTextEditor genreEditor;
    GuiTextEditor filePathEditor;

    // Buttons
    GuiTextButton initButton;

    GuiTextButton saveButton;

    GuiTextButton saveAsButton;

    GuiTextButton loadButton;

    GuiTextButton deleteButton;

    GuiTextButton refreshButton;

    GuiTextButton reflectButton; // Reflect Info

    GuiTextButton copyButton;    // Copy Info to Clipboard

    // Callback for Editor
    std::function<void(const juce::File&)> onDoubleClicked;

    juce::File getSelectedFile() const;
public:
	GuiPreset(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        metaGroup(context),
        pathLabel(context),
        searchBox(context),
        clearSearchButton(context),
        table(context),
        nameEditor(context),
        authorEditor(context),
        versionEditor(context),
        commentEditor(context),
        genreEditor(context),
        filePathEditor(context),
        initButton(context),
        saveButton(context),
        saveAsButton(context),
        loadButton(context),
        deleteButton(context),
        refreshButton(context),
        reflectButton(context),
        copyButton(context)
    {
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    // Data
    juce::File currentFolder;
    std::vector<PresetItem> items; // 読み込んだプリセット一覧
    std::vector<PresetItem> filteredItems; // 検索で絞り込まれた表示用のプリセット一覧

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
    void setMetaData(const juce::String& name, const juce::String& author, const juce::String& version, const juce::String& comment, const juce::String& genre, const juce::String& filePath);    void clearTable();
	void updateTableContent();
	void repaintTable();
    void updatePresetPath();

    void applyFilter(); // 絞り込み実行関数
};
