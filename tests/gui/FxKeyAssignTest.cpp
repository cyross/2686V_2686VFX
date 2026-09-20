// ============================================================================
// 2686VFX: 変調を動かす鍵盤の割り当て (キーアサイン)
// ============================================================================
// 本物のプロセッサに音と MIDI を流して確かめる。
//
// 変調の形 (エンベロープの速さや LFO の波) には頼らない。同じ設定の
// プロセッサを 2 つ立て、片方だけ鍵盤を押して流し、出てきた音を比べる。
// 違えば「動いた」、同じなら「動いていない」。
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <cmath>
#include <functional>
#include <vector>

#include "Core/Processor/PluginProcessor.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "Core/Editor/PluginEditor.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Gui/Fx/GuiFx.h"
#include "Gui/Fx/GuiFxKeyAssign.h"
#include "Processor/Fx/ProcessorFxKeys.h"
#include "Processor/Mod/ProcessorModKeys.h"
#include "Processor/Mod/ProcessorModValues.h"

namespace
{
    namespace KA = ModPrKey::KeyAssign;

    constexpr double kRate = 48000.0;
    constexpr int kBlock = 480;   // 10 ミリ秒
    constexpr int kBlocks = 40;   // 0.4 秒

    juce::String modId(const juce::String& tail) { return ModPrKey::prefix + tail; }
    juce::String keyId(KA::Target t) { return modId(KA::targets[(size_t)t].key); }

    void setReal(AudioPlugin2686V& p, const juce::String& id, float value)
    {
        auto* param = p.apvts.getParameter(id);

        REQUIRE_MESSAGE(param != nullptr, id.toStdString());

        param->setValueNotifyingHost(param->convertTo0to1(value));
    }

    // 範囲の中ほど。速さなどの値の向きに頼らずに「ほどほど」を選ぶため。
    void setMid(AudioPlugin2686V& p, const juce::String& id, float ratio = 0.5f)
    {
        auto* param = p.apvts.getParameter(id);

        REQUIRE_MESSAGE(param != nullptr, id.toStdString());

        param->setValueNotifyingHost(ratio);
    }

    // ------------------------------------------------------------------
    // 区分ごとの「効くようにする」設定
    // ------------------------------------------------------------------
    void enableAmpEnv(AudioPlugin2686V& p)
    {
        setReal(p, modId(ModPrKey::Env::bypass), 0.0f);
        setReal(p, modId(CPK::adsr + CPK::bypass), 0.0f);
        setMid(p, modId(CPK::Adsr::ar), 0.6f);
        setMid(p, modId(CPK::Adsr::dr), 0.6f);
        setReal(p, modId(CPK::Adsr::sl), 0.3f);
    }

    void enableLfoAm(AudioPlugin2686V& p)
    {
        setReal(p, modId(ModPrKey::Lfo::bypass), 0.0f);
        setReal(p, modId(CPK::Opzx7Lfo::am), 1.0f);
        setMid(p, modId(CPK::Opzx7Lfo::ams), 1.0f);
        setMid(p, modId(CPK::Opzx7Lfo::amd), 1.0f);
        setMid(p, modId(CPK::Opzx7Lfo::amFreq), 0.7f);
    }

    void enableShift(AudioPlugin2686V& p)
    {
        setReal(p, modId(ModPrKey::Shift::bypass), 0.0f);
    }

    void enableMul(AudioPlugin2686V& p)
    {
        enableShift(p);

        // 並びの 2 番が x1.0。中ほどを選んで、はっきり音程を変える。
        setMid(p, modId(CPK::mul), 0.5f);
    }

    void enableUnison(AudioPlugin2686V& p)
    {
        enableShift(p);
        setReal(p, modId(CPK::Unison::voices), 4.0f);
        setMid(p, modId(CPK::Unison::detune), 0.5f);
    }

    void useCustom(AudioPlugin2686V& p)
    {
        setReal(p, modId(KA::mode), (float)ModPrValue::KeyAssign::custom);
    }

    // ------------------------------------------------------------------
    // 流す
    // ------------------------------------------------------------------
    struct Press
    {
        int note;
        int onBlock = 0;
        int offBlock = -1;   // 離さないときは -1
    };

    // 設定してから立ち上げ、音と鍵盤を流して左の出力を返す。
    //
    // 入力は 220 Hz の正弦波。音量の変化も音程の変化も出力に現れる。
    std::vector<float> render(const std::function<void(AudioPlugin2686V&)>& configure,
        const std::vector<Press>& presses)
    {
        auto processor = std::make_unique<AudioPlugin2686V>();

        // 後ろの効果 (リバーブなど) は切っておく。尾を引くと、離したあとの
        // 比べ合わせに前の違いが残ってしまう。
        setReal(*processor, FxPrKey::prefix + FxPrKey::bypass, 1.0f);

        configure(*processor);

        processor->setPlayConfigDetails(2, 2, kRate, kBlock);
        processor->prepareToPlay(kRate, kBlock);

        std::vector<float> out;
        out.reserve((size_t)(kBlock * kBlocks));

        juce::AudioBuffer<float> buffer(2, kBlock);

        for (int b = 0; b < kBlocks; ++b)
        {
            for (int i = 0; i < kBlock; ++i)
            {
                double t = (double)(b * kBlock + i) / kRate;
                float v = 0.5f * (float)std::sin(2.0 * juce::MathConstants<double>::pi * 220.0 * t);

                buffer.setSample(0, i, v);
                buffer.setSample(1, i, v);
            }

            juce::MidiBuffer midi;

            for (const auto& p : presses)
            {
                if (p.onBlock == b) midi.addEvent(juce::MidiMessage::noteOn(1, p.note, (juce::uint8)100), 0);
                if (p.offBlock == b) midi.addEvent(juce::MidiMessage::noteOff(1, p.note), 0);
            }

            processor->processBlock(buffer, midi);

            for (int i = 0; i < kBlock; ++i) out.push_back(buffer.getSample(0, i));
        }

        processor->releaseResources();

        return out;
    }

    // [from, to) のブロックの範囲で、2 つの出力の差の最大
    float maxDiff(const std::vector<float>& a, const std::vector<float>& b, int fromBlock = 0, int toBlock = kBlocks)
    {
        float m = 0.0f;

        for (int i = fromBlock * kBlock; i < toBlock * kBlock; ++i)
        {
            m = juce::jmax(m, std::abs(a[(size_t)i] - b[(size_t)i]));
        }

        return m;
    }

    constexpr float kMoved = 1.0e-3f;
    constexpr float kSame = 1.0e-6f;
}

TEST_CASE("キーアサイン: シングルキーアサインでは、どの鍵盤でも動く (これまでどおり)")
{
    auto configure = [](AudioPlugin2686V& p) { enableAmpEnv(p); };

    auto none = render(configure, {});

    for (int note : { 36, 60, 72, 100 })
    {
        CAPTURE(note);
        CHECK(maxDiff(render(configure, { { note } }), none) > kMoved);
    }
}

TEST_CASE("キーアサイン: 初期値はシングルキーアサインで、どの対象も C3 (60)")
{
    AudioPlugin2686V p;

    CHECK((int)std::lround(p.apvts.getRawParameterValue(modId(KA::mode))->load()) == ModPrValue::KeyAssign::single);

    for (int t = 0; t < KA::NumTargets; ++t)
    {
        CAPTURE(KA::targets[(size_t)t].name.toStdString());
        CHECK((int)std::lround(p.apvts.getRawParameterValue(keyId((KA::Target)t))->load()) == 60);
    }
}

TEST_CASE("キーアサイン: カスタマイズでは、割り当てた鍵盤でだけ動く")
{
    auto configure = [](AudioPlugin2686V& p) {
        enableAmpEnv(p);
        useCustom(p);
        setReal(p, keyId(KA::AmpEnv), 62.0f);
    };

    auto none = render(configure, {});

    // 割り当てていない鍵盤では何も起きない
    CHECK(maxDiff(render(configure, { { 60 } }), none) < kSame);
    CHECK(maxDiff(render(configure, { { 61 } }), none) < kSame);

    // 割り当てた鍵盤で動く
    CHECK(maxDiff(render(configure, { { 62 } }), none) > kMoved);
}

TEST_CASE("キーアサイン: 同じ鍵盤を複数の対象へ割り当てると、まとめて動く")
{
    auto configure = [](int ampKey, int lfoKey) {
        return [=](AudioPlugin2686V& p) {
            enableAmpEnv(p);
            enableLfoAm(p);
            useCustom(p);
            setReal(p, keyId(KA::AmpEnv), (float)ampKey);
            setReal(p, keyId(KA::LfoAm), (float)lfoKey);
        };
    };

    auto both = render(configure(64, 64), { { 64 } });
    auto ampOnly = render(configure(64, 65), { { 64 } });
    auto lfoOnly = render(configure(65, 64), { { 64 } });

    // どちらか片方だけのときと違う = 両方が動いた
    CHECK(maxDiff(both, ampOnly) > kMoved);
    CHECK(maxDiff(both, lfoOnly) > kMoved);
}

TEST_CASE("キーアサイン: 複数の鍵盤を押すと、それぞれに割り当てた処理がすべて動く")
{
    auto configure = [](AudioPlugin2686V& p) {
        enableAmpEnv(p);
        enableLfoAm(p);
        useCustom(p);
        setReal(p, keyId(KA::AmpEnv), 60.0f);
        setReal(p, keyId(KA::LfoAm), 67.0f);
    };

    auto both = render(configure, { { 60 }, { 67 } });

    CHECK(maxDiff(both, render(configure, { { 60 } })) > kMoved);
    CHECK(maxDiff(both, render(configure, { { 67 } })) > kMoved);
}

TEST_CASE("キーアサイン: カスタマイズでは、LFO・MUL/DET・UNISON は押している間だけ掛かる")
{
    struct Case
    {
        std::string name;
        KA::Target target;
        std::function<void(AudioPlugin2686V&)> enable;
    };

    const std::vector<Case> cases = {
        { "LFO AM", KA::LfoAm, enableLfoAm },
        { "MUL/DET", KA::MulDet, enableMul },
        { "UNISON/HARMONY", KA::Unison, enableUnison },
    };

    for (const auto& c : cases)
    {
        CAPTURE(c.name);

        auto configure = [&](AudioPlugin2686V& p) {
            c.enable(p);
            useCustom(p);
            setReal(p, keyId(c.target), 60.0f);
        };

        auto none = render(configure, {});

        // 押していなければ掛からない。割り当てていない鍵盤でも同じ。
        CHECK(maxDiff(render(configure, { { 61 } }), none) < kSame);

        // 押している間は掛かる
        auto held = render(configure, { { 60 } });

        CHECK(maxDiff(held, none, 1, kBlocks) > kMoved);

        // 離すと掛からなくなる。継ぎ目をなだらかにしたり、溜めた音が
        // 抜けたりするぶん、少し待ってから比べる。
        auto released = render(configure, { { 60, 0, 10 } });

        CHECK(maxDiff(released, none, 5, 10) > kMoved);
        CHECK(maxDiff(released, none, 20, kBlocks) < kSame);
    }
}

TEST_CASE("キーアサイン: シングルキーアサインでは、LFO・MUL/DET・UNISON は鍵盤に関係なく掛かる (これまでどおり)")
{
    // 何も押さなくても、区分を切ったときとは違う音になる
    auto off = render([](AudioPlugin2686V&) {}, {});

    CHECK(maxDiff(render(enableLfoAm, {}), off) > kMoved);
    CHECK(maxDiff(render(enableMul, {}), off) > kMoved);
    CHECK(maxDiff(render(enableUnison, {}), off) > kMoved);
}

TEST_CASE("キーアサイン: FX のパラメータファイルへ書き、読み戻せる")
{
    AudioPlugin2686V p;

    useCustom(p);

    for (int t = 0; t < KA::NumTargets; ++t)
    {
        setReal(p, keyId((KA::Target)t), (float)(40 + t * 3));
    }

    Io::ParamWriter writer({ "fxParam", 1 });

    GuiFxKeyAssign::writeParams(p.apvts, writer);

    // 全部をよそへ動かしてから読み戻す
    setReal(p, modId(KA::mode), (float)ModPrValue::KeyAssign::single);

    for (int t = 0; t < KA::NumTargets; ++t) setReal(p, keyId((KA::Target)t), 0.0f);

    GuiFxKeyAssign::readParams(p.apvts, writer.reader());

    CHECK((int)std::lround(p.apvts.getRawParameterValue(modId(KA::mode))->load()) == ModPrValue::KeyAssign::custom);

    for (int t = 0; t < KA::NumTargets; ++t)
    {
        CAPTURE(KA::targets[(size_t)t].name.toStdString());
        CHECK((int)std::lround(p.apvts.getRawParameterValue(keyId((KA::Target)t))->load()) == 40 + t * 3);
    }

    // キーアサインを持たないファイル (これまでのもの) では、いまの値のまま
    Io::ParamWriter old({ "fxParam", 1 });

    old.set("bypass", false);

    GuiFxKeyAssign::readParams(p.apvts, old.reader());

    CHECK((int)std::lround(p.apvts.getRawParameterValue(modId(KA::mode))->load()) == ModPrValue::KeyAssign::custom);
    CHECK((int)std::lround(p.apvts.getRawParameterValue(keyId(KA::Arpeggio))->load()) == 40 + 12 * 3);
}

TEST_CASE("キーアサイン: 一覧はカスタマイズのときだけ出る")
{
    AudioPlugin2686V processor;
    std::unique_ptr<juce::AudioProcessorEditor> editor(processor.createEditor());

    std::vector<juce::Component*> all;

    std::function<void(juce::Component*)> collect = [&](juce::Component* root) {
        for (auto* c : root->getChildren()) {
            all.push_back(c);
            collect(c);
        }
    };

    auto find = [&](const juce::String& id) -> juce::Component* {
        all.clear();
        collect(editor.get());

        for (auto* c : all) {
            if (c->getComponentID() == id) return c;
        }

        return nullptr;
    };

    auto* mode = find(modId(KA::mode));

    REQUIRE(mode != nullptr);
    CHECK(mode->isVisible());

    for (int t = 0; t < KA::NumTargets; ++t)
    {
        auto* key = find(keyId((KA::Target)t));

        CAPTURE(KA::targets[(size_t)t].name.toStdString());
        REQUIRE(key != nullptr);
        CHECK_FALSE(key->isVisible());
    }

    // カスタマイズへ切り替えると一覧が出る
    useCustom(processor);

    for (int t = 0; t < KA::NumTargets; ++t)
    {
        CAPTURE(KA::targets[(size_t)t].name.toStdString());
        CHECK(find(keyId((KA::Target)t))->isVisible());
    }

    // 戻すと消える
    setReal(processor, modId(KA::mode), (float)ModPrValue::KeyAssign::single);

    for (int t = 0; t < KA::NumTargets; ++t)
    {
        CHECK_FALSE(find(keyId((KA::Target)t))->isVisible());
    }

    editor.reset();
}

// 見出しの塗り直しは、画面の時計 (30 回 / 秒) から呼ばれる。テストでは
// 時計が回らないので、ここから直に呼ぶ。GuiFx が friend にしている。
struct GuiFxTestAccess
{
    static void refreshTitles(GuiFx& fx) { fx.updateKeyAssignTitles(); }
};

TEST_CASE("キーアサイン: カスタマイズでは、割り当てた鍵盤を押している区分だけ見出しが明るい")
{
    AudioPlugin2686V processor;

    processor.setPlayConfigDetails(2, 2, kRate, kBlock);
    processor.prepareToPlay(kRate, kBlock);

    std::unique_ptr<juce::AudioProcessorEditor> editor(processor.createEditor());

    std::vector<juce::Component*> all;

    std::function<void(juce::Component*)> collect = [&](juce::Component* root) {
        for (auto* c : root->getChildren()) {
            all.push_back(c);
            collect(c);
        }
    };

    collect(editor.get());

    GuiFx* fx = nullptr;

    for (auto* c : all) {
        if (auto* f = dynamic_cast<GuiFx*>(c)) fx = f;
    }

    REQUIRE(fx != nullptr);

    // 見出しは言語で変わらないものを選ぶ
    auto group = [&](const juce::String& title) -> GuiScrollGroup* {
        for (auto* c : all) {
            if (auto* g = dynamic_cast<GuiScrollGroup*>(c); g != nullptr && g->getText() == title) return g;
        }

        return nullptr;
    };

    auto* amp = group("AMP ENV");
    auto* lfo = group("LFO");
    auto* pitch = group("PITCH ENV");

    REQUIRE(amp != nullptr);
    REQUIRE(lfo != nullptr);
    REQUIRE(pitch != nullptr);

    juce::AudioBuffer<float> buffer(2, kBlock);

    auto refresh = [&] { GuiFxTestAccess::refreshTitles(*fx); };

    auto send = [&](const juce::MidiMessage& m) {
        buffer.clear();

        juce::MidiBuffer midi;
        midi.addEvent(m, 0);

        processor.processBlock(buffer, midi);

        refresh();
    };

    // 1. シングルキーアサインでは、鍵盤に関係なく全部明るい (これまでどおり)
    refresh();

    CHECK_FALSE(amp->isTitleIdle());
    CHECK_FALSE(lfo->isTitleIdle());
    CHECK_FALSE(pitch->isTitleIdle());

    // 2. カスタマイズにすると、押すまでは全部灰。LFO は AM と PM の 2 つを持つ
    setReal(processor, keyId(KA::AmpEnv), 60.0f);
    setReal(processor, keyId(KA::LfoAm), 62.0f);
    setReal(processor, keyId(KA::LfoPm), 62.0f);
    setReal(processor, keyId(KA::PitchEnv), 64.0f);

    useCustom(processor);

    CHECK(amp->isTitleIdle());
    CHECK(lfo->isTitleIdle());
    CHECK(pitch->isTitleIdle());

    // 3. 押した鍵盤を割り当てた区分だけが明るくなる
    send(juce::MidiMessage::noteOn(1, 60, (juce::uint8)100));

    CHECK_FALSE(amp->isTitleIdle());
    CHECK(lfo->isTitleIdle());
    CHECK(pitch->isTitleIdle());

    send(juce::MidiMessage::noteOn(1, 62, (juce::uint8)100));

    CHECK_FALSE(amp->isTitleIdle());
    CHECK_FALSE(lfo->isTitleIdle());
    CHECK(pitch->isTitleIdle());

    // 4. 離すと灰へ戻る
    send(juce::MidiMessage::noteOff(1, 60));

    CHECK(amp->isTitleIdle());
    CHECK_FALSE(lfo->isTitleIdle());

    send(juce::MidiMessage::noteOff(1, 62));

    CHECK(amp->isTitleIdle());
    CHECK(lfo->isTitleIdle());
    CHECK(pitch->isTitleIdle());

    // 5. 押したままでも、割り当てを変えれば追う
    send(juce::MidiMessage::noteOn(1, 64, (juce::uint8)100));

    CHECK_FALSE(pitch->isTitleIdle());

    setReal(processor, keyId(KA::PitchEnv), 65.0f);
    send(juce::MidiMessage::noteOff(1, 70));   // 関係のない鍵盤で 1 塊流す

    CHECK(pitch->isTitleIdle());

    // 6. シングルへ戻すと、押していなくても全部明るい
    setReal(processor, modId(KA::mode), (float)ModPrValue::KeyAssign::single);

    CHECK_FALSE(amp->isTitleIdle());
    CHECK_FALSE(lfo->isTitleIdle());
    CHECK_FALSE(pitch->isTitleIdle());

    editor.reset();
    processor.releaseResources();
}
