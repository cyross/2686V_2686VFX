#pragma once

#include <JuceHeader.h>

static inline const float opBoolThread = 0.5f; // 0.5以上をON、未満をOFFとみなす閾値

static inline const int mAlgMin = 0; // 最小値
static inline const int mAlgMax = 7; // 最大値
static inline const int mAlgDefault = 7; // 初期値

static inline const int mOplAlgMin = 0; // 最小値
static inline const int mOplAlgMax = 1; // 最大値
static inline const int mOplAlgDefault = 0; // 初期値

static inline const int mOpl3AlgMin = 0; // 最小値
static inline const int mOpl3AlgMax = 3; // 最大値
static inline const int mOpl3AlgDefault = 0; // 初期値

static inline const int mOpxz3AlgMin = 0; // 最小値
static inline const int mOpxz3AlgMax = 27; // 最大値
static inline const int mOpxz3AlgDefault = 0; // 初期値

static inline const float mFb0Min = 0.0f; // 最小値
static inline const float mFb0Max = 7.0f; // 最大値
static inline const float mFb0Default = 0.0f; // 初期値

static inline const float mFb2Min = 0.0f; // 最小値
static inline const float mFb2Max = 7.0f; // 最大値
static inline const float mFb2Default = 0.0f; // 初期値

static inline const int mBitMin = 1; // 最小値
static inline const int mBitMax = 5; // 最大値
static inline const int mBitDefault = 5; // 初期値
static inline const int mRateMin = 1; // 最小値
static inline const int mRateMax = 7; // 最大値
static inline const int mRateDefault = 2; // 初期値
static inline const float mLfoFreqMin = 0.1f; // 最小値
static inline const float mLfoFreqMax = 200.0f; // 最大値
static inline const float mLfoFreqDefault = 5.0f; // 初期値
static inline const int mLfoPmsMin = 0; // 最小値
static inline const int mLfoPmsMax = 7; // 最大値
static inline const int mLfoPmsDefault = 0; // 初期値
static inline const int mLfoAmsMin = 0; // 最小値
static inline const int mLfoAmsMax = 3; // 最大値
static inline const int mLfoAmsDefault = 0; // 初期値

static inline const int opMulMin = 0; // 最小値（倍率）
static inline const int opMulMax = 15; // 最大値（倍率）
static inline const int opMulDefault = 1; // 初期値（倍率）
static inline const int opDtMin = 0; // 最小値（セント）
static inline const int opDtMax = 7; // 最大値（セント）
static inline const int opDtDefault = 0; // 最大値（セント）
static inline const int opDt1Min = 0; // 最小値（セント）
static inline const int opDt1Max = 7; // 最大値（セント）
static inline const int opDt1Default = 0; // 最大値（セント）
static inline const int opDt2Min = 0; // 最小値（セント）
static inline const int opDt2Max = 3; // 最大値（セント）
static inline const int opDt2Default = 0; // 最大値（セント）
static inline const float opArMin = 0.03f; // 最小値（秒）
static inline const float opArMax = 5.0f;  // 最大値（秒）
static inline const float opArDefault = 0.03f; // 初期値（秒）
static inline const float opDrMin = 0.0f; // 最小値（秒）
static inline const float opDrMax = 5.0f;  // 最大値（秒）
static inline const float opDrDefault = 0.0f; // 初期値（秒）
static inline const float opD1rMin = 0.0f; // 最小値（秒）
static inline const float opD1rMax = 5.0f; // 最大値（秒）
static inline const float opD1rDefault = 0.0f; // 初期値（秒）
static inline const float opD1lMin = 0.0f; // 最小値（レベル）
static inline const float opD1lMax = 1.0f; // 最大値（レベル）
static inline const float opD1lDefault = 1.0f; // 初期値（レベル）
static inline const float opD2rMin = 0.0f; // 最小値（秒）
static inline const float opD2rMax = 10.0f; // 最大値（秒）
static inline const float opD2rDefault = 0.0f; // 初期値（秒）
static inline const float opSlMin = 0.0f; // 最小値
static inline const float opSlMax = 1.0f;  // 最大値
static inline const float opSlDefault = 1.0f; // 初期値
static inline const float opSrMin = 0.0f; // 最小値（秒）
static inline const float opSrMax = 10.0f;  // 最大値（秒）
static inline const float opSrDefault = 0.0f; // 初期値（秒）
static inline const float opRrMin = 0.03f; // 最小値（秒）
static inline const float opRrMax = 5.0f;  // 最大値（秒）
static inline const float opRrDefault = 0.03f; // 初期値（秒）
static inline const float opTlMin = 0.0f; // 最小値
static inline const float opTlMax = 1.0f;  // 最大値
static inline const float opTlDefault = 0.0f; // 初期値
static inline const int opKsMin = 0; // 最小値
static inline const int opKsMax = 3; // 最大値
static inline const int opKsDefault = 0; // 最大値
static inline const bool opAmDefault = false; // 初期値
static inline const bool opVibDefault = false; // 初期値
static inline const bool opEgTypeDefault = true; // 初期値
static inline const bool opKsrDefault = false; // 初期値
static inline const int opKslMin = 0; // 最小値
static inline const int opKslMax = 3; // 最大値
static inline const int opKslDefault = 0; // 最大値
static inline const int opEgMin = 0; // 最小値
static inline const int opEgMax = 3; // 最大値
static inline const int opEgDefault = 0; // 最大値
static inline const int opSeMin = 0; // 最小値
static inline const int opSeMax = 15; // 最大値
static inline const int opSeDefault = 0; // 最大値
static inline const float opSeFreqMin = 0.1f; // 最小値
static inline const float opSeFreqMax = 20.0f; // 最大値
static inline const float opSeFreqDefault = 1.0f; // 初期値
static inline const int opWsMin = 0; // 最小値
static inline const int opWsMax = 29; // 最大値
static inline const int opWsDefault = 0; // 最大値
static inline const bool opFixDefault = false; // 初期値 (Fixed Frequency Switch)
static const juce::NormalisableRange<float> opFixFreqRange = juce::NormalisableRange<float>(0.0f, 8000.0f, 0.0f, 0.3f); // 範囲
static inline const float opFixFreqDefault = 440.0f; // 初期値
static inline const bool opMaskDefault = false; // 初期値

static inline const int ssgBitDefault = 3; // 初期値
static inline const int ssgRateDefault = 6; // 初期値
static inline const float ssgToneMin = 0.0f; // 最小値
static inline const float ssgToneMax = 1.0f;  // 最大値
static inline const float ssgToneDefault = 1.0f; // 初期値
static inline const float ssgNoiseMin = 0.0f; // 最小値
static inline const float ssgNoiseMax = 1.0f;  // 最大値
static inline const float ssgNoiseDefault = 0.0f; // 初期値
static inline const float ssgNoiseMixMin = 0.0f; // 最小値
static inline const float ssgNoiseMixMax = 1.0f;  // 最大値
static inline const float ssgNoiseMixDefault = 0.5f; // 初期値
static inline const float ssgNoiseOnNoteDefault = false; // 初期値
static inline const int ssgWaveformMin = 0; // 最小値
static inline const int ssgWaveformMax = 1; // 最大値
static inline const int ssgWaveformDefault = 0; // 最大値
static inline const float ssgAdsrBypassDefault = false; // 初期値
static inline const int ssgDutyModeMin = 0; // 最小値
static inline const int ssgDutyModeMax = 1; // 最大値
static inline const int ssgDutyModeDefault = 0; // 最大値
static inline const int ssgDutyPresetMin = 0; // 最小値
static inline const int ssgDutyPresetMax = 8; // 最大値
static inline const int ssgDutyPresetDefault = 0; // 最大値
static inline const float ssgDutyVarMin = 0.0f; // 最小値
static inline const float ssgDutyVarMax = 0.5f;  // 最大値
static inline const float ssgDutyVarDefault = 0.5f; // 初期値
static inline const float ssgDutyInvertDefault = false; // 初期値
static inline const float ssgTryKeyTrackDefault = true; // 初期値
static inline const float ssgTryPeakMin = 0.0f; // 最小値
static inline const float ssgTryPeakMax = 1.0f;  // 最大値
static inline const float ssgTryPeakDefault = 0.5f; // 初期値
static const juce::NormalisableRange<float> ssgNoiseFreqRange = juce::NormalisableRange<float>(0.0f, 30000.0f, 1.0f, 0.3f); // 範囲
static inline const float ssgNoiseFreqDefault = 12000.0f; // 初期値
static const juce::NormalisableRange<float> ssgTriangleManualFreqRange = juce::NormalisableRange<float>(0.0f, 8000.0f, 0.0f, 0.3f); // 範囲
static inline const float ssgTriangleManualFreqDefault = 440.0f; // 初期値
static inline const bool ssgHwEnvDefault = false; // 初期値
static inline const int ssgHwEnvShapeMin = 0; // 最小値
static inline const int ssgHwEnvShapeMax = 7; // 最大値
static inline const int ssgHwEnvShapeDefault = 0; // 最大値
static inline const float ssgHwEnvSpeedMin = 0.1f; // 最小値
static inline const float ssgHwEnvSpeedMax = 200.0f; // 最大値
static inline const float ssgHwEnvSpeedDefault = 1.0f; // 初期値

static inline const int wtBitDefault = 3; // 初期値
static inline const int wtRateDefault = 6; // 初期値
static inline const int wtSampleSizeMin = 0; // 最小値
static inline const int wtSampleSizeMax = 1; // 最大値
static inline const int wtSampleSizeDefault = 0; // 最大値
static inline const int wtWaveformMin = 0; // 最小値
static inline const int wtWaveformMax = 8; // 最大値
static inline const int wtWaveformDefault = 0; // 最大値
static inline const float wtCustomMin = -1.0f; // 最小値
static inline const float wtCustomMax = 1.0f;  // 最大値
static inline const float wtCustomDefault = 0.0f; // 初期値
static inline const int wtCustomSize1 = 32; // 最大値
static inline const int wtCustomSize2 = 64; // 最大値
static inline const float wtLevelMin = 0.0f; // 最小値
static inline const float wtLevelMax = 1.0f;  // 最大値
static inline const float wtLevelDefault = 0.5f; // 初期値
static inline const float wtModEnableDefault = false; // 初期値
static inline const float wtModDepthMin = 0.1f; // 最小値
static inline const float wtModDepthMax = 1.0f; // 最大値
static inline const float wtModDepthDefault = 0.2f; // 初期値
static inline const float wtModSpeedMin = 0.1f; // 最小値
static inline const float wtModSpeedMax = 10.0f; // 最大値
static inline const float wtModSpeedDefault = 1.0f; // 初期値

static inline const float rhythmVolMin = 0.0f; // 最小値
static inline const float rhythmVolMax = 1.0f;  // 最大値
static inline const float rhythmVolDefault = 1.0f; // 初期値
static inline const float rhythmPadVolMin = 0.0f; // 最小値
static inline const float rhythmPadVolMax = 1.0f;  // 最大値
static inline const float rhythmPadVolDefault = 1.0f; // 初期値
static inline const float rhythmPadPanMin = 0.0f; // 最小値
static inline const float rhythmPadPanMax = 1.0f;  // 最大値
static inline const float rhythmPadPanDefault = 0.5f; // 初期値
static inline const int rhythmPadNoteMin = 0; // 最小値
static inline const int rhythmPadNoteMax = 127; // 最大値
static inline const int rhythmPadNoteDefaultBase = 60; // 初期値
static inline const int rhythmPadBitMin = 1; // 最小値
static inline const int rhythmPadBitMax = 7; // 最大値
static inline const int rhythmPadBitDefault = 7; // 初期値
static inline const int rhythmPadRateMin = 1; // 最小値
static inline const int rhythmPadRateMax = 7; // 最大値
static inline const int rhythmPadRateDefault = 6; // 初期値
static inline const float rhythmPadOneShotDefault = true; // 初期値

static inline const float adpcmVolMin = 0.0f; // 最小値
static inline const float adpcmVolMax = 1.0f;  // 最大値
static inline const float adpcmVolDefault = 1.0f; // 初期値
static inline const float adpcmPanMin = 0.0f; // 最小値
static inline const float adpcmPanMax = 1.0f;  // 最大値
static inline const float adpcmPanDefault = 0.5f; // 初期値
static inline const float adpcmLoopDefault = true; // 初期値
// 0:Raw, 1:24bit, 2:16bit, 3:8bit, 4:5bit, 5:4bit PCM 6:ADPCM(4bit)
static inline const int adpcmBitMin = 1; // 最小値
static inline const int adpcmBitMax = 7; // 最大値
static inline const int adpcmBitDefault = 1; // 初期値
// 0: 96kHz 1: 55.5kHz 2: 48kHz 3: 44.1kHz 4: 22.5kHz 5: 16k 6: 8k
static inline const int adpcmRateMin = 1; // 最小値
static inline const int adpcmRateMax = 7; // 最大値
static inline const int adpcmRateDefault = 4; // 初期値

static inline const float fxMBypassDefault = false; // 初期値
static inline const float fxBypassDefault = false; // 初期値
static inline const float fxMixMin = 0.0f; // 最小値
static inline const float fxMixMax = 1.0f;  // 最大値
static inline const float fxMixDefault = 0.0f; // 初期値

static inline const float fxTrmRateMin = 0.1f; // 最小値
static inline const float fxTrmRateMax = 20.0f;  // 最大値
static inline const float fxTrmRateDefault = 5.0f; // 初期値
static inline const float fxTrmDepthMin = 0.0f; // 最小値
static inline const float fxTrmDepthMax = 1.0f;  // 最大値
static inline const float fxTrmDepthDefault = 0.5f; // 初期値

static inline const float fxVibRateMin = 0.1f; // 最小値
static inline const float fxVibRateMax = 10.0f;  // 最大値
static inline const float fxVibRateDefault = 2.0f; // 初期値
static inline const float fxVibDepthMin = 0.0f; // 最小値
static inline const float fxVibDepthMax = 1.0f;  // 最大値
static inline const float fxVibDepthDefault = 0.5f; // 初期値

static inline const float fxMbcRateMin = 1.0f; // 最小値
static inline const float fxMbcRateMax = 50.0f;  // 最大値
static inline const float fxMbcRateDefault = 1.0f; // 初期値
static inline const float fxMbcBitMin = 2.0f; // 最小値
static inline const float fxMbcBitMax = 24.0f;  // 最大値
static inline const float fxMbcBitDefault = 24.0f; // 初期値

static inline const float fxDlyTimeMin = 10.0f; // 最小値
static inline const float fxDlyTimeMax = 1000.0f;  // 最大値
static inline const float fxDlyTimeDefault = 375.0f; // 初期値
static inline const float fxDlyFbMin = 0.0f; // 最小値
static inline const float fxDlyFbMax = 0.95f;  // 最大値
static inline const float fxDlyFbDefault = 0.4f; // 初期値

static inline const float fxRvbSizeMin = 0.0f; // 最小値
static inline const float fxRvbSizeMax = 1.0f;  // 最大値
static inline const float fxRvbSizeDefault = 0.5f; // 初期値
static inline const float fxRvbDampMin = 0.0f; // 最小値
static inline const float fxRvbDampMax = 1.0f;  // 最大値
static inline const float fxRvbDampDefault = 0.5f; // 初期値

// 1:Raw, 2:24bit, 3:16bit, 4:8bit, 5:5bit, 6:4bit PCM 7:ADPCM(4bit)
static inline const int fxRbcRateMin = 1; // 最小値
static inline const int fxRbcRateMax = 7;  // 最大値
static inline const int fxRbcRateDefault = 6; // 初期値
// 1: 96kHz 2: 55.5kHz 3: 48kHz 4: 44.1kHz 5: 22.5kHz 6: 16k 7: 8k
static inline const int fxRbcBitMin = 1; // 最小値
static inline const int fxRbcBitMax = 7;  // 最大値
static inline const int fxRbcBitDefault = 7; // 初期値

// FX Filter (State Variable Filter)
static inline const float fxFlBypassDefault = false; // 初期値
static inline const int fxFlTypeMin = 1;
static inline const int fxFlTypeMax = 3;
static inline const int fxFlTypeDefault = 1; // 1:LPF, 2:HPF, 3:BPF
static const juce::NormalisableRange<float> fxFlFreqRange = juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.3f);
static inline const float fxFlFreqDefault = 20000.0f;
static inline const float fxFlQMin = 0.1f;
static inline const float fxFlQMax = 10.0f;
static inline const float fxFlQDefault = 0.707f;
static inline const float fxFlMixMin = 0.0f;
static inline const float fxFlMixMax = 1.0f;
static inline const float fxFlMixDefault = 1.0f;

// FX Soft Clipper
static inline const float fxScBypassDefault = false; // 初期値
static inline const float fxScMixMin = 0.0f;
static inline const float fxScMixMax = 1.0f;
static inline const float fxScMixDefault = 1.0f;

static inline const float masterVolumeMin = -60.0f; // 最小値（dB）
static inline const float masterVolumeMax = 6.0f;   // 最大値（dB）
static inline const float masterVolumeInterval = 0.1f; // ステップ値（dB）
static inline const float masterVolumeSkew = 1.0f; // スキュー（1.0は線形）
static inline const float masterVolumeDefault = -3.0f; // デフォルト値（dB）
