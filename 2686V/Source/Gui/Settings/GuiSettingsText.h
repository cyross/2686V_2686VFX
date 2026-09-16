// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

#include "../../Core/Gui/GuiI18n.h"

namespace SettingsGuiText
{
	namespace Group
	{
		static inline const I18n::Text settingEnv{ u8"環境設定", u8"Preferences" };
	}

	// 画面に出す文字列の言語
	static inline const I18n::Text language{ u8"言語", u8"Language" };


	// 壁紙の並べ方。頭の英語は選択肢の見分けなので、どちらの言語でも同じ。
	static inline const I18n::Text wpStretch{ u8"Stretch(画面アスペクト比維持・画面全体カバー)", u8"Stretch (keeps the screen ratio, covers it all)" };
	static inline const I18n::Text wpFill{ u8"Fill(画像アスペクト比維持・画面全体カバー)", u8"Fill (keeps the image ratio, covers it all)" };
	static inline const I18n::Text wpFit{ u8"Fit(画像アスペクト比準拠・見切りあり)", u8"Fit (keeps the image ratio, may crop)" };
	static inline const I18n::Text wpOriginal{ u8"Original(入力画像そのまま・センタリング)", u8"Original (as loaded, centred)" };

	static inline const I18n::Text uiScale{ u8"UIスケール", u8"UI scale" };
	static inline const I18n::Text fileFormat{ u8"ファイル形式", u8"File format" };
	static inline const I18n::Text chooseFile{ u8"ファイル選択", u8"Choose a file" };
	static inline const I18n::Text chooseFolder{ u8"フォルダ選択", u8"Choose a folder" };
	static inline const I18n::Text wallpaper{ u8"壁紙:", u8"Wallpaper:" };
	static inline const I18n::Text wallpaperChoose{ u8"壁紙画像ファイルを選択してください", u8"Choose a wallpaper image" };
	static inline const I18n::Text wallpaperClear{ u8"解除", u8"Clear" };
	static inline const I18n::Text wallpaperMode{ u8"壁紙表示スケール:", u8"Wallpaper fit:" };
	static inline const I18n::Text dirCat{ u8"フォルダ設定(開閉)", u8"Folders (open/close)" };
	static inline const I18n::Text simpleView{ u8"簡易表示モード", u8"Simple view" };
	static inline const I18n::Text bypassHidden{ u8"非表示中の区分をバイパス", u8"Bypass hidden sections" };
	static inline const I18n::Text simpleViewCat{ u8"簡易表示モードカスタマイズ(開閉)", u8"Simple view customisation (open/close)" };
	static inline const I18n::Text toggleAlign{ u8"トグルボタン配置", u8"Toggle alignment" };
	static inline const I18n::Text toggleAlignCentred{ u8"中央寄せ", u8"Centred" };
	static inline const I18n::Text toggleAlignLeft{ u8"左寄せ", u8"Left" };
	static inline const I18n::Text showTooltips{ u8"ツールチップを表示", u8"Show tooltips" };
	static inline const I18n::Text useHeadroom{ u8"ヘッドルームを確保", u8"Keep headroom" };
	static inline const I18n::Text headroomGain{ u8"ヘッドルームゲイン", u8"Headroom gain" };
	static inline const I18n::Text showVirtualKeyboard{ u8"仮想MIDIキーボード表示", u8"Show the virtual MIDI keyboard" };
	static inline const I18n::Text saveSettings{ u8"設定ファイルに保存", u8"Save to a settings file" };
	static inline const I18n::Text loadSettings{ u8"設定ファイルから読み込み", u8"Load from a settings file" };
	static inline const I18n::Text chooseSettingsFile{ u8"設定ファイルを選択してください", u8"Choose a settings file" };
	static inline const I18n::Text saveStartup{ u8"標準設定として保存", u8"Save as the default" };
	static inline const I18n::Text clearUndoHistory{ u8"アンドゥ・リドゥ履歴の初期化", u8"Clear the undo / redo history" };

	// 簡易表示モードで隠さない区分の札。区分の名前を差し込む。
	static inline const I18n::Text showItem{ u8"%s を表示", u8"Show %s" };

	// 標準設定として保存したあとに出すダイアログ
	static inline const I18n::Text saveStartupOkTitle{ u8"成功", u8"Saved" };
	static inline const I18n::Text saveStartupOkBody{ u8"現在の設定を標準設定として保存しました。", u8"The current settings were saved as the default." };
	static inline const I18n::Text saveStartupNgTitle{ u8"失敗", u8"Not saved" };
	static inline const I18n::Text saveStartupNgBody{ u8"標準設定を保存できませんでした。", u8"The default settings could not be saved." };
	static inline const I18n::Text dialogPlace{ u8"場所: ", u8"Folder: " };
	static inline const I18n::Text dialogFileName{ u8"ファイル名: ", u8"File name: " };

	// 置き場の行。ラベルと、選ぶときのダイアログの見出し。
	namespace Dir
	{
		static inline const I18n::Text sample{ u8"サンプルファイルディレクトリ:", u8"Sample folder:" };
		static inline const I18n::Text sampleChoose{ u8"サンプルファイルディレクトリを選択してください", u8"Choose the sample folder" };
		static inline const I18n::Text preset{ u8"プリセットファイルディレクトリ:", u8"Preset folder:" };
		static inline const I18n::Text presetChoose{ u8"プリセットファイルディレクトリを選択してください", u8"Choose the preset folder" };
		static inline const I18n::Text wavetable{ u8"波形メモリファイルディレクトリ:", u8"Wave memory folder:" };
		static inline const I18n::Text wavetableChoose{ u8"波形メモリファイルディレクトリを選択してください", u8"Choose the wave memory folder" };
		static inline const I18n::Text fxOrder{ u8"FX順番ファイルディレクトリ:", u8"FX order folder:" };
		static inline const I18n::Text fxOrderChoose{ u8"エフェクトオーダーファイルディレクトリを選択してください", u8"Choose the FX order folder" };
		static inline const I18n::Text fxParam{ u8"FXファイルディレクトリ:", u8"FX folder:" };
		static inline const I18n::Text fxParamChoose{ u8"FXファイルディレクトリを選択してください", u8"Choose the FX folder" };
		static inline const I18n::Text channelParam{ u8"CHパラメータファイルディレクトリ:", u8"CH parameter folder:" };
		static inline const I18n::Text channelParamChoose{ u8"CHパラメータファイルディレクトリを選択してください", u8"Choose the CH parameter folder" };
		static inline const I18n::Text curveParam{ u8"カーブ編集パラメータファイルディレクトリ:", u8"Curve parameter folder:" };
		static inline const I18n::Text curveParamChoose{ u8"カーブ編集パラメータファイルディレクトリを選択してください", u8"Choose the curve parameter folder" };
		static inline const I18n::Text lfoParam{ u8"LFOファイルディレクトリ:", u8"LFO folder:" };
		static inline const I18n::Text lfoParamChoose{ u8"LFOファイルディレクトリを選択してください", u8"Choose the LFO folder" };
		static inline const I18n::Text ampEnvParam{ u8"AMP ENVファイルディレクトリ:", u8"AMP ENV folder:" };
		static inline const I18n::Text ampEnvParamChoose{ u8"AMP ENVファイルディレクトリを選択してください", u8"Choose the AMP ENV folder" };
		static inline const I18n::Text pitchEnvParam{ u8"PITCH ENVファイルディレクトリ:", u8"PITCH ENV folder:" };
		static inline const I18n::Text pitchEnvParamChoose{ u8"PITCH ENVファイルディレクトリを選択してください", u8"Choose the PITCH ENV folder" };
		static inline const I18n::Text ssgSwEnvParam{ u8"SSG SW ENVファイルディレクトリ:", u8"SSG SW ENV folder:" };
		static inline const I18n::Text ssgSwEnvParamChoose{ u8"SSG SW ENVファイルディレクトリを選択してください", u8"Choose the SSG SW ENV folder" };
		static inline const I18n::Text ssgHwEnvParam{ u8"SSG HW ENVファイルディレクトリ:", u8"SSG HW ENV folder:" };
		static inline const I18n::Text ssgHwEnvParamChoose{ u8"SSG HW ENVファイルディレクトリを選択してください", u8"Choose the SSG HW ENV folder" };
		static inline const I18n::Text detuneParam{ u8"DETUNE ファイルディレクトリ:", u8"DETUNE folder:" };
		static inline const I18n::Text detuneParamChoose{ u8"DETUNE ファイルディレクトリを選択してください", u8"Choose the DETUNE folder" };
		static inline const I18n::Text unisonParam{ u8"UNISON ファイルディレクトリ:", u8"UNISON folder:" };
		static inline const I18n::Text unisonParamChoose{ u8"UNISON ファイルディレクトリを選択してください", u8"Choose the UNISON folder" };
		static inline const I18n::Text qualityParam{ u8"音質ファイルディレクトリ:", u8"Quality folder:" };
		static inline const I18n::Text qualityParamChoose{ u8"音質ファイルディレクトリを選択してください", u8"Choose the quality folder" };
		static inline const I18n::Text pcmPlayParam{ u8"PCM再生ファイルディレクトリ:", u8"PCM playback folder:" };
		static inline const I18n::Text pcmPlayParamChoose{ u8"PCM再生ファイルディレクトリを選択してください", u8"Choose the PCM playback folder" };
		static inline const I18n::Text colorSetting{ u8"色の設定ファイルディレクトリ:", u8"Colour settings folder:" };
		static inline const I18n::Text colorSettingChoose{ u8"色の設定ファイルディレクトリを選択してください", u8"Choose the colour settings folder" };
		static inline const I18n::Text toneNoiseParam{ u8"トーン/ノイズファイルディレクトリ:", u8"Tone/noise folder:" };
		static inline const I18n::Text toneNoiseParamChoose{ u8"トーン/ノイズファイルディレクトリを選択してください", u8"Choose the tone/noise folder" };
		static inline const I18n::Text wtModParam{ u8"WT MODファイルディレクトリ:", u8"WT MOD folder:" };
		static inline const I18n::Text wtModParamChoose{ u8"WT MODファイルディレクトリを選択してください", u8"Choose the WT MOD folder" };
	}

	// 作り置きした波形プレビューの全削除
	static inline const I18n::Text clearWavePreviews{ u8"プレビュー波形の全削除", u8"Clear all previews" };
	static inline const I18n::Text clearWavePreviewsTitle{ u8"プレビュー波形をすべて消しますか", u8"Clear every preview waveform?" };
	static inline const I18n::Text clearWavePreviewsCount{ u8"%d 件あります。消しても、各画面の生成ボタンで作り直せます。", u8"There are %d. You can build them again with the generate button on each screen." };
	static inline const I18n::Text clearWavePreviewsOk{ u8"すべて消す", u8"Clear all" };
	static inline const I18n::Text clearWavePreviewsCancel{ u8"取り消し", u8"Cancel" };
}
