#pragma once

// ============================================================================
// PCM を鳴らすときに声ごとに掛けるノイズ対策
// ============================================================================
// QUALITY のノイズリダクション設定のうち、再生のときに掛けるもの。
// 素材を作るとき (符号化の前) に掛ける「きれいな間引き」は
// GenPcmHelper::encodeBuffer の側にある。
//
// どれも既定では切れていて、切れているときは入ってきた値をそのまま返す。
// 既存のパッチの鳴りを変えないため。
//
//   無音ゲート  直流のずれを取り除いたうえで、小さい音を 0 まで絞る。
//               1-bit DPCM などは、音が止んでも出力が 0 へ戻らず、
//               ずれたまま止まる (-36dBFS)。先にずれを取らないと、
//               ゲートはそれを音と見て開いたままになる。
//   高域カット  素材の標本化周波数から見て高すぎる成分 (補間の折り返し、
//               粗い量子化のシャリシャリ感) を削る。段階を 3 つ持つ。
class PcmNoiseReducer
{
public:
    // 高域カットの段階。QUALITY の選択肢の並び (1 から) と同じ。
    enum LpfLevel { lpfOff = 1, lpfLight = 2, lpfMedium = 3, lpfStrong = 4 };

    // 鳴らし始めに整える。
    //
    // sampleRate    : 出力 (ホスト) の標本化周波数
    // contentRatio  : 素材の 1 標本が、出力の何標本ぶんの速さで進むか。
    //                 素材の帯域の上端は、出力のナイキストにこれを掛けた
    //                 ところへ来る。音程が上がるほど上端も上がる。
    void setup(double sampleRate, double contentRatio, bool gate, float gateDb, int lpfLevel);

    // 状態を捨てる。鳴らし始めに呼ぶ。
    void reset();

    float process(float x);

    bool isActive() const { return m_gate || m_lpf; }

private:
    // ---------------- 無音ゲート ----------------
    bool m_gate = false;

    // 直流を取る 1 次の高域通過 (約 10Hz)
    float m_dcR = 0.999f;
    float m_dcX1 = 0.0f;
    float m_dcY1 = 0.0f;

    float m_threshold = 0.001f;   // これを下回ると閉じる (振幅)
    float m_envelope = 0.0f;      // 包絡 (ピーク)
    float m_envRelease = 0.999f;  // 包絡の戻り
    float m_gain = 1.0f;
    float m_gainOpen = 0.1f;      // 開くときの速さ
    float m_gainClose = 0.001f;   // 閉じるときの速さ
    bool m_open = true;

    // ---------------- 高域カット (2 次の低域通過) ----------------
    bool m_lpf = false;
    float m_b0 = 1.0f, m_b1 = 0.0f, m_b2 = 0.0f, m_a1 = 0.0f, m_a2 = 0.0f;
    float m_z1 = 0.0f, m_z2 = 0.0f;
};
