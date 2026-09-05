#include "./GenPcmShared.h"

#include "../../../Core/Synth/SynthHelpers.h"

void PcmSharedStore::setSource(const std::vector<float>& src, double rate)
{
    m_master.raw = src;
    m_master.sourceRate = rate;

    // 素材が変わったので、符号化はやり直しになる
    m_master.encoded.clear();
    m_master.qualityMode = -1;
    m_master.rateIndex = -1;

    publish();
}

void PcmSharedStore::clear()
{
    m_master = PcmSharedData{};

    publish();
}

void PcmSharedStore::rebuildIfNeeded(int qualityMode, int rateIndex, double defaultRate)
{
    if (m_master.raw.empty()) return;

    // まだ一度も指定を受け取っていない (音源が ADPCM になったことがない)。
    // 最初に ADPCM のブロックが回ったときに改めて頼まれるので、ここでは作らない。
    if (qualityMode < 0 || rateIndex < 0) return;

    if (m_master.qualityMode == qualityMode && m_master.rateIndex == rateIndex) return;

    double targetRate = getTargetRate(rateIndex, defaultRate);

    // 素材より上へは引き伸ばさない
    if (targetRate > m_master.sourceRate) targetRate = m_master.sourceRate;

    double step = (targetRate > 0.0) ? (m_master.sourceRate / targetRate) : 1.0;
    if (step <= 0.0) step = 1.0;

    // 圧縮の種類ごとの処理は GenPcmHelper に集約している
    GenPcmHelper::encodeBuffer(m_master.raw, step, qualityMode, m_master.encoded);

    m_master.encodedRate = targetRate;
    m_master.qualityMode = qualityMode;
    m_master.rateIndex = rateIndex;

    publish();
}

void PcmSharedStore::publish()
{
    m_slots[(size_t)m_writeSlot] = m_master;

    // 書き終えた枠を差し出し、代わりに前に差し出されていた枠を受け取る。
    // 受け取った枠が次に書く枠になる。
    m_writeSlot = m_ready.exchange(m_writeSlot, std::memory_order_acq_rel);
    m_dirty.store(true, std::memory_order_release);
}
