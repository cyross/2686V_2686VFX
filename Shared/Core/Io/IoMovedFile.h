#pragma once

#include <JuceHeader.h>

// ============================================================================
// 保存した相対パスが指す先で見つからないファイルを、名前で探し直す
// ============================================================================
// 波形ファイルのパスは、設定の置き場 (defaultWavetableDir) からの相対パスで
// 状態へ書いている。
//
// 3.6.0 までは、波形を読み込むたびにその置き場が「読み込んだファイルの
// フォルダ」へ書き換わっていた。そのため、置き場の下のフォルダから読んだ
// 波形は、そのフォルダを基準にした相対パス (ファイル名だけ) で保存されて
// いる。立ち上げ直すと置き場は設定の値へ戻るので、そのままでは見つからない。
//
// そうして保存したプロジェクトやプリセットを救うため、置き場の下を名前で
// 探す。相対パスの後ろの部分まで一致するものを優先し、それが無ければ、
// 同じ名前のファイルがひとつだけのときに限って採る。いくつもあるときは
// 取り違えるおそれがあるので、見つからなかったことにする。
namespace Io
{
    inline juce::File findMovedFile(const juce::File& baseDir, const juce::String& stored)
    {
        if (!baseDir.isDirectory() || stored.isEmpty()) return {};

        juce::StringArray parts;

        parts.addTokens(stored.replaceCharacter('\\', '/'), "/", "");
        parts.removeEmptyStrings();

        // 基準より上へ出る部分は、基準がずれている以上あてにならない
        while (!parts.isEmpty() && (parts[0] == ".." || parts[0] == ".")) parts.remove(0);

        if (parts.isEmpty()) return {};

        const juce::String name = parts[parts.size() - 1];
        const juce::String tail = "/" + parts.joinIntoString("/");

        // フォルダまで書いてあれば、それも合うものを優先する
        const bool hasFolders = parts.size() > 1;

        juce::File only;
        int count = 0;

        for (const auto& entry : juce::RangedDirectoryIterator(baseDir, true, "*", juce::File::findFiles))
        {
            const auto file = entry.getFile();

            if (!file.getFileName().equalsIgnoreCase(name)) continue;

            if (hasFolders && file.getFullPathName().replaceCharacter('\\', '/').endsWithIgnoreCase(tail)) return file;

            only = file;
            ++count;
        }

        return (count == 1) ? only : juce::File();
    }
}
