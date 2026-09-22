// ============================================================================
// ポリフォニックのボイスの上限と、非 FM チャンネルの MUL/DET・FIX
// ============================================================================
// どちらも、ビルドも警告も素通りして、鳴らして初めて分かった不具合。
//
// ボイスの上限
//   ボイスは「最大同時発音数 × 最大のユニゾン数」(10 × 8 = 80) 用意してある。
//   3.6.1 までは、ポリフォニックでこの 80 を全部使って音を割り当てていたので、
//   ユニゾン 1 でも 80 音まで重なった。短い音符を続けて弾くと、前の音の余韻が
//   残ったまま次のボイスが使われ、処理が追いつかなくなった。
//
// MUL/DET・FIX
//   RHYTHM のパッドは素材をそのままの高さで鳴らすので、MUL/DET と FIX で
//   動かした周波数を再生の速さへ掛けていなかった。どちらも効かなかった。
//   ほかの非 FM チャンネルは効いていたが、同じ取りこぼしが無いよう全部見る。
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <cmath>

#include "Core/Processor/PluginProcessor.h"
#include "Shared/Core/Const/ConstGlobal.h"
#include "Shared/Core/Processor/ProcessorKeys.h"

#include "./GuiTestChips.h"

namespace
{
    constexpr double kRate = 48000.0;
    constexpr int kBlock = 480; // 0.01 秒

    juce::File workDir()
    {
        auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory).getChildFile("VoicePitchTest");
        dir.createDirectory();
        return dir;
    }

    // 440Hz の正弦波 (3 秒)。PCM 系のチャンネルへ読ませる。
    juce::File sineWav()
    {
        auto file = workDir().getChildFile("sine440.wav");

        if (file.existsAsFile()) return file;

        constexpr int rate = 44100;
        constexpr int length = rate * 3;

        juce::AudioBuffer<float> sine(1, length);

        for (int i = 0; i < length; ++i) {
            sine.setSample(0, i, 0.5f * (float)std::sin(juce::MathConstants<double>::twoPi * 440.0 * i / rate));
        }

        juce::WavAudioFormat format;
        auto stream = file.createOutputStream();

        REQUIRE(stream != nullptr);

        std::unique_ptr<juce::AudioFormatWriter> writer(format.createWriterFor(stream.get(), rate, 1, 16, {}, 0));

        REQUIRE(writer != nullptr);

        stream.release();
        writer->writeFromAudioSampleBuffer(sine, 0, length);

        return file;
    }

    // 正弦波 1 周期 (32 点) の波形ファイル。WT+ へ読ませる。
    juce::File sineWt()
    {
        auto file = workDir().getChildFile("sine32.wt");
        juce::String text = "32\n";

        for (int i = 0; i < 32; ++i) {
            text << juce::String(std::sin(juce::MathConstants<double>::twoPi * i / 32.0), 4) << "\n";
        }

        file.replaceWithText(text);

        return file;
    }

    // 持っているチャンネルがプラグインごとに違うので、読み込みの口も本に
    // よって無い。あるときだけ呼ぶ。
    template <typename P>
    void finishEncoding(P& p)
    {
        if constexpr (requires { p.handleUpdateNowIfNeeded(); }) p.handleUpdateNowIfNeeded();
    }

    template <typename P>
    void loadRhythm(P& p, const juce::File& file)
    {
        if constexpr (requires { p.loadRhythmFile(file, 0); }) p.loadRhythmFile(file, 0);
    }

    template <typename P>
    void loadAdpcm(P& p, const juce::File& file)
    {
        if constexpr (requires { p.loadAdpcmFile(file); }) p.loadAdpcmFile(file);
    }

    template <typename P>
    void loadAdpcmPlus(P& p, const juce::File& file)
    {
        if constexpr (requires { p.loadAdpcmPlusFile(0, file); }) p.loadAdpcmPlusFile(0, file);
    }

    template <typename P>
    void loadWtPlus(P& p, const juce::File& file)
    {
        if constexpr (requires { p.loadWtPlusWaveFile(0, file); }) p.loadWtPlusWaveFile(0, file);
    }

    struct Rig
    {
        std::unique_ptr<AudioPlugin2686V> plugin{ std::make_unique<AudioPlugin2686V>() };
        juce::AudioBuffer<float> buffer{ 2, kBlock };

        Rig()
        {
            plugin->setRateAndBufferSizeDetails(kRate, kBlock);
            plugin->prepareToPlay(kRate, kBlock);
        }

        bool set(const juce::String& id, float value)
        {
            auto* param = plugin->apvts.getParameter(id);

            if (param == nullptr) return false;

            param->setValueNotifyingHost(param->convertTo0to1(value));

            return true;
        }

        // 1 ブロック流す。PCM の符号化はメッセージスレッドへ頼む作りなので、
        // テストではその場で済ませる。
        void run(juce::MidiBuffer midi = {})
        {
            buffer.clear();
            plugin->processBlock(buffer, midi);
            finishEncoding(*plugin);
        }

        // 鳴らして、0.1〜0.4 秒の間に L が負から正へ横切った回数を数える。
        // 無音なら -1。
        int crossings(int note)
        {
            // 符号化を済ませてから鳴らす。鳴らし始めてから素材が差し替わると、
            // その音の間は高さがずれる。
            for (int i = 0; i < 3; ++i) run();

            noteOn(note);

            // 立ち上がりの 0.1 秒は見ない
            for (int b = 1; b < 10; ++b) run();

            return countCrossings(30);
        }

        void noteOn(int note)
        {
            juce::MidiBuffer on;
            on.addEvent(juce::MidiMessage::noteOn(1, note, (juce::uint8)100), 0);

            run(on);
        }

        // blocks ブロック流し、L が負から正へ横切った回数を数える。無音なら -1。
        int countCrossings(int blocks)
        {
            int count = 0;
            float prev = 0.0f;
            float peak = 0.0f;

            for (int b = 0; b < blocks; ++b)
            {
                run();

                for (int i = 0; i < kBlock; ++i)
                {
                    const float s = buffer.getSample(0, i);

                    peak = std::max(peak, std::abs(s));

                    if (prev < 0.0f && s >= 0.0f) ++count;

                    prev = s;
                }
            }

            return (peak < 1.0e-4f) ? -1 : count;
        }
    };

    bool isFm(const juce::String& modeName)
    {
        return modeName == "OPNA" || modeName == "OPN" || modeName == "OPL"
            || modeName == "OPL3" || modeName == "OPM" || modeName == "OPZX7";
    }

    // チャンネルを選び、音が出るように整える。鳴らす鍵盤を返す。
    int prepareChannel(Rig& rig, int mode, const GuiTestChips::Entry& chip,
                       const juce::File& wav, const juce::File& wt)
    {
        const juce::String name(chip.modeName);
        const juce::String prefix(chip.prefix);
        int note = 69;

        REQUIRE(rig.set("MODE", (float)mode));

        if (name == "RHYTHM") {
            loadRhythm(*rig.plugin, wav);

            // パッドは割り当てた鍵盤で鳴る
            note = (int)rig.plugin->apvts.getRawParameterValue(prefix + "_NOTE")->load();
        }

        if (name == "ADPCM") loadAdpcm(*rig.plugin, wav);
        if (name == "ADPCM+") loadAdpcmPlus(*rig.plugin, wav);
        if (name == "WTPLUS") loadWtPlus(*rig.plugin, wt);

        // 初期の波形は自分で描くもの (空) なので、正弦波へ替える
        if (name == "WAVETABLE" || name == "WT2") rig.set(prefix + "_WAVE", 0.0f);

        // PCM 系は素材をそのままの細かさで持たせる。符号化のざらつきが
        // ゼロ交差の数を乱すため (86V は BIT の初期値が 12)。1 番が Raw。
        if (chip.isPcm) REQUIRE(rig.set(prefix + CPK::QualityPcm::mode, 1.0f));

        return note;
    }
}

TEST_CASE("ポリフォニック: 同時に鳴るボイスが、最大同時発音数 × ユニゾン数を超えない")
{
    // 波形を読まずに鳴るチャンネルを 1 つ選ぶ
    for (int m = 0; m < (int)OscMode::Count; ++m)
    {
        const auto* chip = GuiTestChips::byModeName(getModeName((OscMode)m));

        if (chip == nullptr || chip->isPcm) continue;

        const juce::String name(chip->modeName);
        const juce::String prefix(chip->prefix);

        if (name == "WTPLUS") continue;

        for (int unison : { 1, 2 })
        {
            Rig rig;

            REQUIRE(rig.set("MODE", (float)m));
            REQUIRE(rig.set("MONO_MODE", 0.0f));

            if (!rig.set(prefix + "_UNI_VOICES", (float)unison)) continue;

            if (name == "WAVETABLE" || name == "WT2") rig.set(prefix + "_WAVE", 0.0f);

            // 30 音を離さずに順に押す。余韻が長い音色で速く弾いたときと同じく、
            // どの音もボイスを握ったまま残る。
            for (int n = 0; n < 30; ++n)
            {
                juce::MidiBuffer on;
                on.addEvent(juce::MidiMessage::noteOn(1, 36 + n, (juce::uint8)100), 0);

                rig.run(on);
            }

            INFO(name.toStdString() << " / unison " << unison);

            const int limit = Global::voices * unison;

            CHECK(rig.plugin->getActiveVoiceCount() <= limit);

            // 上限いっぱいまでは使えている (上限が低すぎないこと)
            CHECK(rig.plugin->getActiveVoiceCount() == limit);
        }

        return;
    }
}

TEST_CASE("非 FM チャンネル: MUL/DET と FIX が音の高さに効く")
{
    const auto wav = sineWav();
    const auto wt = sineWt();

    // FM しか持たない本 (OPNV・OPLV・OPMV・OPZX7S) では、見るものが無いまま終わる

    for (int m = 0; m < (int)OscMode::Count; ++m)
    {
        const auto* chip = GuiTestChips::byModeName(getModeName((OscMode)m));

        if (chip == nullptr) continue;

        const juce::String name(chip->modeName);
        const juce::String prefix(chip->prefix);

        if (isFm(name)) continue;

        int note = 69;

        auto prepare = [&](Rig& rig) { note = prepareChannel(rig, m, *chip, wav, wt); };

        INFO(name.toStdString());

        int base = 0;

        {
            Rig rig;
            prepare(rig);
            base = rig.crossings(note);
        }

        REQUIRE(base > 0);

        // MUL の 7 番は ×2.0
        {
            Rig rig;
            prepare(rig);
            REQUIRE(rig.set(prefix + "_MUL", 7.0f));

            const int shifted = rig.crossings(note);

            CHECK(shifted == doctest::Approx(base * 2.0).epsilon(0.05));
        }

        // FIX は鍵盤の高さに関わらず決めた周波数で鳴る。素材や波形は
        // その鍵盤の高さで鳴っていたので、比の分だけ高くなる。
        {
            Rig rig;
            prepare(rig);
            REQUIRE(rig.set(prefix + "_FIX", 1.0f));

            const double noteHz = juce::MidiMessage::getMidiNoteInHertz(note);
            const double fixedHz = noteHz * 1.5;

            REQUIRE(rig.set(prefix + "_FREQ", (float)fixedHz));

            const int shifted = rig.crossings(note);

            CHECK(shifted == doctest::Approx(base * 1.5).epsilon(0.05));
        }
    }
}

// PCM 系は、QUALITY で符号化するモードを選ぶと、決めた標本化周波数へ
// 落とした素材を読む。鳴らしている最中に QUALITY を変えると読む素材が
// 差し替わるが、3.6.1 までは発音時の再生位置と進む量のままだったので、
// その音が終わるまで高さがずれていた (元の 44.1kHz → 16kHz なら 2.76 倍)。
TEST_CASE("PCM 系: 鳴らしている最中に QUALITY を変えても音の高さが変わらない")
{
    const auto wav = sineWav();

    for (int m = 0; m < (int)OscMode::Count; ++m)
    {
        const auto* chip = GuiTestChips::byModeName(getModeName((OscMode)m));

        if (chip == nullptr || !chip->isPcm) continue;

        const juce::String name(chip->modeName);
        const juce::String prefix(chip->prefix);
        const juce::String mode = prefix + CPK::QualityPcm::mode;
        const juce::String rate = prefix + CPK::QualityPcm::rate;

        // 1 番が Raw (元の 44.1kHz のまま)、13 番が YM2608 ADPCM。どちらからどちらへ
        // 替えても、読む素材の標本化周波数が変わる。
        for (const auto [from, to] : { std::pair{ 1, 13 }, std::pair{ 13, 1 } })
        {
            INFO(name.toStdString() << " / QUALITY " << from << " -> " << to);

            Rig rig;
            int note = 69;

            REQUIRE(rig.set("MODE", (float)m));

            if (name == "RHYTHM") {
                loadRhythm(*rig.plugin, wav);
                note = (int)rig.plugin->apvts.getRawParameterValue(prefix + "_NOTE")->load();
            }

            if (name == "ADPCM") loadAdpcm(*rig.plugin, wav);
            if (name == "ADPCM+") loadAdpcmPlus(*rig.plugin, wav);

            REQUIRE(rig.set(rate, 9.0f)); // 16kHz
            REQUIRE(rig.set(mode, (float)from));

            // 素材を整えてから鳴らし、しばらく数える
            for (int i = 0; i < 3; ++i) rig.run();

            rig.noteOn(note);

            for (int b = 0; b < 9; ++b) rig.run();

            const int before = rig.countCrossings(30);

            REQUIRE(before > 0);

            // 鳴らしたまま替える。符号化が済むまで数ブロック待ってから数える。
            REQUIRE(rig.set(mode, (float)to));

            for (int b = 0; b < 5; ++b) rig.run();

            const int after = rig.countCrossings(30);

            CHECK(after == doctest::Approx(before).epsilon(0.05));
        }
    }
}

// MUL/DET・FIX は、押したときの値で音の高さを決める。SETTINGS の
// 「MUL/DET・FIX を鳴らしながら反映」を入れると、鳴らしている最中に動かしても
// その場で効く。切っていれば 3.6.1 までと同じく、次に押した音から効く。
TEST_CASE("MUL/DET・FIX: SETTINGS で、鳴らしている最中にも反映するかを選べる")
{
    const auto wav = sineWav();
    const auto wt = sineWt();

    for (int m = 0; m < (int)OscMode::Count; ++m)
    {
        const auto* chip = GuiTestChips::byModeName(getModeName((OscMode)m));

        if (chip == nullptr) continue;

        const juce::String name(chip->modeName);
        const juce::String prefix(chip->prefix);

        // 動かす MUL と、動かした先の値。FM はオペレーターごとにあるので
        // 全部を 1 段ずつ上げる (表の並びは音源で違うが、どれも高くなる)。
        std::vector<std::pair<juce::String, float>> muls;

        auto collect = [&](Rig& rig) {
            muls.clear();

            const juce::StringArray ids = isFm(name)
                ? juce::StringArray{ prefix + "_OP0_MUL", prefix + "_OP1_MUL", prefix + "_OP2_MUL", prefix + "_OP3_MUL" }
                : juce::StringArray{ prefix + "_MUL" };

            for (const auto& id : ids)
            {
                auto* value = rig.plugin->apvts.getRawParameterValue(id);

                if (value == nullptr) continue;

                muls.push_back({ id, isFm(name) ? value->load() + 1.0f : 7.0f });
            }

            REQUIRE(!muls.empty());
        };

        auto applyMuls = [&](Rig& rig) {
            for (const auto& [id, v] : muls) REQUIRE(rig.set(id, v));
        };

        INFO(name.toStdString());

        int note = 69;
        int base = 0;
        int fresh = 0;

        {
            Rig rig;
            note = prepareChannel(rig, m, *chip, wav, wt);
            base = rig.crossings(note);
        }

        // 押す前に動かしておいたときの高さ。鳴らしながら反映したときは、これと同じになる。
        {
            Rig rig;
            prepareChannel(rig, m, *chip, wav, wt);
            collect(rig);
            applyMuls(rig);
            fresh = rig.crossings(note);
        }

        REQUIRE(base > 0);
        REQUIRE(std::abs(fresh - base) > base / 10);

        for (bool live : { true, false })
        {
            INFO("live " << live);

            Rig rig;
            prepareChannel(rig, m, *chip, wav, wt);
            collect(rig);

            rig.plugin->liveDetune = live;

            for (int i = 0; i < 3; ++i) rig.run();

            rig.noteOn(note);

            for (int b = 0; b < 9; ++b) rig.run();

            // 鳴らしたまま動かす
            applyMuls(rig);

            for (int b = 0; b < 2; ++b) rig.run();

            const int after = rig.countCrossings(30);

            CHECK(after == doctest::Approx(live ? fresh : base).epsilon(0.05));
        }
    }
}
