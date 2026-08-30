#include "doctest/doctest.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "TestHelpers.h"

// ============================================================================
// プロジェクト間のズレ検出
// ============================================================================
// このリポジトリは 4 つのプラグインがソースツリーを丸ごと複製している。
// 共有ライブラリになっていないため、片方だけ直して他方が取り残される事故が
// 起きやすい (実際に OPZX7S のエンベロープ 5 本が古い実装のまま残っていた)。
//
// 音を決めている Effect と Generator は、同じ機能セットのプロジェクト同士で
// 完全に一致しているべきなので、そこを機械的に突き合わせる。
//
// カーブ編集の有無で実装が分かれるため、2 つの組に分けて比較する。
//   Curve 組    : 2686V / OPZX7S
//   非 Curve 組 : 2686VLight / 2686VOrigin

namespace
{
    namespace fs = std::filesystem;

    struct Group
    {
        const char* label;
        std::vector<std::string> projects;   // 先頭が基準
    };

    // 意図的に違えているファイル。増やすときは理由を添えること。
    const std::vector<std::string> kAllowed = {
        // 2686VOrigin の LFO は波形が 0〜7 のみ。Light は 8〜12 の
        // Sample & Hold 系を持つため、波形数まわりが異なる。
        "Effect/Lfo/Opzx7/LfoOpzx7Unit.h",
        "Effect/Lfo/Opzx7/LfoOpzx7Unit.cpp",
    };

    bool isAllowed(const std::string& rel)
    {
        for (const auto& a : kAllowed) {
            if (rel == a) return true;
        }

        return false;
    }

    std::string toRel(const fs::path& p, const fs::path& base)
    {
        std::string s = fs::relative(p, base).generic_string();
        return s;
    }

    // 基準プロジェクトに在って比較先にも在るファイルを、正規化して突き合わせる
    void compareTree(const Group& g, const std::string& subDir)
    {
        fs::path root = fs::path(repoRoot());
        fs::path base = root / g.projects[0] / "Source" / subDir;

        REQUIRE_MESSAGE(fs::exists(base), "基準ディレクトリが無い: " << base.generic_string());

        int compared = 0;

        for (const auto& entry : fs::recursive_directory_iterator(base)) {
            if (!entry.is_regular_file()) continue;

            std::string ext = entry.path().extension().generic_string();
            if (ext != ".h" && ext != ".cpp") continue;

            std::string rel = subDir + "/" + toRel(entry.path(), base);
            if (isAllowed(rel)) continue;

            std::string refText;
            REQUIRE(readNormalized(entry.path().generic_string(), refText));

            for (size_t i = 1; i < g.projects.size(); ++i) {
                fs::path other = root / g.projects[i] / "Source" / rel;

                // 片方にしか無いファイルは対象外 (チャンネル構成が違うため)
                if (!fs::exists(other)) continue;

                std::string otherText;
                REQUIRE(readNormalized(other.generic_string(), otherText));

                ++compared;

                INFO(g.label << " : " << rel << " が " << g.projects[0]
                     << " と " << g.projects[i] << " でずれている");
                CHECK(refText == otherText);
            }
        }

        // 比較が 0 件なら、パスの指定ミスを疑う
        CHECK(compared > 0);
    }

    const Group kCurveGroup{ "Curve 組", { "2686V", "OPZX7S" } };
    const Group kPlainGroup{ "非 Curve 組", { "2686VLight", "2686VOrigin" } };
}

// プロジェクト間のズレ : Effect (Curve 組)
TEST_CASE("Project drift: Effect (curve group)")
{
    compareTree(kCurveGroup, "Effect");
}

// プロジェクト間のズレ : Effect (非 Curve 組)
TEST_CASE("Project drift: Effect (non-curve group)")
{
    compareTree(kPlainGroup, "Effect");
}

// プロジェクト間のズレ : Generator (Curve 組)
TEST_CASE("Project drift: Generator (curve group)")
{
    compareTree(kCurveGroup, "Generator");
}

// プロジェクト間のズレ : Generator (非 Curve 組)
TEST_CASE("Project drift: Generator (non-curve group)")
{
    compareTree(kPlainGroup, "Generator");
}

// ソースの書式 : BOM 付き UTF-8 と CRLF
TEST_CASE("Source format: UTF-8 with BOM and CRLF")
{
    // 全プロジェクトの Source 配下は BOM 付き UTF-8 / CRLF で統一する規約。
    // git は LF で保管するので diff では分からず、バイトを見るしかない。
    namespace fs = std::filesystem;

    fs::path root = fs::path(repoRoot());

    int checked = 0;

    for (const char* proj : { "2686V", "2686VLight", "2686VOrigin", "OPZX7S" }) {
        fs::path base = root / proj / "Source";

        REQUIRE(fs::exists(base));

        for (const auto& entry : fs::recursive_directory_iterator(base)) {
            if (!entry.is_regular_file()) continue;

            std::string ext = entry.path().extension().generic_string();
            if (ext != ".h" && ext != ".cpp") continue;

            std::ifstream ifs(entry.path(), std::ios::binary);
            REQUIRE(ifs.good());

            std::string raw((std::istreambuf_iterator<char>(ifs)), std::istreambuf_iterator<char>());

            ++checked;

            INFO(entry.path().generic_string());

            bool hasBom = raw.size() >= 3
                && (unsigned char)raw[0] == 0xEF
                && (unsigned char)raw[1] == 0xBB
                && (unsigned char)raw[2] == 0xBF;

            CHECK(hasBom);

            int bareLf = 0;

            for (size_t i = 0; i < raw.size(); ++i) {
                if (raw[i] == '\n' && (i == 0 || raw[i - 1] != '\r')) ++bareLf;
            }

            CHECK(bareLf == 0);
        }
    }

    CHECK(checked > 1000);
}
