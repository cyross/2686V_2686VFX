#pragma once

namespace ColorsGuiValue
{
	namespace Table
	{
		// 色の見本を出す列は狭くてよい
		static inline constexpr int SwatchWidth = 60;
		static inline constexpr int ItemWidth = 240;
		static inline constexpr int ValueWidth = 200;

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
	}
}
