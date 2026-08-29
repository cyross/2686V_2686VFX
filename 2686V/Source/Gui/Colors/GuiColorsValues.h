#pragma once

namespace ColorsGuiValue
{
	namespace Table
	{
		// 色の見本を出す列は狭くてよい
		static inline constexpr int SwatchWidth = 60;
		static inline constexpr int ItemWidth = 240;
		static inline constexpr int ValueWidth = 200;

		// 行ごとの「既定へ戻す」ボタンを置く列
		static inline constexpr int ResetWidth = 90;

		// セルの中でボタンの周りに取る余白
		static inline constexpr int CellPadding = 3;

		static inline constexpr int PaddingWidth = 10;
		static inline constexpr int PaddingHeight = 10;
	}

	namespace Row
	{
		static inline constexpr int height = 24;
		static inline constexpr int padding = 6;

		// 断り書きは 2 行に折り返る想定
		static inline constexpr int noticeHeight = 32;

		// 板の内側に取る余白
		static inline constexpr int boardPadding = 8;
	}

	namespace Button
	{
		static inline constexpr int width = 140;

		// 読み込み・書き出しは字が短いので狭くてよい
		static inline constexpr int fileWidth = 90;
	}

	// 色の変更ダイアログ
	namespace Editor
	{
		static inline constexpr int width = 620;

		// JUCE の ColourSelector に渡す高さ
		static inline constexpr int selectorHeight = 260;

		// パレットを覗く窓の高さ。全部は入らないので中で送る。
		static inline constexpr int paletteHeight = 150;

		// 選択肢の左に置く見出しの幅
		static inline constexpr int labelWidth = 170;

		// 色名の左へ出す色見本
		static inline constexpr int swatchWidth = 26;
		static inline constexpr int swatchPadding = 3;
	}
}
