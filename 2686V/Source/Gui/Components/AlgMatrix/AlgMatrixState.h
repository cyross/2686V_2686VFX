#pragma once

#include "../../../Processor/Opzx7/ProcessorOpzx7Values.h"

// マトリックスの状態を保持する構造体
struct AlgMatrixState {
	std::array<bool, Opzx7PrValue::ops> isCarrier{ false }; // キャリアの有効/無効状態
	std::array<std::array<bool, Opzx7PrValue::ops>, Opzx7PrValue::ops> mod{ false }; // モジュレーションの有効/無効状態
	std::array<std::array<bool, Opzx7PrValue::ops>, Opzx7PrValue::ops> fbMod{ false }; // フィードバックモジュレーションの有効/無効状態
};
