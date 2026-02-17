#pragma once

#include <JuceHeader.h>

// プリセットテーブル・カラム名
static inline const juce::String presetTableFileNameColTitle = "File Name";
static inline const juce::String presetTableModeColTitle = "Mode";
static inline const juce::String presetTablePresetNameColTitle = "Preset Name";
static inline const juce::String presetTableAuthorColTitle = "Author";
static inline const juce::String presetTableVersionColTitle = "Ver";

// プリセット名
static inline const juce::String settingPresetName = "presetName";
static inline const juce::String settingPresetAuthor = "presetAuthor";
static inline const juce::String settingPresetVersion = "presetVersion";
static inline const juce::String settingPresetComment = "presetComment";
static inline const juce::String settingActiveModeName = "activeModeName";
static inline const juce::String settingPluginVersion = "pluginVersion";
static inline const juce::String settingAdpcmPath = "adpcmPath";
static inline const juce::String settingRhythmPathPrefix = "rhythmPath";

// 初期メタデータ
static inline const juce::String defaultPresetName = "Init Preset";
static inline const juce::String defaultPresetAuthor = "Anonymous User";
static inline const juce::String defaultPresetVersion = "1.0.0";
static inline const juce::String defaultPresetComment = "";

// メタデータ範囲
static inline const int presetNameLength = 512;
static inline const int presetAuthorLength = 128;
static inline const int presetVersionLength = 64;
static inline const int commentLength = 2048; // コメント欄の長さ(2048文字まで)

// 操作ボタン
static inline const juce::String initPresetBtnLabel = "Init Preset";

// ファイル関係
static inline const juce::String globFiles = "*.xml";
static inline const juce::String invalidXmlNotice = "Invalid XML";
static inline const juce::String openAudioFileDialogTitle = "Select an Audio file";
static inline const juce::String emptyFileName = "(Empty)";
