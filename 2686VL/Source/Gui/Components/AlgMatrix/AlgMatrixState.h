#pragma once

// マトリックスの状態を保持する構造体
struct AlgMatrixState {
    std::array<bool, 8> isCarrier{ false };
    std::array<std::array<bool, 8>, 8> mod{ false };
};
