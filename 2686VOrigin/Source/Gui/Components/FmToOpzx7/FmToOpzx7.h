#pragma once

#include <JuceHeader.h>

#include <functional>
#include <vector>

#include "../../../Core/Io/ParamFile.h"
#include "../../../Core/Gui/GuiContext.h"

// ============================================================================
// FM 音源のチャンネルを OPZX7S のチャンネルへ直す
// ============================================================================
// OPNA / OPN / OPL / OPL3 / OPM の CH Params を、OPZX7S がそのまま読める
// CH Params へ直す。直す元はファイルの中身なので、音源のタブが無い
// プラグイン (OPZX7S) でも同じ変換を使える。
//
// - FM 音源のタブの [EX]OPZX7S Params: タブの中身を書き出してから直し、ファイルへ書く
// - OPZX7S のタブの [IM]FM Params: ファイルを読んで直し、タブへ入れる
//
// 区分の部品 (AMP ENV・PITCH ENV など) は音源どうしで同じものを使っている
// ので、元の中身をそのまま写し、OPZX7S にしか無い項目と、意味の違う項目
// だけを作って上書きする。
//
// レジスタで持っている値は、実機のレート表 (どの FM 音源も同じ式) で秒や
// レベルへ直し、OPZX7S の実数モードへ入れる。同じ動きにできない項目は
// 近い値へ寄せ、何を寄せたかを Notes へ残す。
namespace FmToOpzx7
{
    // 直した先のファイルの種類と拡張子。OPZX7S の [IM]CH Params と同じもの
    inline const juce::String kind = "OPZX7";
    inline const juce::String extension = "param.opzx7s";

    enum class Source
    {
        opna,
        opn,
        opl,
        opl3,
        opm
    };

    // 直す元の音源。ファイルの印・ブラウザの区分・知らせに出す名前
    struct SourceInfo
    {
        Source source;
        Io::ParamFormat format;
        juce::String kind;
    };

    const std::vector<SourceInfo>& sources();

    // 近い値へ寄せた項目。同じ知らせは 1 度だけ出す
    struct Notes
    {
        juce::StringArray items;

        void add(const juce::String& text) { items.addIfNotAlreadyThere(text); }
    };

    // 元の音源の CH Params (src) を、OPZX7S の CH Params (out) へ直す
    void convert(Source source, const Io::ParamReader& src, Io::ParamWriter& out, Notes& notes);

    // FM 音源のタブから: 保存先を選ばせ、writeSource で書いたタブの中身を直して書く
    void exportFile(const GuiContext& ctx, Source source, std::function<void(Io::ParamWriter&)> writeSource);

    // OPZX7S のタブへ: FM 音源のファイルを選ばせ、直した中身を apply でタブへ入れる。
    // 入れたあとは、writeOpzx7 で書いたタブの中身を OPZX7S のファイルとして保存できる。
    void importFile(const GuiContext& ctx, std::function<void(const Io::ParamReader&)> apply,
        std::function<void(Io::ParamWriter&)> writeOpzx7);
}
