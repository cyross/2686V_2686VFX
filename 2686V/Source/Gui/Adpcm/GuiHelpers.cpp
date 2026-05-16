#include "./GuiHelpers.h"

// メイングループにPCMコンポーネントを貼り付け
void layoutMainPcm(const MainConfigPcm& c) 
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    c.loadPcmBtn->setBounds(area.removeFromLeft(c.loadPcmBtnWidth));

    area.removeFromLeft(c.paddingRight);

    c.pcmFileNameLabel->setBounds(area.removeFromLeft(c.pcmFileNameLabelWidth));

    area.removeFromLeft(c.paddingRight);

    c.clearPcmBtn->setBounds(area.removeFromLeft(c.clearPcmBtnWidth));

    c.rect.removeFromTop(c.paddingBottom);
}
