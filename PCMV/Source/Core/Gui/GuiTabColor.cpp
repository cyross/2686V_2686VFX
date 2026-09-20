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
    // ここは tools/gen_plugin.py が書き出している。
    switch (tabIndex)
    {
    case  0: return GuiColor::Tab::Pcm;      // RHYTHM
    case  1: return GuiColor::Tab::Pcm;      // ADPCM
    case  2: return GuiColor::Tab::Pcm;      // ADPCMPLUS
    case  3: return GuiColor::Tab::Advanced; // ADVANCED
    case  4: return GuiColor::Tab::Utility;  // PRESET
    case  5: return GuiColor::Tab::Utility;  // SETTINGS
    case  6: return GuiColor::Tab::Utility;  // COLORS
    case  7: return GuiColor::Tab::Utility;  // ABOUT
    default: return GuiColor::Tab::Other;    // OTHER
    }
}
