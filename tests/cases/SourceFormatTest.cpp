#include "doctest/doctest.h"

#include <filesystem>
#include <fstream>
#include <string>
#include <vector>

#include "TestHelpers.h"

// ============================================================================
// ソースの書式
// ============================================================================
// ここには以前、プラグイン間でソースのコピーがずれていないかを突き合わせる
// テストがあった (Effect と Generator)。どちらも Shared/ へ 1 つにまとめた
// ので、ずれようがなくなり、外した。

// ソースの書式 : BOM 付き UTF-8 と CRLF
TEST_CASE("Source format: UTF-8 with BOM and CRLF")
{
    // 全プロジェクトの Source 配下は BOM 付き UTF-8 / CRLF で統一する規約。
    // git は LF で保管するので diff では分からず、バイトを見るしかない。
    namespace fs = std::filesystem;

    fs::path root = fs::path(repoRoot());

    int checked = 0;

    // 12 本で共有するコード (Shared/) も同じ規約
    std::vector<fs::path> bases = { root / "Shared" };

    for (const char* proj : { "2686V", "2686VLight", "2686VOrigin", "OPZX7S" }) {
        bases.push_back(root / proj / "Source");
    }

    for (const auto& base : bases) {

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
