// MML変換で使用するレジスタ値を管理

#pragma once

namespace mmlValues
{
	// 各パラメータの最大値
	namespace opna
	{
		static inline constexpr int mul = 15;
		static inline constexpr int dt = 7;
		static inline constexpr int ar = 31;
		static inline constexpr int dr = 31;
		static inline constexpr int sr = 31;
		static inline constexpr int sl = 15;
		static inline constexpr int rr = 15;
		static inline constexpr int tl = 127;
		static inline constexpr int ks = 3;
		static inline constexpr int mask = 1;
	};
	namespace opn
	{
		static inline constexpr int mul = 15;
		static inline constexpr int dt = 7;
		static inline constexpr int ar = 31;
		static inline constexpr int dr = 31;
		static inline constexpr int sr = 31;
		static inline constexpr int sl = 15;
		static inline constexpr int rr = 15;
		static inline constexpr int tl = 127;
		static inline constexpr int ks = 3;
		static inline constexpr int mask = 1;
	};
	namespace opl
	{
		static inline constexpr int mul = 15;
		static inline constexpr int dt = 7;
		static inline constexpr int ar = 15;
		static inline constexpr int dr = 15;
		static inline constexpr int sr = 15;
		static inline constexpr int sl = 15;
		static inline constexpr int rr = 15;
		static inline constexpr int tl = 63;
		static inline constexpr int ksr = 1;
		static inline constexpr int ksl = 3;
		static inline constexpr int am = 1;
		static inline constexpr int vib = 1;
		static inline constexpr int egtype = 1;
		static inline constexpr int mask = 1;
	};
	namespace opl3
	{
		static inline constexpr int mul = 15;
		static inline constexpr int ar = 15;
		static inline constexpr int dr = 15;
		static inline constexpr int sr = 15;
		static inline constexpr int sl = 15;
		static inline constexpr int rr = 15;
		static inline constexpr int tl = 63;
		static inline constexpr int ksr = 1;
		static inline constexpr int ksl = 3;
		static inline constexpr int am = 1;
		static inline constexpr int vib = 1;
		static inline constexpr int egtype = 1;
		static inline constexpr int mask = 1;
	};
	namespace opm
	{
		static inline constexpr int mul = 15;
		static inline constexpr int dt1 = 7;
		static inline constexpr int dt2 = 3;
		static inline constexpr int ar = 31;
		static inline constexpr int d1r = 31;
		static inline constexpr int d1l = 15;
		static inline constexpr int d2r = 31;
		static inline constexpr int rr = 15;
		static inline constexpr int tl = 127;
		static inline constexpr int ks = 3;
		static inline constexpr int mask = 1;
	};
	namespace opzx3
	{
		static inline constexpr int mul = 15;
		static inline constexpr int dt1 = 7;
		static inline constexpr int dt2 = 3;
		static inline constexpr int ar = 31;
		static inline constexpr int d1r = 31;
		static inline constexpr int d1l = 15;
		static inline constexpr int d2r = 31;
		static inline constexpr int rr = 15;
		static inline constexpr int tl = 127;
		static inline constexpr int ks = 3;
		static inline constexpr int mask = 1;
	};
};
