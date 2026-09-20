#pragma once

#include <JuceHeader.h>

namespace Io
{
    // 保存に書かれた版を比べる。"3.5.0" のような点区切りの数字を左から見る。
    //
    // 古い保存を読み直すときに、そのころの鳴り方へ合わせるために使う。
    // 版が書かれていないものは、いちばん古いものとして扱う。
    inline bool isVersionOlderThan(const juce::String& version, const juce::String& target)
    {
        if (version.isEmpty()) return true;

        const juce::StringArray a = juce::StringArray::fromTokens(version, ".", {});
        const juce::StringArray b = juce::StringArray::fromTokens(target, ".", {});

        const int count = juce::jmax(a.size(), b.size());

        for (int i = 0; i < count; ++i)
        {
            const int x = (i < a.size()) ? a[i].getIntValue() : 0;
            const int y = (i < b.size()) ? b[i].getIntValue() : 0;

            if (x != y) return x < y;
        }

        return false;
    }
}
