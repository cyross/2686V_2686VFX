#pragma once

#include <JuceHeader.h>

// 共通部分
#if defined(BUILD_AS_FX_PLUGIN)
static inline const juce::String pluginName = "2686VFX";
#else
static inline const juce::String pluginName = "2686V";
#endif
static inline const juce::String pluginVersion = "Version 0.1.0";
static inline const juce::String pluginAuthor = "Copyright (C) 2026 CYROSS";

static inline const juce::String vstNotice = "VST is a registered trademark of Steinberg Media Technologies GmbH.";
static inline const juce::String gplv3Notice = "This software is released under the GNU General Public License v3.0 (GPLv3).";
static inline const juce::String gplv3Navigate = "View Full License (GNU.org)";
static inline const juce::String gplLinkUrl = "https://www.gnu.org/licenses/gpl-3.0.en.html";

static inline const juce::String envCode = "PREF_2686V";

// オーディオ部分
static inline const juce::String audioInputName = "Input";
static inline const juce::String audioOutputName = "Output";

// プラグイン固有部分
static inline const juce::String pluginParametersName = "Parameters";

// 設定部分
static inline const juce::String settingWallpaperPath = "wallpaperPath";
static inline const juce::String settingDefaultSampleDir = "defaultSampleDir";
static inline const juce::String settingDefaultPresetDir = "defaultPresetDir";
static inline const juce::String settingShowTooltips = "showTooltips";
static inline const juce::String settingUseHeadroom = "useHeadRoom";
static inline const juce::String settingHeadroomGain = "headRoomGain";
static inline constexpr bool defaultShowTooltip = true;
static inline constexpr bool defaultUseHeadroom = true;
static inline constexpr float defaultHeadroomGain = 0.25f;
static inline const juce::String allowedControlChars = "\r\n\t";

// プリセット部分
static inline const juce::String presetCbName = "Name: ";
static inline const juce::String presetCbAuther = "Author: ";
static inline const juce::String presetCbVersion = "Ver: ";
static inline const juce::String presetCbComment = "Comment: ";
static inline const juce::String presetCbMode = "Mode: ";
