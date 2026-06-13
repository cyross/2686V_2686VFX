// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

namespace EditorGuiText
{
	namespace Tab
	{
		static inline const juce::String opna = u8"OPNA";
		static inline const juce::String opn = u8"OPN";
		static inline const juce::String opl = u8"OPL";
		static inline const juce::String opl3 = u8"OPL3";
		static inline const juce::String opm = u8"OPM";
		static inline const juce::String opzx7 = u8"OPZX7";
		static inline const juce::String ssg = u8"SSG";
		static inline const juce::String wt = u8"WT";
		static inline const juce::String wt2 = u8"WT2";
		static inline const juce::String rhythm = u8"RHYTHM";
		static inline const juce::String adpcm = u8"ADPCM";
		static inline const juce::String beep = u8"BEEP";
		static inline const juce::String fx = u8"FX";
		static inline const juce::String preset = u8"PRESET";
		static inline const juce::String settings = u8"SETTINGS";
		static inline const juce::String about = u8"ABOUT";
		static inline const juce::String advanced = u8"ADV";
	}

	namespace Preview
	{
		static inline const juce::String show = u8">>";
		static inline const juce::String hide = u8"<<";
		static inline const juce::String label = juce::String("") + "波形プレビュー(オシロスコープ)";
		static inline const juce::String tooltipShow = juce::String("") + "波形プレビューを表示します。";
		static inline const juce::String tooltipHide = juce::String("") + "波形プレビューを隠します。";
	}

	namespace Panic
	{
		static inline const juce::String title = u8"!";
		static inline const juce::String tooltip = juce::String("") + "オーディオエンジンを初期状態に戻します。";
	}

	namespace Undo
	{
		static inline const juce::String title = u8"Un";
		static inline const juce::String tooltip = juce::String("") + "編集を履歴から戻します。";
		static inline const juce::String tooltipNone = juce::String("") + "編集履歴がありません。";
	}

	namespace Redo
	{
		static inline const juce::String title = u8"Re";
		static inline const juce::String tooltip = juce::String("") + "履歴にある編集をやり直します。";
		static inline const juce::String tooltipNone = juce::String("") + "編集履歴がありません。";
	}

	namespace Copy
	{
		static inline const juce::String title = u8"C";
		static inline const juce::String tooltip = juce::String("") + "現在のFM音源のパラメータをクリップボードにコピーします。";
	}

	namespace Paste
	{
		static inline const juce::String title = u8"P";
		static inline const juce::String tooltip = juce::String("") + "クリップボードにコピーしたFMパラメータを現在の画面に貼り付けます";
	}

	namespace Reset
	{
		static inline const juce::String title = u8"Reset";
		static inline const juce::String tooltip = juce::String("") + "現在の音源のパラメータを初期化します。";
	}

	namespace MiniPlayer
	{
		static inline const juce::String titleToMini = u8"M";
		static inline const juce::String titleToMinimum = u8"m";
		static inline const juce::String titleToFull = u8"F";
		static inline const juce::String tooltipToMini = juce::String("") + "ウインドウをミニプレイヤーに切り替えます。";
		static inline const juce::String tooltipToMinimum = juce::String("") + "ウインドウを最小にします。";
		static inline const juce::String tooltipToFull = juce::String("") + "ウインドウをフル表示に切り替えます。";
	}
}
