#include "./RatioEnv.h"

#include <map>

#include "Shared/Core/Editor/EditorParamBrowserText.h"
#include "Shared/Core/Processor/ProcessorValueEnv.h"
#include "../../../Core/Const/ConstFileValues.h"

namespace
{
    // 鍵と範囲は音の側 (ValueEnv) と同じものから作る
    GuiComponentStepEnv::Spec make(const ValueEnv::Spec& v)
    {
        GuiComponentStepEnv::Spec x;

        x.tableKey = v.keys.base;

        x.steps = v.keys.steps;
        x.keep = v.keys.keep;
        x.loop = v.keys.loop;
        x.loopTo = v.keys.loopTo;
        x.loopCount = v.keys.loopCount;
        x.loopMode = v.keys.loopMode;
        x.stl = v.keys.stl;
        x.lr = v.keys.lr;
        x.rr = v.keys.rr;
        x.endl = v.keys.endl;
        x.endlEnable = v.keys.endlEnable;

        x.rateMin = CPV::SsgSwEnv256::R::min;
        x.rateMax = CPV::SsgSwEnv256::R::max;
        x.rateInterval = 0.001;
        x.levelMin = v.levelMin;
        x.levelMax = v.levelMax;

        return x;
    }
}

const GuiComponentStepEnv::Spec& GuiComponentDutyRatioEnv::spec()
{
    static const Spec s = [] {
        Spec x = make(ValueEnv::duty());

        x.kind = Spec::Kind::Duty;
        x.title = "DUTY RATIO ENVELOPE";

        // Invert は掛けないので、0〜100% をそのまま指定する
        x.levelInterval = 0.01;
        x.levelDecimals = 2;
        x.startLabel = "STDR";
        x.levelPrefix = "DR";
        x.levelLabel = "DUTY";
        x.levelTargetLabel = "DR.TARGET";

        x.fileFormat = "dutyEnv";
        x.browserKind = EditorGuiText::ParamBrowser::kindDutyEnv;
        x.extension = Io::Extension::DutyEnvParam;

        return x;
    }();

    return s;
}

const GuiComponentStepEnv::Spec& GuiComponentPeakRatioEnv::spec()
{
    static const Spec s = [] {
        Spec x = make(ValueEnv::peak());

        x.kind = Spec::Kind::Peak;
        x.title = "PEAK RATIO ENVELOPE";

        x.levelInterval = 0.001;
        x.levelDecimals = 3;
        x.startLabel = "STPK";
        x.levelPrefix = "PK";
        x.levelLabel = "PEAK";
        x.levelTargetLabel = "PK.TARGET";

        x.fileFormat = "peakEnv";
        x.browserKind = EditorGuiText::ParamBrowser::kindPeakEnv;
        x.extension = Io::Extension::PeakEnvParam;

        return x;
    }();

    return s;
}

const GuiComponentStepEnv::Spec& GuiComponentTlEnv::spec(int tlMax)
{
    auto build = [](const ValueEnv::Spec& v) {
        Spec x = make(v);

        x.kind = Spec::Kind::Tl;
        x.title = "TL ENV";

        // 常に KEEP。KEEP の札を持たない
        x.keep = {};

        x.levelInterval = 1.0;
        x.levelDecimals = 0;
        x.startLabel = "STTL";
        x.levelPrefix = "TL";
        x.levelLabel = "TL";
        x.levelTargetLabel = "TL.TARGET";

        x.fileFormat = "tlEnv";
        x.browserKind = EditorGuiText::ParamBrowser::kindTlEnv;
        x.extension = Io::Extension::TlEnvParam;

        return x;
    };

    static const Spec s127 = build(ValueEnv::tl127());
    static const Spec s63 = build(ValueEnv::tl63());

    return tlMax > 63 ? s127 : s63;
}

const GuiComponentStepEnv::Spec& GuiComponentAlgEnv::spec(int algorithms)
{
    static std::map<int, Spec> specs;

    auto it = specs.find(algorithms);

    if (it == specs.end()) {
        Spec x = make(ValueEnv::alg(algorithms));

        x.kind = Spec::Kind::Alg;
        x.title = "ALG ENV";

        // 常に KEEP。KEEP の札を持たない
        x.keep = {};

        x.levelInterval = 1.0;
        x.levelDecimals = 0;
        x.startLabel = "STALG";
        x.levelPrefix = "ALG";
        x.levelLabel = "ALG";
        x.levelTargetLabel = "ALG.TARGET";

        x.fileFormat = "algEnv";
        x.browserKind = EditorGuiText::ParamBrowser::kindAlgEnv;
        x.extension = Io::Extension::AlgEnvParam;

        it = specs.emplace(algorithms, x).first;
    }

    return it->second;
}
