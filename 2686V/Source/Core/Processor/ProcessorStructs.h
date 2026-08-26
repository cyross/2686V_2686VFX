#pragma once

#include <array>
#include <atomic>

#include "../Const/ConstGlobal.h"

struct PrPtrsAlgFb {
    std::atomic<float>* alg = nullptr;
    std::atomic<float>* fb = nullptr;
};

struct PrPtrsOpzx7AlgFb {
    std::atomic<float>* alg = nullptr;
    std::atomic<float>* fb1 = nullptr;
    std::atomic<float>* fb2 = nullptr;
    std::atomic<float>* fb3 = nullptr;
    std::atomic<float>* fb4 = nullptr;
    std::atomic<float>* fb5 = nullptr;
    std::atomic<float>* fb6 = nullptr;
    std::atomic<float>* fb7 = nullptr;
    std::atomic<float>* fb8 = nullptr;
};

struct PrPtrsQuality {
    std::atomic<float>* depth = nullptr;
    std::atomic<float>* rate = nullptr;
};

struct PrPtrsQualityPcm {
    std::atomic<float>* mode = nullptr;
    std::atomic<float>* rate = nullptr;
    std::atomic<float>* interp = nullptr;
};

struct PrPtrsAdsrAmpEnv {
    std::atomic<float>* bypass = nullptr;
    std::atomic<float>* stl = nullptr;
    std::atomic<float>* ar = nullptr;
    std::atomic<float>* dr = nullptr;
    std::atomic<float>* sl = nullptr;
    std::atomic<float>* rr = nullptr;
    std::atomic<float>* kor = nullptr;
};

struct PrPtrsSsgSwEnv {
    std::atomic<float>* bypass = nullptr;
    std::atomic<float>* steps = nullptr;
    std::atomic<float>* loop = nullptr;
    std::atomic<float>* loopTo = nullptr;
    std::atomic<float>* loopCount = nullptr;
    std::atomic<float>* r1 = nullptr;
    std::atomic<float>* r2 = nullptr;
    std::atomic<float>* r3 = nullptr;
    std::atomic<float>* r4 = nullptr;
    std::atomic<float>* r5 = nullptr;
    std::atomic<float>* r6 = nullptr;
    std::atomic<float>* stl = nullptr;
    std::atomic<float>* l1 = nullptr;
    std::atomic<float>* l2 = nullptr;
    std::atomic<float>* l3 = nullptr;
    std::atomic<float>* l4 = nullptr;
    std::atomic<float>* l5 = nullptr;
    std::atomic<float>* l6 = nullptr;
};

struct PrPtrsSsgSwEnvOp {
    std::atomic<float>* enable = nullptr;
    std::atomic<float>* steps = nullptr;
    std::atomic<float>* loop = nullptr;
    std::atomic<float>* loopTo = nullptr;
    std::atomic<float>* loopCount = nullptr;
    std::atomic<float>* r1 = nullptr;
    std::atomic<float>* r2 = nullptr;
    std::atomic<float>* r3 = nullptr;
    std::atomic<float>* r4 = nullptr;
    std::atomic<float>* r5 = nullptr;
    std::atomic<float>* r6 = nullptr;
    std::atomic<float>* stl = nullptr;
    std::atomic<float>* l1 = nullptr;
    std::atomic<float>* l2 = nullptr;
    std::atomic<float>* l3 = nullptr;
    std::atomic<float>* l4 = nullptr;
    std::atomic<float>* l5 = nullptr;
    std::atomic<float>* l6 = nullptr;
};

struct PrPtrsSsgSwEnv11 {
    std::atomic<float>* bypass = nullptr;
    std::atomic<float>* steps = nullptr;
    std::atomic<float>* loop = nullptr;
    std::atomic<float>* loopTo = nullptr;
    std::atomic<float>* loopCount = nullptr;
    std::atomic<float>* r1 = nullptr;
    std::atomic<float>* r2 = nullptr;
    std::atomic<float>* r3 = nullptr;
    std::atomic<float>* r4 = nullptr;
    std::atomic<float>* r5 = nullptr;
    std::atomic<float>* r6 = nullptr;
    std::atomic<float>* r7 = nullptr;
    std::atomic<float>* r8 = nullptr;
    std::atomic<float>* r9 = nullptr;
    std::atomic<float>* r10 = nullptr;
    std::atomic<float>* r11 = nullptr;
    std::atomic<float>* stl = nullptr;
    std::atomic<float>* l1 = nullptr;
    std::atomic<float>* l2 = nullptr;
    std::atomic<float>* l3 = nullptr;
    std::atomic<float>* l4 = nullptr;
    std::atomic<float>* l5 = nullptr;
    std::atomic<float>* l6 = nullptr;
    std::atomic<float>* l7 = nullptr;
    std::atomic<float>* l8 = nullptr;
    std::atomic<float>* l9 = nullptr;
    std::atomic<float>* l10 = nullptr;
    std::atomic<float>* l11 = nullptr;
};

struct PrPtrsSsgSwEnv11Op {
    std::atomic<float>* enable = nullptr;
    std::atomic<float>* steps = nullptr;
    std::atomic<float>* loop = nullptr;
    std::atomic<float>* loopTo = nullptr;
    std::atomic<float>* loopCount = nullptr;
    std::atomic<float>* r1 = nullptr;
    std::atomic<float>* r2 = nullptr;
    std::atomic<float>* r3 = nullptr;
    std::atomic<float>* r4 = nullptr;
    std::atomic<float>* r5 = nullptr;
    std::atomic<float>* r6 = nullptr;
    std::atomic<float>* r7 = nullptr;
    std::atomic<float>* r8 = nullptr;
    std::atomic<float>* r9 = nullptr;
    std::atomic<float>* r10 = nullptr;
    std::atomic<float>* r11 = nullptr;
    std::atomic<float>* stl = nullptr;
    std::atomic<float>* l1 = nullptr;
    std::atomic<float>* l2 = nullptr;
    std::atomic<float>* l3 = nullptr;
    std::atomic<float>* l4 = nullptr;
    std::atomic<float>* l5 = nullptr;
    std::atomic<float>* l6 = nullptr;
    std::atomic<float>* l7 = nullptr;
    std::atomic<float>* l8 = nullptr;
    std::atomic<float>* l9 = nullptr;
    std::atomic<float>* l10 = nullptr;
    std::atomic<float>* l11 = nullptr;
};

struct PrPtrsPitchEnv {
    std::atomic<float>* bypass = nullptr;
    std::atomic<float>* ar = nullptr;
    std::atomic<float>* dr = nullptr;
    std::atomic<float>* rr = nullptr;
    std::atomic<float>* stl = nullptr;
    std::atomic<float>* atl = nullptr;
    std::atomic<float>* ssl = nullptr;
    std::atomic<float>* rll = nullptr;
};

struct PrPtrsPitchEnvOp {
    std::atomic<float>* enable = nullptr;
    std::atomic<float>* ar = nullptr;
    std::atomic<float>* dr = nullptr;
    std::atomic<float>* rr = nullptr;
    std::atomic<float>* stl = nullptr;
    std::atomic<float>* atl = nullptr;
    std::atomic<float>* ssl = nullptr;
    std::atomic<float>* rll = nullptr;
};

struct PrPtrsSsgSwPEnv11 {
    std::atomic<float>* bypass = nullptr;
    std::atomic<float>* steps = nullptr;
    std::atomic<float>* loop = nullptr;
    std::atomic<float>* loopTo = nullptr;
    std::atomic<float>* loopCount = nullptr;
    std::atomic<float>* r1 = nullptr;
    std::atomic<float>* r2 = nullptr;
    std::atomic<float>* r3 = nullptr;
    std::atomic<float>* r4 = nullptr;
    std::atomic<float>* r5 = nullptr;
    std::atomic<float>* r6 = nullptr;
    std::atomic<float>* r7 = nullptr;
    std::atomic<float>* r8 = nullptr;
    std::atomic<float>* r9 = nullptr;
    std::atomic<float>* r10 = nullptr;
    std::atomic<float>* r11 = nullptr;
    std::atomic<float>* stl = nullptr;
    std::atomic<float>* l1 = nullptr;
    std::atomic<float>* l2 = nullptr;
    std::atomic<float>* l3 = nullptr;
    std::atomic<float>* l4 = nullptr;
    std::atomic<float>* l5 = nullptr;
    std::atomic<float>* l6 = nullptr;
    std::atomic<float>* l7 = nullptr;
    std::atomic<float>* l8 = nullptr;
    std::atomic<float>* l9 = nullptr;
    std::atomic<float>* l10 = nullptr;
    std::atomic<float>* l11 = nullptr;
};

struct PrPtrsSsgSwPEnv11Op {
    std::atomic<float>* enable = nullptr;
    std::atomic<float>* steps = nullptr;
    std::atomic<float>* loop = nullptr;
    std::atomic<float>* loopTo = nullptr;
    std::atomic<float>* loopCount = nullptr;
    std::atomic<float>* r1 = nullptr;
    std::atomic<float>* r2 = nullptr;
    std::atomic<float>* r3 = nullptr;
    std::atomic<float>* r4 = nullptr;
    std::atomic<float>* r5 = nullptr;
    std::atomic<float>* r6 = nullptr;
    std::atomic<float>* r7 = nullptr;
    std::atomic<float>* r8 = nullptr;
    std::atomic<float>* r9 = nullptr;
    std::atomic<float>* r10 = nullptr;
    std::atomic<float>* r11 = nullptr;
    std::atomic<float>* stl = nullptr;
    std::atomic<float>* l1 = nullptr;
    std::atomic<float>* l2 = nullptr;
    std::atomic<float>* l3 = nullptr;
    std::atomic<float>* l4 = nullptr;
    std::atomic<float>* l5 = nullptr;
    std::atomic<float>* l6 = nullptr;
    std::atomic<float>* l7 = nullptr;
    std::atomic<float>* l8 = nullptr;
    std::atomic<float>* l9 = nullptr;
    std::atomic<float>* l10 = nullptr;
    std::atomic<float>* l11 = nullptr;
};

struct PrPtrsOplDetune {
    std::atomic<float>* mutiple = nullptr;
};

struct PrPtrsOpmDetune {
    std::atomic<float>* multiple = nullptr;
    std::atomic<float>* mulRatio = nullptr;
    std::atomic<float>* detune = nullptr;
    std::atomic<float>* detune2 = nullptr;
};

struct PrPtrsOpnDetune {
    std::atomic<float>* multiple = nullptr;
    std::atomic<float>* detune = nullptr;
};

struct PrPtrsOpzx7Detune {
    std::atomic<float>* multiple = nullptr;
    std::atomic<float>* mulRatio = nullptr;
    std::atomic<float>* detune = nullptr;
    std::atomic<float>* detune2 = nullptr;
    std::atomic<float>* detune3 = nullptr;
};

struct PrPtrsOplLfo {
    std::atomic<float>* pmEnable = nullptr;
    std::atomic<float>* pms = nullptr;
    std::atomic<float>* pmd = nullptr;
    std::atomic<float>* amEnable = nullptr;
    std::atomic<float>* ams = nullptr;
    std::atomic<float>* amd = nullptr;
};

struct PrPtrsOpmLfo {
    std::atomic<float>* freq = nullptr;
    std::atomic<float>* syncDelay = nullptr;
    std::atomic<float>* pmWave = nullptr;
    std::atomic<float>* amWave = nullptr;
    std::atomic<float>* pmEnable = nullptr;
    std::atomic<float>* pmd = nullptr;
    std::atomic<float>* pms = nullptr;
    std::atomic<float>* amEnable = nullptr;
    std::atomic<float>* amSmRt = nullptr;
    std::atomic<float>* amd = nullptr;
    std::atomic<float>* ams = nullptr;
};

struct PrPtrsOpOpmLfo {
    std::atomic<float>* pOpLfoAmsEnable = nullptr;
};

struct PrPtrsOpnaLfo {
    std::atomic<float>* freqs = nullptr;
    std::atomic<float>* syncDelay = nullptr;
    std::atomic<float>* pmEnable = nullptr;
    std::atomic<float>* pms = nullptr;
    std::atomic<float>* amEnable = nullptr;
    std::atomic<float>* ams = nullptr;
};

struct PrPtrsN88Lfo {
    std::atomic<float>* freq = nullptr;
    std::atomic<float>* syncDelay = nullptr;
    std::atomic<float>* shape = nullptr;
    std::atomic<float>* pmEnable = nullptr;
    std::atomic<float>* pmd = nullptr;
    std::atomic<float>* pms = nullptr;
    std::atomic<float>* amEnable = nullptr;
    std::atomic<float>* amSmRt = nullptr;
    std::atomic<float>* amd = nullptr;
};

struct PrPtrsOpN88Lfo {
    std::atomic<float>* pOpN88LfoAms = nullptr;
};

struct PrPtrsOpzx7Lfo {
    std::atomic<float>* pmSyncDelay = nullptr;
    std::atomic<float>* amSyncDelay = nullptr;
    std::atomic<float>* amSmoothRatio = nullptr;
    std::atomic<float>* pmFreq = nullptr;
    std::atomic<float>* amFreq = nullptr;
    std::atomic<float>* pmShape = nullptr;
    std::atomic<float>* amShape = nullptr;
    std::atomic<float>* pm = nullptr;
    std::atomic<float>* am = nullptr;
    std::atomic<float>* pmd = nullptr;
    std::atomic<float>* pms = nullptr;
    std::atomic<float>* amd = nullptr;
    std::atomic<float>* ams = nullptr;
};

struct PrPtrsFix {
    std::atomic<float>* enable = nullptr;
    std::atomic<float>* freq = nullptr;
};

struct PrPtrsUnison {
    std::atomic<float>* voices = nullptr;
    std::atomic<float>* detuneCents = nullptr;
    std::atomic<float>* spread = nullptr;
    std::atomic<float>* arpEnable = nullptr;
    std::atomic<float>* arpFreq = nullptr;
    std::atomic<float>* arpSmooth = nullptr;
    std::array<std::atomic<float>*, Global::unisonParaVoices> paraDistance{};
    std::array<std::atomic<float>*, Global::unisonParaVoices> paraDetune{};
};

struct PrPtrsToneNoise {
    std::atomic<float>* tone = nullptr;
    std::atomic<float>* noise = nullptr;
    std::atomic<float>* noiseFreq = nullptr;
    std::atomic<float>* noiseOnNote = nullptr;
    std::atomic<float>* mix = nullptr;
};

struct PrPtrsPcm {
    std::atomic<float>* offset = nullptr;
    std::atomic<float>* ratio = nullptr;
};

struct PrPtrsLp {
    std::atomic<float>* enable = nullptr;
    std::atomic<float>* start = nullptr;
    std::atomic<float>* end = nullptr;
};

struct PrPtrsWtMod {
    std::atomic<float>* enable = nullptr;
    std::atomic<float>* depth = nullptr;
    std::atomic<float>* speed = nullptr;
};

struct PrPtrsSsgDuty {
    std::atomic<float>* mode = nullptr;
    std::atomic<float>* preset = nullptr;
    std::atomic<float>* var = nullptr;
    std::atomic<float>* invert = nullptr;
    std::atomic<float>* fc = nullptr;
    std::atomic<float>* fcFluc = nullptr;
};

struct PrPtrsSsgTri {
    std::atomic<float>* keyTrk = nullptr;
    std::atomic<float>* peak = nullptr;
    std::atomic<float>* freq = nullptr;
};

struct PrPtrsSsgHwEnv {
    std::atomic<float>* enable = nullptr;
    std::atomic<float>* shape = nullptr;
    std::atomic<float>* period = nullptr;
    std::atomic<float>* min = nullptr;
    std::atomic<float>* max = nullptr;
    std::atomic<float>* smooth = nullptr;
};

struct PrPtrsPanpot {
    std::atomic<float>* enable = nullptr;
    std::atomic<float>* panpot = nullptr;
};

struct PrPtrsWtBasic {
    std::atomic<float>* level = nullptr;
    std::atomic<float>* wave = nullptr;
    std::atomic<float>* sampleSize = nullptr;
    std::atomic<float>* step = nullptr;
};

struct PrPtrsWt2Basic {
    std::atomic<float>* level = nullptr;
    std::atomic<float>* wave = nullptr;
    std::atomic<float>* sampleSize = nullptr;
    std::atomic<float>* resolution = nullptr;
};

struct PrPtrsWtCustomWave {
    std::array<std::atomic<float>*, 32> c32 = { nullptr };
    std::array<std::atomic<float>*, 64> c64 = { nullptr };
    std::array<std::atomic<float>*, 128> c128 = { nullptr };
    std::array<std::atomic<float>*, 256> c256 = { nullptr };
};

struct PrPtrsAdpcmBasic {
    std::atomic<float>* level = nullptr;
    std::atomic<float>* pan = nullptr;
    std::atomic<float>* loop = nullptr;
};

struct PrPtrsBeepBasic {
    std::atomic<float>* level = nullptr;
    std::atomic<float>* antiAlias = nullptr;
};

struct PrPtrsOplBasic {
    std::atomic<float>* level = nullptr;
};

struct PrPtrsOpl3Basic {
    std::atomic<float>* level = nullptr;
};

struct PrPtrsOpmBasic {
    std::atomic<float>* level = nullptr;
    std::atomic<float>* pan = nullptr;
};

struct PrPtrsOpnBasic {
    std::atomic<float>* level = nullptr;
};

struct PrPtrsOpnaBasic {
    std::atomic<float>* level = nullptr;
    std::atomic<float>* pan = nullptr;
};

struct PrPtrsOpzx7Basic {
    std::atomic<float>* level = nullptr;
};

struct PrPtrsRhythmBasic {
    std::atomic<float>* level = nullptr;
};

struct PrPtrsRhythmPadBasic {
    std::atomic<float>* level = nullptr;
    std::atomic<float>* pan = nullptr;
    std::atomic<float>* noteNumber = nullptr;
    std::atomic<float>* isOneShot = nullptr;
};

struct PrPtrsSsgBasic {
    std::atomic<float>* level = nullptr;
    std::atomic<float>* waveform = nullptr;
};

struct PrPtrsOplAdsr {
    std::atomic<float>* bypass = nullptr;
    std::atomic<float>* ar = nullptr;
    std::atomic<float>* dr = nullptr;
    std::atomic<float>* sl = nullptr;
    std::atomic<float>* rr = nullptr;
    std::atomic<float>* tl = nullptr;
    std::atomic<float>* ksr = nullptr;
    std::atomic<float>* ksl = nullptr;
    std::atomic<float>* egType = nullptr;
    std::atomic<float>* sus = nullptr;
    std::atomic<float>* xof = nullptr;
    std::atomic<float>* kor = nullptr;
};

struct PrPtrsOpl3Adsr {
    std::atomic<float>* bypass = nullptr;
    std::atomic<float>* ar = nullptr;
    std::atomic<float>* dr = nullptr;
    std::atomic<float>* sl = nullptr;
    std::atomic<float>* rr = nullptr;
    std::atomic<float>* tl = nullptr;
    std::atomic<float>* ksr = nullptr;
    std::atomic<float>* ksl = nullptr;
    std::atomic<float>* xof = nullptr;
    std::atomic<float>* kor = nullptr;
    std::atomic<float>* egType = nullptr;
};

struct PrPtrsOpmAdsr {
    std::atomic<float>* bypass = nullptr;
    std::atomic<float>* ar = nullptr;
    std::atomic<float>* d1r = nullptr;
    std::atomic<float>* d1l = nullptr;
    std::atomic<float>* d2r = nullptr;
    std::atomic<float>* rr = nullptr;
    std::atomic<float>* tl = nullptr;
    std::atomic<float>* ksMode = nullptr;
    std::atomic<float>* ks = nullptr;
    std::atomic<float>* ksrOPP = nullptr;
    std::atomic<float>* kslOPP = nullptr;
    std::atomic<float>* xof = nullptr;
    std::atomic<float>* kor = nullptr;
};

struct PrPtrsOpnAdsr {
    std::atomic<float>* bypass = nullptr;
    std::atomic<float>* ar = nullptr;
    std::atomic<float>* dr = nullptr;
    std::atomic<float>* sl = nullptr;
    std::atomic<float>* sr = nullptr;
    std::atomic<float>* rr = nullptr;
    std::atomic<float>* tl = nullptr;
    std::atomic<float>* ks = nullptr;
    std::atomic<float>* xof = nullptr;
    std::atomic<float>* kor = nullptr;
};

struct PrPtrsOpzx7Adsr {
    std::atomic<float>* bypass = nullptr;
    std::atomic<float>* rgEnable = nullptr;
    std::atomic<float>* realAr = nullptr;
    std::atomic<float>* realD1r = nullptr;
    std::atomic<float>* realD1l = nullptr;
    std::atomic<float>* realD2r = nullptr;
    std::atomic<float>* realRr = nullptr;
    std::atomic<float>* realTl = nullptr;
    std::atomic<float>* ar = nullptr;
    std::atomic<float>* d1r = nullptr;
    std::atomic<float>* d1l = nullptr;
    std::atomic<float>* d2r = nullptr;
    std::atomic<float>* rr = nullptr;
    std::atomic<float>* tl = nullptr;
    std::atomic<float>* ksEn = nullptr;
    std::atomic<float>* ksMode = nullptr;
    std::atomic<float>* ksrMA7 = nullptr;
    std::atomic<float>* kslMA7 = nullptr;
    std::atomic<float>* ksrOPZ = nullptr;
    std::atomic<float>* kslOPZ = nullptr;
    std::atomic<float>* ksBp = nullptr;
    std::atomic<float>* ksLc = nullptr;
    std::atomic<float>* ksRc = nullptr;
    std::atomic<float>* ksLd = nullptr;
    std::atomic<float>* ksRd = nullptr;
    std::atomic<float>* ksRs = nullptr;
    std::atomic<float>* sus = nullptr;
    std::atomic<float>* xof = nullptr;
    std::atomic<float>* kor = nullptr;
};

struct PrPtrsOplEg {
    std::atomic<float>* eg = nullptr;
};

struct PrPtrsSsgEg {
    std::atomic<float>* ssgEg = nullptr;
    std::atomic<float>* ssgEgFreq = nullptr;
};

struct PrPtrsWs {
    std::atomic<float>* waveSelect = nullptr;
};
