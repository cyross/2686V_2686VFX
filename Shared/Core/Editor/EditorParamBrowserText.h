#pragma once

#include <JuceHeader.h>

#include "../Gui/GuiI18n.h"

// ============================================================================
// パラメータファイルのブラウザーの文言と、ファイルの種類
// ============================================================================
// 種類 (kind*) は、区分の部品がブラウザーを開くときに「どれを選ばせるか」を
// 伝えるのに使う。部品は 12 本で共有するので、ここも共有する。
//
// ブラウザーそのものを持たないプラグイン (2686VFX) は、種類から OS のダイアログの
// 題と拡張子を引くのに使う。
namespace EditorGuiText
{
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

		// 行のプレビューの作り方と見せ方 (4.4.1)。全フォルダ共通
		static inline const I18n::Text customTempo{ u8"テンポ", u8"Tempo" };
		static inline const I18n::Text customFollowDaw{ u8"DAW同期", u8"DAW sync" };
		static inline const I18n::Text customFollowDawTooltip{ u8"作るときの DAW のテンポと拍子を使います。スタンドアロンでは使えません。", u8"Uses the DAW's tempo and time signature when building. Not available in the standalone app." };
		static inline const I18n::Text customBars{ u8"小節", u8"Bars" };
		static inline const I18n::Text customInterval{ u8"キーオン周期", u8"Key-on interval" };
		static inline const I18n::Text customLength{ u8"音符長", u8"Note length" };
		static inline const I18n::Text customKey{ u8"キー", u8"Key" };
		static inline const I18n::Text customCycles{ u8"生成周期", u8"Cycles" };
		static inline const I18n::Text customZoom{ u8"表示倍率", u8"Zoom" };
		static inline const I18n::Text customFps{ u8"FPS", u8"FPS" };
		static inline const I18n::Text customTooltip{ u8"行のプレビューの作り方 (テンポ〜キー) と見せ方 (生成周期〜FPS)。全フォルダで共通です。作り方を変えると、その作り方で作ったプレビューに切り替わります (無ければ未生成)。", u8"How row previews are built (Tempo to Key) and shown (Cycles to FPS). Shared by every folder. Changing how they are built switches to previews built that way (Not built if there are none)." };
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

		// 設定で決めてある置き場へ戻る
		static inline const I18n::Text homeFolder{ u8"初期フォルダ", u8"Default folder" };

		static inline const I18n::Text search{ u8"検索", u8"Search" };

		// 一覧のプレビューをまとめて止める・動かす
		static inline const I18n::Text pauseAll{ u8"すべて停止", u8"Pause all" };
		static inline const I18n::Text playAll{ u8"すべて再生", u8"Play all" };

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
		static inline const juce::String kindSsgSwEnv256 = "SSG SW ENV256";
		static inline const juce::String kindDutyEnv = "DUTY RATIO ENV";
		static inline const juce::String kindPeakEnv = "PEAK RATIO ENV";
		static inline const juce::String kindAlgEnv = "ALG ENV";
		static inline const juce::String kindParamEnv = "PARAM ENV";
		static inline const juce::String kindTapParam = "TAP PARAM SEQUENCER";
		static inline const juce::String kindTapAmp = "TAP AMP SEQUENCER";
		static inline const juce::String kindTapPitch = "TAP PITCH SEQUENCER";
		static inline const juce::String kindSsgSwPEnv256 = "SSG SW PENV256";
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
}
