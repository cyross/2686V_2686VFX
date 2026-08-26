#pragma once

#include <algorithm>
#include <cmath>

// ユニゾン・ハーモニーの共通計算
// 以前は同じ式が全シンセコアに重複していたため、ここへ集約している。
// SynthCore が本体を持ち、各コアは m_unison 経由で参照する。
struct UnisonState {
    // paraDetuneCents / paraDistance はこのボイス専用の加算値。
    // ボイス0はメインなので常に 0 が渡される。
    void setParams(int index, int total, float detuneCents, float spread,
                   float paraDetuneCents = 0.0f, float paraDistance = 0.0f) {
        m_index = index;
        m_total = total;
        m_detuneCents = detuneCents;
        m_spread = spread;
        m_paraDetuneCents = paraDetuneCents;
        m_paraDistance = paraDistance;
    }

    bool isActive() const { return m_total > 1; }
    int getIndex() const { return m_index; }
    int getTotal() const { return m_total; }

    // 現在のボイスが全体のどこに配置されるかを -1.0(一番下) 〜 1.0(一番上) で算出
    // 例(3ボイス): -1.0,  0.0,  1.0
    // 例(4ボイス): -1.0, -0.33, 0.33, 1.0
    float getSpreadPos() const {
        if (m_total <= 1) return 0.0f;
        return ((float)m_index / (float)(m_total - 1)) * 2.0f - 1.0f;
    }

    // 最大デチューン幅に位置を掛け合わせて、このボイスのズレ量(セント)を周波数へ反映する。
    // ボイス単位の Para-Detune はその上に加算される。
    // (1200セント ＝ 1オクターブ ＝ 周波数2倍)
    float applyDetune(float freq) const {
        if (m_total <= 1) return freq;
        const float centOffset = getSpreadPos() * m_detuneCents + m_paraDetuneCents;
        if (centOffset == 0.0f) return freq;
        return freq * std::pow(2.0f, centOffset / 1200.0f);
    }

    // ボイスインデックスに応じて位相を均等に散らす (例: 3ボイスなら 0.0, 0.33, 0.66)
    float getPhaseOffset() const {
        if (m_total <= 1) return 0.0f;
        return (float)m_index / (float)m_total;
    }

    // spreadPos が -1(L) の時、Right 側の音量を下げる。逆も然り。
    // ボイス単位の Para-Distance はその上に加算される。
    void applyPan(float& panL, float& panR) const {
        if (m_total <= 1) return;
        const float panOffset = (getSpreadPos() * m_spread + m_paraDistance) * 0.5f; // 最大で ±0.5 動く
        panL = std::clamp(panL - panOffset, 0.0f, 1.0f);
        panR = std::clamp(panR + panOffset, 0.0f, 1.0f);
    }

    // 音量補正 (ボイス数が増えると爆音になるため下げる)
    float getGainComp() const {
        if (m_total <= 1) return 1.0f;
        return 1.0f / std::sqrt((float)m_total);
    }

private:
    int m_index = 0;
    int m_total = 1;
    float m_detuneCents = 0.0f;
    float m_spread = 0.0f;
    float m_paraDetuneCents = 0.0f;
    float m_paraDistance = 0.0f;
};
