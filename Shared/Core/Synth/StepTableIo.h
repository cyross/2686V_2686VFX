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

    // SKIP の段を文字にする。段の番号 (1 から) を "," で並べる
    inline juce::String skipsToText(const StepTable& table)
    {
        juce::StringArray parts;

        for (int s = 1; s <= StepTable::size; ++s) if (table.skipped(s)) parts.add(juce::String(s));

        return parts.joinIntoString(",");
    }

    inline void skipsFromText(StepTable& table, const juce::String& text)
    {
        std::array<bool, StepTable::size> on{};

        for (const auto& part : juce::StringArray::fromTokens(text, ",", "")) {
            const int s = part.trim().getIntValue();

            if (s >= 1 && s <= StepTable::size) on[(size_t)s - 1] = true;
        }

        for (int i = 0; i < StepTable::size; ++i) table.skips[(size_t)i].store(on[(size_t)i], std::memory_order_relaxed);
    }

    // TAP PARAM SEQUENCER の組を文字にする (4.3.0)。
    // "段:scope:index:値;段:scope:index:値;..."。段は 1 から、同じ段の組は並び順のまま。
    inline juce::String pairsToText(const StepTable& table)
    {
        juce::StringArray parts;

        for (int s = 1; s <= StepTable::size; ++s) {
            for (const auto& p : table.pairList(s)) {
                parts.add(juce::String(s) + ":" + juce::String(p.scope) + ":" + juce::String(p.index) + ":" + juce::String(p.value, 6));
            }
        }

        return parts.joinIntoString(";");
    }

    inline void pairsFromText(StepTable& table, const juce::String& text)
    {
        std::array<std::vector<StepPair>, StepTable::size> steps;

        for (const auto& part : juce::StringArray::fromTokens(text, ";", "")) {
            const auto v = juce::StringArray::fromTokens(part, ":", "");

            if (v.size() < 4) continue;

            const int s = v[0].getIntValue();

            if (s < 1 || s > StepTable::size) continue;

            steps[(size_t)s - 1].push_back({ v[1].getIntValue(), v[2].getIntValue(), juce::jlimit(0.0f, 1.0f, v[3].getFloatValue()) });
        }

        for (int s = 1; s <= StepTable::size; ++s) table.setPairs(s, steps[(size_t)s - 1]);
    }

    // PARAM ENV の指し先ごとの値を文字にする (4.3.0)。初期値と違うものだけを
    // "組:位置:値;組:位置:値;..." で並べる。位置は 0 が STL、1〜256 が段、257 が LR、258 が ENDL
    inline juce::String setsToText(const StepTable& table)
    {
        juce::StringArray parts;

        for (int set = 0; set < table.setCount(); ++set) {
            const float def = table.setDefault(set);

            for (int slot = 0; slot < StepTable::setSlots; ++slot) {
                const float v = table.setValue(set, slot);

                if (v != def) parts.add(juce::String(set) + ":" + juce::String(slot) + ":" + juce::String(v, 6));
            }
        }

        return parts.joinIntoString(";");
    }

    inline void setsFromText(StepTable& table, const juce::String& text)
    {
        for (const auto& part : juce::StringArray::fromTokens(text, ";", "")) {
            const auto v = juce::StringArray::fromTokens(part, ":", "");

            if (v.size() < 3) continue;

            table.setSetValue(v[0].getIntValue(), v[1].getIntValue(), juce::jlimit(0.0f, 1.0f, v[2].getFloatValue()));
        }
    }

    // 文字で書き出す。"r1,r2,...|l1,l2,..."。後ろの既定の値は省く。
    // ループの組があるときだけ、後ろへ "|start:end:count;..." を足す (4.1.0)。
    // SKIP の段があるときは、さらに後ろへ "|s1,s2,..." を足す (組が無ければ空のまま挟む)。
    inline juce::String toText(const StepTable& table)
    {
        auto text = detail::join(table.r, table.defaultR) + "|" + detail::join(table.l, table.defaultL);

        const bool skips = table.hasSkips();

        if (table.loopCount() > 0 || skips) text << "|" << loopsToText(table.loopList());
        if (skips) text << "|" << skipsToText(table);

        return text;
    }

    // toText の逆。書かれていない段は既定の値に戻す。ループの組や SKIP が無ければ空にする。
    inline void fromText(StepTable& table, const juce::String& text)
    {
        const auto halves = juce::StringArray::fromTokens(text, "|", "");

        detail::split(halves.size() > 0 ? halves[0] : juce::String(), table.r, table.defaultR);
        detail::split(halves.size() > 1 ? halves[1] : juce::String(), table.l, table.defaultL);
        skipsFromText(table, halves.size() > 3 ? halves[3] : juce::String());

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
            const bool pairs = table.hasAnyPairs();
            const auto setText = setsToText(table);

            if (text == "|" && !pairs && setText.isEmpty()) return;

            auto* item = node->createNewChildElement(xmlItem);

            item->setAttribute("key", key);
            item->setAttribute("values", text);

            // TAP PARAM SEQUENCER の組と、PARAM ENV の指し先ごとの値 (4.3.0)
            if (pairs) item->setAttribute("pairs", pairsToText(table));
            if (setText.isNotEmpty()) item->setAttribute("sets", setText);
        });
    }

    // 書かれていない表は既定の値へ戻す
    inline void read(StepTableStore& store, const juce::XmlElement& parent)
    {
        store.resetAll();

        if (auto* node = parent.getChildByName(xmlTag)) {
            for (auto* item : node->getChildWithTagNameIterator(xmlItem)) {
                if (auto* table = store.find(item->getStringAttribute("key"))) {
                    fromText(*table, item->getStringAttribute("values"));

                    if (table->hasPairs()) pairsFromText(*table, item->getStringAttribute("pairs"));
                    if (table->setCount() > 0) setsFromText(*table, item->getStringAttribute("sets"));
                }
            }
        }
    }
}
