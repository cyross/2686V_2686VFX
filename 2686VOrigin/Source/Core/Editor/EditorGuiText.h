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
		static inline const juce::String label = juce::String("") + "波形プレビュー(オシロスコープ)";
		static inline const juce::String labelL = juce::String("") + "[L]";
		static inline const juce::String labelMono = juce::String("") + "[M]";
		static inline const juce::String labelR = juce::String("") + "[R]";
		static inline const juce::String tooltipShow = juce::String("") + "波形プレビューを表示します。";
		static inline const juce::String tooltipHide = juce::String("") + "波形プレビューを隠します。";
	}

	namespace PresetScan
	{
		static inline const juce::String working = juce::String("") + "プリセットの見出しを読んでいます";
	}

	namespace Loading
	{
		static inline const juce::String cancel = juce::String("") + "中止";
		static inline const juce::String cancelling = juce::String("") + "中止しています…";
	}

	namespace ParamBrowser
	{
		static inline const juce::String keywordHint = juce::String("") + "名前で絞り込む";
		static inline const juce::String filterAll = juce::String("") + "すべて";
		static inline const juce::String formatJson = "JSON";
		static inline const juce::String formatYaml = "YAML";
		static inline const juce::String formatPlain = juce::String("") + "無印";

		static inline const juce::String columnName = juce::String("") + "ファイル名";
		static inline const juce::String columnCategory = juce::String("") + "区分";
		static inline const juce::String columnFormat = juce::String("") + "形式";
		static inline const juce::String columnPreview = juce::String("") + "波形";
		static inline const juce::String columnAction = juce::String("") + "生成";

		static inline const juce::String sortUp = juce::String::fromUTF8("▲");
		static inline const juce::String sortDown = juce::String::fromUTF8("▼");

		static inline const juce::String folder = juce::String("") + "フォルダ…";
		static inline const juce::String bulkGenerate = juce::String("") + "このフォルダのプレビューを作る";
		static inline const juce::String bulkDelete = juce::String("") + "プレビューを消す";
		static inline const juce::String working = juce::String("") + "波形を作っています";
		static inline const juce::String noPreview = juce::String("") + "未生成";
		static inline const juce::String empty = juce::String("") + "見つかりませんでした";
		static inline const juce::String folderTitle = juce::String("") + "パラメータファイルのフォルダを選ぶ";

		static inline const juce::String parentFolder = juce::String("") + ".. (上のフォルダ)";
		static inline const juce::String folderMark = juce::String("") + "▸ ";

		static inline const juce::String newFolder = juce::String("") + "新規フォルダ";
		static inline const juce::String newFolderTitle = juce::String("") + "新しいフォルダ";
		static inline const juce::String newFolderPrompt = juce::String("") + "名前";
		static inline const juce::String create = juce::String("") + "作成";
		static inline const juce::String cancel = juce::String("") + "取り消し";

		static inline const juce::String deleteFolder = juce::String("") + "フォルダ削除";
		static inline const juce::String deleteFolderTitle = juce::String("") + "フォルダをごみ箱へ入れますか";
		static inline const juce::String deleteFolderCount = juce::String("") + "中に %d 件あります。ごみ箱からなら戻せます。";
		static inline const juce::String moveToTrash = juce::String("") + "ごみ箱へ入れる";

		static inline const juce::String saveName = juce::String("") + "名前";
		static inline const juce::String save = juce::String("") + "保存";
		static inline const juce::String overwriteTitle = juce::String("") + "同じ名前のファイルがあります";
		static inline const juce::String overwrite = juce::String("") + "上書きする";

		static inline const juce::String waveWt = juce::String("") + "波形(WT)";
		static inline const juce::String waveWt2 = juce::String("") + "波形(WT2)";
		static inline const juce::String audioFile = juce::String("") + "音声";

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
		static inline const juce::String title = juce::String("") + "生成波形";
		static inline const juce::String generate = juce::String("") + "生成";
		static inline const juce::String regenerate = juce::String("") + "再生成";
		static inline const juce::String remove = juce::String("") + "削除";
		static inline const juce::String cycles = juce::String("") + "周期";

		// 3 段の見出し。リアルタイムのオシロと同じ並び。
		static inline const juce::String channelL = "L";
		static inline const juce::String channelM = "M";
		static inline const juce::String channelR = "R";
		static inline const juce::String working = juce::String("") + "波形を作っています…";
		static inline const juce::String empty = juce::String("") + "「生成」で作ります";
		static inline const juce::String generateTooltip = juce::String("") + "今の設定で 10 秒ぶんの波形を作ります。値を変えたら押し直してください。";
		static inline const juce::String removeTooltip = juce::String("") + "作った波形を捨てます。";
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

	// 区分の一括開閉。簡易表示モードで隠す区分が対象。
	namespace CategoryToggle
	{
		static inline const juce::String titleOpen = u8"OP";
		static inline const juce::String titleClose = u8"CL";
		static inline const juce::String tooltipOpen = juce::String("") + "バイパスしていない区分をまとめて開きます。";
		static inline const juce::String tooltipClose = juce::String("") + "バイパスしている区分をまとめて閉じます。";
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
