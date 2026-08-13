#pragma once

// マトリックスの状態を保持する構造体
struct AlgMatrixState {
	std::array<bool, 8> isCarrier{ false }; // キャリアの有効/無効状態
	std::array<std::array<bool, 8>, 8> mod{ false }; // モジュレーションの有効/無効状態
	std::array<std::array<bool, 8>, 8> fbMod{ false }; // フィードバックモジュレーションの有効/無効状態
};
