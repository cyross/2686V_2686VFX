#pragma once

#include <type_traits>

#include "./StageProbe.h"
#include "./SynthCoreParams.h"

// StageSwitch の続き。音源の値の写しを書き換える (音源の値の型を知っている側だけが使う)
namespace StageSwitch
{
	inline void lfoOff(LfoN88Params& p) { p.pm = false; p.am = false; }
	inline void lfoOff(LfoN88OpParams& p) { p.ams = 0.0f; }
	inline void lfoOff(LfoOplParams& p) { p.pm = false; p.am = false; }
	inline void lfoOff(LfoOpmParams& p) { p.pm = false; p.am = false; }
	inline void lfoOff(LfoOpmOpParams& p) { p.amsEnable = false; }
	inline void lfoOff(LfoOpnaParams& p) { p.pm = false; p.am = false; }
	inline void lfoOff(LfoOpzx7Params& p) { p.pmEnable = false; p.amEnable = false; }

	// off に立っているまとまりを切る。p が持っているまとまりを返す
	template <typename P>
	Mask apply(P& p, Mask off)
	{
		Mask has = 0;

		const auto on = [&](int m) { has |= bit(m); return (off & bit(m)) != 0; };

		if constexpr (requires { p.ampEnvG.bypass; }) { if (on(ampEnv)) p.ampEnvG.bypass = true; }
		if constexpr (requires { p.adsr.bypass; }) { if (on(ampEnv)) p.adsr.bypass = true; }

		if constexpr (requires { p.m_adsrParams.bypass; }) { if (on(opEnv)) p.m_adsrParams.bypass = true; }

		if constexpr (requires { p.pitchAdsr.bypass; }) { if (on(pitchEnv)) p.pitchAdsr.bypass = true; }
		if constexpr (requires { p.pitchEnvEnable; }) { if (on(pitchEnv)) p.pitchEnvEnable = false; }

		if constexpr (requires { lfoOff(p.glLfo); }) { if (on(lfo)) lfoOff(p.glLfo); }
		if constexpr (requires { lfoOff(p.lfo); }) { if (on(lfo)) lfoOff(p.lfo); }
		if constexpr (requires { lfoOff(p.n88Lfo); }) { if (on(lfo)) lfoOff(p.n88Lfo); }
		if constexpr (requires { lfoOff(p.hwLfo); }) { if (on(lfo)) lfoOff(p.hwLfo); }

		if constexpr (requires { p.ssgHwEnv.enable; }) { if (on(ssgHwEnv)) p.ssgHwEnv.enable = false; }
		if constexpr (requires { p.ssgHwPEnv.enable; }) { if (on(ssgHwEnv)) p.ssgHwPEnv.enable = false; }

		// SSG は自分のハードウェアエンベロープを env の名前で持つ
		if constexpr (requires { p.env; }) {
			if constexpr (std::is_same_v<std::remove_cvref_t<decltype(p.env)>, SsgHwEnvParams>) { if (on(ssgHwEnv)) p.env.enable = false; }
		}

		if constexpr (requires { p.ssgSwEnv256.bypass; }) { if (on(ssgSwEnv)) p.ssgSwEnv256.bypass = true; }
		if constexpr (requires { p.ssgSwEnv256g.bypass; }) { if (on(ssgSwEnv)) p.ssgSwEnv256g.bypass = true; }
		if constexpr (requires { p.ssgSwPEnv256.bypass; }) { if (on(ssgSwEnv)) p.ssgSwPEnv256.bypass = true; }
		if constexpr (requires { p.ssgSwPEnv256g.bypass; }) { if (on(ssgSwEnv)) p.ssgSwPEnv256g.bypass = true; }
		if constexpr (requires { p.ssgEnv256Enable; }) { if (on(ssgSwEnv)) p.ssgEnv256Enable = false; }
		if constexpr (requires { p.ssgPEnv256Enable; }) { if (on(ssgSwEnv)) p.ssgPEnv256Enable = false; }

		if constexpr (requires { p.wtMod.enable; }) { if (on(wtMod)) p.wtMod.enable = false; }
		if constexpr (requires { p.wtAmpMod.enable; }) { if (on(wtMod)) p.wtAmpMod.enable = false; }

		// 波形メモリの音源は WT MOD を mod の名前で持つ
		if constexpr (requires { p.mod; }) {
			if constexpr (std::is_same_v<std::remove_cvref_t<decltype(p.mod)>, WtModParams>) { if (on(wtMod)) p.mod.enable = false; }
		}

		if constexpr (requires { p.algEnvEnable; }) { if (on(stepEnv)) p.algEnvEnable = false; }
		if constexpr (requires { p.tlEnvEnable; }) { if (on(stepEnv)) p.tlEnvEnable = false; }
		if constexpr (requires { p.dutyEnvEnable; }) { if (on(stepEnv)) p.dutyEnvEnable = false; }
		if constexpr (requires { p.peakEnvEnable; }) { if (on(stepEnv)) p.peakEnvEnable = false; }

		if constexpr (requires { p.paramMod.envCount; }) {
			if (on(paramEnv)) {
				for (auto& env : p.paramMod.envs) env.enable = false;

				p.paramMod.envCount = 0;
			}
		}

		// オペレーター・パッドの中も同じように見る
		if constexpr (requires { p.op.size(); }) { for (auto& op : p.op) has |= apply(op, off); }
		if constexpr (requires { p.pads.size(); }) { for (auto& pad : p.pads) has |= apply(pad, off); }

		return has;
	}

	// その音源が持っているまとまり
	template <typename P>
	Mask present()
	{
		P p{};

		return apply(p, 0);
	}

	// 全部の音源の値へ当てる
	inline void applyAll(SynthCoreParams& params, Mask off)
	{
		if (off == 0) return;

		apply(params.opna, off);
		apply(params.opn, off);
		apply(params.opl, off);
		apply(params.opl3, off);
		apply(params.opm, off);
		apply(params.opzx7, off);
		apply(params.ssg, off);
		apply(params.wt, off);
		apply(params.wt2, off);
		apply(params.wtPlus, off);
		apply(params.rhythm, off);
		apply(params.adpcm, off);
		apply(params.adpcmPlus, off);
		apply(params.beep, off);
	}
}
