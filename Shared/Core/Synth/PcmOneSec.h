#pragma once

#include <algorithm>
#include <cmath>

#include "Shared/Core/Processor/ProcessorValues.h"

// ============================================================================
// PCM の 1 秒モード (4.5.0)
// ============================================================================
// 再生区間 (P.OFFSET・P.RATIO で切り出したところ) を、長さ 1 秒の素材に
// まとめてから鳴らす。鳴らす速さはいつもどおり (キー・等速) で決まり、
// 1 秒の素材の上の位置 (仮の位置) を、ここで元の素材の位置へ移す。
//
//   区間が 1 秒より長いとき
//     間引き   : 区間全体を 1 秒へ縮める (間を置いて拾う)
//     スライス : 頭から 1 秒だけ使う
//   区間が 1 秒より短いとき
//     拡張     : 区間全体を 1 秒へ引き伸ばす
//     繰り返し : 区間の終わりまで来たら頭へ戻る
//     無音     : 区間の終わりから 1 秒までは鳴らさない
//
// 長さはどれも素材の標本の数で渡す。1 秒は素材の標本化周波数そのもの。
namespace PcmOneSec
{
    struct Fit
    {
        // 仮の位置 (0 〜 oneSecond) 1 つあたりに、元の素材を何標本進むか
        double scale = 1.0;

        // 区間の終わりまで来たら頭へ戻るか (繰り返し)
        bool repeat = false;

        // 区間の終わりから先は鳴らさないか (無音)
        bool silence = false;
    };

    // 区間の長さ (playSize) と 1 秒の長さ (oneSecond) から、まとめ方を決める
    inline Fit fit(double playSize, double oneSecond, int longFit, int shortFit)
    {
        Fit f;

        if (playSize <= 0.0 || oneSecond <= 0.0) return f;

        if (playSize > oneSecond) {
            if (longFit == CPV::Pcm::OneSecLong::decimate) f.scale = playSize / oneSecond;
        }
        else if (playSize < oneSecond) {
            if (shortFit == CPV::Pcm::OneSecShort::stretch)     f.scale = playSize / oneSecond;
            else if (shortFit == CPV::Pcm::OneSecShort::repeat) f.repeat = true;
            else                                                f.silence = true;
        }

        return f;
    }

    // 仮の位置 (区間の頭からの標本数) を、区間の頭からの元の素材の標本数へ移す。
    // 鳴らさないところ (無音) では silent を立てる
    inline double map(const Fit& f, double virtualPos, double playSize, bool& silent)
    {
        silent = false;

        const double pos = virtualPos * f.scale;

        if (f.repeat && playSize > 0.0) return std::fmod(pos, playSize);

        if (f.silence && pos >= playSize) {
            silent = true;
            return std::max(0.0, playSize - 1.0);
        }

        return pos;
    }
}
