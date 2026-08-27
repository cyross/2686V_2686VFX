#pragma once

#include <array>

// ============================================================================
// ファミコンディスクシステム(RAMアダプター / 2C33)の変調テーブル
// ============================================================================
// 実機の変調ユニットは 32 エントリ・各 3bit の「増減値テーブル」を持ち、
// 変調器のクロックが上がるたびに次のエントリを読んで、符号付き 7bit の
// カウンタ(-64〜63)へ積算する。そのカウンタ値がそのまま周波数比になる。
//
//   3bit 値 : 0 = +0 / 1 = +1 / 2 = +2 / 3 = +4
//             4 = カウンタを 0 にリセット
//             5 = -4 / 6 = -2 / 7 = -1
//
// 増減量が最大でも ±4 しかないため、実機の変調波形は必ず 32 段の階段状になる。
// テーブル編集 UI は持たないので、性格の違う 4 種類を作り置きしてある。
// どのテーブルも 1 周ぶんの平均(DC)が 0 になるよう組んであり、変調をかけただけで
// 音程が上下にずれることはない。出力は -1.0〜1.0 に正規化し、Depth の意味を
// 正弦波モードと揃えている。
namespace FdsMod
{
	inline constexpr int tableSize = 32;
	inline constexpr int tableCount = 4;

	// 0 : Triangle  対称三角。正弦波にいちばん近く、素直な揺れ。
	// 1 : Saw       非対称。偶数次倍音が出るので正弦波との違いが分かりやすい。
	// 2 : Reset     リセットエントリ(4)で断ち切る、実機にしかない不連続。
	// 3 : Pulse     上下の端で保持する台形。ピッチが端に張り付く独特の動き。
	inline constexpr std::array<std::array<int, tableSize>, tableCount> tables = { {
		// Triangle : +4 x 8 -> -4 x 16 -> +4 x 8
		{ 3,3,3,3,3,3,3,3, 5,5,5,5,5,5,5,5, 5,5,5,5,5,5,5,5, 3,3,3,3,3,3,3,3 },
		// Saw : -4 x 4 -> +2 x 16 -> -4 x 4 -> 平坦 x 8
		{ 5,5,5,5, 2,2,2,2,2,2,2,2,2,2,2,2,2,2,2,2, 5,5,5,5, 0,0,0,0,0,0,0,0 },
		// Reset : -4 x 6 -> +4 x 14 -> リセット -> 平坦 x 11
		{ 5,5,5,5,5,5, 3,3,3,3,3,3,3,3,3,3,3,3,3,3, 4, 0,0,0,0,0,0,0,0,0,0,0 },
		// Pulse : +4 x 4 -> 平坦 x 8 -> -4 x 8 -> 平坦 x 8 -> +4 x 4
		{ 3,3,3,3, 0,0,0,0,0,0,0,0, 5,5,5,5,5,5,5,5, 0,0,0,0,0,0,0,0, 3,3,3,3 },
	} };

	// 3bit のテーブル値をカウンタへの増減量に変換する
	inline constexpr int decodeIncrement(int entry)
	{
		switch (entry) {
		case 0: return 0;
		case 1: return 1;
		case 2: return 2;
		case 3: return 4;
		case 4: return 0; // リセットは積算側で扱う
		case 5: return -4;
		case 6: return -2;
		case 7: return -1;
		}

		return 0;
	}

	// テーブルを 1 周ぶん積算し、-1.0〜1.0 に正規化した階段波を作る
	inline constexpr std::array<float, tableSize> makeSteps(const std::array<int, tableSize>& table)
	{
		std::array<int, tableSize> counters{};

		int counter = 0;
		int peak = 1;

		for (int i = 0; i < tableSize; i++) {
			// そのステップで出力されるのは「テーブルを進める前」のカウンタ値。
			// こうしておくと位相 0 で変調量も 0 になり、発音の頭でピッチが飛ばない。
			counters[i] = counter;

			int magnitude = counter < 0 ? -counter : counter;
			if (magnitude > peak) peak = magnitude;

			// 次のステップに向けてテーブルを 1 つ進める
			if (table[i] == 4) {
				counter = 0; // 実機のリセットエントリ
			}
			else {
				counter += decodeIncrement(table[i]);
			}

			// 実機のカウンタは符号付き 7bit
			if (counter > 63) counter -= 128;
			if (counter < -64) counter += 128;
		}

		std::array<float, tableSize> result{};

		for (int i = 0; i < tableSize; i++) {
			result[i] = (float)counters[i] / (float)peak;
		}

		return result;
	}

	inline constexpr std::array<std::array<float, tableSize>, tableCount> steps = { {
		makeSteps(tables[0]),
		makeSteps(tables[1]),
		makeSteps(tables[2]),
		makeSteps(tables[3]),
	} };

	// テーブル番号と位相(0.0〜1.0)から階段状の変調値(-1.0〜1.0)を引く
	inline float value(int tableIndex, float phase) noexcept
	{
		if (tableIndex < 0) tableIndex = 0;
		if (tableIndex >= tableCount) tableIndex = tableCount - 1;

		int index = (int)(phase * (float)tableSize);

		index &= (tableSize - 1); // 0〜31 に丸める

		return steps[tableIndex][index];
	}
}
