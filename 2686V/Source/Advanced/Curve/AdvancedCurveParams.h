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
	PositionParams cp;
};

struct Spline2Params
{
	PositionParams cp1;
	PositionParams cp2;
};

struct Linear1Params
{
	PositionParams pos;
};

struct Linear1CurveParams
{
	PositionParams pos;
	float rate = 0.0f;
};

struct Linear1Spline1Params
{
	PositionParams pos;
	PositionParams cp;
};

struct Linear1Spline2Params
{
	PositionParams pos;
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
	PositionParams cp;
};

struct Linear2Spline2Params
{
	PositionParams pos1;
	PositionParams pos2;
	PositionParams cp1;
	PositionParams cp2;
};

struct BaseCurveParams
{
	int logic = 0;                   // CurveParams::Logic の値を入れる
	float k = 1.0f;                  // 各 rate の倍率(UIの便宜上追加)

	                                 //  0 | 線形: なし
	                                 //  1 | 円弧(指数): なし
	                                 //  2 | 円弧(対数): なし
	CurveRateParams expCurve;        //  3 | 指数
	CurveRateParams logCurve;        //  4 | 対数
	Spline1Params sp1Curve;          //  5 | 1点スプライン
	Spline2Params sp2Curve;          //  6 | 2点スプライン
	Linear1Params linear1ArcExp;     //  7 | 線形 + 円弧(指数)
	Linear1Params linear1ArcLog;     //  8 | 線形 + 円弧(対数)
	Linear1CurveParams linear1Exp;   //  9 | 線形 + 指数
	Linear1CurveParams linear1Log;   // 10 | 線形 + 対数
	Linear1Spline1Params linear1Sp1; // 11 | 線形 + 1点スプライン
	Linear1Spline2Params linear1Sp2; // 12 | 線形 + 2点スプライン
	Linear1Params arcExpLinear1;     // 13 | 円弧(指数) + 線形
	Linear1Params arcLogLinear1;     // 14 | 円弧(対数) + 線形
	Linear1CurveParams expLinear1;   // 15 | 指数 + 線形
	Linear1CurveParams logLinear1;   // 16 | 対数 + 線形
	Linear1Spline1Params sp1Linear1; // 17 | 1点スプライン + 線形
	Linear1Spline2Params sp2Linear1; // 18 | 2点スプライン + 線形
	Linear2Params linear2ArcExp;     // 19 | 線形 + 円弧(指数) + 線形
	Linear2Params linear2ArcLog;     // 20 | 線形 + 円弧(対数) + 線形
	Linear2CurveParams linear2Exp;   // 21 | 線形 + 指数 + 線形
	Linear2CurveParams linear2Log;   // 22 | 線形 + 対数 + 線形
	Linear2Spline1Params linear2Sp1; // 23 | 線形 + 1点スプライン + 線形
	Linear2Spline2Params linear2Sp2; // 24 | 線形 + 2点スプライン + 線形
};

struct CurveParams
{
    enum class Position { Common, Op1, Op2, Op3, Op4, Op5, Op6, Size };
    enum class Target { RegValue, AmpEnv, PitchEnv, SsgSwEnv, Size };
	enum class TargetRegValue { Ar, Dr, Sl, Rr, Sr, Tl, Size };
	enum class TargetAmpEnv { Ar, Dr, Rr, Sr, Size };
	enum class TargetPitchEnv { Ar, Dr, Rr, Size };
	enum class TargetSsgSwEnv { R1, R2, R3, R4, R5, R6, LoopTo, Size };
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
		Size           // 25 | ロジックの種類
	};

    bool enable = false; // 従来モードかカーブモードか

	// params[Position][Target][(Ar/Dr or R1/R2)...]
	std::array<std::array<std::array<BaseCurveParams, 8>, (int)Target::Size>, (int)Position::Size> params;
};
