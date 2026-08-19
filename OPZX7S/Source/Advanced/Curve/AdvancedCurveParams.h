#pragma once

#include <array>

struct PositionParams
{
	float x = 0.0f;
	float y = 0.0f;
};

struct CurveRateParams
{
    float rate = 0.0f;
};

struct Spline1Params
{
	PositionParams cp1;
};

struct Spline2Params
{
	PositionParams cp1;
	PositionParams cp2;
};

struct Linear1Params
{
	PositionParams pos1;
};

struct Linear1CurveParams
{
	PositionParams pos1;
	float rate = 0.0f;
};

struct Linear1Spline1Params
{
	PositionParams pos1;
	PositionParams cp1;
};

struct Linear1Spline2Params
{
	PositionParams pos1;
	PositionParams cp1;
	PositionParams cp2;
};

struct Linear2Params
{
	PositionParams pos1;
	PositionParams pos2;
};

struct Linear2CurveParams
{
	PositionParams pos1;
	PositionParams pos2;
	float rate = 0.0f;
};

struct Linear2Spline1Params
{
	PositionParams pos1;
	PositionParams pos2;
	PositionParams cp1;
};

struct Linear2Spline2Params
{
	PositionParams pos1;
	PositionParams pos2;
	PositionParams cp1;
	PositionParams cp2;
};

struct Spline12Params
{
	PositionParams pos1;
	PositionParams cp1;
	PositionParams cp2;
};

struct Spline22Params
{
	PositionParams pos1;
	PositionParams cp1;
	PositionParams cp2;
	PositionParams cp3;
	PositionParams cp4;
};

struct Spline13Params
{
	PositionParams pos1;
	PositionParams pos2;
	PositionParams cp1;
	PositionParams cp2;
	PositionParams cp3;
};

struct Spline23Params
{
	PositionParams pos1;
	PositionParams pos2;
	PositionParams cp1;
	PositionParams cp2;
	PositionParams cp3;
	PositionParams cp4;
	PositionParams cp5;
	PositionParams cp6;
};

struct BaseCurveParams
{
	int logic = 0;                   // CurveParams::Logic の値を入れる
	float k = 1.0f;                  // 各 rate の倍率(UIの便宜上追加)

	                                   //  0 | 線形                                          | なし
									   //  1 | 円弧(指数)                                    | なし
									   //  2 | 円弧(対数)                                    | なし
	CurveRateParams expCurve;          //  3 | 指数                                          | r
	CurveRateParams logCurve;          //  4 | 対数                                          | r
	Spline1Params sp1Curve;            //  5 | 1点スプライン                                 | cp1.x, cp1.y
	Spline2Params sp2Curve;            //  6 | 2点スプライン                                 | cp1.x, cp1.y, cp2.x, cp2.y
	Linear1Params linear1ArcExp;       //  7 | 線形 + 円弧(指数)                             | pos1.x, pos1.y
	Linear1Params linear1ArcLog;       //  8 | 線形 + 円弧(対数)                             | pos1.x, pos1.y
	Linear1CurveParams linear1Exp;     //  9 | 線形 + 指数                                   | pos1.x, pos1.y, rate
	Linear1CurveParams linear1Log;     // 10 | 線形 + 対数                                   | pos1.x, pos1.y, rate
	Linear1Spline1Params linear1Sp1;   // 11 | 線形 + 1点スプライン                          | pos1.x, pos1.y, cp1.x, cp1.y
	Linear1Spline2Params linear1Sp2;   // 12 | 線形 + 2点スプライン                          | pos1.x, pos1.y, cp1.x, cp1.y, cp2.x, cp2.y
	Linear1Params arcExpLinear1;       // 13 | 円弧(指数) + 線形                             | pos1.x, pos1.y
	Linear1Params arcLogLinear1;       // 14 | 円弧(対数) + 線形                             | pos1.x, pos1.y
	Linear1CurveParams expLinear1;     // 15 | 指数 + 線形                                   | pos1.x, pos1.y, rate
	Linear1CurveParams logLinear1;     // 16 | 対数 + 線形                                   | pos1.x, pos1.y, rate
	Linear1Spline1Params sp1Linear1;   // 17 | 1点スプライン + 線形                          | pos1.x, pos1.y, cp1.x, cp1.y
	Linear1Spline2Params sp2Linear1;   // 18 | 2点スプライン + 線形                          | pos1.x, pos1.y, cp1.x, cp1.y, cp2.x, cp2.y
	Linear2Params linear2ArcExp;       // 19 | 線形 + 円弧(指数) + 線形                      | pos1.x, pos1.y, pos2.x, pos2.y
	Linear2Params linear2ArcLog;       // 20 | 線形 + 円弧(対数) + 線形                      | pos1.x, pos1.y, pos2.x, pos2.y
	Linear2CurveParams linear2Exp;     // 21 | 線形 + 指数 + 線形                            | pos1.x, pos1.y, pos2.x, pos2.y, rate
	Linear2CurveParams linear2Log;     // 22 | 線形 + 対数 + 線形                            | pos1.x, pos1.y, pos2.x, pos2.y, rate
	Linear2Spline1Params linear2Sp1;   // 23 | 線形 + 1点スプライン + 線形                   | pos1.x, pos1.y, pos2.x, pos2.y, cp1.x, cp1.y
	Linear2Spline2Params linear2Sp2;   // 24 | 線形 + 2点スプライン + 線形                   | pos1.x, pos1.y, pos2.x, pos2.y, cp1.x, cp1.y, cp2.x, cp2.y
	Linear1Params linear2;             // 25 | 線形 + 線形                                   | pos1.x, pos1.y
	Linear2Params linear3;             // 26 | 線形 + 線形 + 線形                            | pos1.x, pos1.y, pos2.x, pos2.y
	Spline12Params sprine12;           // 27 | 1点スプライン + 1点スプライン                 | pos1.x, pos1.y, cp1.x, cp1.y, cp2.x, cp2.y
	Spline22Params sprine22;           // 28 | 2点スプライン + 2点スプライン                 | pos1.x, pos1.y, cp1.x, cp1.y, cp2.x, cp2.y, cp3.x, cp3.y, cp4.x, cp4.y
	Spline13Params sprine13;           // 29 | 1点スプライン + 1点スプライン + 1点スプライン | pos1.x, pos1.y, pos2.x, pos2.y, cp1.x, cp1.y, cp2.x, cp2.y, cp3.x, cp3.y
	Spline23Params sprine23;           // 30 | 2点スプライン + 2点スプライン + 2点スプライン | pos1.x, pos1.y, pos2.x, pos2.y, cp1.x, cp1.y, cp2.x, cp2.y, cp3.x, cp3.y, cp4.x, cp4.y, cp5.x, cp5.y, cp6.x, cp6.y
};

struct CurveParams
{
    enum class Position { Op1, Op2, Op3, Op4, Op5, Op6, Op7, Op8, Size };
    enum class Target { RegValue, AmpEnv, PitchEnv, SsgSwEnv, SsgSwEnv11, SsgSwPEnv11, Size };
	enum class TargetRegValue { Ar, Dr, Sl, Rr, Sr, Tl, Size };
	enum class TargetAmpEnv { Ar, Dr, Rr, Sr, Size };
	enum class TargetPitchEnv { Ar, Dr, Rr, Size };
	enum class TargetSsgSwEnv { R1, R2, R3, R4, R5, R6, LoopTo, Size };
	enum class TargetSsgSwEnv11 { R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, LoopTo, Size };
	enum class TargetSsgSwPEnv11 { R1, R2, R3, R4, R5, R6, R7, R8, R9, R10, R11, LoopTo, Size };
	enum class Logic {
		Linear,        //  0 | 線形
		ArcExp,        //  1 | 円弧(指数的)
		ArcLog,        //  2 | 円弧(対数的)
		Exp,           //  3 | 指数
		Log,           //  4 | 対数
		Sp1,           //  5 | 1点スプライン
		Sp2,           //  6 | 2点スプライン
		LinearArcExp,  //  7 | 線形 + 円弧(指数的)
		LinearArcLog,  //  8 | 線形 + 円弧(対数的)
		LinearExp,     //  9 | 線形 + 指数
		LinearLog,     // 10 | 線形 + 対数
		LinearSp1,     // 11 | 線形 + 1点スプライン
		LinearSp2,     // 12 | 線形 + 2点スプライン
		ArcExpLinear,  // 13 | 円弧(指数的) + 線形
		ArcLogLinear,  // 14 | 円弧(対数的) + 線形
		ExpLinear,     // 15 | 指数 + 線形
		LogLinear,     // 16 | 対数 + 線形
		Sp1Linear,     // 17 | 1点スプライン + 線形
		Sp2Linear,     // 18 | 2点スプライン + 線形
		Linear2ArcExp, // 19 | 線形 + 円弧(指数的) + 線形
		Linear2ArcLog, // 20 | 線形 + 円弧(対数的) + 線形
		Linear2Exp,    // 21 | 線形 + 指数 + 線形
		Linear2Log,    // 22 | 線形 + 対数 + 線形
		Linear2Sp1,    // 23 | 線形 + 1点スプライン + 線形
		Linear2Sp2,    // 24 | 線形 + 2点スプライン + 線形
		Linear2,       // 25 | 線形 + 線形
		Linear3,       // 26 | 線形 + 線形 + 線形
		Sprine12,      // 27 | 1点スプライン + 1点スプライン
	    Sprine22,      // 28 | 2点スプライン + 2点スプライン
	    Sprine13,      // 29 | 1点スプライン + 1点スプライン + 1点スプライン
	    Sprine23,      // 30 | 2点スプライン + 2点スプライン + 2点スプライン
	    Size           // 31 | ロジックの種類
	};

    bool enable = false; // 従来モードかカーブモードか

	// params[Position][Target][(Ar/Dr or R1/R2)...]
	std::array<std::array<std::array<BaseCurveParams, 16>, (int)Target::Size>, (int)Position::Size> params;
};
