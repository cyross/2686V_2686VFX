#pragma once

#include <JuceHeader.h>

#include "./StepTable.h"

// ============================================================================
// 段の表を文字・state (XML) へ出し入れする
// ============================================================================
// 表そのもの (StepTable.h) は音の側でも使うので、文字や XML の道具はここへ分けた。
namespace StepTableIo
{
    namespace detail
    {
        inline juce::String join(const std::array<std::atomic<float>, StepTable::size>& values, float fallback)
        {
            int last = StepTable::size - 1;

            while (last >= 0 && values[(size_t)last].load(std::memory_order_relaxed) == fallback) --last;

            juce::StringArray parts;

            for (int i = 0; i <= last; ++i) parts.add(juce::String(values[(size_t)i].load(std::memory_order_relaxed), 6));

            return parts.joinIntoString(",");
        }

        inline void split(const juce::String& text, std::array<std::atomic<float>, StepTable::size>& values, float fallback)
        {
            const auto parts = juce::StringArray::fromTokens(text, ",", "");

            for (int i = 0; i < StepTable::size; ++i) {
                const bool has = i < parts.size() && parts[i].trim().isNotEmpty();

                values[(size_t)i].store(has ? parts[i].getFloatValue() : fallback, std::memory_order_relaxed);
            }
        }
    }

    // ループの組を文字にする。"start:end:count;start:end:count;..."
    inline juce::String loopsToText(const std::vector<StepLoop::Entry>& loops)
    {
        juce::StringArray parts;

        for (const auto& e : loops) parts.add(juce::String(e.start) + ":" + juce::String(e.end) + ":" + juce::String(e.count));

        return parts.joinIntoString(";");
    }

    inline std::vector<StepLoop::Entry> loopsFromText(const juce::String& text)
    {
        std::vector<StepLoop::Entry> loops;

        for (const auto& part : juce::StringArray::fromTokens(text, ";", "")) {
            const auto v = juce::StringArray::fromTokens(part, ":", "");

            if (v.size() < 3) continue;

            loops.push_back({ v[0].getIntValue(), v[1].getIntValue(), v[2].getIntValue() });
        }

        return loops;
    }

    // 文字で書き出す。"r1,r2,...|l1,l2,..."。後ろの既定の値は省く。
    // ループの組があるときだけ、後ろへ "|start:end:count;..." を足す (4.1.0)。
    inline juce::String toText(const StepTable& table)
    {
        auto text = detail::join(table.r, table.defaultR) + "|" + detail::join(table.l, table.defaultL);

        if (table.loopCount() > 0) text << "|" << loopsToText(table.loopList());

        return text;
    }

    // toText の逆。書かれていない段は既定の値に戻す。ループの組が無ければ空にする。
    inline void fromText(StepTable& table, const juce::String& text)
    {
        const auto halves = juce::StringArray::fromTokens(text, "|", "");

        detail::split(halves.size() > 0 ? halves[0] : juce::String(), table.r, table.defaultR);
        detail::split(halves.size() > 1 ? halves[1] : juce::String(), table.l, table.defaultL);

        // 版を進めるのは setLoops がする
        table.setLoops(loopsFromText(halves.size() > 2 ? halves[2] : juce::String()));
    }

    // state の要素の名前
    inline const juce::String xmlTag = "STEP_TABLES";
    inline const juce::String xmlItem = "TABLE";

    // 既定の値のままの表は書かない
    inline void write(const StepTableStore& store, juce::XmlElement& parent)
    {
        // 前に読んだ state から残っていることがあるので、先に外す
        while (auto* old = parent.getChildByName(xmlTag)) parent.removeChildElement(old, true);

        auto* node = parent.createNewChildElement(xmlTag);

        store.forEach([node](const juce::String& key, StepTable& table) {
            const auto text = toText(table);

            if (text == "|") return;

            auto* item = node->createNewChildElement(xmlItem);

            item->setAttribute("key", key);
            item->setAttribute("values", text);
        });
    }

    // 書かれていない表は既定の値へ戻す
    inline void read(StepTableStore& store, const juce::XmlElement& parent)
    {
        store.resetAll();

        if (auto* node = parent.getChildByName(xmlTag)) {
            for (auto* item : node->getChildWithTagNameIterator(xmlItem)) {
                if (auto* table = store.find(item->getStringAttribute("key"))) fromText(*table, item->getStringAttribute("values"));
            }
        }
    }
}
