#include <cmath>
#include <algorithm>

#include "./AdvancedCurve.h"

// ヘルパー: ある範囲の値を別の範囲にマッピングする
inline float mapRange(float val, float inMin, float inMax, float outMin, float outMax) {
	if (std::abs(inMax - inMin) < 0.00001f) return outMin; // 0除算防止
	return outMin + (val - inMin) * (outMax - outMin) / (inMax - inMin);
}

CurveCore::CurveCore() {
	// -------------------------------------------------------------
	// 1. 基本となる数学関数群
	// -------------------------------------------------------------
	auto calcLinear = [](float x) { return x; };

	auto calcArcExp = [](float x) { return std::sqrt(1.0f - std::pow(1.0f - x, 2.0f)); };

	auto calcArcLog = [](float x) { return 1.0f - std::sqrt(1.0f - std::pow(x, 2.0f)); };

	auto calcExp = [](float x, float rate) {
		if (std::abs(rate) < 0.001f) return x;
		return std::log(1.0f + rate * x) / std::log(1.0f + rate);
		};

	auto calcLog = [](float x, float rate) {
		if (std::abs(rate) < 0.001f) return x;
		return (std::exp(rate * x) - 1.0f) / (std::exp(rate) - 1.0f);
		};

	// 1点スプライン (二次ベジェ曲線の厳密解)
	auto calcSp1 = [](float x, float cx, float cy) {
		if (x <= 0.0f) return 0.0f;
		if (x >= 1.0f) return 1.0f;
		float t = x;
		// A*t^2 + B*t + C = 0 を解くことで正しい t を求める
		if (std::abs(cx - 0.5f) >= 0.001f) {
			float A = 1.0f - 2.0f * cx;
			float B = 2.0f * cx;
			float C = -x;
			float det = B * B - 4.0f * A * C;
			if (det >= 0.0f) {
				t = (-B + std::sqrt(det)) / (2.0f * A);
			}
		}
		t = std::clamp(t, 0.0f, 1.0f);
		return (1.0f - t) * (1.0f - t) * 0.0f + 2.0f * (1.0f - t) * t * cy + t * t * 1.0f;
		};

	// 2点スプライン (三次ベジェ曲線の近似解)
	auto calcSp2 = [](float x, float cx1, float cy1, float cx2, float cy2) -> float {
		if (x <= 0.0001f) return 0.0f;
		if (x >= 0.9999f) return 1.0f;

		auto getX = [cx1, cx2](float t) {
			float mt = 1.0f - t;
			return 3.0f * mt * mt * t * cx1 + 3.0f * mt * t * t * cx2 + t * t * t;
			};
		auto getSlope = [cx1, cx2](float t) {
			float mt = 1.0f - t;
			return 3.0f * mt * mt * cx1 + 6.0f * mt * t * (cx2 - cx1) + 3.0f * t * t * (1.0f - cx2);
			};

		float t = x;
		for (int i = 0; i < 4; i++) {
			float currentX = getX(t);
			float slope = getSlope(t);
			if (std::abs(slope) < 0.0001f) break;
			float dt = (currentX - x) / slope;
			t -= dt;
			t = std::clamp(t, 0.0f, 1.0f);
			if (std::abs(dt) < 0.0001f) break;
		}

		float mt = 1.0f - t;
		return 3.0f * mt * mt * t * cy1 + 3.0f * mt * t * t * cy2 + t * t * t;
		};

	// -------------------------------------------------------------
	// 2. ロジックごとの関数マッピング
	// -------------------------------------------------------------
	logics[CurveParams::Logic::Linear] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		return calcLinear(x);
		};

	logics[CurveParams::Logic::ArcExp] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		return calcArcExp(x);
		};

	logics[CurveParams::Logic::ArcLog] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		return calcArcLog(x);
		};

	logics[CurveParams::Logic::Exp] = [this, calcExp](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].expCurve;
		float k = m_params.params[pIdx][tIdx][prmIdx].k;
		return calcExp(x, p.rate * k); // ★kを適用
		};

	logics[CurveParams::Logic::Log] = [this, calcLog](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].logCurve;
		float k = m_params.params[pIdx][tIdx][prmIdx].k;
		return calcLog(x, p.rate * k); // ★kを適用
		};

	logics[CurveParams::Logic::Sp1] = [this, calcSp1](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].sp1Curve;
		return calcSp1(x, p.cp.x, p.cp.y);
		};

	logics[CurveParams::Logic::Sp2] = [this, calcSp2](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].sp2Curve;
		return calcSp2(x, p.cp1.x, p.cp1.y, p.cp2.x, p.cp2.y);
		};

	logics[CurveParams::Logic::LinearArcExp] = [this, calcLinear, calcArcExp](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear1ArcExp;
		float px = p.pos.x; float py = p.pos.y;
		if (x <= px) return mapRange(calcLinear(mapRange(x, 0.0f, px, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py);
		else         return mapRange(calcArcExp(mapRange(x, px, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py, 1.0f);
		};

	logics[CurveParams::Logic::LinearArcLog] = [this, calcLinear, calcArcLog](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear1ArcLog;
		float px = p.pos.x; float py = p.pos.y;
		if (x <= px) return mapRange(calcLinear(mapRange(x, 0.0f, px, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py);
		else         return mapRange(calcArcLog(mapRange(x, px, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py, 1.0f);
		};

	logics[CurveParams::Logic::LinearExp] = [this, calcLinear, calcExp](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear1Exp;
		float k = m_params.params[pIdx][tIdx][prmIdx].k;
		float px = p.pos.x; float py = p.pos.y;
		if (x <= px) return mapRange(calcLinear(mapRange(x, 0.0f, px, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py);
		else         return mapRange(calcExp(mapRange(x, px, 1.0f, 0.0f, 1.0f), p.rate * k), 0.0f, 1.0f, py, 1.0f);
		};

	logics[CurveParams::Logic::LinearLog] = [this, calcLinear, calcLog](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear1Log;
		float k = m_params.params[pIdx][tIdx][prmIdx].k;
		float px = p.pos.x; float py = p.pos.y;
		if (x <= px) return mapRange(calcLinear(mapRange(x, 0.0f, px, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py);
		else         return mapRange(calcLog(mapRange(x, px, 1.0f, 0.0f, 1.0f), p.rate * k), 0.0f, 1.0f, py, 1.0f);
		};

	logics[CurveParams::Logic::LinearSp1] = [this, calcLinear, calcSp1](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear1Sp1;
		float px = p.pos.x; float py = p.pos.y;
		if (x <= px) {
			return mapRange(calcLinear(mapRange(x, 0.0f, px, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			// ローカル座標へのマッピングを適用
			float localCX = mapRange(p.cp.x, px, 1.0f, 0.0f, 1.0f);
			float localCY = mapRange(p.cp.y, py, 1.0f, 0.0f, 1.0f);
			return mapRange(calcSp1(mapRange(x, px, 1.0f, 0.0f, 1.0f), localCX, localCY), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::LinearSp2] = [this, calcLinear, calcSp2](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear1Sp2;
		float px = p.pos.x; float py = p.pos.y;
		if (x <= px) {
			return mapRange(calcLinear(mapRange(x, 0.0f, px, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			float localCX1 = mapRange(p.cp1.x, px, 1.0f, 0.0f, 1.0f);
			float localCY1 = mapRange(p.cp1.y, py, 1.0f, 0.0f, 1.0f);
			float localCX2 = mapRange(p.cp2.x, px, 1.0f, 0.0f, 1.0f);
			float localCY2 = mapRange(p.cp2.y, py, 1.0f, 0.0f, 1.0f);
			return mapRange(calcSp2(mapRange(x, px, 1.0f, 0.0f, 1.0f), localCX1, localCY1, localCX2, localCY2), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::ArcExpLinear] = [this, calcLinear, calcArcExp](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].arcExpLinear1;
		float px = p.pos.x; float py = p.pos.y;
		if (x <= px) return mapRange(calcArcExp(mapRange(x, 0.0f, px, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py);
		else         return mapRange(calcLinear(mapRange(x, px, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py, 1.0f);
		};

	logics[CurveParams::Logic::ArcLogLinear] = [this, calcLinear, calcArcLog](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].arcLogLinear1;
		float px = p.pos.x; float py = p.pos.y;
		if (x <= px) return mapRange(calcArcLog(mapRange(x, 0.0f, px, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py);
		else         return mapRange(calcLinear(mapRange(x, px, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py, 1.0f);
		};

	logics[CurveParams::Logic::ExpLinear] = [this, calcLinear, calcExp](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].expLinear1;
		float k = m_params.params[pIdx][tIdx][prmIdx].k;
		float px = p.pos.x; float py = p.pos.y;
		if (x <= px) return mapRange(calcExp(mapRange(x, 0.0f, px, 0.0f, 1.0f), p.rate * k), 0.0f, 1.0f, 0.0f, py);
		else         return mapRange(calcLinear(mapRange(x, px, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py, 1.0f);
		};

	logics[CurveParams::Logic::LogLinear] = [this, calcLinear, calcLog](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].logLinear1;
		float k = m_params.params[pIdx][tIdx][prmIdx].k;
		float px = p.pos.x; float py = p.pos.y;
		if (x <= px) return mapRange(calcLog(mapRange(x, 0.0f, px, 0.0f, 1.0f), p.rate * k), 0.0f, 1.0f, 0.0f, py);
		else         return mapRange(calcLinear(mapRange(x, px, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py, 1.0f);
		};

	logics[CurveParams::Logic::Sp1Linear] = [this, calcLinear, calcSp1](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].sp1Linear1;
		float px = p.pos.x; float py = p.pos.y;
		if (x <= px) {
			float localCX = mapRange(p.cp.x, 0.0f, px, 0.0f, 1.0f);
			float localCY = mapRange(p.cp.y, 0.0f, py, 0.0f, 1.0f);
			return mapRange(calcSp1(mapRange(x, 0.0f, px, 0.0f, 1.0f), localCX, localCY), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			return mapRange(calcLinear(mapRange(x, px, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::Sp2Linear] = [this, calcLinear, calcSp2](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].sp2Linear1;
		float px = p.pos.x; float py = p.pos.y;
		if (x <= px) {
			float localCX1 = mapRange(p.cp1.x, 0.0f, px, 0.0f, 1.0f);
			float localCY1 = mapRange(p.cp1.y, 0.0f, py, 0.0f, 1.0f);
			float localCX2 = mapRange(p.cp2.x, 0.0f, px, 0.0f, 1.0f);
			float localCY2 = mapRange(p.cp2.y, 0.0f, py, 0.0f, 1.0f);
			return mapRange(calcSp2(mapRange(x, 0.0f, px, 0.0f, 1.0f), localCX1, localCY1, localCX2, localCY2), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			return mapRange(calcLinear(mapRange(x, px, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::Linear2ArcExp] = [this, calcLinear, calcArcExp](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear2ArcExp;
		// ユーザー操作による破綻を防ぐため、px1とpx2の順序を補正
		float px1 = std::min(p.pos1.x, p.pos2.x); float px2 = std::max(p.pos1.x, p.pos2.x);
		float py1 = (p.pos1.x <= p.pos2.x) ? p.pos1.y : p.pos2.y; float py2 = (p.pos1.x <= p.pos2.x) ? p.pos2.y : p.pos1.y;

		if (x <= px1) return mapRange(calcLinear(mapRange(x, 0.0f, px1, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py1);
		else if (x <= px2) return mapRange(calcArcExp(mapRange(x, px1, px2, 0.0f, 1.0f)), 0.0f, 1.0f, py1, py2);
		else return mapRange(calcLinear(mapRange(x, px2, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py2, 1.0f);
		};

	logics[CurveParams::Logic::Linear2ArcLog] = [this, calcLinear, calcArcLog](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear2ArcLog;
		float px1 = std::min(p.pos1.x, p.pos2.x); float px2 = std::max(p.pos1.x, p.pos2.x);
		float py1 = (p.pos1.x <= p.pos2.x) ? p.pos1.y : p.pos2.y; float py2 = (p.pos1.x <= p.pos2.x) ? p.pos2.y : p.pos1.y;

		if (x <= px1) return mapRange(calcLinear(mapRange(x, 0.0f, px1, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py1);
		else if (x <= px2) return mapRange(calcArcLog(mapRange(x, px1, px2, 0.0f, 1.0f)), 0.0f, 1.0f, py1, py2);
		else return mapRange(calcLinear(mapRange(x, px2, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py2, 1.0f);
		};

	logics[CurveParams::Logic::Linear2Exp] = [this, calcLinear, calcExp](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear2Exp;
		float k = m_params.params[pIdx][tIdx][prmIdx].k;
		float px1 = std::min(p.pos1.x, p.pos2.x); float px2 = std::max(p.pos1.x, p.pos2.x);
		float py1 = (p.pos1.x <= p.pos2.x) ? p.pos1.y : p.pos2.y; float py2 = (p.pos1.x <= p.pos2.x) ? p.pos2.y : p.pos1.y;

		if (x <= px1) return mapRange(calcLinear(mapRange(x, 0.0f, px1, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py1);
		else if (x <= px2) return mapRange(calcExp(mapRange(x, px1, px2, 0.0f, 1.0f), p.rate * k), 0.0f, 1.0f, py1, py2);
		else return mapRange(calcLinear(mapRange(x, px2, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py2, 1.0f);
		};

	logics[CurveParams::Logic::Linear2Log] = [this, calcLinear, calcLog](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear2Log;
		float k = m_params.params[pIdx][tIdx][prmIdx].k;
		float px1 = std::min(p.pos1.x, p.pos2.x); float px2 = std::max(p.pos1.x, p.pos2.x);
		float py1 = (p.pos1.x <= p.pos2.x) ? p.pos1.y : p.pos2.y; float py2 = (p.pos1.x <= p.pos2.x) ? p.pos2.y : p.pos1.y;

		if (x <= px1) return mapRange(calcLinear(mapRange(x, 0.0f, px1, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py1);
		else if (x <= px2) return mapRange(calcLog(mapRange(x, px1, px2, 0.0f, 1.0f), p.rate * k), 0.0f, 1.0f, py1, py2);
		else return mapRange(calcLinear(mapRange(x, px2, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py2, 1.0f);
		};

	logics[CurveParams::Logic::Linear2Sp1] = [this, calcLinear, calcSp1](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear2Sp1;
		float px1 = std::min(p.pos1.x, p.pos2.x); float px2 = std::max(p.pos1.x, p.pos2.x);
		float py1 = (p.pos1.x <= p.pos2.x) ? p.pos1.y : p.pos2.y; float py2 = (p.pos1.x <= p.pos2.x) ? p.pos2.y : p.pos1.y;

		if (x <= px1) return mapRange(calcLinear(mapRange(x, 0.0f, px1, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py1);
		else if (x <= px2) {
			float localCX = mapRange(p.cp.x, px1, px2, 0.0f, 1.0f);
			float localCY = mapRange(p.cp.y, py1, py2, 0.0f, 1.0f);
			return mapRange(calcSp1(mapRange(x, px1, px2, 0.0f, 1.0f), localCX, localCY), 0.0f, 1.0f, py1, py2);
		}
		else return mapRange(calcLinear(mapRange(x, px2, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py2, 1.0f);
		};

	logics[CurveParams::Logic::Linear2Sp2] = [this, calcLinear, calcSp2](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear2Sp2;
		float px1 = std::min(p.pos1.x, p.pos2.x); float px2 = std::max(p.pos1.x, p.pos2.x);
		float py1 = (p.pos1.x <= p.pos2.x) ? p.pos1.y : p.pos2.y; float py2 = (p.pos1.x <= p.pos2.x) ? p.pos2.y : p.pos1.y;

		if (x <= px1) return mapRange(calcLinear(mapRange(x, 0.0f, px1, 0.0f, 1.0f)), 0.0f, 1.0f, 0.0f, py1);
		else if (x <= px2) {
			float localCX1 = mapRange(p.cp1.x, px1, px2, 0.0f, 1.0f);
			float localCY1 = mapRange(p.cp1.y, py1, py2, 0.0f, 1.0f);
			float localCX2 = mapRange(p.cp2.x, px1, px2, 0.0f, 1.0f);
			float localCY2 = mapRange(p.cp2.y, py1, py2, 0.0f, 1.0f);
			return mapRange(calcSp2(mapRange(x, px1, px2, 0.0f, 1.0f), localCX1, localCY1, localCX2, localCY2), 0.0f, 1.0f, py1, py2);
		}
		else return mapRange(calcLinear(mapRange(x, px2, 1.0f, 0.0f, 1.0f)), 0.0f, 1.0f, py2, 1.0f);
		};
}

void CurveCore::setParameters(const CurveParams& params)
{
	this->m_params = params;

	this->index = this->m_params.enable ? 1 : 0; // カーブモードなら1、従来モードなら0

	if (this->index == 1) {
		bakeCurves();
	}
}

void CurveCore::bakeCurves()
{
	for (int p = 0; p < (int)CurveParams::Position::Size; ++p) {
		for (int t = 0; t < (int)CurveParams::Target::Size; ++t) {
			for (int prm = 0; prm < 8; ++prm) {
				// 線形や無効なパラメータはスキップしても良いが、単純化のため全部計算する
				for (int i = 0; i < LUT_SIZE; ++i) {
					float x = (float)i / (float)(LUT_SIZE - 1); // 0.0 ~ 1.0
					curveLUT[p][t][prm][i] = processRaw(p, t, prm, x);
				}
			}
		}
	}
}

void CurveCore::bakeCurvesPrim(int positionIndex, int targetIndex, int paramIndex)
{
	// 線形や無効なパラメータはスキップしても良いが、単純化のため全部計算する
	for (int i = 0; i < LUT_SIZE; ++i) {
		float x = (float)i / (float)(LUT_SIZE - 1); // 0.0 ~ 1.0
		curveLUT[positionIndex][targetIndex][paramIndex][i] = processRaw(positionIndex, targetIndex, paramIndex, x);
	}
}

// 以前の process の中身 (重い計算)
float CurveCore::processRaw(int positionIndex, int targetIndex, int paramIndex, float x)
{
	if (x <= 1e-5f) return 0.0f;
	if (x >= 1.0f - 1e-5f) return 1.0f;

	int logicIndex = m_params.params[positionIndex][targetIndex][paramIndex].logic;
	auto logic = static_cast<CurveParams::Logic>(logicIndex);

	if (logics.find(logic) == logics.end()) return x;
	return logics[logic](positionIndex, targetIndex, paramIndex, x);
}

std::function<float(int, int, int, float)> CurveCore::getFunction(int logicIndex) {
	return logics[(CurveParams::Logic)logicIndex];
}
