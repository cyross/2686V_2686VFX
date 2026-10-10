#pragma once

#include <JuceHeader.h>
#include <array>
#include <map>
#include <vector>
#include <span>

#include "./GuiContext.h"

class GuiBase: public juce::Component
{
public:
	GuiBase(const GuiContext& context) : ctx(context) {}

    virtual void setup() {};
    virtual void layout(juce::Rectangle<int> content) {};

    // チャンネルパラメータファイルを読んで反映する。
    //
    // ダイアログを出さずに直に読ませるための入口で、パラメータ
    // ファイルのブラウザから呼ぶ。チャンネルを持たないタブは
    // 何もしない。
    virtual void applyChParamFile(const juce::File& file) {};

    // 簡易表示モードで隠す区分への一括操作。
    //
    // どの区分が対象かはタブごとに違うので、実際の中身はタブ側で書く。
    // 対象を持たないタブは何もしない。
    //
    // 入り切りの札を持たない区分 (MUL・DET / UNISON・HARMONY) は、
    // どの操作でも触らない。
    virtual void bypassHiddenCategories() {};
    virtual void openEnabledCategories() {};
    virtual void closeBypassedCategories() {};

    // パラメータの切り取り・コピー・貼り付け (ParamEdit) で、パラメータの外に
    // 持っている素材 (音声・波形ファイル) を写す。中身は GuiEditorHost の
    // 同じ名前の関数を参照。素材を持たないタブは何もしない。
    virtual void collectClipAssets(const juce::String& prefix, bool wholeChannel,
        std::map<juce::String, juce::String>& out)
    {
        juce::ignoreUnused(prefix, wholeChannel, out);
    }

    virtual void applyClipAssets(const juce::String& prefix, bool wholeChannel,
        const std::map<juce::String, juce::String>& assets, bool clearOthers)
    {
        juce::ignoreUnused(prefix, wholeChannel, assets, clearOthers);
    }
protected:
	// GuiOpnaなどでも使うので、using宣言でエイリアスを作っておく
    using SliderAttachment = juce::AudioProcessorValueTreeState::SliderAttachment;
    using ButtonAttachment = juce::AudioProcessorValueTreeState::ButtonAttachment;
    using ComboBoxAttachment = juce::AudioProcessorValueTreeState::ComboBoxAttachment;

    GuiContext ctx;

    // TARGET のつまみを鍵で動かす。
    //
    // Ctrl + ← / → で 1 つずつ、Ctrl + 数字 (テンキーも) でその番号へ。
    // 端まで来たら反対の端へ回る。
    //
    // 番号の始まりはつまみの幅から読む。オペレータとパッドは 1 から、
    // 波形メモリと PCM のスロットは 0 から数えるので、決め打ちにすると
    // どちらかがずれる。
    //
    // withNumbers を切ると数字を見ない。スロットが 32 本あるところは
    // 数字で選んでも 8 番までしか届かないので、左右だけを受ける。
    bool moveTargetByKey(juce::Slider& target, const juce::KeyPress& key, bool withNumbers = true)
    {
        if (!key.getModifiers().isCommandDown()) return false;

        const double lo = target.getMinimum();
        const double hi = target.getMaximum();

        if (hi <= lo) return false;

        const int code = key.getKeyCode();

        if (code == juce::KeyPress::leftKey || code == juce::KeyPress::rightKey) {
            double next = target.getValue() + (code == juce::KeyPress::rightKey ? 1.0 : -1.0);

            // 端まで来たら反対の端へ回る
            if (next > hi) next = lo;
            if (next < lo) next = hi;

            target.setValue(next, juce::sendNotification);

            return true;
        }

        if (!withNumbers) return false;

        for (int n = 1; n <= 8; ++n) {
            if (code != ('0' + n) && code != (juce::KeyPress::numberPad0 + n)) continue;

            const double next = lo + (double)(n - 1);

            // 無い番号を押しても何も起きない。鍵は食べておく。
            if (next <= hi) target.setValue(next, juce::sendNotification);

            return true;
        }

        return false;
    }

    // つまみを通さずにパラメータそのものを読み書きする。
    //
    // 対象を選ぶつまみで値の組を切り替える造り (SSG SW ENV の段、
    // WT+ のスロット) では、画面へ出ているのは 1 組だけになる。
    // 出ていない組をファイルへ残すときにここを通す。
    float getParamValue(const juce::String& id) const
    {
        auto* v = ctx.apvts.getRawParameterValue(id);

        return (v != nullptr) ? v->load() : 0.0f;
    }

    void setParamValue(const juce::String& id, float value)
    {
        if (auto* p = ctx.apvts.getParameter(id)) {
            p->setValueNotifyingHost(p->convertTo0to1(value));
        }
    }
};
