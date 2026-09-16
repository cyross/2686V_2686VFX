// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

#include "../../Core/Gui/GuiI18n.h"

namespace EditorGuiText
{
	namespace Tab
	{
		static inline const juce::String opna = u8"OPNA";
		static inline const juce::String opn = u8"OPN";
		static inline const juce::String opl = u8"OPL";
		static inline const juce::String opl3 = u8"OPL3";
		static inline const juce::String opm = u8"OPM";
		static inline const juce::String opzx7 = u8"OPZX7S";
		static inline const juce::String ssg = u8"SSG";
		static inline const juce::String wt = u8"WT";
		static inline const juce::String wt2 = u8"WT2";
		static inline const juce::String rhythm = u8"RHYTHM";
		static inline const juce::String adpcm = u8"PCM";
		static inline const juce::String beep = u8"BEEP";
		static inline const juce::String fx = u8"FX";
		static inline const juce::String preset = u8"PRESET";
		static inline const juce::String settings = u8"SETTINGS";
		static inline const juce::String about = u8"ABOUT";
		static inline const juce::String colors = u8"COLORS";
		static inline const juce::String advanced = u8"ADV";
	}

	namespace Preview
	{
		static inline const juce::String show = u8">>";
		static inline const juce::String hide = u8"<<";
		static inline const I18n::Text label{ u8"波形プレビュー(オシロスコープ)", u8"Waveform preview" };
		static inline const juce::String labelL = juce::String("") + "[L]";
		static inline const juce::String labelMono = juce::String("") + "[M]";
		static inline const juce::String labelR = juce::String("") + "[R]";
		static inline const I18n::Text tooltipShow{ u8"波形プレビューを表示します。", u8"Shows the waveform preview." };
		static inline const I18n::Text tooltipHide{ u8"波形プレビューを隠します。", u8"Hides the waveform preview." };
	}

	namespace PresetScan
	{
		static inline const I18n::Text working{ u8"プリセットの見出しを読んでいます", u8"Reading preset headings" };
	}

	namespace Loading
	{
		static inline const I18n::Text cancel{ u8"中止", u8"Cancel" };
		static inline const I18n::Text cancelling{ u8"中止しています…", u8"Cancelling…" };
	}

	namespace Panic
	{
		static inline const juce::String title = u8"!";
		static inline const I18n::Text tooltip{ u8"オーディオエンジンを初期状態に戻します。", u8"Returns the audio engine to its initial state." };
	}

	namespace Undo
	{
		static inline const juce::String title = u8"Un";
		static inline const I18n::Text tooltip{ u8"編集を履歴から戻します。", u8"Steps back through the edit history." };
		static inline const I18n::Text tooltipNone{ u8"編集履歴がありません。", u8"There is no edit history." };
	}

	namespace Redo
	{
		static inline const juce::String title = u8"Re";
		static inline const I18n::Text tooltip{ u8"履歴にある編集をやり直します。", u8"Steps forward through the edit history." };
		static inline const I18n::Text tooltipNone{ u8"編集履歴がありません。", u8"There is no edit history." };
	}

	namespace Copy
	{
		static inline const juce::String title = u8"C";
		static inline const I18n::Text tooltip{ u8"現在のFM音源のパラメータをクリップボードにコピーします。", u8"Copies the current FM parameters to the clipboard." };
	}

	namespace Paste
	{
		static inline const juce::String title = u8"P";
		static inline const I18n::Text tooltip{ u8"クリップボードにコピーしたFMパラメータを現在の画面に貼り付けます", u8"Pastes the FM parameters on the clipboard into this tab" };
	}

	namespace Reset
	{
		static inline const juce::String title = u8"Reset";
		static inline const I18n::Text tooltip{ u8"現在の音源のパラメータを初期化します。", u8"Resets this chip's parameters to their defaults." };
	}

	// 区分の一括開閉。簡易表示モードで隠す区分が対象。
	namespace CategoryToggle
	{
		static inline const juce::String titleOpen = u8"OP";
		static inline const juce::String titleClose = u8"CL";
		static inline const I18n::Text tooltipOpen{ u8"バイパスしていない区分をまとめて開きます。", u8"Opens every section that is not bypassed." };
		static inline const I18n::Text tooltipClose{ u8"バイパスしている区分をまとめて閉じます。", u8"Closes every section that is bypassed." };
	}

	namespace MiniPlayer
	{
		static inline const juce::String titleToMini = u8"M";
		static inline const juce::String titleToMinimum = u8"m";
		static inline const juce::String titleToFull = u8"F";
		static inline const I18n::Text tooltipToMini{ u8"ウインドウをミニプレイヤーに切り替えます。", u8"Switches the window to the mini player." };
		static inline const I18n::Text tooltipToMinimum{ u8"ウインドウを最小にします。", u8"Switches the window to the minimum view." };
		static inline const I18n::Text tooltipToFull{ u8"ウインドウをフル表示に切り替えます。", u8"Switches the window back to the full view." };
	}
}
