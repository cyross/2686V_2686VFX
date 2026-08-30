#pragma once

#include <string>
#include <vector>
#include <fstream>

// テストから見たリポジトリのルート (CMake が定義する)
inline std::string repoRoot() { return std::string(TEST_SOURCE_ROOT); }

// 4 プラグインで複製されているファイルを読み比べるためのヘルパ。
// 改行の違いは無視する (リポジトリは LF で保管し、作業ツリーは CRLF)。
inline bool readNormalized(const std::string& path, std::string& out)
{
    std::ifstream ifs(path, std::ios::binary);
    if (!ifs) return false;

    std::string raw((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

    // BOM を落とす
    if (raw.size() >= 3 && (unsigned char)raw[0] == 0xEF && (unsigned char)raw[1] == 0xBB && (unsigned char)raw[2] == 0xBF) {
        raw.erase(0, 3);
    }

    out.clear();
    out.reserve(raw.size());

    for (char c : raw) {
        if (c != '') out.push_back(c);
    }

    return true;
}
