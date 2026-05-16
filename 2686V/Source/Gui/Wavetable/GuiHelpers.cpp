#include "./GuiHelpers.h"

void layoutRowWtWaveValueUpdate(const RowConfigWtWaveValueUpdate& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    c.resetTo0Btn->setBounds(area.removeFromLeft(c.resetTo0BtnWidth));

    area.removeFromLeft(c.paddingRight);

    c.resetTo1Btn->setBounds(area.removeFromLeft(c.resetTo1Width));

    area.removeFromLeft(c.paddingRight);

    c.resetToM1Btn->setBounds(area.removeFromLeft(c.resetToM1Width));

    c.rect.removeFromTop(c.paddingBottom);
}
