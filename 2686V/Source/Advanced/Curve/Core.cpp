#include <cmath>
#include <algorithm>

#include "./Core.h"

// ヘルパー: ある範囲の値を別の範囲にマッピングする
inline float mapRange(float val, float inMin, float inMax, float outMin, float outMax) {
	if (inMax - inMin == 0.0f) return outMin;
	return outMin + (val - inMin) * (outMax - outMin) / (inMax - inMin);
}

CurveCore::CurveCore() {
	// -------------------------------------------------------------
	// 1. 基本となる数学関数群 (常に 0.0 〜 1.0 を入力とし、0.0 〜 1.0 を返す)
	// -------------------------------------------------------------
	auto calcLinear = [](float x) { return x; };

	auto calcArcExp = [](float x) { return std::sqrt(1.0f - std::pow(1.0f - x, 2.0f)); };

	auto calcArcLog = [](float x) { return 1.0f - std::sqrt(1.0f - std::pow(x, 2.0f)); };

	auto calcExp = [](float x, float rate) {
		if (std::abs(rate) < 0.001f) return x; // 0除算防止
		return std::log(1.0f + rate * x) / std::log(1.0f + rate);
		};

	auto calcLog = [](float x, float rate) {
		if (std::abs(rate) < 0.001f) return x;
		return (std::exp(rate * x) - 1.0f) / (std::exp(rate) - 1.0f);
		};

	// 1点スプライン (2次ベジェ: xからtを逆算してyを求める簡易近似)
	auto calcSp1 = [](float x, float cx, float cy) {
		// ※厳密には二次方程式の解の公式が必要ですが、DSP負荷を下げるための簡易マッピング例
		// t^2(1 - 2cx) + 2cx*t - x = 0 を t について解く処理が本来は入ります
		float t = x; // 仮置き（パフォーマンスと相談してニュートン法などを実装）
		return (1.0f - t) * (1.0f - t) * 0.0f + 2.0f * (1.0f - t) * t * cy + t * t * 1.0f;
		};

	auto calcSp2 = [](float x, float cx1, float cy1, float cx2, float cy2) -> float {
		// 端点に張り付いている時は計算をスキップして即リターン
		if (x <= 0.0001f) return 0.0f;
		if (x >= 0.9999f) return 1.0f;

		// --- X(t) を求める式 ---
		auto getX = [cx1, cx2](float t) {
			float mt = 1.0f - t;
			return 3.0f * mt * mt * t * cx1 + 3.0f * mt * t * t * cx2 + t * t * t;
			};

		// --- X'(t) (Xをtで微分した傾き) を求める式 ---
		auto getSlope = [cx1, cx2](float t) {
			float mt = 1.0f - t;
			// 3次ベジェの導関数
			return 3.0f * mt * mt * cx1 + 6.0f * mt * t * (cx2 - cx1) + 3.0f * t * t * (1.0f - cx2);
			};

		// --- ニュートン法で t を近似 ---
		// 曲線は概ね x = t に近い動きをするため、初期値として t = x を使うと非常に早く収束します
		float t = x;
		for (int i = 0; i < 4; i++) { // 最大4回のイテレーション（DSPではこれで十分です）
			float currentX = getX(t);
			float slope = getSlope(t);

			if (std::abs(slope) < 0.0001f) break; // 傾きが0（計算不能）になったら安全のため終了

			float dt = (currentX - x) / slope;
			t -= dt;
			t = std::clamp(t, 0.0f, 1.0f); // t は必ず 0.0 〜 1.0 の間に収める

			if (std::abs(dt) < 0.0001f) break; // 誤差が十分小さくなったら早く抜ける
		}

		// --- 求めた t を Y(t) の式に代入してレベルを求める ---
		float mt = 1.0f - t;
		return 3.0f * mt * mt * t * cy1 + 3.0f * mt * t * t * cy2 + t * t * t;
		};

	logics[CurveParams::Logic::Linear] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		return calcLinear(x);
		};

	logics[CurveParams::Logic::ArcExp] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		return calcArcExp(x);
		};

	logics[CurveParams::Logic::ArcLog] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		return calcArcLog(x);
		};

	logics[CurveParams::Logic::Exp] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].expCurve;
		return calcExp(x, p.rate);
		};

	logics[CurveParams::Logic::Log] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].logCurve;
		return calcLog(x, p.rate);
		};

	logics[CurveParams::Logic::Sp1] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].sp1Curve;
		return calcSp1(x, p.cp.x, p.cp.y);
		};

	logics[CurveParams::Logic::Sp2] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].sp2Curve;
		return calcSp2(x, p.cp1.x, p.cp1.y, p.cp2.x, p.cp2.y);
		};

	logics[CurveParams::Logic::LinearArcExp] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear1ArcExp;
		float px = p.pos.x;
		float py = p.pos.y;

		if (x <= px) {
			// 前半: 0.0 〜 px の区間を 0.0 〜 1.0 に引き伸ばして線形処理、その後 0.0 〜 py に圧縮
			float localX = mapRange(x, 0.0f, px, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			// 後半: px 〜 1.0 の区間を 0.0 〜 1.0 に引き伸ばして指数処理、その後 py 〜 1.0 に圧縮
			float localX = mapRange(x, px, 1.0f, 0.0f, 1.0f);
			return mapRange(calcArcExp(localX), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::LinearArcLog] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear1ArcLog;
		float px = p.pos.x;
		float py = p.pos.y;

		if (x <= px) {
			// 前半: 0.0 〜 px の区間を 0.0 〜 1.0 に引き伸ばして線形処理、その後 0.0 〜 py に圧縮
			float localX = mapRange(x, 0.0f, px, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			// 後半: px 〜 1.0 の区間を 0.0 〜 1.0 に引き伸ばして指数処理、その後 py 〜 1.0 に圧縮
			float localX = mapRange(x, px, 1.0f, 0.0f, 1.0f);
			return mapRange(calcArcLog(localX), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::LinearExp] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear1Exp;
		float px = p.pos.x;
		float py = p.pos.y;

		if (x <= px) {
			// 前半: 0.0 〜 px の区間を 0.0 〜 1.0 に引き伸ばして線形処理、その後 0.0 〜 py に圧縮
			float localX = mapRange(x, 0.0f, px, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			// 後半: px 〜 1.0 の区間を 0.0 〜 1.0 に引き伸ばして指数処理、その後 py 〜 1.0 に圧縮
			float localX = mapRange(x, px, 1.0f, 0.0f, 1.0f);
			return mapRange(calcExp(localX, p.rate), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::LinearLog] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear1Log;
		float px = p.pos.x;
		float py = p.pos.y;

		if (x <= px) {
			// 前半: 0.0 〜 px の区間を 0.0 〜 1.0 に引き伸ばして線形処理、その後 0.0 〜 py に圧縮
			float localX = mapRange(x, 0.0f, px, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			// 後半: px 〜 1.0 の区間を 0.0 〜 1.0 に引き伸ばして指数処理、その後 py 〜 1.0 に圧縮
			float localX = mapRange(x, px, 1.0f, 0.0f, 1.0f);
			return mapRange(calcLog(localX, p.rate), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::LinearSp1] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear1Sp1;
		float px = p.pos.x;
		float py = p.pos.y;

		if (x <= px) {
			// 前半: 0.0 〜 px の区間を 0.0 〜 1.0 に引き伸ばして線形処理、その後 0.0 〜 py に圧縮
			float localX = mapRange(x, 0.0f, px, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			// 後半: px 〜 1.0 の区間を 0.0 〜 1.0 に引き伸ばして指数処理、その後 py 〜 1.0 に圧縮
			float localX = mapRange(x, px, 1.0f, 0.0f, 1.0f);
			return mapRange(calcSp1(localX, p.cp.x, p.cp.y), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::LinearSp2] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear1Sp2;
		float px = p.pos.x;
		float py = p.pos.y;

		if (x <= px) {
			// 前半: 0.0 〜 px の区間を 0.0 〜 1.0 に引き伸ばして線形処理、その後 0.0 〜 py に圧縮
			float localX = mapRange(x, 0.0f, px, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			// 後半: px 〜 1.0 の区間を 0.0 〜 1.0 に引き伸ばして指数処理、その後 py 〜 1.0 に圧縮
			float localX = mapRange(x, px, 1.0f, 0.0f, 1.0f);
			return mapRange(calcSp2(localX, p.cp1.x, p.cp1.y, p.cp2.x, p.cp2.y), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::ArcExpLinear] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].arcExpLinear1;
		float px = p.pos.x;
		float py = p.pos.y;

		if (x <= px) {
			// 前半: 0.0 〜 px の区間を 0.0 〜 1.0 に引き伸ばして線形処理、その後 0.0 〜 py に圧縮
			float localX = mapRange(x, 0.0f, px, 0.0f, 1.0f);
			return mapRange(calcArcExp(localX), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			// 後半: px 〜 1.0 の区間を 0.0 〜 1.0 に引き伸ばして指数処理、その後 py 〜 1.0 に圧縮
			float localX = mapRange(x, px, 1.0f, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::ArcLogLinear] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].arcLogLinear1;
		float px = p.pos.x;
		float py = p.pos.y;

		if (x <= px) {
			// 前半: 0.0 〜 px の区間を 0.0 〜 1.0 に引き伸ばして線形処理、その後 0.0 〜 py に圧縮
			float localX = mapRange(x, 0.0f, px, 0.0f, 1.0f);
			return mapRange(calcArcLog(localX), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			// 後半: px 〜 1.0 の区間を 0.0 〜 1.0 に引き伸ばして指数処理、その後 py 〜 1.0 に圧縮
			float localX = mapRange(x, px, 1.0f, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::ExpLinear] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].expLinear1;
		float px = p.pos.x;
		float py = p.pos.y;

		if (x <= px) {
			// 前半: 0.0 〜 px の区間を 0.0 〜 1.0 に引き伸ばして線形処理、その後 0.0 〜 py に圧縮
			float localX = mapRange(x, 0.0f, px, 0.0f, 1.0f);
			return mapRange(calcExp(localX, p.rate), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			// 後半: px 〜 1.0 の区間を 0.0 〜 1.0 に引き伸ばして指数処理、その後 py 〜 1.0 に圧縮
			float localX = mapRange(x, px, 1.0f, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::LogLinear] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].logLinear1;
		float px = p.pos.x;
		float py = p.pos.y;

		if (x <= px) {
			// 前半: 0.0 〜 px の区間を 0.0 〜 1.0 に引き伸ばして線形処理、その後 0.0 〜 py に圧縮
			float localX = mapRange(x, 0.0f, px, 0.0f, 1.0f);
			return mapRange(calcLog(localX, p.rate), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			// 後半: px 〜 1.0 の区間を 0.0 〜 1.0 に引き伸ばして指数処理、その後 py 〜 1.0 に圧縮
			float localX = mapRange(x, px, 1.0f, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::Sp1Linear] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].sp1Linear1;
		float px = p.pos.x;
		float py = p.pos.y;

		if (x <= px) {
			// 前半: 0.0 〜 px の区間を 0.0 〜 1.0 に引き伸ばして線形処理、その後 0.0 〜 py に圧縮
			float localX = mapRange(x, 0.0f, px, 0.0f, 1.0f);
			return mapRange(calcSp1(localX, p.cp.x, p.cp.y), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			// 後半: px 〜 1.0 の区間を 0.0 〜 1.0 に引き伸ばして指数処理、その後 py 〜 1.0 に圧縮
			float localX = mapRange(x, px, 1.0f, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::Sp2Linear] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].sp2Linear1;
		float px = p.pos.x;
		float py = p.pos.y;

		if (x <= px) {
			// 前半: 0.0 〜 px の区間を 0.0 〜 1.0 に引き伸ばして線形処理、その後 0.0 〜 py に圧縮
			float localX = mapRange(x, 0.0f, px, 0.0f, 1.0f);
			return mapRange(calcSp2(localX, p.cp1.x, p.cp1.y, p.cp2.x, p.cp2.y), 0.0f, 1.0f, 0.0f, py);
		}
		else {
			// 後半: px 〜 1.0 の区間を 0.0 〜 1.0 に引き伸ばして指数処理、その後 py 〜 1.0 に圧縮
			float localX = mapRange(x, px, 1.0f, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, py, 1.0f);
		}
		};

	logics[CurveParams::Logic::Linear2ArcExp] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear2ArcExp;
		float px1 = p.pos1.x;
		float py1 = p.pos1.y;
		float px2 = p.pos2.x;
		float py2 = p.pos2.y;

		if (x <= px1) {
			float localX = mapRange(x, 0.0f, px1, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, 0.0f, py1);
		}
		else if (x <= px2) {
			float localX = mapRange(x, px1, px2, 0.0f, 1.0f);
			return mapRange(calcArcExp(localX), 0.0f, 1.0f, py1, py2);
		}
		else {
			float localX = mapRange(x, px2, 1.0f, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, py2, 1.0f);
		}
		};

	logics[CurveParams::Logic::Linear2ArcLog] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear2ArcLog;
		float px1 = p.pos1.x;
		float py1 = p.pos1.y;
		float px2 = p.pos2.x;
		float py2 = p.pos2.y;

		if (x <= px1) {
			float localX = mapRange(x, 0.0f, px1, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, 0.0f, py1);
		}
		else if (x <= px2) {
			float localX = mapRange(x, px1, px2, 0.0f, 1.0f);
			return mapRange(calcArcLog(localX), 0.0f, 1.0f, py1, py2);
		}
		else {
			float localX = mapRange(x, px2, 1.0f, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, py2, 1.0f);
		}
		};

	logics[CurveParams::Logic::Linear2Exp] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear2Exp;
		float px1 = p.pos1.x;
		float py1 = p.pos1.y;
		float px2 = p.pos2.x;
		float py2 = p.pos2.y;

		if (x <= px1) {
			float localX = mapRange(x, 0.0f, px1, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, 0.0f, py1);
		}
		else if (x <= px2) {
			float localX = mapRange(x, px1, px2, 0.0f, 1.0f);
			return mapRange(calcExp(localX, p.rate), 0.0f, 1.0f, py1, py2);
		}
		else {
			float localX = mapRange(x, px2, 1.0f, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, py2, 1.0f);
		}
		};

	logics[CurveParams::Logic::Linear2Log] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear2Log;
		float px1 = p.pos1.x;
		float py1 = p.pos1.y;
		float px2 = p.pos2.x;
		float py2 = p.pos2.y;

		if (x <= px1) {
			float localX = mapRange(x, 0.0f, px1, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, 0.0f, py1);
		}
		else if (x <= px2) {
			float localX = mapRange(x, px1, px2, 0.0f, 1.0f);
			return mapRange(calcLog(localX, p.rate), 0.0f, 1.0f, py1, py2);
		}
		else {
			float localX = mapRange(x, px2, 1.0f, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, py2, 1.0f);
		}
		};

	logics[CurveParams::Logic::Linear2Sp1] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear2Sp1;
		float px1 = p.pos1.x;
		float py1 = p.pos1.y;
		float px2 = p.pos2.x;
		float py2 = p.pos2.y;

		if (x <= px1) {
			float localX = mapRange(x, 0.0f, px1, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, 0.0f, py1);
		}
		else if (x <= px2) {
			float localX = mapRange(x, px1, px2, 0.0f, 1.0f);
			return mapRange(calcSp1(localX, p.cp.x, p.cp.y), 0.0f, 1.0f, py1, py2);
		}
		else {
			float localX = mapRange(x, px2, 1.0f, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, py2, 1.0f);
		}
		};

	logics[CurveParams::Logic::Linear2Sp2] = [=](int pIdx, int tIdx, int prmIdx, float x) {
		auto& p = m_params.params[pIdx][tIdx][prmIdx].linear2Sp2;
		float px1 = p.pos1.x;
		float py1 = p.pos1.y;
		float px2 = p.pos2.x;
		float py2 = p.pos2.y;

		if (x <= px1) {
			float localX = mapRange(x, 0.0f, px1, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, 0.0f, py1);
		}
		else if (x <= px2) {
			float localX = mapRange(x, px1, px2, 0.0f, 1.0f);
			return mapRange(calcSp2(localX, p.cp1.x, p.cp1.y, p.cp2.x, p.cp2.y), 0.0f, 1.0f, py1, py2);
		}
		else {
			float localX = mapRange(x, px2, 1.0f, 0.0f, 1.0f);
			return mapRange(calcLinear(localX), 0.0f, 1.0f, py2, 1.0f);
		}
		};
}

void CurveCore::setParameters(const CurveParams& params)
{
	this->m_params = params;

	this->index = this->m_params.enable ? 1 : 0; // カーブモードなら1、従来モードなら0
}

float CurveCore::process(int positionIndex, int targetIndex, int paramIndex, float x)
{
	// xが0.0や1.0に張り付いている時は計算をスキップして即リターン（パフォーマンス最適化）
	if (x <= 0.0001f) return 0.0f;
	if (x >= 0.9999f) return 1.0f;

	int logicIndex = m_params.params[positionIndex][targetIndex][paramIndex].logic;
	auto logic = static_cast<CurveParams::Logic>(logicIndex);

	// 安全装置: マップに登録されていなければ線形を返す
	if (logics.find(logic) == logics.end()) return x;

	return logics[logic](positionIndex, targetIndex, paramIndex, x);
}
