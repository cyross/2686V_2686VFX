#pragma once

// ============================================================================
// テスト用の JuceHeader 代替
// ============================================================================
// Effect / Generator / Advanced が JUCE から直接使うのは MathConstants と
// String だけ。String は定数宣言 (ConstGlobal.h) に出てくるだけで、音の
// 計算には関わらないので std::string を包んだだけの Fake で足りる。
//
// 本物の JuceHeader は標準ライブラリを芋づるで引き込むため、それに頼って
// いる箇所が動くよう、よく使うものもここで揃えておく。
//
// テスト対象のソースは出荷されるものと同一ファイルを指しており、
// 差し替えているのはこのヘッダだけ。

#include <algorithm>
#include <array>
#include <cmath>
#include <cstdint>
#include <cstring>
#include <functional>
#include <limits>
#include <map>
#include <memory>
#include <string>
#include <vector>

namespace juce
{
    template <typename Type>
    struct MathConstants
    {
        static constexpr Type pi = static_cast<Type>(3.141592653589793238L);
        static constexpr Type twoPi = static_cast<Type>(2 * 3.141592653589793238L);
        static constexpr Type halfPi = static_cast<Type>(3.141592653589793238L / 2);
        static constexpr Type euler = static_cast<Type>(2.71828182845904523536L);
        static constexpr Type sqrt2 = static_cast<Type>(1.4142135623730950488L);
    };

    // 定数宣言に出てくるぶんだけの Fake
    class String
    {
        std::string s;
    public:
        String() = default;
        String(const char* v) : s(v != nullptr ? v : "") {}
        String(const std::string& v) : s(v) {}

        const std::string& toStdString() const { return s; }
        const char* toRawUTF8() const { return s.c_str(); }
        bool isEmpty() const { return s.empty(); }
        bool isNotEmpty() const { return !s.empty(); }
        int length() const { return (int)s.size(); }

        String operator+(const String& o) const { return String(s + o.s); }
        String& operator+=(const String& o) { s += o.s; return *this; }
        bool operator==(const String& o) const { return s == o.s; }
        bool operator!=(const String& o) const { return s != o.s; }
        bool operator<(const String& o) const { return s < o.s; }
    };
}

// JUCE が生成する ProjectInfo の代わり
namespace ProjectInfo
{
    static constexpr const char* projectName = "RetroSynthsTests";
    static constexpr const char* versionString = "0.0.0";
}
