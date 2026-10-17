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

    // 文字で書き出す。"r1,r2,...|l1,l2,..."。後ろの既定の値は省く。
    inline juce::String toText(const StepTable& table)
    {
        return detail::join(table.r, table.defaultR) + "|" + detail::join(table.l, table.defaultL);
    }

    // toText の逆。書かれていない段は既定の値に戻す。
    inline void fromText(StepTable& table, const juce::String& text)
    {
        const auto halves = juce::StringArray::fromTokens(text, "|", "");

        detail::split(halves.size() > 0 ? halves[0] : juce::String(), table.r, table.defaultR);
        detail::split(halves.size() > 1 ? halves[1] : juce::String(), table.l, table.defaultL);

        table.version.fetch_add(1, std::memory_order_release);
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
