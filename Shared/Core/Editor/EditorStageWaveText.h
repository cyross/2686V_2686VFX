#pragma once

#include <JuceHeader.h>

#include "../Gui/GuiI18n.h"

// ============================================================================
// 生成中の波形の窓 (4.4.0) の文言
// ============================================================================
namespace EditorGuiText
{
	namespace StageWave
	{
		static inline const I18n::Text windowTitle{ u8"生成過程プレビュー", u8"Build process preview" };

		// エディタに置く、窓を開くボタン
		static inline const I18n::Text open{ u8"生成過程をプレビュー", u8"Preview build process" };
		static inline const I18n::Text close{ u8"生成過程のプレビューを閉じる", u8"Close build process preview" };
		static inline const I18n::Text openTooltip{ u8"音源の中で波形がどう作られていくかを、段ごとに別の窓で見せます。Ctrl+M で開いて作ります。", u8"Shows, in a separate window, how the waveform is built inside the sound source, stage by stage. Ctrl+M opens it and builds." };

		static inline const I18n::Text generate{ u8"生成", u8"Generate" };
		static inline const I18n::Text regenerate{ u8"再生成", u8"Regenerate" };
		static inline const I18n::Text remove{ u8"削除", u8"Delete" };
		static inline const I18n::Text generateTooltip{ u8"今の設定と打鍵の並びで波形を作ります。音は出しません。値を変えたら押し直してください (SETTINGS の自動生成が入っていれば、触り終えてしばらくすると作り直します)。", u8"Builds the waveform from the current settings and the note pattern. No sound is played. Press it again after changing a value (with auto-generate on in SETTINGS, it rebuilds a moment after you stop editing)." };
		static inline const I18n::Text working{ u8"波形を作っています…", u8"Building the waveform…" };
		static inline const I18n::Text empty{ u8"「生成」で作ります", u8"Press Generate to build it" };
		static inline const I18n::Text truncated{ u8"30 秒で打ち切りました", u8"Cut off at 30 seconds" };
		static inline const I18n::Text noSynth{ u8"このプラグインには音源がありません", u8"This plugin has no sound source" };

		static inline const I18n::Text key{ u8"キー", u8"Key" };
		static inline const I18n::Text cycles{ u8"表示周期", u8"View cycles" };
		static inline const I18n::Text fps{ u8"FPS", u8"FPS" };
		static inline const I18n::Text fpsTooltip{ u8"再生の 1 秒あたりのコマ数。上げるほどコマが細かくなり、コマの数 (長さ × FPS) も増えます。", u8"Frames per second for playback. Higher values give finer frames and more of them (length × FPS)." };
		static inline const I18n::Text tempo{ u8"テンポ", u8"Tempo" };
		static inline const I18n::Text followDaw{ u8"DAW 追従", u8"Follow DAW" };
		static inline const I18n::Text followDawTooltip{ u8"「生成」を押した瞬間の DAW のテンポと拍子を使います。スタンドアロンでは使えません。", u8"Uses the DAW's tempo and time signature at the moment Generate is pressed. Not available in the standalone app." };
		static inline const I18n::Text releaseTail{ u8"最後のリリースを反映", u8"Include last release" };
		static inline const I18n::Text releaseTailTooltip{ u8"入れると、最後に離したあとの余韻 (リリース) まで作ります。切ると打鍵の並び (小節数) のちょうどの長さで切るので、繰り返し再生してもつなぎ目で間が空きません。「生成」を押し直すと効きます。", u8"When on, the build runs on through the release after the last key-up. When off, it stops at exactly the length of the pattern (the bars), so looped playback has no gap at the seam. Press Generate again for it to take effect." };
		static inline const I18n::Text beat{ u8"拍", u8"Beat" };
		static inline const I18n::Text beatTooltip{ u8"1 小節の拍。音符と、その数 (1〜12)。4 分音符なら 4、8 分音符なら 6 が既定です。DAW 追従のときは DAW の拍子を使います。", u8"Beats per bar: the note and how many (1-12). The default is 4 for quarter notes and 6 for eighth notes. With Follow DAW on, the DAW's time signature is used." };
		static inline const I18n::Text bars{ u8"小節", u8"Bars" };
		static inline const I18n::Text interval{ u8"間隔", u8"Interval" };
		static inline const I18n::Text length{ u8"音符", u8"Note" };
		static inline const I18n::Text triplet{ u8"3連", u8"Triplet" };
		static inline const I18n::Text dotted{ u8"付点", u8"Dotted" };
		static inline const I18n::Text switches{ u8"加工", u8"Process" };
		static inline const I18n::Text switchesTooltip{ u8"切ったものは、この窓で作るときだけ外します。パッチは変わりません。エンベロープを切ると、押しているあいだは最大のまま鳴り、離したところで止まります。", u8"Anything switched off is left out only when building in this window. The patch is not changed. With an envelope off, the sound stays at full level while held and stops on release." };
		static inline const I18n::Text pads{ u8"混ぜるパッド", u8"Pads to mix" };

		// 音符の長さの名前 (1 = 全音符 … 32 = 32 分音符)
		static inline const I18n::Text whole{ u8"全音符", u8"Whole" };
		static inline const I18n::Text fraction{ u8"分音符", u8"th" };

		// 段の見出し
		static inline const I18n::Text row1Fm{ u8"第 1 段: 変調前のオペレーター", u8"Row 1: operators before modulation" };
		static inline const I18n::Text row1Other{ u8"第 1 段: 加工前の音", u8"Row 1: before processing" };
		static inline const I18n::Text row1Rhythm{ u8"第 1 段: パッド (それぞれの鍵で)", u8"Row 1: pads (each on its own key)" };
		static inline const I18n::Text row2Fm{ u8"第 2 段: 変調後のオペレーター", u8"Row 2: operators after modulation" };
		static inline const I18n::Text row3{ u8"第 3 段: 加工の段ごと", u8"Row 3: each processing stage" };

		static inline const I18n::Text stageRaw{ u8"① 加工前", u8"1 Raw" };
		static inline const I18n::Text stageCore{ u8"② 音源の加工後", u8"2 After core" };
		static inline const I18n::Text stageUnison{ u8"③ ユニゾン後", u8"3 After unison" };
		static inline const I18n::Text stageDac{ u8"④ CHIP DAC 後", u8"4 After CHIP DAC" };
		static inline const I18n::Text stageFx{ u8"⑤ FX 後 (完成)", u8"5 After FX (final)" };
		static inline const I18n::Text rawOnly{ u8"加工前", u8"Raw" };

		static inline const juce::String off = "OFF";

		// 入れ切りできるまとまりの名前 (StageSwitch::Module の順)
		static inline const juce::String moduleNames[] = {
			"AMP ENV", "OP EG", "PITCH ENV", "LFO", "SSG HW ENV", "SSG SW ENV", "WT MOD", "STEP ENV", "PARAM ENV",
		};
	}
}
