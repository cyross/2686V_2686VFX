// ============================================================================
// 音声・波形ファイルの場所をどこからの相対で書くか (Io::PathRoot)
// ============================================================================
// 3.6.3 で、相対パスの基準を「設定の置き場」と「読み書きしているファイルの
// 置き場」から選べるようにした。
//
// 見ているのは次の 3 つ。
//   - 設定の置き場を選んでいるときは、これまでと同じ形で書く
//   - ファイルの置き場を選ぶと、そのファイルからの相対で書き、読み戻せる
//   - もう一方の設定で書いたファイルも読める
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include "Shared/Core/Io/IoPathRoot.h"

namespace
{
    // 使い終わったら消える置き場
    struct TempTree
    {
        juce::File root;

        TempTree()
            : root(juce::File::getSpecialLocation(juce::File::tempDirectory)
                .getNonexistentChildFile("pathRootTest", "", false))
        {
            root.createDirectory();
        }

        ~TempTree() { root.deleteRecursively(); }

        juce::File add(const juce::String& relative)
        {
            auto file = root.getChildFile(relative);

            file.create();

            return file;
        }
    };

    constexpr int kSettings = Io::PathRoot::settingsFolder;
    constexpr int kDocument = Io::PathRoot::documentFolder;
}

TEST_CASE("相対パスの基準: 設定の置き場では、これまでと同じ形で書く")
{
    TempTree tree;

    const auto samples = tree.root.getChildFile("Samples");
    const auto sample = tree.add("Samples/Kick/bd.wav");
    const auto preset = tree.root.getChildFile("Presets/a.2686v.json");

    // プリセットは設定の置き場からの相対
    CHECK(Io::toStoredPath(sample.getFullPathName(), kSettings, preset, samples.getFullPathName(), true)
        == juce::String("Kick") + juce::File::getSeparatorString() + "bd.wav");

    // チャンネルパラメータは絶対パス
    CHECK(Io::toStoredPath(sample.getFullPathName(), kSettings, preset, samples.getFullPathName(), false)
        == sample.getFullPathName());

    // 空は空のまま
    CHECK(Io::toStoredPath({}, kSettings, preset, samples.getFullPathName(), true).isEmpty());
    CHECK(Io::toStoredPath("(Empty)", kSettings, preset, samples.getFullPathName(), true).isEmpty());
}

TEST_CASE("相対パスの基準: ファイルの置き場を選ぶと、そのファイルからの相対で書いて読み戻せる")
{
    TempTree tree;

    const auto samples = tree.root.getChildFile("Samples");
    const auto sample = tree.add("Kit/bd.wav");
    const auto document = tree.root.getChildFile("Kit/Presets/kit.param.rhythm.json");

    const auto stored = Io::toStoredPath(sample.getFullPathName(), kDocument, document, samples.getFullPathName(), false);

    CHECK_FALSE(juce::File::isAbsolutePath(stored));
    CHECK(Io::fromStoredPath(stored, kDocument, document, samples.getFullPathName()) == sample);

    // フォルダごと移しても読める
    const auto moved = tree.root.getChildFile("Moved");

    tree.root.getChildFile("Kit").copyDirectoryTo(moved);

    const auto movedDocument = moved.getChildFile("Presets/kit.param.rhythm.json");

    CHECK(Io::fromStoredPath(stored, kDocument, movedDocument, samples.getFullPathName())
        == moved.getChildFile("bd.wav"));
}

TEST_CASE("相対パスの基準: ファイルを伴わない保存は、選んでいても設定の置き場を使う")
{
    TempTree tree;

    const auto samples = tree.root.getChildFile("Samples");
    const auto sample = tree.add("Samples/bd.wav");

    // DAW のプロジェクトへ書くとき
    CHECK(Io::toStoredPath(sample.getFullPathName(), kDocument, juce::File(), samples.getFullPathName(), true)
        == "bd.wav");
    CHECK(Io::fromStoredPath("bd.wav", kDocument, juce::File(), samples.getFullPathName()) == sample);
}

TEST_CASE("相対パスの基準: もう一方の設定で書いたファイルも読める")
{
    TempTree tree;

    const auto samples = tree.root.getChildFile("Samples");
    const auto inSamples = tree.add("Samples/sd.wav");
    const auto inKit = tree.add("Kit/hh.wav");
    const auto document = tree.root.getChildFile("Kit/kit.param.rhythm.json");

    // 設定の置き場で書いたものを、ファイルの置き場を選んで読む
    CHECK(Io::fromStoredPath("sd.wav", kDocument, document, samples.getFullPathName()) == inSamples);

    // ファイルの置き場で書いたものを、設定の置き場を選んで読む
    CHECK(Io::fromStoredPath("hh.wav", kSettings, document, samples.getFullPathName()) == inKit);

    // 絶対パスはそのまま
    CHECK(Io::fromStoredPath(inKit.getFullPathName(), kSettings, document, samples.getFullPathName()) == inKit);
}
