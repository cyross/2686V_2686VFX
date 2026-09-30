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
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "Core/Editor/PluginEditor.h"
#include "Shared/Core/Gui/GuiComponents.h"

// 画面の中で、テストだけが触る口。PluginEditor が friend にしている。
struct EditorTestAccess
{
    // タブの中身を後から作る仕掛けを持たない本 (2686VFX) もある
    //
    // 型に依存させないと、無い関数を requires の中で書いた時点で弾かれる。
    template <typename Editor>
    static void materializeAll(Editor& e)
    {
        if constexpr (requires { e.materializeAllTabs(); }) e.materializeAllTabs();
    }
    static juce::TabbedComponent& tabs(AudioPlugin2686VEditor& e) { return e.tabs; }
};

namespace
{
    // どの区分も持っている本 (2686V) でだけ、区分が見つからないことを失敗にする
#if defined(GUI_TEST_REQUIRE_ALL)
    constexpr bool kRequireAll = true;
#else
    constexpr bool kRequireAll = false;
#endif

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

        // QUALITY (PCM) の無音ゲート。しきい値はゲートを入れたときだけ効く
        r.push_back({ "QUALITY NR Gate", CPK::QualityPcm::nrGate, true, { CPK::QualityPcm::nrGateLevel },
            { CPK::QualityPcm::nrGateLevel }, {} });

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

        // どこにも無い区分は、表か名札のどちらかが壊れている。音源を絞った
        // 本は持っていない区分があって当たり前なので、2686V でだけ見る。
        INFO(std::string(rule.name));
        if (kRequireAll) CHECK(checked > 0);

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
        if (kRequireAll) REQUIRE(!instances.empty());

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

    if (kRequireAll) CHECK(checkedTabs > 0);
    MESSAGE("TARGET の切り替え: " << checkedTabs << " 通り");
}

TEST_CASE("QUALITY のきれいな間引きは、符号化するモードのときだけ押せる")
{
    // 1〜12 は素材をそのまま鳴らすので間引かない。13 以降 (YM2608 ADPCM 〜)
    // だけが符号化の前に間引く。
    Env env;

    std::vector<juce::Component*> all;
    for (auto* root : env.tabRoots()) collect(root, all);

    int checked = 0;

    for (auto* c : all) {
        auto* t = dynamic_cast<GuiToggleButton*>(c);

        if (t == nullptr || !t->getComponentID().endsWith(CPK::QualityPcm::nrResample)) continue;

        const auto prefix = t->getComponentID().dropLastCharacters(CPK::QualityPcm::nrResample.length());
        auto* mode = env.processor->apvts.getParameter(prefix + CPK::QualityPcm::mode);

        REQUIRE(mode != nullptr);

        INFO(prefix.toStdString());

        const float original = mode->getValue();

        mode->setValueNotifyingHost(mode->convertTo0to1(4.0f));    // 16-bit PCM
        CHECK(t->isDimmed());

        mode->setValueNotifyingHost(mode->convertTo0to1(13.0f));   // YM2608 ADPCM
        CHECK_FALSE(t->isDimmed());

        mode->setValueNotifyingHost(original);

        ++checked;
    }

    if (kRequireAll) CHECK(checked > 0);
    MESSAGE("QUALITY: " << checked << " か所");
}

// ホールドと部分再生 (WaveHold) は、HOLD / KEEP の入り切りで中のつまみを
// 開け閉めする。KEEP は押したときの処理を上書きしていて、並べ直しが
// 走らず、次に画面を組み直すまで START などが押せないままだった。
//
// 値はパラメータから入れる。押したときも、TARGET の切り替えやプリセットの
// 読み込みも、束縛を通ってこの形で届く。
TEST_CASE("HOLD / KEEP を入り切りすると、中のつまみがその場で開け閉めされる")
{
    Env env;

    std::vector<juce::Component*> all;
    for (auto* root : env.tabRoots()) collect(root, all);

    struct Switch
    {
        juce::String flag;
        std::vector<juce::String> deps;
        int covered = 0;
    };

    std::vector<Switch> switches = {
        { CPK::WaveHold::holdEnable,
          { CPK::WaveHold::holdCount, CPK::WaveHold::holdTarget, CPK::WaveHold::holdMin, CPK::WaveHold::holdMax } },
        { CPK::WaveHold::keepEnable,
          { CPK::WaveHold::waveStart, CPK::WaveHold::keepStart, CPK::WaveHold::waveEnd, CPK::WaveHold::keepEnd } },
    };

    for (auto& sw : switches) {
        for (auto* c : all) {
            const auto id = c->getComponentID();

            if (!id.endsWith(sw.flag)) continue;

            auto* flag = dynamic_cast<GuiToggleButton*>(c);

            // 親の区分が切れている / もう止まる形を選んでいる組は対象外
            if (flag == nullptr || isStopped(flag)) continue;

            const auto prefix = id.dropLastCharacters(sw.flag.length());
            const bool original = env.getParam(id);

            env.setParam(id, true);
            checkDeps(env, all, prefix, sw.deps, false, "switch on");

            env.setParam(id, false);
            checkDeps(env, all, prefix, sw.deps, true, "switch off");

            env.setParam(id, true);
            checkDeps(env, all, prefix, sw.deps, false, "switch on again");

            env.setParam(id, original);

            ++sw.covered;
        }

        INFO(sw.flag.toStdString());

        if (kRequireAll) CHECK(sw.covered > 0);
    }
}

// DAC の行の「適応」ボタンが、見える場所に収まっているか。
//
// 3.6.3 の初め、行の幅を固定 (80 + 155 + 35 = 270) で取っていた。区分の
// 中は 230 ほどしかないので、右端のボタンが枠の外へ出て見えなかった。
TEST_CASE("DAC: 「適応」ボタンが区分の中に収まっている")
{
    Env env;

    auto& tabs = EditorTestAccess::tabs(env.ed());
    int checked = 0;

    for (int i = 0; i < tabs.getNumTabs(); ++i)
    {
        tabs.setCurrentTabIndex(i);

        auto* root = tabs.getTabContentComponent(i);

        if (root == nullptr) continue;

        // QUALITY は閉じていることがあるので、開いてから並べ直す
        std::vector<juce::Component*> all;

        collect(root, all);

        for (auto* c : all)
        {
            if (auto* cat = dynamic_cast<GuiCategoryLabel*>(c)) {
                if (cat->getText() == "QUALITY") cat->setDetailVisible(true);
            }
        }

        env.ed().resized();

        all.clear();
        collect(root, all);

        for (auto* c : all)
        {
            auto* combo = dynamic_cast<GuiComboBox*>(c);

            if (combo == nullptr || !combo->isVisible() || combo->label.getText() != "DAC") continue;

            auto* parent = combo->getParentComponent();

            // 同じ行の右にあるボタンが「適応」
            GuiTextButton* apply = nullptr;

            for (auto* sibling : parent->getChildren())
            {
                auto* btn = dynamic_cast<GuiTextButton*>(sibling);

                if (btn != nullptr && btn->getY() == combo->getY() && btn->getX() >= combo->getRight()) apply = btn;
            }

            INFO(tabs.getTabNames()[i].toStdString());
            INFO("combo " << combo->getBounds().toString().toStdString());

            REQUIRE(apply != nullptr);

            INFO("apply " << apply->getBounds().toString().toStdString());
            INFO("parent " << parent->getLocalBounds().toString().toStdString());

            CHECK(apply->isVisible());
            CHECK(apply->getWidth() > 0);
            CHECK(parent->getLocalBounds().contains(apply->getBounds()));

            ++checked;
        }
    }

    CHECK(checked > 0);
}

// 相対パスの基準は環境設定に 1 つだけ。どのチャンネルの UTILITY や SETTINGS で
// 変えても、ほかの表示がそろう。
TEST_CASE("REL.PATH: どの UTILITY で変えても、全体の値とほかの表示がそろう")
{
    Env env;

    std::vector<GuiComboBox*> selectors;

    for (auto* root : env.tabRoots())
    {
        std::vector<juce::Component*> all;

        collect(root, all);

        for (auto* c : all)
        {
            auto* combo = dynamic_cast<GuiComboBox*>(c);

            if (combo != nullptr && combo->label.getText() == "REL.PATH") selectors.push_back(combo);
        }
    }

    // SETTINGS にも置いていない本 (2686VFX) には無い
    if (selectors.empty()) return;

    CHECK(env.processor->relativePathRoot == Io::PathRoot::settingsFolder);

    for (auto* s : selectors) CHECK(s->getSelectedItemIndex() == Io::PathRoot::settingsFolder);

    selectors.front()->setSelectedItemIndex(Io::PathRoot::documentFolder, juce::sendNotificationSync);

    CHECK(env.processor->relativePathRoot == Io::PathRoot::documentFolder);

    for (auto* s : selectors) CHECK(s->getSelectedItemIndex() == Io::PathRoot::documentFolder);

    selectors.back()->setSelectedItemIndex(Io::PathRoot::settingsFolder, juce::sendNotificationSync);

    CHECK(env.processor->relativePathRoot == Io::PathRoot::settingsFolder);

    for (auto* s : selectors) CHECK(s->getSelectedItemIndex() == Io::PathRoot::settingsFolder);
}
