#pragma once

double getTargetRate(int index, double defaultValue = 55500.0f);
float getTargetBitDepth(int index);
float getTargetMaxVal(int index);

// 量子化 (ビットクラッシュ)。
// バイポーラのまま丸めるので 0.0 は必ず 0.0 のままになり、
// getTargetBitDepth() の ±N という定義どおり 2N+1 段階になる。
// steps <= 0 のときは量子化しない (Raw)。
// 入力のクランプは呼び出し側の方針に任せるため、ここでは行わない。
float quantizeSample(float input, float steps);
