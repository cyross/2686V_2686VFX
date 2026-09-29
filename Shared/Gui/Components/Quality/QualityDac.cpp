#include "./QualityDac.h"

#include "Shared/Core/Gui/GuiI18n.h"

namespace
{
    using Values = GuiComponentDac::Values;

    struct Machine
    {
        const char* name;
        Values values;
    };

    // ------------------------------------------------------------------
    // 項目の位置 (0 始まり)。一覧そのものは Quality・QualityPcm にある。
    // ------------------------------------------------------------------
    // 音源の BIT RATE
    namespace QBit { constexpr int b4 = 0, b5 = 1, b9 = 5, b10 = 6, b16 = 8; }

    // PCM の BIT RATE (2686VFX の PCM ビットクラッシャーは頭の 12 個だけ)
    namespace PBit
    {
        constexpr int b16 = 3, b12 = 4, b8 = 7, b7 = 8, b5 = 10, b4 = 11;
        constexpr int ym2608Adpcm = 12, dpcm1 = 13, snesBrr = 14, ps1Vag = 15, imaAdpcm = 16;
    }

    // SMP.RATE (音源・PCM で同じ)
    namespace Rate
    {
        constexpr int k96 = 0, k55_5 = 1, k49_7 = 2, k44_1 = 4, k33_08 = 5, k32 = 6, k22_05 = 7, k16 = 8, k8 = 11;
    }

    // INTERP (音源・PCM で同じ)
    namespace Interp { constexpr int gaussian = 2, zoh = 3; }

    // ------------------------------------------------------------------
    // 機種ごとの値
    // ------------------------------------------------------------------
    // 実機の出力段に合わせる。一覧に無いレートは近いものを採る。
    // DAC の出口は階段のまま出るので、埋め方は ZOH にする。SFC と PS1 の
    // PCM は、読み出すときにガウス補間を掛けるのでそちらに合わせる。
    //
    // 98 を 88 より前に置く。まとめたときの名前を「98/88」にするため。

    // 音源の QUALITY: FM などの出力が通る DAC
    const std::vector<Machine> qualityMachines = {
        { "98",    { QBit::b10, Rate::k55_5,  Interp::zoh } }, // OPN / OPNA + YM3014 (仮数 10bit)
        { "88",    { QBit::b10, Rate::k55_5,  Interp::zoh } }, // OPN / OPNA + YM3014
        { "MSX",   { QBit::b9,  Rate::k49_7,  Interp::zoh } }, // OPLL (YM2413) の 9bit DAC
        { "X68K",  { QBit::b10, Rate::k55_5,  Interp::zoh } }, // OPM + YM3012 (62.5kHz は一覧に無い)
        { "FC",    { QBit::b4,  Rate::k96,    Interp::zoh } }, // 2A03 の矩形波・三角波 (4bit)
        { "SFC",   { QBit::b16, Rate::k32,    Interp::zoh } }, // S-DSP (16bit / 32kHz)
        { "PCE",   { QBit::b5,  Rate::k96,    Interp::zoh } }, // HuC6280 (5bit)
        { "MD",    { QBit::b9,  Rate::k55_5,  Interp::zoh } }, // YM2612 の 9bit DAC (53.3kHz)
        { "GB",    { QBit::b4,  Rate::k96,    Interp::zoh } }, // 4bit DAC
        { "TOWNS", { QBit::b9,  Rate::k55_5,  Interp::zoh } }, // YM3438 (OPN2C)
        { "PS1",   { QBit::b16, Rate::k44_1,  Interp::zoh } }, // SPU (16bit / 44.1kHz)
    };

    // PCM の QUALITY: サンプルを鳴らす回路
    const std::vector<Machine> qualityPcmMachines = {
        { "98",    { PBit::b16,         Rate::k44_1,  Interp::zoh } },      // PC-9801-86 の PCM (16bit / 44.1kHz)
        { "88",    { PBit::ym2608Adpcm, Rate::k16,    Interp::zoh } },      // OPNA の ADPCM
        { "MSX",   { PBit::b8,          Rate::k16,    Interp::zoh } },      // turbo R の PCM (8bit / 15.7kHz)
        { "X68K",  { PBit::imaAdpcm,    Rate::k16,    Interp::zoh } },      // MSM6258 (OKI ADPCM / 15.6kHz)
        { "FC",    { PBit::dpcm1,       Rate::k33_08, Interp::zoh } },      // DMC (1bit DPCM / 33.1kHz)
        { "SFC",   { PBit::snesBrr,     Rate::k32,    Interp::gaussian } }, // BRR + ガウス補間
        { "PCE",   { PBit::b5,          Rate::k8,     Interp::zoh } },      // DDA (5bit / 約 7kHz)
        { "MD",    { PBit::b8,          Rate::k22_05, Interp::zoh } },      // YM2612 の DAC チャンネル (8bit)
        { "GB",    { PBit::b4,          Rate::k8,     Interp::zoh } },      // 波形メモリ (4bit)
        { "TOWNS", { PBit::b8,          Rate::k22_05, Interp::zoh } },      // RF5C68 (8bit / 20.8kHz)
        { "PS1",   { PBit::ps1Vag,      Rate::k44_1,  Interp::gaussian } }, // VAG + ガウス補間
    };

    // 2686VFX の PCM ビットクラッシャー。ADPCM などの形式は選べないので、
    // 実機が符号を解いたあとの幅で近づける。
    const std::vector<Machine> fxPcmMachines = {
        { "98",    { PBit::b16, Rate::k44_1,  Interp::zoh } },
        { "88",    { PBit::b16, Rate::k16,    Interp::zoh } },      // OPNA の ADPCM は 16bit へ解く
        { "MSX",   { PBit::b8,  Rate::k16,    Interp::zoh } },
        { "X68K",  { PBit::b12, Rate::k16,    Interp::zoh } },      // MSM6258 は 12bit へ解く
        { "FC",    { PBit::b7,  Rate::k33_08, Interp::zoh } },      // DMC の 7bit DAC
        { "SFC",   { PBit::b16, Rate::k32,    Interp::gaussian } },
        { "PCE",   { PBit::b5,  Rate::k8,     Interp::zoh } },
        { "MD",    { PBit::b8,  Rate::k22_05, Interp::zoh } },
        { "GB",    { PBit::b4,  Rate::k8,     Interp::zoh } },
        { "TOWNS", { PBit::b8,  Rate::k22_05, Interp::zoh } },
        { "PS1",   { PBit::b16, Rate::k44_1,  Interp::gaussian } },
    };

    bool sameValues(const Values& a, const Values& b)
    {
        return a.bit == b.bit && a.rate == b.rate && a.interp == b.interp;
    }

    // 同じ値の機種を 1 つにまとめる。並びは最初に出てきた機種の位置。
    std::vector<GuiComponentDac::Preset> merge(const std::vector<Machine>& machines)
    {
        std::vector<GuiComponentDac::Preset> list;

        for (const auto& m : machines)
        {
            bool merged = false;

            for (auto& p : list)
            {
                if (!sameValues(p.values, m.values)) continue;

                p.name += "/" + juce::String(m.name);
                merged = true;

                break;
            }

            if (!merged) list.push_back({ juce::String(m.name), m.values });
        }

        return list;
    }
}

const std::vector<GuiComponentDac::Preset>& GuiComponentDac::presets(Kind kind)
{
    static const std::vector<Preset> quality = merge(qualityMachines);
    static const std::vector<Preset> qualityPcm = merge(qualityPcmMachines);
    static const std::vector<Preset> fxPcm = merge(fxPcmMachines);

    switch (kind)
    {
    case Kind::QualityPcm: return qualityPcm;
    case Kind::FxPcm: return fxPcm;
    case Kind::Quality:
    default: return quality;
    }
}

GuiComponentDac::GuiComponentDac(const GuiContext& context) :
    selector(context),
    applyBtn(context)
{
}

void GuiComponentDac::setupComponent(juce::Component& parent, Kind k, int& tabOrder,
    std::function<void(const Values&)> onApply)
{
    kind = k;
    applyValues = std::move(onApply);

    std::vector<SelectItem> items;
    int id = 1;

    for (const auto& p : presets(kind)) items.push_back({ .name = p.name, .value = id++ });

    // パラメータへは束ねない。選んだ機種は画面だけのもの。
    selector.setup({ .parent = parent, .title = "DAC", .items = items, .isReset = false });
    selector.setWantsKeyboardFocus(true);
    selector.setExplicitFocusOrder(++tabOrder);

    applyBtn.setup({ .parent = parent, .title = I18n::pick(u8"適応", u8"Apply"), .isReset = false });
    applyBtn.setWantsKeyboardFocus(true);
    applyBtn.setExplicitFocusOrder(++tabOrder);

    applyBtn.onClick = [this] {
        const int index = selector.getSelectedItemIndex();
        const auto& list = presets(kind);

        if (index < 0 || index >= (int)list.size() || !applyValues) return;

        applyValues(list[(size_t)index].values);
    };
}

void GuiComponentDac::layoutComponent(juce::Rectangle<int>& rect, int rowHeight, int paddingTop, int paddingBottom,
    int labelWidth, int comboWidth, int buttonWidth)
{
    auto area = rect.removeFromTop(rowHeight);

    rect.removeFromTop(paddingTop);

    selector.label.setBounds(area.removeFromLeft(labelWidth));

    // 区分によっては行が渡した幅より狭い。コンボボックスを先に取ると
    // ボタンがはみ出して見えなくなるので、ボタンのぶんを残して縮める。
    const int fitWidth = juce::jmax(0, juce::jmin(comboWidth, area.getWidth() - buttonWidth));

    selector.setBounds(area.removeFromLeft(fitWidth));
    applyBtn.setBounds(area.removeFromLeft(juce::jmin(buttonWidth, area.getWidth())));

    rect.removeFromTop(paddingBottom);
}

void GuiComponentDac::setVisibles(bool visible)
{
    selector.setVisibleWithLabel(visible);
    applyBtn.setVisible(visible);
}

void GuiComponentDac::setEnableds(bool enabled)
{
    selector.setEnabledWithLabel(enabled);
    applyBtn.setEnabled(enabled);
}
