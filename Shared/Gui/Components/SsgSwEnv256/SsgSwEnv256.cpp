#include "./SsgSwEnv256.h"

#include "Shared/Core/Editor/EditorParamBrowserText.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "Shared/Core/Processor/ProcessorValues.h"
#include "../../../Core/Const/ConstFileValues.h"

const GuiComponentStepEnv::Spec& GuiComponentSsgSwEnv256::spec()
{
    static const Spec s = [] {
        Spec x;

        x.kind = Spec::Kind::Amp;
        x.title = "SSG SW AMP ENV[256]";
        x.tableKey = CPK::ssgSwEnv256;

        x.steps = CPK::SsgSwEnv256::steps;
        x.keep = CPK::SsgSwEnv256::keep;
        x.strictRate = CPK::SsgSwEnv256::strictRate;
        x.loop = CPK::SsgSwEnv256::loop;
        x.loopTo = CPK::SsgSwEnv256::loopTo;
        x.loopCount = CPK::SsgSwEnv256::loopCount;
        x.loopMode = CPK::SsgSwEnv256::loopMode;
        x.stl = CPK::SsgSwEnv256::stl;
        x.lr = CPK::SsgSwEnv256::lr;
        x.rr = CPK::SsgSwEnv256::rr;
        x.endl = CPK::SsgSwEnv256::endl;
        x.endlEnable = CPK::SsgSwEnv256::endlEnable;

        x.rateMin = CPV::SsgSwEnv256::R::min;
        x.rateMax = CPV::SsgSwEnv256::R::max;
        x.rateInterval = 0.001;
        x.levelMin = CPV::SsgSwEnv256::L::min;
        x.levelMax = CPV::SsgSwEnv256::L::max;
        x.levelInterval = 0.001;
        x.levelDecimals = 3;

        x.fileFormat = "ssgSwEnv256";
        x.legacyFileFormat = "ssgSwEnv11";
        x.sixStepFileFormat = "ssgSwEnv";
        x.browserKind = EditorGuiText::ParamBrowser::kindSsgSwEnv256;
        x.extension = Io::Extension::SsgSwEnvParam256;

        return x;
    }();

    return s;
}
