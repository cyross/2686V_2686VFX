#include <array>
#include <cmath>

struct RateLUT {
    std::array<float, 64> baseTime;
    std::array<float, 64> attackBaseTime;

    RateLUT() {
        for (int i = 0; i < 64; ++i) {
            if (i >= 60) {
                baseTime[i] = 0.001f;
                attackBaseTime[i] = 0.001f;
            }
            else {
                float diff = 60.0f - (float)i;
                float timeFactor = std::pow(2.0f, diff / 4.0f);
                float baseTimeValue = 0.0003f * timeFactor;

                baseTime[i] = std::min(baseTimeValue, 15.0f) * 1.5f;
                attackBaseTime[i] = std::min(baseTimeValue * 0.33f, 15.0f) * 1.5f;
            }
        }
    }

    // シングルトンとしてアクセス
    static const RateLUT& get() {
        static RateLUT instance; // 初回アクセス時にコンストラクタが走り、以降キャッシュされる

        return instance;
    }
};
