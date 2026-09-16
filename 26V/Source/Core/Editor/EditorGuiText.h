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
		static inline const juce::String adpcm = u8"ADPCM";
		static inline const juce::String beep = u8"BEEP";
		static inline const juce::String wtPlus = u8"WT+";
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

	namespace ParamBrowser
	{
		static inline const I18n::Text keywordHint{ u8"名前で絞り込む", u8"Filter by name" };
		static inline const I18n::Text filterAll{ u8"すべて", u8"All" };
		static inline const juce::String formatJson = "JSON";
		static inline const juce::String formatYaml = "YAML";
		static inline const I18n::Text formatPlain{ u8"無印", u8"Plain" };

		static inline const I18n::Text columnName{ u8"ファイル名", u8"File name" };
		static inline const I18n::Text columnCategory{ u8"区分", u8"Kind" };
		static inline const I18n::Text columnFormat{ u8"形式", u8"Format" };
		static inline const I18n::Text columnPreview{ u8"波形", u8"Waveform" };
		static inline const I18n::Text columnAction{ u8"生成", u8"Generate" };

		static inline const juce::String sortUp = juce::String::fromUTF8("▲");
		static inline const juce::String sortDown = juce::String::fromUTF8("▼");

		static inline const I18n::Text folder{ u8"フォルダ…", u8"Folder…" };
		static inline const I18n::Text bulkGenerate{ u8"このフォルダのプレビューを作る", u8"Generate all in this folder" };
		static inline const I18n::Text bulkDelete{ u8"プレビューを消す", u8"Delete all previews" };
		static inline const I18n::Text working{ u8"波形を作っています", u8"Building the waveform" };
		static inline const I18n::Text noPreview{ u8"未生成", u8"Not built" };
		static inline const I18n::Text empty{ u8"見つかりませんでした", u8"Nothing found" };
		static inline const I18n::Text folderTitle{ u8"パラメータファイルのフォルダを選ぶ", u8"Choose a folder for parameter files" };

		static inline const I18n::Text parentFolder{ u8".. (上のフォルダ)", u8".. (parent folder)" };
		static inline const juce::String folderMark = juce::String("") + "▸ ";

		static inline const I18n::Text newFolder{ u8"新規フォルダ", u8"New folder" };
		static inline const I18n::Text newFolderTitle{ u8"新しいフォルダ", u8"New folder" };
		static inline const I18n::Text newFolderPrompt{ u8"名前", u8"Name" };
		static inline const I18n::Text create{ u8"作成", u8"Create" };
		static inline const I18n::Text cancel{ u8"取り消し", u8"Cancel" };

		static inline const I18n::Text deleteFolder{ u8"フォルダ削除", u8"Delete folder" };
		static inline const I18n::Text deleteFolderTitle{ u8"フォルダをごみ箱へ入れますか", u8"Move this folder to the recycle bin?" };
		static inline const I18n::Text deleteFolderCount{ u8"中に %d 件あります。ごみ箱からなら戻せます。", u8"It holds %d item(s). You can put them back from the recycle bin." };
		static inline const I18n::Text moveToTrash{ u8"ごみ箱へ入れる", u8"Move to recycle bin" };

		static inline const I18n::Text saveName{ u8"名前", u8"Name" };
		static inline const I18n::Text save{ u8"保存", u8"Save" };
		static inline const I18n::Text overwriteTitle{ u8"同じ名前のファイルがあります", u8"A file with that name already exists" };
		static inline const I18n::Text overwrite{ u8"上書きする", u8"Overwrite" };

		static inline const I18n::Text waveWt{ u8"波形(WT)", u8"Wave (WT)" };
		static inline const I18n::Text waveWt2{ u8"波形(WT2)", u8"Wave (WT2)" };
		static inline const I18n::Text audioFile{ u8"音声", u8"Audio" };

		// 区分の名前。ブラウザの表と、呼び出し側の指定で同じものを使う。
		static inline const juce::String kindOpnaOp = "OPNA OP";
		static inline const juce::String kindOpnOp = "OPN OP";
		static inline const juce::String kindOplOp = "OPL OP";
		static inline const juce::String kindOpl3Op = "OPL3 OP";
		static inline const juce::String kindOpmOp = "OPM OP";
		static inline const juce::String kindOpzx7Op = "OPZX7 OP";
		static inline const juce::String kindRhythmPad = "RHYTHM PAD";
		static inline const juce::String kindHwLfo = "HW LFO";
		static inline const juce::String kindLfoN88 = "LFO(N88)";
		static inline const juce::String kindLfoOpm = "LFO(OPM)";
		static inline const juce::String kindLfoOpl = "LFO(OPL)";
		static inline const juce::String kindLfoOpzx7 = "LFO(OPZX7)";
		static inline const juce::String kindAmpEnv = "AMP ENV";
		static inline const juce::String kindSsgHwEnv = "SSG HW ENV";
		static inline const juce::String kindSsgHwPEnv = "SSG HW PENV";
		static inline const juce::String kindSsgSwEnv = "SSG SW ENV";
		static inline const juce::String kindSsgSwEnv11 = "SSG SW ENV11";
		static inline const juce::String kindSsgSwPEnv11 = "SSG SW PENV11";
		static inline const juce::String kindPitchEnv = "PITCH ENV";
		static inline const juce::String kindDetune = "MUL/DET";
		static inline const juce::String kindUnison = "UNISON";
		static inline const juce::String kindQuality = "QUALITY";
		static inline const juce::String kindPcmQuality = "PCM QUALITY";
		static inline const juce::String kindPcmPlay = "PCM PLAY";
		static inline const juce::String kindToneNoise = "TONE/NOISE";
		static inline const juce::String kindWtMod = "WT MOD";
		static inline const juce::String kindWtAmpMod = "WT AMP MOD";
		static inline const juce::String kindCurve = "CURVE";
		static inline const juce::String kindColors = "COLORS";
		static inline const juce::String kindFxOrder = "FX ORDER";
		static inline const juce::String kindFxParam = "FX PARAM";
	}

	namespace GenWave
	{
		static inline const I18n::Text title{ u8"生成波形", u8"Generated waveform" };
		static inline const I18n::Text generate{ u8"生成", u8"Generate" };
		static inline const I18n::Text regenerate{ u8"再生成", u8"Regenerate" };
		static inline const I18n::Text remove{ u8"削除", u8"Delete" };
		static inline const I18n::Text cycles{ u8"周期", u8"Cycles" };

		// 3 段の見出し。リアルタイムのオシロと同じ並び。
		static inline const juce::String channelL = "L";
		static inline const juce::String channelM = "M";
		static inline const juce::String channelR = "R";
		static inline const I18n::Text working{ u8"波形を作っています…", u8"Building the waveform…" };
		static inline const I18n::Text empty{ u8"「生成」で作ります", u8"Press Generate to build it" };
		static inline const I18n::Text generateTooltip{ u8"今の設定で 10 秒ぶんの波形を作ります。値を変えたら押し直してください。", u8"Builds 10 seconds of waveform from the current settings. Press it again after changing a value." };
		static inline const I18n::Text removeTooltip{ u8"作った波形を捨てます。", u8"Throws away the waveform that was built." };
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
