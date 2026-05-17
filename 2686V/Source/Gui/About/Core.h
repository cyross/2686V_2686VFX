#pragma once

#include <JuceHeader.h>

#include "VstLogoForAbout.h"
#include "AppIconForAbout.h"

#include "../../Core/Gui/GuiComponents.h"
#include "../../Core/Gui/GuiBase.h"
#include "../../Core/Gui/GuiContext.h"

class GuiAbout : public GuiBase
{
    GuiImage iconImage;

    // Labels
    GuiLabel pluginNameLabel;
    GuiLabel versionLabel;
    GuiLabel copyrightLabel;

    // VST Guideline
    GuiLabel vstGuidelineLabel;

    // Logos
    GuiImage vst3Logo;

    // GPL Licese View
    GuiLabel gplNoticeLabel;
    GuiHyperLink gplLinkButton;
public:
    GuiAbout(const GuiContext& context) :
        GuiBase(context),
        iconImage(context),
        pluginNameLabel(context),
        versionLabel(context),
		copyrightLabel(context),
		vstGuidelineLabel(context),
        gplNoticeLabel(context),
		vst3Logo(context),
		gplLinkButton(context)
    {
        setFocusContainerType(FocusContainerType::keyboardFocusContainer);
    }

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
};
