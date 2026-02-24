#pragma once

#include <JuceHeader.h>

#include "VstLogoForAbout.h"
#include "../gui/GuiComponents.h"
#include "../gui/GuiBase.h"
#include "../gui/GuiContext.h"

class GuiAbout : public GuiBase
{
    GuiGroup mainGroup;

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
        mainGroup(context),
        pluginNameLabel(context),
        versionLabel(context),
		copyrightLabel(context),
		vstGuidelineLabel(context),
        gplNoticeLabel(context),
		vst3Logo(context),
		gplLinkButton(context)
    {}

    void setup() override;
    void layout(juce::Rectangle<int> content) override;
};
