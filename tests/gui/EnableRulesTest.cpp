// ============================================================================
// 札 (Bypass / Enable) に合わせて、区分の中の部品が止まるか
// ============================================================================
// 本物のプロセッサと画面を立ち上げ、全タブを作ったうえで、札を持つ部品を
// パラメータ ID で探す。札を切り替え、同じ指し先 (オペレーター・パッド・
// チャンネル) の従う部品が止まる / 戻るかを見る。
//
// 札の ID の末尾で区分を見分け、前に付いている分 (チャンネルと指し先) を
// そのまま従う部品の ID へ付けて引く。区分を増やしたときは、ここの表へ
// 足せば全タブ・全指し先で確かめられる。
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <functional>
#include <vector>

#include "Core/Processor/PluginProcessor.h"
#include "Core/Processor/ProcessorKeys.h"
#include "Core/Editor/PluginEditor.h"
#include "Core/Gui/GuiComponents.h"

// 画面の中で、テストだけが触る口。PluginEditor が friend にしている。
struct EditorTestAccess
{
    static void materializeAll(AudioPlugin2686VEditor& e) { e.materializeAllTabs(); }
    static juce::TabbedComponent& tabs(AudioPlugin2686VEditor& e) { return e.tabs; }
};

namespace
{
    // ------------------------------------------------------------------
    // プラグインを立ち上げて、全タブを作る
    // ------------------------------------------------------------------
    struct Env
    {
        std::unique_ptr<AudioPlugin2686V> processor;
        std::unique_ptr<juce::AudioProcessorEditor> editor;

        Env()
        {
            processor = std::make_unique<AudioPlugin2686V>();
            editor.reset(processor->createEditor());

            EditorTestAccess::materializeAll(ed());
        }

        ~Env()
        {
            // 画面はプロセッサを指しているので、先に閉じる
            editor.reset();
            processor.reset();
        }

        AudioPlugin2686VEditor& ed() { return *static_cast<AudioPlugin2686VEditor*>(editor.get()); }

        // タブの中身。開いていないタブは画面の木から外れているので、
        // タブから直に受け取る。
        std::vector<juce::Component*> tabRoots()
        {
            std::vector<juce::Component*> roots;

            auto& t = EditorTestAccess::tabs(ed());

            for (int i = 0; i < t.getNumTabs(); ++i) {
                if (auto* c = t.getTabContentComponent(i)) roots.push_back(c);
            }

            return roots;
        }

        void setParam(const juce::String& id, bool on)
        {
            auto* p = processor->apvts.getParameter(id);

            REQUIRE_MESSAGE(p != nullptr, id.toStdString());

            // メッセージスレッドから書くので、束縛はその場で部品へ届ける
            p->setValueNotifyingHost(on ? 1.0f : 0.0f);
        }

        bool getParam(const juce::String& id)
        {
            auto* p = processor->apvts.getParameter(id);

            return p != nullptr && p->getValue() >= 0.5f;
        }
    };

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

    // 止まっているか。トグルは値を取りこぼさないよう、止めずに薄くする。
    bool isStopped(juce::Component* c)
    {
        if (auto* t = dynamic_cast<GuiToggleButton*>(c)) {
            if (t->isDimmed()) return true;
        }

        return !c->isEnabled();
    }

    // ------------------------------------------------------------------
    // 区分ごとの決まり
    // ------------------------------------------------------------------
    struct Nested
    {
        juce::String toggle;               // 入れ子の札 (LOOP / Use Endl)
        std::vector<juce::String> deps;    // それに従うもの
    };

    struct Rule
    {
        const char* name = nullptr;
        juce::String flag;                 // 札の ID の末尾
        bool isEnable = false;             // true: 入っていると効く / false: Bypass
        // この区分だと見分ける部品。どれか 1 つでも無ければ別の区分。
        // 末尾が同じ札 (N88 / OPM / OPZX7 の LFO) を見分けるのに使う。
        std::vector<juce::String> signature;
        std::vector<juce::String> deps;    // 札が効いていないと止まるもの
        std::vector<Nested> nested;
    };

    std::vector<Rule> rules()
    {
        std::vector<Rule> r;

        // AMP ENV
        r.push_back({ "AMP ENV", CPK::adsr + CPK::bypass, false, { CPK::Adsr::ar },
            { CPK::Adsr::stl, CPK::Adsr::ar, CPK::Adsr::dr, CPK::Adsr::sl, CPK::Adsr::rr,
              CPK::Adsr::endlEnable, CPK::Adsr::kor },
            { { CPK::Adsr::endlEnable, { CPK::Adsr::endl } } } });

        // PITCH ENV (Bypass の札のものと Enable の札のもの)
        const std::vector<juce::String> pitchDeps{
            CPK::PitchAdsr::keep, CPK::PitchAdsr::ar, CPK::PitchAdsr::dr, CPK::PitchAdsr::rr,
            CPK::PitchAdsr::stl, CPK::PitchAdsr::atl, CPK::PitchAdsr::ssl, CPK::PitchAdsr::rll,
            CPK::PitchAdsr::endlEnable };
        const std::vector<Nested> pitchNested{ { CPK::PitchAdsr::endlEnable, { CPK::PitchAdsr::endl } } };

        r.push_back({ "PITCH ENV (Bypass)", CPK::pitchAdsr + CPK::bypass, false, { CPK::PitchAdsr::ar }, pitchDeps, pitchNested });
        r.push_back({ "PITCH ENV (Enable)", CPK::PitchAdsr::enable, true, { CPK::PitchAdsr::ar }, pitchDeps, pitchNested });

        // SSG HW AMP ENV / SSG HW PITCH ENV
        r.push_back({ "SSG HW AMP ENV", CPK::SsgHwEnv::enable, true, { CPK::SsgHwEnv::shape },
            { CPK::SsgHwEnv::smooth, CPK::SsgHwEnv::shape, CPK::SsgHwEnv::period,
              CPK::SsgHwEnv::min, CPK::SsgHwEnv::max }, {} });
        r.push_back({ "SSG HW PITCH ENV", CPK::SsgHwPEnv::enable, true, { CPK::SsgHwPEnv::shape },
            { CPK::SsgHwPEnv::smooth, CPK::SsgHwPEnv::shape, CPK::SsgHwPEnv::period,
              CPK::SsgHwPEnv::min, CPK::SsgHwPEnv::max }, {} });

        // SSG SW AMP ENV (6 タップ)
        const std::vector<juce::String> swDeps{ CPK::SsgSwEnv::steps, CPK::SsgSwEnv::loop };
        const std::vector<Nested> swNested{ { CPK::SsgSwEnv::loop, { CPK::SsgSwEnv::loopTo, CPK::SsgSwEnv::loopCount } } };

        r.push_back({ "SSG SW AMP ENV (Bypass)", CPK::SsgSwEnv::bypass, false, { CPK::SsgSwEnv::steps }, swDeps, swNested });
        r.push_back({ "SSG SW AMP ENV (Enable)", CPK::SsgSwEnv::enable, true, { CPK::SsgSwEnv::steps }, swDeps, swNested });

        // SSG SW AMP ENV[11] / SSG SW PITCH ENV[11]
        const std::vector<juce::String> sw11Deps{
            CPK::SsgSwEnv11::steps, CPK::SsgSwEnv11::keep, CPK::SsgSwEnv11::loop, CPK::SsgSwEnv11::endlEnable };
        const std::vector<Nested> sw11Nested{
            { CPK::SsgSwEnv11::loop, { CPK::SsgSwEnv11::loopTo, CPK::SsgSwEnv11::loopCount } },
            { CPK::SsgSwEnv11::endlEnable, { CPK::SsgSwEnv11::endl } } };

        r.push_back({ "SSG SW AMP ENV[11] (Bypass)", CPK::SsgSwEnv11::bypass, false, { CPK::SsgSwEnv11::steps }, sw11Deps, sw11Nested });
        r.push_back({ "SSG SW AMP ENV[11] (Enable)", CPK::SsgSwEnv11::enable, true, { CPK::SsgSwEnv11::steps }, sw11Deps, sw11Nested });

        const std::vector<juce::String> swp11Deps{
            CPK::SsgSwPEnv11::steps, CPK::SsgSwPEnv11::keep, CPK::SsgSwPEnv11::loop, CPK::SsgSwPEnv11::endlEnable };
        const std::vector<Nested> swp11Nested{
            { CPK::SsgSwPEnv11::loop, { CPK::SsgSwPEnv11::loopTo, CPK::SsgSwPEnv11::loopCount } },
            { CPK::SsgSwPEnv11::endlEnable, { CPK::SsgSwPEnv11::endl } } };

        r.push_back({ "SSG SW PITCH ENV[11] (Bypass)", CPK::SsgSwPEnv11::bypass, false, { CPK::SsgSwPEnv11::steps }, swp11Deps, swp11Nested });
        r.push_back({ "SSG SW PITCH ENV[11] (Enable)", CPK::SsgSwPEnv11::enable, true, { CPK::SsgSwPEnv11::steps }, swp11Deps, swp11Nested });

        // OPZX7 の LFO。PM と AM は別々の札
        r.push_back({ "LFO (OPZX7S) PM", CPK::Opzx7Lfo::pm, true, { CPK::Opzx7Lfo::pmFreq },
            { CPK::Opzx7Lfo::pmFreq, CPK::Opzx7Lfo::pmSyncDelay, CPK::Opzx7Lfo::pgShape,
              CPK::Opzx7Lfo::pms, CPK::Opzx7Lfo::pmd }, {} });
        r.push_back({ "LFO (OPZX7S) AM", CPK::Opzx7Lfo::am, true, { CPK::Opzx7Lfo::amFreq },
            { CPK::Opzx7Lfo::amFreq, CPK::Opzx7Lfo::amSyncDelay, CPK::Opzx7Lfo::egShape,
              CPK::Opzx7Lfo::amSmoothRatio, CPK::Opzx7Lfo::ams, CPK::Opzx7Lfo::amd }, {} });

        // N88 LFO。両方で使うもの (速さ・形・同期) は別に見る
        r.push_back({ "N88 LFO PM", CPK::N88Lfo::pm, true, { CPK::N88Lfo::freq, CPK::N88Lfo::shape },
            { CPK::N88Lfo::pmd, CPK::N88Lfo::pms }, {} });
        r.push_back({ "N88 LFO AM", CPK::N88Lfo::am, true, { CPK::N88Lfo::freq, CPK::N88Lfo::shape },
            { CPK::N88Lfo::amd, CPK::N88Lfo::amSmoothRatio }, {} });

        // OPM の LFO。N88 と同じ形で、形は PM (PG) と AM (EG) に分かれている
        r.push_back({ "OPM LFO PM", CPK::OpmLfo::pm, true, { CPK::OpmLfo::freq, CPK::OpmLfo::pgShape },
            { CPK::OpmLfo::pgShape, CPK::OpmLfo::pms, CPK::OpmLfo::pmd }, {} });
        r.push_back({ "OPM LFO AM", CPK::OpmLfo::am, true, { CPK::OpmLfo::freq, CPK::OpmLfo::egShape },
            { CPK::OpmLfo::egShape, CPK::OpmLfo::amSmoothRatio, CPK::OpmLfo::ams, CPK::OpmLfo::amd }, {} });

        // FIX
        r.push_back({ "FIX", CPK::fix, true, { CPK::fixFreq }, { CPK::fixFreq }, {} });

        return r;
    }

    // 札を持つ部品を探す。指し先の分だけ前に付いたものが見つかる。
    struct Instance
    {
        GuiToggleButton* flag = nullptr;
        juce::String prefix;
    };

    std::vector<Instance> findInstances(const std::vector<juce::Component*>& all, const Rule& rule)
    {
        std::vector<Instance> found;

        for (auto* c : all) {
            auto* t = dynamic_cast<GuiToggleButton*>(c);

            if (t == nullptr) continue;

            const auto id = t->getComponentID();

            if (!id.endsWith(rule.flag)) continue;

            const auto prefix = id.dropLastCharacters(rule.flag.length());

            // 末尾が同じ別の区分 (N88 と OPZX7 の LFO など) を弾く
            bool match = true;
            for (const auto& key : rule.signature) {
                if (findById(all, prefix + key) == nullptr) match = false;
            }
            if (!match) continue;

            found.push_back({ t, prefix });
        }

        return found;
    }

    void checkDeps(Env& env, const std::vector<juce::Component*>& all, const juce::String& prefix,
        const std::vector<juce::String>& deps, bool shouldStop, const char* what)
    {
        for (const auto& key : deps) {
            auto* c = findById(all, prefix + key);

            INFO(what << " : " << (prefix + key).toStdString());
            REQUIRE(c != nullptr);
            CHECK(isStopped(c) == shouldStop);
        }

        (void)env;
    }

    // 1 つの札について、効いている / 効いていない / 入れ子を見る
    int checkInstance(Env& env, const std::vector<juce::Component*>& all, const Rule& rule, const Instance& inst)
    {
        // 外から止められている指し先 (鳴っていないオペレーター) は対象外
        if (!inst.flag->isEnabled()) return 0;

        const auto flagId = inst.prefix + rule.flag;
        const bool originalFlag = env.getParam(flagId);

        std::vector<bool> originalNested;
        for (const auto& n : rule.nested) originalNested.push_back(env.getParam(inst.prefix + n.toggle));

        const bool on = rule.isEnable;

        // 1. 札が効いていて、入れ子も入っている → すべて押せる
        for (const auto& n : rule.nested) env.setParam(inst.prefix + n.toggle, true);
        env.setParam(flagId, on);

        checkDeps(env, all, inst.prefix, rule.deps, false, "active");
        for (const auto& n : rule.nested) checkDeps(env, all, inst.prefix, n.deps, false, "active nested");

        // 2. 札が効いていない → 札のほかは止まる。札そのものは押せる
        env.setParam(flagId, !on);

        checkDeps(env, all, inst.prefix, rule.deps, true, "inactive");
        for (const auto& n : rule.nested) checkDeps(env, all, inst.prefix, n.deps, true, "inactive nested");
        CHECK_MESSAGE(!isStopped(inst.flag), flagId.toStdString());

        // 止めてあるあいだに来た値も、表示へ届く。TARGET の切り替えや
        // プリセットの読み込みはこの形で値を入れる。JUCE のトグルは
        // setEnabled(false) だとこれを捨てるので、薄くする形にしてある。
        for (const auto& key : rule.deps) {
            auto* t = dynamic_cast<GuiToggleButton*>(findById(all, inst.prefix + key));

            if (t == nullptr) continue;

            const auto id = inst.prefix + key;
            const bool before = env.getParam(id);

            INFO("value while stopped : " << id.toStdString());

            env.setParam(id, !before);
            CHECK(t->getToggleState() == !before);

            env.setParam(id, before);
            CHECK(t->getToggleState() == before);
        }

        // 3. 札は効いているが入れ子が切れている → 入れ子に従うものだけ止まる
        env.setParam(flagId, on);

        for (const auto& n : rule.nested) {
            env.setParam(inst.prefix + n.toggle, false);

            checkDeps(env, all, inst.prefix, n.deps, true, "nested off");

            CHECK_MESSAGE(!isStopped(findById(all, inst.prefix + n.toggle)), (inst.prefix + n.toggle).toStdString());

            env.setParam(inst.prefix + n.toggle, true);
        }

        // 元へ戻す
        for (size_t i = 0; i < rule.nested.size(); ++i) env.setParam(inst.prefix + rule.nested[i].toggle, originalNested[i]);
        env.setParam(flagId, originalFlag);

        return 1;
    }
}

TEST_CASE("札が効いていないあいだは、区分の中の部品が止まる")
{
    Env env;

    std::vector<juce::Component*> all;
    for (auto* root : env.tabRoots()) collect(root, all);

    for (const auto& rule : rules()) {
        const auto instances = findInstances(all, rule);

        int checked = 0;

        for (const auto& inst : instances) {
            INFO(std::string(rule.name) << " @ " << inst.prefix.toStdString());
            checked += checkInstance(env, all, rule, inst);
        }

        // どこにも無い区分は、表か名札のどちらかが壊れている
        INFO(std::string(rule.name));
        CHECK(checked > 0);

        MESSAGE(std::string(rule.name) << " : " << checked << " か所");
    }
}

TEST_CASE("LFO の両方で使うつまみは、PM と AM が両方切れたときだけ止まる")
{
    Env env;

    std::vector<juce::Component*> all;
    for (auto* root : env.tabRoots()) collect(root, all);

    struct Shared
    {
        const char* rule;
        juce::String am;
        std::vector<juce::String> keys;
    };

    const std::vector<Shared> cases{
        { "N88 LFO PM", CPK::N88Lfo::am, { CPK::N88Lfo::freq, CPK::N88Lfo::shape, CPK::N88Lfo::syncDelay } },
        { "OPM LFO PM", CPK::OpmLfo::am, { CPK::OpmLfo::freq, CPK::OpmLfo::syncDelay } },
    };

    for (const auto& sc : cases) {
        Rule pm;
        for (const auto& r : rules()) {
            if (juce::String(r.name) == sc.rule) pm = r;
        }
        REQUIRE(pm.name != nullptr);

        const auto instances = findInstances(all, pm);

        INFO(std::string(sc.rule));
        REQUIRE(!instances.empty());

        for (const auto& inst : instances) {
            INFO(inst.prefix.toStdString());

            const auto pmId = inst.prefix + pm.flag;
            const auto amId = inst.prefix + sc.am;
            const bool pm0 = env.getParam(pmId);
            const bool am0 = env.getParam(amId);

            env.setParam(pmId, true);
            env.setParam(amId, false);
            checkDeps(env, all, inst.prefix, sc.keys, false, "PM only");

            env.setParam(pmId, false);
            env.setParam(amId, true);
            checkDeps(env, all, inst.prefix, sc.keys, false, "AM only");

            env.setParam(amId, false);
            checkDeps(env, all, inst.prefix, sc.keys, true, "both off");

            env.setParam(pmId, pm0);
            env.setParam(amId, am0);
        }
    }
}

TEST_CASE("TARGET を切り替えても、指し先の札の状態に合わせて止まる")
{
    // 札の状態が違う 2 つの指し先を行き来して、束縛の張り替えのあとも
    // 表示と止め方が新しい指し先に揃うかを見る。トグルを setEnabled(false)
    // で止めていたころは、ここで前の指し先の値が残った。
    Env env;

    int checkedTabs = 0;

    for (auto* root : env.tabRoots()) {
        // 指し先を選ぶつまみは、タブの直下に置いてある
        GuiSlider* target = nullptr;

        for (auto* c : root->getChildren()) {
            if (auto* s = dynamic_cast<GuiSlider*>(c); s != nullptr && s->label.getText() == "TARGET") target = s;
        }

        // タブの器 (GuiTabHost) の中に入っているものもある
        if (target == nullptr) {
            for (auto* host : root->getChildren()) {
                for (auto* c : host->getChildren()) {
                    if (auto* s = dynamic_cast<GuiSlider*>(c); s != nullptr && s->label.getText() == "TARGET") target = s;
                }
            }
        }

        if (target == nullptr) continue;

        auto findAll = [root] {
            std::vector<juce::Component*> all;
            collect(root, all);
            return all;
        };

        for (const auto& rule : rules()) {
            target->setValue(1.0, juce::sendNotificationSync);

            auto all1 = findAll();
            auto inst1 = findInstances(all1, rule);

            if (inst1.empty()) continue;

            target->setValue(2.0, juce::sendNotificationSync);

            auto all2 = findAll();
            auto inst2 = findInstances(all2, rule);

            if (inst2.empty() || inst2.front().prefix == inst1.front().prefix) continue;

            const auto p1 = inst1.front().prefix;
            const auto p2 = inst2.front().prefix;
            const bool on = rule.isEnable;

            INFO(std::string(rule.name) << " : " << p1.toStdString() << " <-> " << p2.toStdString());

            const bool f1 = env.getParam(p1 + rule.flag);
            const bool f2 = env.getParam(p2 + rule.flag);

            // 1 番は効いていない、2 番は効いている
            env.setParam(p1 + rule.flag, !on);
            env.setParam(p2 + rule.flag, on);

            // いまは 2 番を指している
            if (inst2.front().flag->isEnabled()) {
                checkDeps(env, all2, p2, rule.deps, false, "target 2");
            }

            target->setValue(1.0, juce::sendNotificationSync);

            if (inst1.front().flag->isEnabled()) {
                // 同じ部品が 1 番の ID へ付け替わっている
                auto all = findAll();
                checkDeps(env, all, p1, rule.deps, true, "target 1");

                // 札の表示も 1 番の値になっている
                auto* flag = dynamic_cast<GuiToggleButton*>(findById(all, p1 + rule.flag));
                REQUIRE(flag != nullptr);
                CHECK(flag->getToggleState() == !on);
            }

            target->setValue(2.0, juce::sendNotificationSync);

            if (inst2.front().flag->isEnabled()) {
                auto all = findAll();
                checkDeps(env, all, p2, rule.deps, false, "back to target 2");
            }

            env.setParam(p1 + rule.flag, f1);
            env.setParam(p2 + rule.flag, f2);

            ++checkedTabs;
        }

        target->setValue(1.0, juce::sendNotificationSync);
    }

    CHECK(checkedTabs > 0);
    MESSAGE("TARGET の切り替え: " << checkedTabs << " 通り");
}
