#pragma once

#include <JuceHeader.h>

#include <array>

// ============================================================================
// 簡易表示モード
// ============================================================================
// 区分が増えて 1 つのタブが縦に長くなったので、よく使うものだけ残して
// 畳んでしまうための切り替え。出しているものを隠すだけで、音には影響しない。
//
// 何を隠すかはタブごとに決めてある。実機の機能として持っている区分は
// 残し、他チャンネルから借りて置いている区分を隠すのが基本。
// たとえば SSG では SSG HW AMP ENV が、WT では WT PITCH MOD が残る。
//
// そのうえで、ここに並べた区分は 1 つずつ「隠さない」と指定できる。
// SETTINGS のカスタマイズがそれで、入れておくと簡易表示モードでも出る。
namespace SimpleView
{
	// カスタマイズできる区分。並びがそのまま SETTINGS の並びになる。
	enum Cat
	{
		AmpEnv = 0,
		SsgHwAmpEnv,
		SsgSwAmpEnv,
		SsgSwAmpEnv11,
		WtAmpMod,
		PitchEnv,
		SsgHwPitchEnv,
		SsgSwPitchEnv11,
		WtPitchMod,
		Lfo,
		MulDet,
		Fix,
		Unison,

		Size
	};

	struct Item
	{
		// SETTINGS に出す名前
		const char* title;

		// 設定ファイルの鍵。並びを変えても値が迷子にならないよう名前で持つ。
		const char* key;
	};

	inline const std::array<Item, Size>& items()
	{
		static const std::array<Item, Size> list = { {
			{ "AMP ENV",              "simpleViewShowAmpEnv" },
			{ "SSG HW AMP ENV",       "simpleViewShowSsgHwAmpEnv" },
			{ "SSG SW AMP ENV",       "simpleViewShowSsgSwAmpEnv" },
			{ "SSG SW AMP ENV[11]",   "simpleViewShowSsgSwAmpEnv11" },
			{ "WT AMP MOD",           "simpleViewShowWtAmpMod" },
			{ "PITCH ENV",            "simpleViewShowPitchEnv" },
			{ "SSG HW PITCH ENV",     "simpleViewShowSsgHwPitchEnv" },
			{ "SSG SW PITCH ENV[11]", "simpleViewShowSsgSwPitchEnv11" },
			{ "WT PITCH MOD",         "simpleViewShowWtPitchMod" },
			{ "LFO",                  "simpleViewShowLfo" },
			{ "MUL/DET",              "simpleViewShowMulDet" },
			{ "FIX",                  "simpleViewShowFix" },
			{ "UNISON/HARMONY",       "simpleViewShowUnison" },
		} };

		return list;
	}

	// 隠す対象になっている区分を、いま出すかどうか。
	// 簡易表示モードが切ってあれば常に出す。
	inline bool isShown(bool simpleView, const std::array<bool, Size>& show, Cat cat)
	{
		if (!simpleView) return true;

		return show[(size_t)cat];
	}
}
