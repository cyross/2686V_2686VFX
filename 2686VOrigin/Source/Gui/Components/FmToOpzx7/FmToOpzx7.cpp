#include "./FmToOpzx7.h"

#include <algorithm>
#include <array>
#include <cmath>
#include <optional>

#include "./FmToOpzx7Tables.h"

#include "../../../Core/Const/ConstGlobal.h"
#include "../../../Core/Editor/PluginEditor.h"
#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Processor/ProcessorValues.h"

namespace
{
    using FmToOpzx7::Notes;

    // ファイルの中身を見分ける印。OPZX7S のチャンネルのファイルと同じもの
    const Io::ParamFormat opzx7Format{ "opzx7", 1 };

    // OPZX7S のオペレータの数
    constexpr int opzx7Ops = 8;

    // ------------------------------------------------------------------
    // 元のファイルから読む
    // ------------------------------------------------------------------
    // 選ぶ部品は ID (1 から) で持っている。番号 (0 から) へ直して範囲へ収める
    int readIndex(const Io::ParamReader& r, const char* key, int count, int fallbackIndex = 0)
    {
        return std::clamp(r.getInt(key, fallbackIndex + 1) - 1, 0, count - 1);
    }

    // レジスタ値。つまみは小数で持っているので丸める
    int readReg(const Io::ParamReader& r, const char* key, int fallback = 0)
    {
        return (int)std::lround(r.getFloat(key, (float)fallback));
    }

    // ------------------------------------------------------------------
    // 実機のレート表
    // ------------------------------------------------------------------
    // どの FM 音源のエンベロープも同じ式で時間を出している
    // (EnvFmRgAdssr / EnvFmRgAdddr / EnvOplAdsr / EnvOpzx7Adddr)。
    // 式を変えたときはここも合わせること。
    float rateTime(int effectiveRate, bool isAttack)
    {
        if (effectiveRate >= 60) return 0.001f;

        float diff = 60.0f - (float)effectiveRate;
        float seconds = 0.0003f * std::pow(2.0f, diff / 4.0f);

        if (isAttack) seconds *= 0.33f;

        return std::min(seconds, 15.0f) * 1.5f;
    }

    // レジスタ値を秒へ直す。キースケールの足し分は含めない。
    // 止まる (レート 0) ときは負を返す。
    float regToSeconds(int regVal, int regMax, bool isRelease, bool isAttack)
    {
        if (regMax <= 0) return -1.0f;

        if (regVal == 0 && !isRelease) return -1.0f;

        if (regVal <= 1 && isRelease) return Global::RateMaxSeconds::reg;

        int effectiveRate = std::min((int)((float)regVal * 63.0 / (float)regMax), 63);

        float seconds = rateTime(effectiveRate, isAttack);

        if (isRelease) return std::max(seconds, 0.005f);
        if (isAttack) return std::max(seconds, 0.001f);

        return std::max(seconds, 0.0001f);
    }

    const float realMin = CPV::Opzx7Real::Ar::min;
    const float realMax = CPV::Opzx7Real::Ar::max;

    // 秒を実数モードの範囲へ収める。はみ出したら知らせる。
    float fitSeconds(float seconds, const juce::String& name, Notes& notes)
    {
        if (seconds > realMax) {
            notes.add(name + juce::String("") + " が " + juce::String(realMax, 0) + " 秒を超えるので、" + juce::String(realMax, 0) + " 秒にしました");

            return realMax;
        }

        return std::max(seconds, realMin);
    }

    // 減衰 (dB) をレベル (1.0 = 減衰なし) へ
    float dbToLevel(float db)
    {
        return std::pow(10.0f, -db / 20.0f);
    }

    // 倍率 (1.0 に足す量) をセントへ直す
    float ratioToCents(float ratio)
    {
        if (ratio <= -1.0f) return -4800.0f;

        return 1200.0f * std::log2(1.0f + ratio);
    }

    // ------------------------------------------------------------------
    // エンベロープ
    // ------------------------------------------------------------------
    // レジスタのエンベロープ。無い段は 0 と最大値 0 を渡す。
    struct RegEnvelope
    {
        int ar = 0, arMax = 31;
        int dr = 0, drMax = 31;

        // 知らせに出す段の名前。音源の画面の表記に合わせる (OPM は D1R / D2R)
        const char* drName = "DR";
        const char* srName = "SR";

        // SR (OPN) / D2R (OPM)。OPL は持たないので 0
        int sr = 0, srMax = 31;

        int sl = 0;
        int rr = 0, rrMax = 15;
        int tl = 0;

        // OPL の EG TYPE が切 (減衰音)。SL へ達したあと RR で落ち続ける
        bool percussive = false;

        // OPL の SUS。離したあとはレート 5 で落ちる
        bool sus = false;

        bool xof = false;
        bool kor = false;
        bool bypass = false;
        bool mask = false;
    };

    void writeEnvelope(Io::ParamWriter& op, const RegEnvelope& env, Notes& notes)
    {
        // 実数モードで書く
        op.set("rgEn", false);

        // ---- AR ----
        float ar = regToSeconds(env.ar, env.arMax, false, true);

        if (ar < 0.0f) {
            // レート 0 は立ち上がらない。実数モードでは表せないので最長にする
            notes.add(juce::String("") + "AR が 0 (立ち上がらない) のオペレーターは、AR を最長の " + juce::String(realMax, 0) + " 秒にしました");

            ar = realMax;
        }

        op.set("ar", fitSeconds(ar, "AR", notes));

        // ---- TL ----
        op.set("tl", dbToLevel((float)env.tl * 0.75f));

        // ---- DR / SL ----
        float dr = regToSeconds(env.dr, env.drMax, false, false);

        if (dr < 0.0f) {
            // レート 0 は減衰しない。立ち上がった高さに留まるので、
            // 減衰の行き先を 1.0 にして、その先の SR も止める
            op.set("d1r", realMin);
            op.set("d1l", 1.0f);
            op.set("d2r", realMin);
        }
        else {
            op.set("d1r", fitSeconds(dr, env.drName, notes));
            op.set("d1l", env.sl >= 15 ? 0.0f : dbToLevel((float)env.sl * 3.0f));

            // ---- SR ----
            // 実数モードの SR は「最長時間に対する残りの割合」で持つ
            // (srTime = RateMaxSeconds::real * (1 - d2r))。
            // 下限 (0.001) 以下は減衰しない。0 はパラメータの範囲の外なので下限で書く
            auto srFromSeconds = [&](float seconds, const char* name) {
                if (seconds < 0.0f) return realMin;

                if (seconds >= Global::RateMaxSeconds::real) {
                    notes.add(juce::String(name) + " が " + juce::String(Global::RateMaxSeconds::real, 0) + " 秒を超えるので、減衰しない扱いにしました");

                    return realMin;
                }

                return std::clamp(1.0f - seconds / Global::RateMaxSeconds::real, realMin, 1.0f);
                };

            if (env.percussive) {
                // 減衰音は SL から RR で落ち続ける
                float rrSeconds = regToSeconds(env.sus ? 5 : env.rr, env.rrMax, true, false);

                op.set("d2r", env.kor ? realMin : srFromSeconds(rrSeconds, "RR"));
            }
            else {
                op.set("d2r", srFromSeconds(regToSeconds(env.sr, env.srMax, false, false), env.srName));
            }
        }

        // ---- RR ----
        // SUS (OPL) は離したあとをレート 5 で落とす。OPZX7S の SUS は
        // 時間の決まり方が違うので使わず、RR へ置き換える
        op.set("rr", fitSeconds(regToSeconds(env.sus ? 5 : env.rr, env.rrMax, true, false), "RR", notes));
        op.set("sus", false);

        op.set("xof", env.xof);
        op.set("kor", env.kor);
        op.set("bypass", env.bypass);
        op.set("mask", env.mask);

        // レジスタ側は OPZX7S の初期値のまま
        op.set("rgAr", CPV::Opzx7Rg::Ar::initial);
        op.set("rgD1r", CPV::Opzx7Rg::D1r::initial);
        op.set("rgD1l", CPV::Opzx7Rg::D1l::initial);
        op.set("rgD2r", CPV::Opzx7Rg::D2r::initial);
        op.set("rgRr", CPV::Opzx7Rg::Rr::initial);
        op.set("rgTl", CPV::Opzx7Rg::Tl::initial);
    }

    // ------------------------------------------------------------------
    // 周波数
    // ------------------------------------------------------------------
    // mul はレジスタ値 (0〜15)。useRatio のときは MUL.RATE の値を使う (OPM)。
    // dt1Scale は元の音源の DT1 の倍率 (0.001 なら +0.1%)。
    void writeDetune(Io::ParamWriter& op, int mul, bool useRatio, float mulRatio, float dt1Scale, int dt2, Notes& notes)
    {
        auto w = op.child("mulDetune");

        // OPZX7S の MUL は 0: x0.5 / 2: x1 / 7: x2 / 8〜20: x3〜x15 / 21: MUL.RATE
        int index = 2;

        if (useRatio) {
            index = 21;
        }
        else {
            mul = std::clamp(mul, 0, 15);

            if (mul == 0) index = 0;
            else if (mul == 1) index = 2;
            else if (mul == 2) index = 7;
            else index = mul + 5;
        }

        w.set("mul", index);
        w.set("mulRatio", useRatio ? mulRatio : CPV::Opzx7Detune::MulRatio::initial);

        // DT1 は OPZX7S の刻みと合わないので、同じ倍率になるセントで DT3 へ入れる
        int cents = (int)std::lround(ratioToCents(dt1Scale));

        if (dt1Scale != 0.0f) {
            notes.add(juce::String("") + "DT1 は OPZX7S の刻みと合わないため、同じ量のセントとして DT3 へ入れました (1 セント単位に丸めています)");
        }

        w.set("dt1", 0);
        w.set("dt2", (float)std::clamp(dt2, 0, 3));
        w.set("dt3", (float)cents);
    }

    // ------------------------------------------------------------------
    // キースケール
    // ------------------------------------------------------------------
    // OPN / OPM (OPZ の刻みと同じ): ksr 0〜3、ksl 0〜99
    void writeKeyScaleOpz(Io::ParamWriter& op, int ksr, int ksl, Notes& notes)
    {
        const bool used = ksr > 0 || ksl > 0;

        op.set("ksEn", used);
        op.set("ksMode", 2); // OPZ
        op.set("ksrMA7", CPV::Opzx7Ks::KsMA7::initial);
        op.set("kslMA7", CPV::Opzx7Ks::KslMA7::initial + 1);
        op.set("ksrOPZ", (float)std::clamp(ksr, 0, 3));
        op.set("kslOPZ", (float)std::clamp(ksl, 0, 99));
        op.set("ksBp", (float)CPV::Opzx7Ks::KsBp::initial);
        op.set("ksLc", CPV::Opzx7Ks::KsLc::initial + 1);
        op.set("ksRc", CPV::Opzx7Ks::KsRc::initial + 1);
        op.set("ksLd", (float)CPV::Opzx7Ks::KsLd::initial);
        op.set("ksRd", (float)CPV::Opzx7Ks::KsRd::initial);
        op.set("ksRs", (float)CPV::Opzx7Ks::KsRs::initial);

        if (ksr > 0) {
            notes.add(juce::String("") + "実数モードのキースケール (KSR) は時間の縮め方がレジスタとは違うため、近い効き方になります");
        }
    }

    // OPL / OPL3 (MA-7 の刻みと同じ): ksr 入切、ksl 0〜3
    void writeKeyScaleMa7(Io::ParamWriter& op, bool ksr, int ksl, Notes& notes)
    {
        // OPL は KSR が切でも鍵盤で少し速まる。実数モードでは入れたときだけ効くので、
        // どちらかを使っていれば入れる
        op.set("ksEn", true);
        op.set("ksMode", 1); // MA-7
        op.set("ksrMA7", ksr);
        op.set("kslMA7", std::clamp(ksl, 0, 3) + 1);
        op.set("ksrOPZ", (float)CPV::Opzx7Ks::KsrOPZ::initial);
        op.set("kslOPZ", (float)CPV::Opzx7Ks::KslOPZ::initial);
        op.set("ksBp", (float)CPV::Opzx7Ks::KsBp::initial);
        op.set("ksLc", CPV::Opzx7Ks::KsLc::initial + 1);
        op.set("ksRc", CPV::Opzx7Ks::KsRc::initial + 1);
        op.set("ksLd", (float)CPV::Opzx7Ks::KsLd::initial);
        op.set("ksRd", (float)CPV::Opzx7Ks::KsRd::initial);
        op.set("ksRs", (float)CPV::Opzx7Ks::KsRs::initial);

        if (ksr) {
            notes.add(juce::String("") + "実数モードのキースケール (KSR) は時間の縮め方がレジスタとは違うため、近い効き方になります");
        }
    }

    // ------------------------------------------------------------------
    // LFO
    // ------------------------------------------------------------------
    // OPZX7S の LFO の形の番号
    namespace Shape
    {
        constexpr int sine = 0;
        constexpr int sawUp = 1;
        constexpr int sawDown = 2;
        constexpr int square = 3;
        constexpr int triangle = 4;
        constexpr int sampleHold = 5;
        constexpr int sawDownOneShot = 6;
        constexpr int triangleOneShot = 7;
    }

    // 片側ぶん。深さは PM ならセント、AM なら dB の最大幅で渡す。
    struct LfoSide
    {
        bool enable = false;
        float freq = 5.0f;
        int shape = Shape::triangle;
        int syncDelay = 0;
        float depth = 0.0f;
        float smooth = 0.005f;
    };

    void writeHold(Io::ParamWriter& w)
    {
        w.set("holdEnable", CPV::WaveHold::Hold::Enable::initial);
        w.set("holdCount", CPV::WaveHold::Hold::Count::initial);
        w.set("holdTarget", CPV::WaveHold::Hold::Target::initial);
        w.set("holdMin", CPV::WaveHold::Hold::Bipolar::Min::initial);
        w.set("holdMax", CPV::WaveHold::Hold::Bipolar::Max::initial);

        w.set("keepEnable", false);
        w.set("waveStart", 0.0f);
        w.set("keepStart", false);
        w.set("waveEnd", 1.0f);
        w.set("keepEnd", false);
    }

    void writeLfo(Io::ParamWriter& parent, const juce::String& key, const LfoSide& pm, const LfoSide& am, Notes& notes)
    {
        auto w = parent.child(key);

        // 使っていない側は、はみ出していても知らせない
        auto fitFreq = [&notes](float freq, bool enable) {
            const float lo = CPV::Opzx7Lfo::PmFreq::min;
            const float hi = CPV::Opzx7Lfo::PmFreq::max;

            if (enable && (freq > hi || freq < lo)) {
                notes.add(juce::String("") + "LFO の速さは " + juce::String(lo, 1) + "〜" + juce::String(hi, 0) + "Hz に収めました");
            }

            return std::clamp(freq, lo, hi);
            };

        // 深さは SENS × DEPTH で決まる。SENS を 1 にして DEPTH で表す
        auto fitDepth = [&notes](float ratio, bool enable) {
            if (enable && ratio > 1.0f) {
                notes.add(juce::String("") + "LFO の深さが OPZX7S の上限を超えるので、上限にしました");

                return 1.0f;
            }

            return std::max(ratio, 0.0f);
            };

        const float pmDepth = std::min(fitDepth(std::abs(pm.depth) / 1200.0f, pm.enable), 1.0f);
        const float amDepth = std::min(fitDepth(std::abs(am.depth) / Global::Lfo::maxAmDepthDb, am.enable), 1.0f);

        w.set("pmEnable", pm.enable && pmDepth > 0.0f);
        w.set("pmFreq", fitFreq(pm.freq, pm.enable));
        w.set("pmSyncDelay", (float)std::clamp(pm.syncDelay, CPV::Opzx7Lfo::SyncDelay::min, CPV::Opzx7Lfo::SyncDelay::max));
        w.set("pgShape", pm.shape);
        w.set("pms", pmDepth > 0.0f ? 1.0f : 0.0f);
        w.set("pmd", pmDepth);

        w.set("amEnable", am.enable && amDepth > 0.0f);
        w.set("amFreq", fitFreq(am.freq, am.enable));
        w.set("egShape", am.shape);
        w.set("amSyncDelay", (float)std::clamp(am.syncDelay, CPV::Opzx7Lfo::SyncDelay::min, CPV::Opzx7Lfo::SyncDelay::max));
        w.set("amSmRt", std::clamp(am.smooth, CPV::Opzx7Lfo::AmSmRt::min, CPV::Opzx7Lfo::AmSmRt::max));
        w.set("ams", amDepth > 0.0f ? 1.0f : 0.0f);
        w.set("amd", amDepth);

        auto pmHold = w.child("pmHold");
        auto amHold = w.child("amHold");

        writeHold(pmHold);
        writeHold(amHold);
    }

    // N88 LFO の形 (0: Saw Up / 1: Square / 2: Triangle / 3: S&H / 4: Saw Down 1 回 / 5: Triangle 1 回)
    constexpr std::array<int, 6> n88Shapes = {
        Shape::sawUp, Shape::square, Shape::triangle, Shape::sampleHold, Shape::sawDownOneShot, Shape::triangleOneShot };

    // ------------------------------------------------------------------
    // オペレータのその他
    // ------------------------------------------------------------------
    // 波形・PCM の再生・ループ・SSG-EG など、元の音源に無い項目を
    // OPZX7S の初期値で埋める。ws は波形の ID (1 = Sine)。
    void writeOpDefaults(Io::ParamWriter& op, int wsId)
    {
        op.set("ws", wsId);

        op.set("pcmFile", juce::String());
        op.set("wtFile", juce::String());
        op.set("wt2File", juce::String());

        op.set("pcmOffset", 0.0f);
        op.set("speed", CPV::Speed::initial);
        op.set("pcmRatio", 1.0f);
        op.set("loopPointEnable", false);
        op.set("loopPointStart", CPV::Lp::Start::initial);
        op.set("loopPointEnd", CPV::Lp::End::initial);
        op.set("loopCount", (float)CPV::Lp::Count::initial);

        // SSG-EG は OPNA が後から上書きする
        op.set("se", 1);
        op.set("seFreq", 1.0f);
    }

    // FIX を持たない音源 (OPL / OPL3) は切にしておく
    void writeFixOff(Io::ParamWriter& op)
    {
        auto fix = op.child("fix");

        fix.set("enable", CPV::FixMode::Enable::initial);
        fix.set("freq", CPV::FixMode::Freq::initial);
    }

    // 使わないオペレータ。どこにも繋がないので音には出ない。
    void writeUnusedOp(Io::ParamWriter& op)
    {
        // 値は OPZX7S の初期値にしておく
        op.set("rgEn", CPV::Opzx7Rg::Enable::initial);
        op.set("rgAr", CPV::Opzx7Rg::Ar::initial);
        op.set("rgD1r", CPV::Opzx7Rg::D1r::initial);
        op.set("rgD1l", CPV::Opzx7Rg::D1l::initial);
        op.set("rgD2r", CPV::Opzx7Rg::D2r::initial);
        op.set("rgRr", CPV::Opzx7Rg::Rr::initial);
        op.set("rgTl", CPV::Opzx7Rg::Tl::initial);

        op.set("ar", CPV::Opzx7Real::Ar::initial);
        op.set("d1r", CPV::Opzx7Real::D1r::initial);
        op.set("d1l", CPV::Opzx7Real::D1l::initial);
        op.set("d2r", CPV::Opzx7Real::D2r::initial);
        op.set("rr", CPV::Opzx7Real::Rr::initial);
        op.set("tl", CPV::Opzx7Real::Tl::initial);
        op.set("sus", false);
        op.set("xof", false);
        op.set("kor", false);
        op.set("bypass", false);
        op.set("mask", false);

        Notes ignored;

        writeDetune(op, 1, false, 1.0f, 0.0f, 0, ignored);

        writeKeyScaleOpz(op, 0, 0, ignored);
        op.set("ksMode", CPV::Opzx7Ks::KsMode::initial + 1);

        writeOpDefaults(op, 1);

        LfoSide off;

        writeLfo(op, "lfo", off, off, ignored);

        writeFixOff(op);
    }

    // 元の音源の中身にあって、OPZX7S では意味が違う・使わない項目を外す
    void removeSourceKeys(Io::ParamWriter& root, int sourceOps)
    {
        for (const char* key : { "feedback", "pan", "lfoFreq", "lfoShape", "lfoAmSmRt", "lfoSyncDelay",
            "lfoPm", "lfoPms", "lfoPmd", "lfoAm", "lfoAmd", "lfoPgShape", "lfoEgShape", "lfoAms" })
        {
            root.remove(key);
        }

        for (int i = 0; i < sourceOps; ++i)
        {
            auto op = root.arrayItem(Io::ParamKey::ops, i);

            for (const char* key : { "mul", "mulRatio", "dt", "dt1", "dt2", "dr", "sl", "sr",
                "ks", "ksr", "ksl", "ksrOPP", "kslOPP", "egType", "eg", "freq", "syncDelay",
                "pm", "pms", "am", "ams", "amd", "pmd", "vib", "n88Ams", "ssgEnv", "ssgEnvFreq", "amsEnable" })
            {
                op.remove(key);
            }
        }
    }

    // ------------------------------------------------------------------
    // アルゴリズムとフィードバック
    // ------------------------------------------------------------------
    // レジスタの FB を OPZX7S の値 (-8.0〜8.0) へ直す。
    // 元の音源の深さは 2^(fb - sub)、OPZX7S は 2^(|x| - 5)。
    float feedbackValue(int fb, float sub, Notes& notes)
    {
        if (fb <= 0) return 0.0f;

        // 2^(fb - sub) = 2^(x - 5) → x = fb - sub + 5
        float x = (float)fb - sub + 5.0f;

        if (x <= 0.0f) {
            // OPZX7S は 2^-5 より浅くできない
            notes.add(juce::String("") + "FB が浅すぎて OPZX7S では表せないため、表せる一番浅い値へ寄せました");

            return 0.001f;
        }

        // OPZX7S の FB の範囲 (-8.0〜8.0)。86V は OPZX7 の定数を持たないので値で書く
        return std::clamp(x, -8.0f, 8.0f);
    }

    // 元の音源のつなぎ方をアルゴリズムマトリックスへ写す。
    // フィードバックは、戻りを受けるオペレータへ入れる。
    void writeRouting(Io::ParamWriter& root, const FmToOpzx7::Tables::Routing& routing, int sourceOps, float feedback)
    {
        root.set("algMode", 1);
        root.set("alg", 1);

        auto matrix = root.child("algMatrixComp");

        for (int i = 0; i < opzx7Ops; ++i)
        {
            auto op = matrix.arrayItem("ops", i);

            std::vector<int> mod(opzx7Ops, 0);
            std::vector<int> fbMod(opzx7Ops, 0);

            bool carrier = false;
            bool feedbackTarget = false;

            if (i < sourceOps)
            {
                carrier = routing.out[(size_t)i];

                // 並びは [src][dest]。元のつなぎ方は [dest][src] で持っている
                for (int j = 0; j < sourceOps; ++j)
                {
                    mod[(size_t)j] = routing.mod[(size_t)j][(size_t)i] ? 1 : 0;
                    fbMod[(size_t)j] = routing.fbMod[(size_t)j][(size_t)i] ? 1 : 0;

                    if (routing.fbMod[(size_t)i][(size_t)j]) feedbackTarget = true;
                }
            }

            op.set("isCarrier", carrier);
            op.setArray("mod", mod);
            op.setArray("fbMod", fbMod);

            root.set("feedback" + juce::String(i + 1), feedbackTarget ? feedback : 0.0f);
        }
    }

    // ------------------------------------------------------------------
    // パン
    // ------------------------------------------------------------------
    // 元の音源のパン (-1: 左 / 0: 中央 / 1: 右)
    void writePan(Io::ParamWriter& root, int pan, Notes& notes)
    {
        // OPZX7S のパンは 0〜31 の 32 段で、端でも反対側がわずかに鳴る
        if (pan == 0) {
            root.set("panpotEnable", false);
            root.set("panpot", (float)CPV::Panpot::Panpot::initial);

            return;
        }

        root.set("panpotEnable", true);
        root.set("panpot", pan < 0 ? (float)CPV::Panpot::Panpot::min : (float)CPV::Panpot::Panpot::max);

        notes.add(juce::String("") + "左右どちらかだけのパンは、OPZX7S のパンの端 (反対側がわずかに鳴る) にしました");
    }

    // 元の音源に無いオペレータを埋める
    void writeUnusedOps(Io::ParamWriter& out, int sourceOps)
    {
        for (int i = sourceOps; i < opzx7Ops; ++i)
        {
            auto op = out.arrayItem(Io::ParamKey::ops, i);

            writeUnusedOp(op);
        }
    }

    // ==================================================================
    // 音源ごとの変換
    // ==================================================================
    // OPNA / OPN。違いは OPNA のパン・ハードウェア LFO・SSG-EG だけ
    void convertOpn(const Io::ParamReader& src, Io::ParamWriter& out, Notes& notes, bool opna)
    {
        constexpr int sourceOps = 4;

        removeSourceKeys(out, sourceOps);

        writeRouting(out, FmToOpzx7::Tables::opnRoutings[(size_t)readIndex(src, "alg", 8)], sourceOps,
            feedbackValue(readReg(src, "feedback"), 8.0f, notes));

        // OPN はパンを持たない
        writePan(out, opna ? readReg(src, "pan") : 0, notes);

        // ---- N88 LFO ----
        // PM はチャンネル全体へ同じだけ掛かるので、OPZX7S の全体の LFO へ入れる。
        // AM はオペレータごとの N88 AMS で深さが変わるので、オペレータの LFO へ入れる。
        const float n88Freq = src.getFloat("lfoFreq") * 60.0f / 65536.0f;
        const int n88Shape = n88Shapes[(size_t)readIndex(src, "lfoShape", 6)];
        const int n88Delay = readReg(src, "lfoSyncDelay");
        const float n88Smooth = src.getFloat("lfoAmSmRt", CPV::Opzx7Lfo::AmSmRt::initial);
        const bool n88PmOn = src.getBool("lfoPm");
        const bool n88AmOn = src.getBool("lfoAm");
        const float n88Pmd = src.getFloat("lfoPmd");
        const float n88Amd = src.getFloat("lfoAmd");

        if ((n88PmOn && n88Pmd < 0.0f) || (n88AmOn && n88Amd < 0.0f)) {
            notes.add(juce::String("") + "N88 LFO の PMD / AMD が負 (逆向きの揺れ) のものは、向きを戻しました");
        }

        LfoSide glPm;
        glPm.enable = n88PmOn;
        glPm.freq = n88Freq;
        glPm.shape = n88Shape;
        glPm.syncDelay = n88Delay;
        glPm.depth = (std::abs(n88Pmd) / 127.0f) * (src.getFloat("lfoPms") / 15.0f) * 1200.0f;

        writeLfo(out, "glLfo", glPm, LfoSide{}, notes);

        // ---- オペレータ ----
        for (int i = 0; i < sourceOps; ++i)
        {
            const auto s = src.arrayItem(Io::ParamKey::ops, i);
            auto op = out.arrayItem(Io::ParamKey::ops, i);

            RegEnvelope env;

            env.ar = readReg(s, "ar");
            env.arMax = CPV::OpnRg::Ar::max;
            env.dr = readReg(s, "dr");
            env.drMax = CPV::OpnRg::Dr::max;
            env.sr = readReg(s, "sr");
            env.srMax = CPV::OpnRg::Sr::max;
            env.sl = readReg(s, "sl");
            env.rr = readReg(s, "rr");
            env.rrMax = CPV::OpnRg::Rr::max;
            env.tl = readReg(s, "tl");
            env.xof = s.getBool("xof");
            env.kor = s.getBool("kor");
            env.bypass = s.getBool("bypass");
            env.mask = s.getBool("mask");

            writeEnvelope(op, env, notes);

            writeDetune(op, readIndex(s, "mul", 16, 1), false, 1.0f,
                FmToOpzx7::Tables::opnDtScales[(size_t)readIndex(s, "dt", 8)], 0, notes);

            writeKeyScaleOpz(op, readIndex(s, "ks", 4), 0, notes);

            writeOpDefaults(op, 1);

            LfoSide pmSide;
            LfoSide amSide;

            const bool n88AmUsed = n88AmOn && s.getFloat("n88Ams") > 0.0f;

            auto setN88Am = [&] {
                amSide.enable = true;
                amSide.freq = n88Freq;
                amSide.shape = n88Shape;
                amSide.syncDelay = n88Delay;
                amSide.depth = (std::abs(n88Amd) / 127.0f) * (s.getFloat("n88Ams") / 15.0f) * Global::Lfo::maxAmDepthDb;
                amSide.smooth = n88Smooth;
                };

            if (opna) {
                // SSG-EG は OPZX7S にも同じものがある
                op.set("se", std::clamp(s.getInt("ssgEnv", 1), 1, 9));
                op.set("seFreq", s.getFloat("ssgEnvFreq", 1.0f));

                // ---- ハードウェア LFO と N88 AMS ----
                const float hwFreq = FmToOpzx7::Tables::opnaLfoFreqs[(size_t)readIndex(s, "freq", 8)];

                pmSide.enable = s.getBool("pm");
                pmSide.freq = hwFreq;
                pmSide.shape = Shape::triangle;
                pmSide.syncDelay = readReg(s, "syncDelay");
                pmSide.depth = ratioToCents(FmToOpzx7::Tables::opnaPmsDepths[(size_t)readIndex(s, "pms", 8)]);

                if (s.getBool("am")) {
                    amSide.enable = true;
                    amSide.freq = hwFreq;
                    amSide.shape = Shape::triangle;
                    amSide.syncDelay = readReg(s, "syncDelay");
                    amSide.depth = FmToOpzx7::Tables::opnaAmsDepths[(size_t)readIndex(s, "ams", 4)] * Global::Lfo::maxAmDepthDb;
                    amSide.smooth = CPV::Opzx7Lfo::AmSmRt::initial;

                    if (n88AmUsed) {
                        notes.add(juce::String("") + "HW LFO と N88 LFO の AM を両方使っているオペレーターは、HW LFO の AM だけを使いました");
                    }
                }
                else if (n88AmUsed) {
                    setN88Am();
                }
            }
            else if (n88AmUsed) {
                setN88Am();
            }

            writeLfo(op, "lfo", pmSide, amSide, notes);
        }

        writeUnusedOps(out, sourceOps);
    }

    // OPL / OPL3。違いはオペレータの数・FB の深さ・SUS・波形の数
    void convertOpl(const Io::ParamReader& src, Io::ParamWriter& out, Notes& notes, bool opl3)
    {
        const int sourceOps = opl3 ? 4 : 2;

        removeSourceKeys(out, sourceOps);

        const auto& routing = opl3
            ? FmToOpzx7::Tables::opl3Routings[(size_t)readIndex(src, "alg", 8)]
            : FmToOpzx7::Tables::oplRoutings[(size_t)readIndex(src, "alg", 2)];

        writeRouting(out, routing, sourceOps, feedbackValue(readReg(src, "feedback"), opl3 ? 5.0f : 8.0f, notes));

        // OPL はパンも、チャンネル全体の LFO も持たない
        writePan(out, 0, notes);
        writeLfo(out, "glLfo", LfoSide{}, LfoSide{}, notes);

        // ---- オペレータ ----
        for (int i = 0; i < sourceOps; ++i)
        {
            const auto s = src.arrayItem(Io::ParamKey::ops, i);
            auto op = out.arrayItem(Io::ParamKey::ops, i);

            RegEnvelope env;

            env.ar = readReg(s, "ar");
            env.arMax = CPV::OplRg::Ar::max;
            env.dr = readReg(s, "dr");
            env.drMax = CPV::OplRg::Dr::max;
            env.srMax = 0;
            env.sl = readReg(s, "sl");
            env.rr = readReg(s, "rr");
            env.rrMax = CPV::OplRg::Rr::max;
            env.tl = readReg(s, "tl");
            env.percussive = !s.getBool("egType", true);
            env.sus = !opl3 && s.getBool("sus");
            env.xof = s.getBool("xof");
            env.kor = s.getBool("kor");
            env.bypass = s.getBool("bypass");
            env.mask = s.getBool("mask");

            writeEnvelope(op, env, notes);

            writeDetune(op, readIndex(s, "mul", 16, 1), false, 1.0f, 0.0f, 0, notes);

            writeKeyScaleMa7(op, s.getBool("ksr"), readIndex(s, "ksl", 4), notes);

            // 波形は同じ名前のものへ。0〜6 は MA-7 と同じ並び、
            // OPL3 の 7 (Derived Square)・8 (Round Square)・9 (Log Saw) は OPZX7S の別の番号にある
            static constexpr std::array<int, 10> opl3Waves = { 1, 2, 3, 4, 5, 6, 7, 34, 41, 8 };

            writeOpDefaults(op, opl3 ? opl3Waves[(size_t)readIndex(s, "eg", 10)] : readIndex(s, "eg", 4) + 1);

            // FIX は OPL に無い
            writeFixOff(op);

            // ---- AM / VIB ----
            // OPL の AMS / PMS は速さ (Hz)、AMD は dB、PMD はセント
            LfoSide pmSide;
            pmSide.enable = s.getBool("vib");
            pmSide.freq = s.getFloat("pms", 6.4f);
            pmSide.shape = Shape::triangle;
            pmSide.depth = s.getFloat("pmd");

            LfoSide amSide;
            amSide.enable = s.getBool("am");
            amSide.freq = s.getFloat("ams", 3.7f);
            amSide.shape = Shape::triangle;
            amSide.depth = s.getFloat("amd");

            writeLfo(op, "lfo", pmSide, amSide, notes);
        }

        writeUnusedOps(out, sourceOps);
    }

    void convertOpm(const Io::ParamReader& src, Io::ParamWriter& out, Notes& notes)
    {
        constexpr int sourceOps = 4;

        removeSourceKeys(out, sourceOps);

        writeRouting(out, FmToOpzx7::Tables::opnRoutings[(size_t)readIndex(src, "alg", 8)], sourceOps,
            feedbackValue(readReg(src, "feedback"), 8.0f, notes));

        writePan(out, readReg(src, "pan"), notes);

        // ---- OPM LFO ----
        // PM はチャンネル全体へ同じだけ掛かるので、OPZX7S の全体の LFO へ入れる。
        // AM は AMS-EN を入れたオペレータにだけ掛かるので、オペレータの LFO へ入れる。
        static constexpr std::array<int, 4> pmShapes = { Shape::sawUp, Shape::square, Shape::triangle, Shape::sampleHold };
        static constexpr std::array<int, 4> amShapes = { Shape::sawDown, Shape::square, Shape::triangle, Shape::sampleHold };

        const float lfoFreq = src.getFloat("lfoFreq", 5.0f);
        const int lfoDelay = readReg(src, "lfoSyncDelay");

        LfoSide glPm;
        glPm.enable = src.getBool("lfoPm");
        glPm.freq = lfoFreq;
        glPm.shape = pmShapes[(size_t)readIndex(src, "lfoPgShape", 4)];
        glPm.syncDelay = lfoDelay;
        glPm.depth = ratioToCents(FmToOpzx7::Tables::opmPmsDepths[(size_t)readIndex(src, "lfoPms", 8)]
            * (src.getFloat("lfoPmd") / 127.0f));

        writeLfo(out, "glLfo", glPm, LfoSide{}, notes);

        LfoSide opAm;
        opAm.freq = lfoFreq;
        opAm.shape = amShapes[(size_t)readIndex(src, "lfoEgShape", 4)];
        opAm.syncDelay = lfoDelay;
        opAm.depth = FmToOpzx7::Tables::opmAmsDepths[(size_t)readIndex(src, "lfoAms", 4)]
            * (src.getFloat("lfoAmd") / 127.0f) * Global::Lfo::maxAmDepthDb;
        opAm.smooth = src.getFloat("lfoAmSmRt", CPV::Opzx7Lfo::AmSmRt::initial);

        const bool lfoAmOn = src.getBool("lfoAm");

        // ---- オペレータ ----
        for (int i = 0; i < sourceOps; ++i)
        {
            const auto s = src.arrayItem(Io::ParamKey::ops, i);
            auto op = out.arrayItem(Io::ParamKey::ops, i);

            RegEnvelope env;

            env.ar = readReg(s, "ar");
            env.arMax = CPV::OpmRg::Ar::max;
            env.dr = readReg(s, "d1r");
            env.drMax = CPV::OpmRg::D1r::max;
            env.drName = "D1R";
            env.sr = readReg(s, "d2r");
            env.srMax = CPV::OpmRg::D2r::max;
            env.srName = "D2R";
            env.sl = readReg(s, "d1l");
            env.rr = readReg(s, "rr");
            env.rrMax = CPV::OpmRg::Rr::max;
            env.tl = readReg(s, "tl");
            env.xof = s.getBool("xof");
            env.kor = s.getBool("kor");
            env.bypass = s.getBool("bypass");
            env.mask = s.getBool("mask");

            writeEnvelope(op, env, notes);

            // MUL の 16 番目は MUL.RATE の値を使う
            const int mulIndex = readIndex(s, "mul", 17, 1);

            writeDetune(op, mulIndex, mulIndex >= 16, s.getFloat("mulRatio", 1.0f),
                FmToOpzx7::Tables::opmDtScales[(size_t)(readReg(s, "dt1") & 7)], readReg(s, "dt2"), notes);

            // OPM の KS はレートだけ、OPP はレートとレベル。どちらも OPZ と同じ刻み
            if (readIndex(s, "ksMode", 2) == 1) {
                writeKeyScaleOpz(op, readReg(s, "ksrOPP"), readReg(s, "kslOPP"), notes);
            }
            else {
                writeKeyScaleOpz(op, readIndex(s, "ks", 4), 0, notes);
            }

            writeOpDefaults(op, 1);

            LfoSide amSide = opAm;
            amSide.enable = lfoAmOn && s.getBool("amsEnable");

            writeLfo(op, "lfo", LfoSide{}, amSide, notes);
        }

        writeUnusedOps(out, sourceOps);
    }

    // ------------------------------------------------------------------
    // 画面
    // ------------------------------------------------------------------
    // 寄せた項目の並び。無ければ空
    juce::String notesText(const Notes& notes)
    {
        if (notes.items.isEmpty()) return {};

        juce::String text = juce::String("") + "\n次の項目は同じ動きにできないため、近い値へ寄せています。\n";

        for (const auto& item : notes.items) text += "\n- " + item;

        return text;
    }

    void warn(juce::Component* parent, const juce::String& title, const juce::String& message)
    {
        juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon, title, message, juce::String(), parent);
    }

    // 元のファイルの名前から種類の部分を外す ("piano.param.opna.json" → "piano")
    juce::String baseNameOf(const juce::File& file)
    {
        return file.getFileName().upToFirstOccurrenceOf(".param.", false, true);
    }
}

namespace FmToOpzx7
{
    const std::vector<SourceInfo>& sources()
    {
        static const std::vector<SourceInfo> list = {
            { Source::opna, { "opna", 1 }, "OPNA" },
            { Source::opn, { "opn", 1 }, "OPN" },
            { Source::opl, { "opl", 1 }, "OPL" },
            { Source::opl3, { "opl3", 1 }, "OPL3" },
            { Source::opm, { "opm", 1 }, "OPM" },
        };

        return list;
    }

    void convert(Source source, const Io::ParamReader& src, Io::ParamWriter& out, Notes& notes)
    {
        // 区分の部品は同じものなので、まず丸ごと写し、違うところを上書きする
        out.copyFrom(src);

        switch (source)
        {
        case Source::opna: convertOpn(src, out, notes, true); break;
        case Source::opn: convertOpn(src, out, notes, false); break;
        case Source::opl: convertOpl(src, out, notes, false); break;
        case Source::opl3: convertOpl(src, out, notes, true); break;
        case Source::opm: convertOpm(src, out, notes); break;
        }
    }

    void exportFile(const GuiContext& ctx, Source source, std::function<void(Io::ParamWriter&)> writeSource)
    {
        const auto& list = sources();
        const auto it = std::find_if(list.begin(), list.end(), [source](const SourceInfo& s) { return s.source == source; });

        if (it == list.end()) return;

        const SourceInfo info = *it;

        juce::Component::SafePointer<juce::Component> editor(&ctx.editor);
        auto* processor = &ctx.audioProcessor;

        ctx.editor.openParamBrowserToSave(ctx.audioProcessor.defaultChannelParamDir, { kind }, extension,
            [editor, processor, info, writeSource](const juce::File& file) {
                if (file == juce::File{}) return;

                processor->defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                // タブの中身を元の音源のファイルと同じ形で書いてから直す
                Io::ParamWriter sourceWriter(info.format);

                writeSource(sourceWriter);

                Io::ParamWriter writer(opzx7Format);
                Notes notes;

                convert(info.source, sourceWriter.reader(), writer, notes);

                if (!writer.writeTo(file)) {
                    warn(editor.getComponent(), juce::String("") + "失敗",
                        juce::String("") + "OPZX7S のパラメータファイルを書き出せませんでした。\n\n" + file.getFileName());

                    return;
                }

                if (notes.items.isEmpty()) return;

                juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon,
                    juce::String("") + "OPZX7S へ書き出し",
                    juce::String("") + "OPZX7S のパラメータファイルを書き出しました。\n" + notesText(notes),
                    juce::String(), editor.getComponent());
            });
    }

    void importFile(const GuiContext& ctx, std::function<void(const Io::ParamReader&)> apply,
        std::function<void(Io::ParamWriter&)> writeOpzx7)
    {
        juce::Component::SafePointer<AudioPlugin2686VEditor> editor(&ctx.editor);
        auto* processor = &ctx.audioProcessor;

        juce::StringArray kinds;

        for (const auto& s : sources()) kinds.add(s.kind);

        ctx.editor.openParamBrowser(ctx.audioProcessor.defaultChannelParamDir, kinds,
            [editor, processor, apply, writeOpzx7](const juce::File& file) {
                if (!file.existsAsFile()) return;

                processor->defaultChannelParamDir = file.getParentDirectory().getFullPathName();

                // 中身の印で、どの音源のファイルかを見分ける
                std::optional<Io::ParamReader> reader;
                const SourceInfo* found = nullptr;

                for (const auto& s : sources())
                {
                    reader = Io::ParamReader::open(file, s.format, false);

                    if (reader.has_value()) {
                        found = &s;

                        break;
                    }
                }

                if (found == nullptr) {
                    if (Io::isLegacyFile(file)) {
                        // 3.0.0 より前の形式。読めないことは読み手が知らせる
                        Io::ParamReader::open(file, sources().front().format, true);
                    }
                    else {
                        warn(editor.getComponent(), juce::String("") + "読み込めません",
                            juce::String("") + "OPNA / OPN / OPL / OPL3 / OPM のチャンネルのファイルではありません。\n\n" + file.getFileName());
                    }

                    return;
                }

                Io::ParamWriter converted(opzx7Format);
                Notes notes;

                convert(found->source, *reader, converted, notes);

                apply(converted.reader());

                // 読み込んだ中身を、OPZX7S のファイルとして残せるようにする
                const juce::String name = baseNameOf(file);

                auto save = [editor, processor, writeOpzx7, name](int) {
                    if (editor == nullptr) return;

                    editor->openParamBrowserToSave(processor->defaultChannelParamDir, { kind }, extension,
                        [editor, processor, writeOpzx7](const juce::File& target) {
                            if (target == juce::File{}) return;

                            processor->defaultChannelParamDir = target.getParentDirectory().getFullPathName();

                            Io::ParamWriter writer(opzx7Format);

                            writeOpzx7(writer);

                            if (!writer.writeTo(target)) {
                                warn(editor.getComponent(), juce::String("") + "失敗",
                                    juce::String("") + "OPZX7S のパラメータファイルを書き出せませんでした。\n\n" + target.getFileName());
                            }
                        }, {}, name);
                    };

                juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon,
                    juce::String("") + "FM 音源のパラメータを読み込み",
                    found->kind + " のパラメータを OPZX7S へ読み込みました。\n"
                    + "閉じると、OPZX7S のパラメータファイルとして保存する先を選べます (保存しないときは取り消してください)。\n"
                    + notesText(notes),
                    juce::String(), editor.getComponent(), juce::ModalCallbackFunction::create(save));
            });
    }
}
