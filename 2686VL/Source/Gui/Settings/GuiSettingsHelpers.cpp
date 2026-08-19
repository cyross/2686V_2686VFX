#include "./GuiSettingsHelpers.h"

void layoutRowSettingsIo(const RowConfigSettingsIo& c)
{
    auto area = c.rect.removeFromTop(c.rowHeight);

    c.rect.removeFromTop(c.paddingTop);

    c.loadSettingsBtn->setBounds(area.removeFromLeft(c.loadSettingsBtnWidth));

    area.removeFromLeft(c.paddingRight);

    c.saveSettingsBtn->setBounds(area.removeFromLeft(c.saveSettingsBtnWidth));

    area.removeFromLeft(c.paddingRight);

    c.saveStartupSettingsBtn->setBounds(area.removeFromLeft(c.saveStartupSettingsBtnWidth));

    c.rect.removeFromTop(c.paddingBottom);
}
