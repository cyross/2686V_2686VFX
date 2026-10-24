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

namespace
{
    // PARAM ENV の頭。鍵は ParamEnv (音の側) と同じものから作る
    GuiComponentStepEnv::Spec paramEnvBase()
    {
        const auto& k = ParamEnv::keys();

        GuiComponentStepEnv::Spec x;

        x.kind = GuiComponentStepEnv::Spec::Kind::Param;
        x.title = "PARAM ENV[256]";
        x.tableKey = k.base;

        x.steps = k.steps;
        x.keep = k.keep;
        x.loop = k.loop;
        x.loopTo = k.loopTo;
        x.loopCount = k.loopCount;
        x.loopMode = k.loopMode;
        x.stl = k.stl;
        x.lr = k.lr;
        x.rr = k.rr;
        x.endl = k.endl;
        x.endlEnable = k.endlEnable;
        x.target = k.target;

        x.rateMin = CPV::SsgSwEnv256::R::min;
        x.rateMax = CPV::SsgSwEnv256::R::max;
        x.rateInterval = 0.001;
        x.levelMin = 0.0;
        x.levelMax = 1.0;
        x.levelInterval = 0.001;
        x.levelDecimals = 3;

        x.startLabel = "STL";
        x.levelPrefix = "L";
        x.levelLabel = "VALUE";
        x.levelTargetLabel = "L.TARGET";

        x.fileFormat = "paramEnv";
        x.browserKind = EditorGuiText::ParamBrowser::kindParamEnv;
        x.extension = Io::Extension::ParamEnvParam;

        return x;
    }
}

GuiComponentStepEnv::Spec GuiComponentParamEnv::opSpec(const ParamModCatalog::Catalog& catalog, float tlMax)
{
    Spec x = paramEnvBase();

    x.targetLabels = ParamModCatalog::opTargetLabels(catalog);

    // code はオペレーターの頭 (OPNA_OP0)。尻尾を足せば指し先の鍵
    x.targetParamId = [&catalog](const juce::String& code, int target) {
        return target >= 0 && target < (int)catalog.op.size() ? code + catalog.op[(size_t)target].suffix : juce::String();
    };

    // 4.2.x までの TL ENV。ファイル (.tlEnv) は一覧で PARAM ENV の種類として出る
    x.legacyTlFormat = "tlEnv";
    x.legacyTlKey = "tlEnv";
    x.tlTarget = juce::jmax(0, catalog.tlIndex);
    x.tlMax = tlMax;

    return x;
}

GuiComponentStepEnv::Spec GuiComponentParamEnv::channelSpec(const ParamModCatalog::Catalog& catalog, int ops)
{
    Spec x = paramEnvBase();

    x.targetLabels = ParamModCatalog::channelTargetLabels(catalog, ops);

    // code はチャンネルの頭 (OPNA)。RHYTHM はパッドの指し先も並ぶ
    x.targetParamId = [&catalog](const juce::String& code, int target) {
        int scope = -1, index = 0;

        ParamModCatalog::decodeChannelTarget(catalog, target, scope, index);

        return ParamModCatalog::paramId(code, catalog, scope, index);
    };

    return x;
}
