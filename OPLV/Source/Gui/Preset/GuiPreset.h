#pragma once

#include <JuceHeader.h>
#include <array>

#include "../../Core/Synth/SynthMode.h"

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"

class GuiPreset : public GuiBase
{
    juce::Font buttonFont = juce::Font(juce::FontOptions(16.0f));

    GuiGroup mainGroup;
    GuiGroup metaGroup;

    // UI Components
    GuiLabel pathLabel; // パス表示用

    GuiComboBox viewSelector; // すべて / お気に入り / 履歴
    GuiComboBox formatSelector; // すべて / XML / JSON / YAML
    GuiTextButton clearHistoryButton;

    GuiTextEditor searchBox; // 検索ボックス
    GuiTextButton clearSearchButton; // 検索クリアボタン

    // チャンネルでの絞り込み。
    //
    // チャンネルの数だけチェックを並べると、13 個が横に伸びて検索欄を
    // 押し出してしまう。対象を選ぶコンボと、その入り切りのチェックの
    // 2 つに畳み、いま入っているものは右の文字で見せる。
    GuiComboBox channelSelector;
    GuiToggleButton channelCheck;
    GuiTextButton channelAllOnButton;
    GuiTextButton channelAllOffButton;
    GuiLabel channelSummary;

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

    // 覚えている並べ替えを、今の一覧へ掛け直す
    void sortFiltered();
public:
	GuiPreset(const GuiContext& context) :
        GuiBase(context),
        mainGroup(context),
        metaGroup(context),
        pathLabel(context),
        viewSelector(context),
        formatSelector(context),
        clearHistoryButton(context),
        searchBox(context),
        clearSearchButton(context),
        channelSelector(context),
        channelCheck(context),
        channelAllOnButton(context),
        channelAllOffButton(context),
        channelSummary(context),
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
    // 一覧の見方。お気に入りと履歴は今見ているフォルダの外も指すので、
    // 元になる並びごと切り替える。
    enum class View { all = 0, favourites, history };

    View view = View::all;

    // 一覧に出すファイル形式。読み込みはどの形式でもできるので、これは
    // 探しやすさのための絞り込み。
    enum class Format { all = 0, xml, json, yaml };

    Format formatFilter = Format::all;

    // チャンネルごとの絞り込み。並びは OscMode。
    //
    // ひとつも入っていないときは絞らない。「すべて入っている」と
    // 「ひとつも入っていない」は結果が同じなので、初期値は分かりやすい
    // ほうを選んでいる (下の setup を参照)。
    std::array<bool, (size_t)OscMode::Count> channelFilter{};

    // 表の並べ替え。
    //
    // 絞り込みは並びをまるごと作り直すので、そのままだと押すたびに
    // 並べ替えが消えてしまう。プリセットを読み込んだときも作り直しが
    // 走るので、何で並べていたかを覚えておいて掛け直す。
    // 0 は「並べ替えなし」。
    int sortColumnId = 0;
    bool sortForwards = true;

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

    // チェックと、いま対象にしているものの文字を今の値へ合わせる
    void refreshChannelRow();
};
