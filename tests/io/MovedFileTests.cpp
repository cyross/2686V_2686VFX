// ============================================================================
// 相対パスの基準がずれたファイルを、名前で探し直す (Io::findMovedFile)
// ============================================================================
// 3.6.0 までは、波形を読み込むたびに基準の置き場 (波形フォルダ) が読み込んだ
// ファイルのフォルダへ書き換わっていた。そのころ保存したものは、相対パスが
// ずれた基準からのものになっている。読み直すときに名前で探して救う。
//
// 見ているのは、救えるものを救うことと、取り違えそうなときに手を出さないこと。
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include "Shared/Core/Io/IoMovedFile.h"

namespace
{
    // 使い終わったら消える置き場
    struct TempTree
    {
        juce::File root;

        TempTree()
            : root(juce::File::getSpecialLocation(juce::File::tempDirectory)
                .getNonexistentChildFile("movedFileTest", "", false))
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
}

TEST_CASE("移ったファイル: 下のフォルダから読んで名前だけで保存したものを見つける")
{
    TempTree tree;

    const auto wave = tree.add("N88/square.wt");

    CHECK(Io::findMovedFile(tree.root, "square.wt") == wave);
}

TEST_CASE("移ったファイル: 基準より上へ出る部分は外して、後ろの部分で探す")
{
    TempTree tree;

    const auto wave = tree.add("Kit/Bass/saw.wt");

    // 基準が Kit/Lead にずれていたときの書き方
    CHECK(Io::findMovedFile(tree.root, "..\\Bass\\saw.wt") == wave);
}

TEST_CASE("移ったファイル: 同じ名前がいくつもあるときは、フォルダまで合うものを採る")
{
    TempTree tree;

    tree.add("A/tri.wt");
    const auto wanted = tree.add("B/tri.wt");

    CHECK(Io::findMovedFile(tree.root, "../B/tri.wt") == wanted);
}

TEST_CASE("移ったファイル: 同じ名前がいくつもあって決められないときは見つからないことにする")
{
    TempTree tree;

    tree.add("A/tri.wt");
    tree.add("B/tri.wt");

    CHECK(Io::findMovedFile(tree.root, "tri.wt") == juce::File());
}

TEST_CASE("移ったファイル: 無いものは無い")
{
    TempTree tree;

    tree.add("A/tri.wt");

    CHECK(Io::findMovedFile(tree.root, "sine.wt") == juce::File());
    CHECK(Io::findMovedFile(tree.root, "") == juce::File());
    CHECK(Io::findMovedFile(tree.root.getChildFile("nothing"), "tri.wt") == juce::File());
}
