#include "./GuiRhythmHelpers.h"

void layoutRowRhythmPadPcmFile(const RowConfigRhythmPadPcmFile& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    c.loadBtn->setBounds(area.removeFromLeft(c.loadBtnWidth));

    area.removeFromLeft(c.paddingRight);

    c.filenameLabel->setBounds(area.removeFromLeft(c.filenameLabelWidth));

    area.removeFromLeft(c.paddingRight);

    c.clearBtn->setBounds(area.removeFromLeft(c.clearBtnWidth));

    c.rect.removeFromTop(c.paddingBottom);
}
