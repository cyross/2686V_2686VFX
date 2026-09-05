#pragma once

#include <algorithm>
#include <vector>

// 書かれていた FX の並びを、このプラグインで使える並びへならす。
//
// 効果の数はプラグインごとに違う (2686VFX だけ PCM ビットクラッシャーがあり
// 1 つ多い)。番号をそのまま流し込むと、多いほうで書いた並びを少ないほうで
// 読んだときに、範囲外の番号だけが弾かれてその位置が更新されず、ある効果が
// 処理から抜け落ちて別の効果が二重に掛かる。
//
// ここで
//   - 範囲外の番号を落とす
//   - 同じものが二度出てきたら後のほうを捨てる
//   - 書かれていなかったものを後ろへ足す
// をやり、必ず 0 から effectSize-1 が 1 回ずつ並んだ形にして返す。
inline std::vector<int> normalizeFxOrder(const std::vector<int>& ids, int effectSize)
{
    std::vector<int> out;

    if (effectSize <= 0) return out;

    out.reserve((size_t)effectSize);

    for (int id : ids)
    {
        if (id < 0 || id >= effectSize) continue;
        if (std::find(out.begin(), out.end(), id) != out.end()) continue;

        out.push_back(id);
    }

    for (int i = 0; i < effectSize; ++i)
    {
        if (std::find(out.begin(), out.end(), i) == out.end()) out.push_back(i);
    }

    return out;
}
