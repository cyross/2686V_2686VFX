#pragma once

#include<JuceHeader.h>

// タブ名
static inline const juce::String opnaTabName = "OPNA";
static inline const juce::String opnTabName = "OPN";
static inline const juce::String oplTabName = "OPL";
static inline const juce::String opl3TabName = "OPL3";
static inline const juce::String opmTabName = "OPM";
static inline const juce::String opzx3TabName = "OPZX3";
static inline const juce::String ssgTabName = "SSG";
static inline const juce::String wtTabName = "WT";
static inline const juce::String rhythmTabName = "RHYTHM";
static inline const juce::String adpcmTabName = "ADPCM";
static inline const juce::String fxTabName = "FX";
static inline const juce::String presetTabName = "PRESET";
static inline const juce::String settingsTabName = "SETTINGS";
static inline const juce::String aboutTabName = "ABOUT";

// グループタイトル
static inline const juce::String mGroupTitle = "Main";

// メイン
static inline const juce::String mBitTitle = "BIT";
static inline const juce::String mRateTitle = "RATE";
static inline const juce::String mAlgTitle = "ALG";
static inline const juce::String mFb0Title = "FB0";
static inline const juce::String mFb2Title = "FB2";
static inline const juce::String mLfoFreq = "L.FREQ";
static inline const juce::String mLfoAms = "L.AMS";
static inline const juce::String mLfoPms = "L.PMS";
static inline const juce::String mArLabel = "AR";
static inline const juce::String mDrLabel = "DR";
static inline const juce::String mSlLabel = "SL";
static inline const juce::String mRrLabel = "RR";

// オペレーター
static inline const juce::String opGroupPrefix = "Operator ";
static inline const juce::String opLabel = " OP";

static inline const juce::String opArLabel = "AR";
static inline const juce::String opDrLabel = "DR";
static inline const juce::String opSlLabel = "SL";
static inline const juce::String opRrLabel = "RR";
static inline const juce::String opSrLabel = "SR";
static inline const juce::String opD1rLabel = "D1R";
static inline const juce::String opD1lLabel = "D1L";
static inline const juce::String opD2rLabel = "D2R";
static inline const juce::String opMulLabel = "MUL";
static inline const juce::String opTlLabel = "TL";
static inline const juce::String opDtLabel = "DT";
static inline const juce::String opDt1Label = "DT1";
static inline const juce::String opDt2Label = "DT2";
static inline const juce::String opKsLabel = "KS";
static inline const juce::String opKsrLabel = "KSR";
static inline const juce::String opKslLabel = "KSL";
static inline const juce::String opMaskLabel = "MASK";
static inline const juce::String opFixLabel = "FIX";
static inline const juce::String opFFreqLabel = "FFREQ";
static inline const juce::String opSEnvLabel = "SENV";
static inline const juce::String opSFreqLabel = "SFREQ";
static inline const juce::String opAmLabel = "AM";
static inline const juce::String opVibLabel = "VIB";
static inline const juce::String opEgTypeLabel = "EGTYPE";
static inline const juce::String opEgLabel = "EG";
static inline const juce::String opWsLabel = "WS";
static inline const juce::String opPcmLabel = "PCM";
static inline const juce::String opPcmClearLabel = "X";
static inline const juce::String opFreqTo0Label = "Freq -> 0Hz";
static inline const juce::String opFreqTo440Label = "Freq -> 440Hz";

static inline const juce::String mPostBitTitle = " BIT";
static inline const juce::String mPostRateTitle = " RATE";
static inline const juce::String mPostAlgTitle = " ALG";
static inline const juce::String mPostFb0Title = " FB0";
static inline const juce::String mPostFb2Title = " FB2";
static inline const juce::String mPostLfoFreq = " L.FREQ";
static inline const juce::String mPostLfoAms = " L.AMS";
static inline const juce::String mPostLfoPms = " L.PMS";

static inline const juce::String opPostArLabel = " AR";
static inline const juce::String opPostDrLabel = " DR";
static inline const juce::String opPostSlLabel = " SL";
static inline const juce::String opPostRrLabel = " RR";
static inline const juce::String opPostSrLabel = " SR";
static inline const juce::String opPostD1rLabel = " D1R";
static inline const juce::String opPostD1lLabel = " D1L";
static inline const juce::String opPostD2rLabel = " D2R";
static inline const juce::String opPostMulLabel = " MUL";
static inline const juce::String opPostTlLabel = " Tl";
static inline const juce::String opPostDtLabel = " Dt";
static inline const juce::String opPostDt1Label = " Dt1";
static inline const juce::String opPostDt2Label = " Dt2";
static inline const juce::String opPostSeLabel = " SE";
static inline const juce::String opPostSeFreqLabel = " SEFREQ";
static inline const juce::String opPostKsLabel = " KS";
static inline const juce::String opPostKsrLabel = " KSR";
static inline const juce::String opPostKslLabel = " KSL";
static inline const juce::String opPostMaskLabel = " MASK";
static inline const juce::String opPostFixLabel = " FIX";
static inline const juce::String opPostFixFreqLabel = " FREQ";
static inline const juce::String opPostWsLabel = " WS";
static inline const juce::String opPostAmLabel = " AM";
static inline const juce::String opPostVibLabel = " VIB";
static inline const juce::String opPostEgTypeLabel = " EG TYPE";
static inline const juce::String opPostEgLabel = " EG";
static inline const juce::String opPostPanLabel = " Pan";

// Ssg
static inline const juce::String ssgPostTone = " Tone";
static inline const juce::String ssgPostNoise = " Noise";
static inline const juce::String ssgPostNoiseFreq = " Noise Freq";
static inline const juce::String ssgPostNoiseOnNote = " Noise On Note";
static inline const juce::String ssgPostToneNoiseMix = " Tone/Noise Mix";
static inline const juce::String ssgPostWaveform = " Waveform";
static inline const juce::String ssgPostAdsrBypass = " ADSR Bypass";
static inline const juce::String ssgPostDutyMode = " Duty Mode";
static inline const juce::String ssgPostDutyPreset = " Duty Preset";
static inline const juce::String ssgPostDutyVar = " Duty Var";
static inline const juce::String ssgPostDutyInvert = " Duty Invert";
static inline const juce::String ssgPostTriangleKeyTrack = " Triangle Key Track";
static inline const juce::String ssgPostTrianglePeak = " Triangle Peak";
static inline const juce::String ssgPostTriangleManualFreq = " Tri Manual Freq";
static inline const juce::String ssgPostHwEnvEnable = " HW Env Enable";
static inline const juce::String ssgPostEnvShape = " Env Shape";
static inline const juce::String ssgPostEnvSpeed = " Env Speed";

// Wt
static inline const juce::String wtCodeCustom32 = " Custom32 ";
static inline const juce::String wtCodeCustom64 = " Custom64 ";
static inline const juce::String wtPostSampleSize = " Sample Size";
static inline const juce::String wtPostWaveform = " Waveform";
static inline const juce::String wtPostModEnable = " Mod Enable";
static inline const juce::String wtPostModDepth = " Mod Depth";
static inline const juce::String wtPostModSpeed = " Mod Speed";
static inline const juce::String wtPostLevel = " Level";

// Rhythm
static inline const juce::String rhythmCodePad = " Pad";
static inline const juce::String rhythmPostVol = " Vol";
static inline const juce::String rhythmPostNote = " Note";
static inline const juce::String rhythmPostQuality = " Quality";
static inline const juce::String rhythmPostOneShot = " One Shot";

// Adpcm
static inline const juce::String adpcmPostLevel = " Level";
static inline const juce::String adpcmPostLoop = " Loop";
static inline const juce::String adpcmPostQuality = " Quality";

// FX
static inline const juce::String masterBypassLabel = "M.Bypass";
static inline const juce::String fxTremoloLabel = " Tremolo";
static inline const juce::String fxVibratoLabel = " Vibrato";
static inline const juce::String fxMBCLabel = " Modern BC";
static inline const juce::String fxDelayLabel = " Delay";
static inline const juce::String fxReverbLabel = " Reverb";
static inline const juce::String fxRBCLabel = " Retro BC";
static inline const juce::String fxPostBypassLabel = " Bypass";
static inline const juce::String fxPostMixLabel = " Mix";
static inline const juce::String fxPostFbLabel = " Feedback";
static inline const juce::String fxPostRateLabel = " Rate";
static inline const juce::String fxPostDepthLabel = " Depth";
static inline const juce::String fxPostQualityLabel = " Quality";
static inline const juce::String fxPostTimeLabel = " Time";
static inline const juce::String fxPostSizeLabel = " Size";
static inline const juce::String fxPostDampLabel = " Damp";

// マスターボリューム
static inline const juce::String masterVolumeLabel = "M.Vol";
static inline const juce::String masterVolumeUnit = " dB";

static inline const juce::String modeLabel = "Mode";
