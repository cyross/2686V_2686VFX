// テストから音源のチャンネルを引くための表。
//
// OscMode の番号はプラグインごとに違うので、getModeName が返す名前で引く。
// 持っていないチャンネルは、パラメータが無いことで分かる。
#pragma once

#include <JuceHeader.h>

namespace GuiTestChips
{
    struct Entry
    {
        const char* modeName; // getModeName が返す名前
        const char* prefix;   // パラメータの頭
        bool isPcm;           // PCM 系は QUALITY(PCM) で、項目の名前が違う
    };

    inline constexpr Entry all[] = {
        { "OPNA",      "OPNA",        false },
        { "OPN",       "OPN",         false },
        { "OPL",       "OPL",         false },
        { "OPL3",      "OPL3",        false },
        { "OPM",       "OPM",         false },
        { "OPZX7",     "OPZX7",       false },
        { "SSG",       "SSG",         false },
        { "BEEP",      "BEEP",        false },
        { "WAVETABLE", "WT",          false },
        { "WT2",       "WT2",         false },
        { "WTPLUS",    "WTPLUS",      false },
        { "RHYTHM",    "RHYTHM_PAD0", true  },
        { "ADPCM",     "ADPCM",       true  },
        { "ADPCM+",    "ADPCMP",      true  },
    };

    inline const Entry* byModeName(const juce::String& name)
    {
        for (const auto& e : all) {
            if (name == e.modeName) return &e;
        }

        return nullptr;
    }
}
