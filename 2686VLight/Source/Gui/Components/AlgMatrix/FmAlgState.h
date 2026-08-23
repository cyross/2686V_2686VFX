#pragma once
#include <array>
#include <vector>

// マトリックスの状態を保持し、動的に有効なオペレータを算出する構造体
struct FmAlgState {
    int numOps = 8;
    std::array<bool, 8> isCarrier{ false }; // キャリアの有効/無効状態
    std::array<std::array<bool, 8>, 8> mod{ false }; // モジュレーションの有効/無効状態 [src][dest]
    std::array<std::array<bool, 8>, 8> fbMod{ false }; // フィードバックモジュレーションの有効/無効状態 [src][dest]

    // ルーティングを走査し、出力に到達可能(有効)なオペレータの一覧を返す
    std::vector<bool> getActiveOperators() const {
        std::vector<bool> active(numOps, false);

        // 1. キャリアをアクティブにする
        for (int i = 0; i < numOps; ++i) {
            if (isCarrier[i]) {
                active[i] = true;
            }
        }

        // 2. アクティブなオペレータに接続されているモジュレータをアクティブにする（逆算）
        bool changed = true;
        while (changed) {
            changed = false;

            // 全オペレータ(dest)について確認
            for (int dest = 0; dest < numOps; ++dest) {
                if (!active[dest]) continue; // 接続先が非アクティブならスキップ

                // その dest に接続している src (モジュレータ) を探す
                for (int src = 0; src < numOps; ++src) {
                    if (active[src]) continue; // すでにアクティブならスキップ

                    // 接続元の配列インデックスに注意: mod[src][dest]
                    if (mod[src][dest] || fbMod[src][dest]) {
                        active[src] = true;
                        changed = true; // 新たにアクティブになったオペレータがいればもう一周
                    }
                }
            }
        }
        return active;
    }
};

// 互換性・移行用
using AlgMatrixState = FmAlgState;
