// ============================================================================
// PCM の 1 秒モード (4.5.0) の画面
// ============================================================================
// ・1 秒モードを入れると「長いとき」「短いとき」が使えるようになり、
//   ループポイント (Loop Point Enable) が使えなくなる
// ・音色モードが優先する。音色モードのあいだは 1 秒モードとまとめ方も使えない
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <vector>

#include "Core/Processor/PluginProcessor.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "Core/Editor/PluginEditor.h"
#include "Shared/Core/Gui/GuiComponents.h"

#include "./EditorTestAccess.h"

namespace
{
#if defined(GUI_TEST_REQUIRE_ALL)
    constexpr bool kRequireAll = true;
#else
    constexpr bool kRequireAll = false;
#endif

    void collect(juce::Component* root, std::vector<juce::Component*>& out)
    {
        for (auto* c : root->getChildren()) {
            out.push_back(c);
            collect(c, out);
        }
    }

    juce::Component* findById(const std::vector<juce::Component*>& all, const juce::String& id)
    {
        for (auto* c : all) {
            if (c->getComponentID() == id) return c;
        }

        return nullptr;
    }
}

TEST_CASE("PcmOneSec: 1 秒モードでまとめ方が使え、ループポイントが止まる。音色モードが優先する")
{
    auto processor = std::make_unique<AudioPlugin2686V>();
    EditorTestAccess::unfoldAllGroups(processor->apvts);

    std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());
    auto& ed = *static_cast<AudioPlugin2686VEditor*>(editor.get());
    EditorTestAccess::materializeAll(ed);

    auto& tabs = EditorTestAccess::tabs(ed);

    auto set = [&](const juce::String& id, bool on) {
        auto* p = processor->apvts.getParameter(id);
        REQUIRE_MESSAGE(p != nullptr, id.toStdString());
        p->setValueNotifyingHost(on ? 1.0f : 0.0f);
    };

    int checked = 0;

    for (int i = 0; i < tabs.getNumTabs(); ++i)
    {
        auto* root = tabs.getTabContentComponent(i);
        if (root == nullptr) continue;

        std::vector<juce::Component*> all;
        collect(root, all);

        for (auto* c : all)
        {
            auto* oneSec = dynamic_cast<GuiToggleButton*>(c);
            if (oneSec == nullptr || !oneSec->getComponentID().endsWith(CPK::pcmOneSec)) continue;

            const auto prefix = oneSec->getComponentID().dropLastCharacters(CPK::pcmOneSec.length());

            auto* tone = findById(all, prefix + CPK::pcmTone);
            auto* longFit = findById(all, prefix + CPK::pcmOneSecLong);
            auto* shortFit = findById(all, prefix + CPK::pcmOneSecShort);
            auto* lpEnable = findById(all, prefix + CPK::lpEnable);

            INFO(tabs.getTabNames()[i].toStdString() << " / " << prefix.toStdString());
            REQUIRE(tone != nullptr);
            REQUIRE(longFit != nullptr);
            REQUIRE(shortFit != nullptr);
            REQUIRE(lpEnable != nullptr);

            auto* p1 = processor->apvts.getParameter(prefix + CPK::pcmTone);
            auto* p2 = processor->apvts.getParameter(prefix + CPK::pcmOneSec);
            const float origTone = p1->getValue();
            const float origOneSec = p2->getValue();

            set(prefix + CPK::pcmTone, false);
            set(prefix + CPK::pcmOneSec, false);

            // 外から止められている指し先 (鳴っていないオペレーター) は対象外
            if (!oneSec->isEnabled()) {
                p1->setValueNotifyingHost(origTone);
                p2->setValueNotifyingHost(origOneSec);
                continue;
            }

            // 切っているあいだは、まとめ方は使えず、ループポイントは使える
            CHECK_FALSE(longFit->isEnabled());
            CHECK_FALSE(shortFit->isEnabled());
            CHECK(lpEnable->isEnabled());

            // 入れると、まとめ方が使え、ループポイントは止まる
            set(prefix + CPK::pcmOneSec, true);
            CHECK(longFit->isEnabled());
            CHECK(shortFit->isEnabled());
            CHECK_FALSE(lpEnable->isEnabled());

            // 音色モードが優先する
            set(prefix + CPK::pcmTone, true);
            CHECK_FALSE(oneSec->isEnabled());
            CHECK_FALSE(longFit->isEnabled());
            CHECK_FALSE(shortFit->isEnabled());
            CHECK_FALSE(lpEnable->isEnabled());

            // 音色モードを切ると戻る
            set(prefix + CPK::pcmTone, false);
            CHECK(oneSec->isEnabled());
            CHECK(longFit->isEnabled());
            CHECK_FALSE(lpEnable->isEnabled());

            p1->setValueNotifyingHost(origTone);
            p2->setValueNotifyingHost(origOneSec);

            ++checked;
        }
    }

    if (kRequireAll) CHECK(checked > 0);
}
