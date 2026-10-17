#include "./SsgSwPEnv256.h"

#include "Shared/Core/Editor/EditorParamBrowserText.h"
#include "Shared/Core/Processor/ProcessorKeys.h"
#include "Shared/Core/Processor/ProcessorValues.h"
#include "../../../Core/Const/ConstFileValues.h"

const GuiComponentStepEnv::Spec& GuiComponentSsgSwPEnv256::spec()
{
    static const Spec s = [] {
        Spec x;

        x.kind = Spec::Kind::Pitch;
        x.title = "SSG SW PITCH ENV[256]";
        x.tableKey = CPK::ssgSwPEnv256;

        x.steps = CPK::SsgSwPEnv256::steps;
        x.keep = CPK::SsgSwPEnv256::keep;
        x.loop = CPK::SsgSwPEnv256::loop;
        x.loopTo = CPK::SsgSwPEnv256::loopTo;
        x.loopCount = CPK::SsgSwPEnv256::loopCount;
        x.stl = CPK::SsgSwPEnv256::stl;
        x.lr = CPK::SsgSwPEnv256::lr;
        x.rr = CPK::SsgSwPEnv256::rr;
        x.endl = CPK::SsgSwPEnv256::endl;
        x.endlEnable = CPK::SsgSwPEnv256::endlEnable;

        x.rateMin = CPV::SsgSwPEnv256::R::min;
        x.rateMax = CPV::SsgSwPEnv256::R::max;
        x.rateInterval = 0.001;
        x.levelMin = (double)CPV::SsgSwPEnv256::L::min;
        x.levelMax = (double)CPV::SsgSwPEnv256::L::max;
        x.levelInterval = 1.0;
        x.levelDecimals = 0;

        x.fileFormat = "ssgSwPEnv256";
        x.legacyFileFormat = "ssgSwPEnv11";
        x.browserKind = EditorGuiText::ParamBrowser::kindSsgSwPEnv256;
        x.extension = Io::Extension::SsgSwPEnvParam256;

        return x;
    }();

    return s;
}
