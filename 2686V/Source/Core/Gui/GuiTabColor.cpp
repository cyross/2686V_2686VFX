// ============================================================================
// このプラグインのタブの見出しの色
// ============================================================================
// タブの見た目 (CustomTabLookAndFeel) は 12 本で共有する。タブの並びは
// プラグインごとに違うので、番号ごとの色だけをここで決める。
#include "Shared/Core/Gui/GuiLF.h"
#include "Shared/Core/Gui/GuiColor.h"

juce::Colour CustomTabLookAndFeel::getTabHeaderColor(int tabIndex)
{
    // 色そのものは GuiColor::Tab にある。タブの並びはプラグインごとに
    // 違うので、どの番号がどの系統かだけをここで決める。
    switch (tabIndex)
    {
    case  0: return GuiColor::Tab::Fm;       // OPNA
    case  1: return GuiColor::Tab::Fm;       // OPN
    case  2: return GuiColor::Tab::Fm;       // OPL
    case  3: return GuiColor::Tab::Fm;       // OPL3
    case  4: return GuiColor::Tab::Fm;       // OPM
    case  5: return GuiColor::Tab::Fm;       // OPZX7
    case  6: return GuiColor::Tab::Ssg;      // SSG
    case  7: return GuiColor::Tab::Wt;       // WT
    case  8: return GuiColor::Tab::Wt;       // WT2
    case  9: return GuiColor::Tab::Wt;       // WT+
    case 10: return GuiColor::Tab::Pcm;      // RHYTHM
    case 11: return GuiColor::Tab::Pcm;      // ADPCM
    case 12: return GuiColor::Tab::Pcm;      // ADPCM+
    case 13: return GuiColor::Tab::Beep;     // BEEP
    case 14: return GuiColor::Tab::Advanced; // ADVANCED
    case 15: return GuiColor::Tab::Utility;  // PRESET
    case 16: return GuiColor::Tab::Utility;  // SETTINGS
    case 17: return GuiColor::Tab::Utility;  // COLORS
    case 18: return GuiColor::Tab::Utility;  // ABOUT
    default: return GuiColor::Tab::Other;    // OTHER
    }
}
