// UIで使用するコンボボックスの選択肢を管理

#pragma once

#include <vector>

#include "../gui/GuiStructs.h"

static std::vector<SelectItem> bdItems = {
    {.name = "1: 4-bit (16 steps)",  .value = 1 },
    {.name = "2: 5-bit (32 steps)",  .value = 2 },
    {.name = "3: 6-bit (64 steps)",  .value = 3 },
    {.name = "4: 8-bit (256 steps)", .value = 4 },
    {.name = "5: Raw",               .value = 5 },
};

static std::vector<SelectItem> qualityItems = {
    {.name = "1: Raw (32bit)", .value = 1 },
    {.name = "2: 24-bit PCM",  .value = 2 },
    {.name = "3: 16-bit PCM",  .value = 3 },
    {.name = "4: 8-bit PCM",   .value = 4 },
    {.name = "5: 5-bit PCM",   .value = 5 },
    {.name = "6: 4-bit PCM",   .value = 6 },
    {.name = "7: 4-bit ADPCM", .value = 7 },
};

static std::vector<SelectItem> rateItems = {
    {.name = "1: 96kHz",    .value = 1 },
    {.name = "2: 55.5kHz",  .value = 2 },
    {.name = "3: 48kHz",    .value = 3 },
    {.name = "4: 44.1kHz",  .value = 4 },
    {.name = "5: 22.05kHz", .value = 5 },
    {.name = "6: 16kHz",    .value = 6 },
    {.name = "7: 8kHz",     .value = 7 },
};

static std::vector<SelectItem> pmsItems = {
    {.name = "1: Pms 0", .value = 1 },
    {.name = "2: Pms 1", .value = 2 },
    {.name = "3: Pms 2", .value = 3 },
    {.name = "4: Pms 3", .value = 4 },
    {.name = "5: Pms 4", .value = 5 },
    {.name = "6: Pms 5", .value = 6 },
    {.name = "7: Pms 6", .value = 7 },
    {.name = "8: Pms 7", .value = 8 },
};

static std::vector<SelectItem> amsItems = {
    {.name = "1: Ams 0", .value = 1 },
    {.name = "2: Ams 1", .value = 2 },
    {.name = "3: Ams 2", .value = 3 },
    {.name = "4: Ams 3", .value = 4 },
};

static std::vector<SelectItem> ksItems = { {.name = "0 OFF", .value = 1}, {.name = "1 (Weak)", .value = 2}, {.name = "2 (Mid)", .value = 3}, {.name = "3 (Strong)", .value = 4}, };

static std::vector<SelectItem> kslItems = { {.name = "KSL: 0", .value = 1}, {.name = "KSL: 1", .value = 2}, {.name = "KSL: 2", .value = 3}, {.name = "KSL: 3", .value = 4}, };

static std::vector<SelectItem> opnaAlgItems = {
    {.name = "0: [OP0]:FB -> [OP1] -> [OP2] -> [OP3]", .value = 1 },
    {.name = "1: ([OP0]:FB + [OP1]) -> [OP2] -> [OP3]", .value = 2 },
    {.name = "2: ([OP0] -> [OP1] + [OP2]:FB) -> [OP3]", .value = 3 },
    {.name = "3: ([OP0]:FB -> [OP1]) + [OP2] -> [OP3]", .value = 4 },
    {.name = "4: ([OP0]:FB -> [OP1]) + ([OP2] -> [OP3])", .value = 5 },
    {.name = "5: ([OP0]:FB -> [OP1]) + ([OP0]:FB -> [OP2]) + ([OP0]:FB -> [OP3])", .value = 6 },
    {.name = "6: ([OP0]:FB -> [OP1]) + [OP2] + [OP3]", .value = 7 },
    {.name = "7: [OP0]:FB + [OP1] + [OP2] + [OP3]", .value = 8 },
};

static std::vector<SelectItem> opnaSeItems = {
    {.name = "0: Normal", .value = 1 },
    {.name = "1: Saw Down", .value = 2 },
    {.name = "2: Saw Down & Hold", .value = 3 },
    {.name = "3: Triangle", .value = 4 },
    {.name = "4: Alternative Saw Down & Hold", .value = 5 },
    {.name = "5: Saw Up", .value = 6 },
    {.name = "6: Saw Up & Hold", .value = 7 },
    {.name = "7: Triangle Invert", .value = 8 },
    {.name = "8: Alternative Saw Up & Hold", .value = 9 },
};

static std::vector<SelectItem> oplAlgItems = {
    {.name = "0: [OP0]:FB -> [OP1]", .value = 1 },
    {.name = "1: [OP0]:FB + [OP1]", .value = 2 },
};

static std::vector<SelectItem> oplEgItems = { {.name = "0: Sine", .value = 1}, {.name = "1: Half", .value = 2}, {.name = "2: Abs", .value = 3}, {.name = "3: Pulse", .value = 4}, };

static std::vector<SelectItem> opl3AlgItems = {
    {.name = "0: [OP0]:FB -> [OP1] -> [OP2] -> [OP3]", .value = 1 },
    {.name = "1: [OP0]:FB + ([OP1] -> [OP2] -> [OP3])", .value = 2 },
    {.name = "2: ([OP0]:FB -> [OP1]) + ([OP2] -> [OP3])", .value = 3 },
    {.name = "3: [OP0]:FB + ([OP1] + [OP2]) + [OP3]", .value = 4 },
};

static std::vector<SelectItem> opl3EgItems = {
    {.name = "0: Sine", .value = 1},
    {.name = "1: Half Sine", .value = 2},
    {.name = "2: Abs Sine", .value = 3},
    {.name = "3: Pulse", .value = 4},
    {.name = "4: Alternative Sine", .value = 5},
    {.name = "5: Alternative Abs Sine", .value = 6},
    {.name = "6: Square", .value = 7},
    {.name = "7: Derived Square", .value = 8},
};

static std::vector<SelectItem> opmAlgItems = {
    {.name = "0: [OP0]:FB -> [OP1] -> [OP2] -> [OP3]", .value = 1 },
    {.name = "1: ([OP0]:FB + [OP1]) -> [OP2] -> [OP3]", .value = 2 },
    {.name = "2: [OP0]:FB + ([OP1] -> [OP2]) -> [OP3]", .value = 3 },
    {.name = "3: ([OP0]:FB -> [OP1]) + [OP2] -> [OP3]", .value = 4 },
    {.name = "4: ([OP0]:FB -> [OP1]) + ([OP2] -> [OP3])", .value = 5 },
    {.name = "5: ([OP0]:FB -> [OP1]) + ([OP0]:FB -> [OP2]) + ([OP0]:FB -> [OP3])", .value = 6 },
    {.name = "6: ([OP0]:FB -> [OP1]) + [OP2] + [OP3]", .value = 7 },
    {.name = "7: [OP0]:FB + [OP1] + [OP2] + [OP3]", .value = 8 },
};

static std::vector<SelectItem> opzx3AlgItems = {
    {.name = "00: [OP0]:FB -> [OP1] -> [OP2] -> [OP3]", .value = 1 },
    {.name = "01: ([OP0] -> [OP1]):FB -> [OP2] -> [OP3]", .value = 2 },
    {.name = "02: ([OP0]:FB + [OP1]) -> [OP2] -> [OP3]", .value = 3 },
    {.name = "03: ([OP0]:FB -> [OP3]) + ([OP1] -> [OP2] -> [OP3])", .value = 4 },
    {.name = "04: ([OP0]:FB -> [OP1]) + ([OP0]:FB -> [OP2]) + ([OP0]:FB -> [OP3])", .value = 5 },
    {.name = "05: ([OP0] -> [OP1]):FB + ([OP0] -> [OP2]) + ([OP0] -> [OP3])", .value = 6 },
    {.name = "06: ([OP0]:FB -> [OP1]) + ([OP2] -> [O32])", .value = 7 },
    {.name = "07: ([OP0] -> [OP1]):FB + ([OP2] -> [O32])", .value = 8 },
    {.name = "08: [OP0]:FB + ([OP1] -> [OP2] -> [OP3])", .value = 9 },
    {.name = "09: [OP0]:FB + ([OP1] -> [OP2]) -> [OP3]", .value = 10 },
    {.name = "10: ([OP0]:FB -> [OP1]) + [OP2] + [OP3]", .value = 11 },
    {.name = "11: ([OP0] -> [OP1]):FB + [OP2] + [OP3]", .value = 12 },
    {.name = "12: ([OP0]:FB -> [OP1]) + ([OP0]:FB -> [OP2]) + ([OP0]:FB -> [OP3])", .value = 13 },
    {.name = "13: [OP0]:FB + ([OP1] -> [OP2]) + [OP3]", .value = 14 },
    {.name = "14: [OP0]:FB + ([OP0]:FB -> [OP1]) + ([OP2] -> [OP3])", .value = 15 },
    {.name = "15: [OP0]:FB + [OP1] + [OP2] + [OP3]", .value = 16 },
    {.name = "16 [OP0]:FB -> [OP1] -> [OP2]", .value = 17 },
    {.name = "17 ([OP0] -> [OP1]):FB -> [OP2]", .value = 18 },
    {.name = "18 ([OP0]:FB + [OP2]) -> [OP1]", .value = 19 },
    {.name = "19: [OP0]:FB + [OP2] -> [OP1]", .value = 20 },
    {.name = "20: ([OP0]:FB -> [OP2]) + [OP1]", .value = 21 },
    {.name = "21: ([OP0]:FB -> [OP1]):FB + [OP2]", .value = 22 },
    {.name = "22: [OP0]:FB + ([OP1] -> [OP2]) + [OP1]", .value = 23 },
    {.name = "23: [OP0]:FB + ([OP0] -> [OP2]) + [OP1]", .value = 24 },
    {.name = "24: [OP0]:FB -> [OP1]", .value = 25 },
    {.name = "25: ([OP0] -> [OP1]):FB", .value = 26 },
    {.name = "26: [OP0]:FB + [OP1]", .value = 27 },
    {.name = "27: [OP0]:FB + ([OP0] -> [OP1])", .value = 28 },
};

static std::vector<SelectItem> opzx3WsItems = {
    {.name = "00: Sine", .value = 1},
    {.name = "01: Half Sine", .value = 2},
    {.name = "02: Abs Sine", .value = 3},
    {.name = "03: Alternating Half Sine", .value = 4},
    {.name = "04: Alternating Sine", .value = 5},
    {.name = "05: Alternating Abs Sine", .value = 6},
    {.name = "06: Square", .value = 7},
    {.name = "07: Derived Square", .value = 8},
    {.name = "08: Saw Down", .value = 9},
    {.name = "09: Saw Up", .value = 10},
    {.name = "10: Triangle", .value = 11},
    {.name = "11: Saw + Sine", .value = 12},
    {.name = "12: Log Saw", .value = 13},
    {.name = "13: Pulse 25%", .value = 14},
    {.name = "14: Pulse 12.5%", .value = 15},
    {.name = "15: Pulse 6.25%", .value = 16},
    {.name = "16: Round Square", .value = 17},
    {.name = "17: Impulse Train", .value = 18},
    {.name = "18: Comb / Multi-pulse", .value = 19},
    {.name = "19: Resonant Saw (Low)", .value = 20},
    {.name = "20: Resonant Saw (High)", .value = 21},
    {.name = "21: Resonant Triangle", .value = 22},
    {.name = "22: Bulb Sine", .value = 23},
    {.name = "23: Double Hump", .value = 24},
    {.name = "24: Pseudo Voice Formant 1", .value = 25},
    {.name = "25: Pseudo Voice Formant 2", .value = 26},
    {.name = "26: Metallic 1", .value = 27},
    {.name = "27: Metallic 2", .value = 28},
    {.name = "28: Noise-Like", .value = 29},
    {.name = "29: PCM(Audio) File", .value = 30},
};

static std::vector<SelectItem> ssgWsItems = {
    {.name = "0: Pulse(Rect)", .value = 1 },
    {.name = "1: Triangle / Saw", .value = 2 },
};

static std::vector<SelectItem> ssgDmItems = {
    {.name = "0: Preset Ratio", .value = 1 },
    {.name = "1: Variable (Slider)", .value = 2 },
};

static std::vector<SelectItem> ssgPrItems = {
    {.name = "0: 1:1 (50%)", .value = 1 },
    {.name = "1: 3:5 (37.5%)", .value = 2 },
    {.name = "2: 5:11 (31.25%)", .value = 3 },
    {.name = "3: 1:3 (25%)", .value = 4 },
    {.name = "4: 1:4 (20%)", .value = 5 },
    {.name = "5: 3:13 (18.75%)", .value = 6 },
    {.name = "6: 1:7 (12.5%)", .value = 7 },
    {.name = "7: 1:15 (6.25%)", .value = 8 },
};

static std::vector<SelectItem> ssgEnvItems = {
    {.name = "0: Saw Down", .value = 1 },
    {.name = "1: Saw Down & Hold", .value = 2 },
    {.name = "2: Triangle", .value = 3 },
    {.name = "3: Alternative Saw Down & Hold", .value = 4 },
    {.name = "4: Saw Up", .value = 5 },
    {.name = "5: Saw Up & Hold", .value = 6 },
    {.name = "6: Triangle Invert", .value = 7 },
    {.name = "7: Alternative Saw Up & Hold", .value = 8 },
};

static std::vector<SelectItem> wtWsItems = {
    {.name = "0: Sine",          .value = 1 },
    {.name = "1: Triangle",      .value = 2 },
    {.name = "2: Saw Up",        .value = 3 },
    {.name = "3: Saw Down",      .value = 4 },
    {.name = "4: Square (50%)",  .value = 5 },
    {.name = "5: Pulse (25%)",   .value = 6 },
    {.name = "6: Pulse (12.5%)", .value = 7 },
    {.name = "7: Digital Noise", .value = 8 },
    {.name = "8: Custom(Draw)",  .value = 9 },
};

static std::vector<SelectItem> wtTsItems = {
    {.name = "0: 32 Samples",  .value = 1 },
    {.name = "1: 64 Samples",  .value = 2 },
};

static std::vector<SelectItem> flTypeItems = {
    {.name = "LPF (Low Pass)", .value = 1 },
    {.name = "HPF (High Pass)", .value = 2 },
    {.name = "BPF (Band Pass)", .value = 3 }
};

// DT (デチューン1) 用のコンボボックスアイテム
// レジスタ仕様: 0=0, 1=+1, 2=+2, 3=+3, 4=0, 5=-1, 6=-2, 7=-3
static std::vector<SelectItem> dtItems = {
    {.name = " 0", .value = 1 },
    {.name = "-3", .value = 2 },
    {.name = "-2", .value = 3 },
    {.name = "-1", .value = 4 },
    {.name = " 0", .value = 5 }, // 実質0ですが、レジスタ4として一応用意
    {.name = "+1", .value = 6 },
    {.name = "+2", .value = 7 },
    {.name = "+3", .value = 8 }
};
