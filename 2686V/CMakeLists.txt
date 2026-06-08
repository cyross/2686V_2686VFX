cmake_minimum_required(VERSION 3.20)

# ==============================================================================
# 共通設定 (変えない部分)
# ==============================================================================
set(CY_MANUFACTURER_CODE "CYRS") # 例: あなたのメーカーID (4文字)
set(CY_COMPANY_NAME "CYROSS")  # 会社名・作者名

# VSTロゴの登録
juce_add_binary_data(VstLogoForAbout
    HEADER_NAME "VstLogoForAbout.h"
    NAMESPACE VstLogoForAboutData
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/vst3_logo.png"
)

# アイコンの登録
juce_add_binary_data(AppIconForAbout
    HEADER_NAME "AppIconForAbout.h"
    NAMESPACE AppIconAboutForAboutData
    SOURCES
        "${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.png"
)

# 定義
file(GLOB_RECURSE ALL_SOURCES 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.cpp" 
    "${CMAKE_CURRENT_SOURCE_DIR}/Source/*.h"
)

set(CONST_FILES
    "Source/Core/Const/ConstGlobal.h"
    "Source/Core/Const/ConstFileValues.h"
    "Source/Core/Const/ConstMmlKeys.h"
    "Source/Core/Const/ConstMmlValues.h"
)

set(SYNTH_FILES
    "Source/Core/Synth/SynthMode.h"
    "Source/Core/Synth/SynthCore.h"
    "Source/Core/Synth/SynthParams.h"
    "Source/Core/Synth/SynthVoice.h"
    "Source/Core/Synth/SynthVoice.cpp"
    "Source/Core/Synth/SynthHelpers.h"
    "Source/Core/Synth/SynthHelpers.cpp"
)

set(FM_FIX_FILES
    "Source/Generator/Fm/Fix/FmFixParams.h"
    "Source/Generator/Fm/Fix/FmFix.h"
    "Source/Generator/Fm/Fix/FmFix.cpp"
)

set(LFSR_NOISE_GEN_FILES
    "Source/Generator/Noise/Lfsr/GenNoiseLfsr.h"
    "Source/Generator/Noise/Lfsr/GenNoiseLfsr.cpp"
)

set(SSG_NOISE_GEN_FILES
    "Source/Generator/Noise/Ssg/GenNoiseSsg.h"
    "Source/Generator/Noise/Ssg/GenNoiseSsg.cpp"
)

set(PCM_GENERATOR_FILES
    "Source/Generator/Pcm/Adpcm/GenAdpcm.h"
    "Source/Generator/Pcm/Adpcm/GenAdpcm.cpp"
    "Source/Generator/Pcm/Dpcm/GenDpcm.h"
    "Source/Generator/Pcm/Dpcm/GenDpcm.cpp"
    "Source/Generator/Pcm/Helper/GenDpcm.h"
    "Source/Generator/Pcm/Helper/GenDpcm.cpp"
)

set(FM_FILES
    "Source/Core/Fm/FmClipboard.h"
    "Source/Core/Fm/FmCore.h"
    "Source/Core/Fm/FmCore.cpp"
    "Source/Core/Fm/FmOperator.h"
    "Source/Core/Fm/FmOperator.cpp"
    "Source/Core/Fm/FmOpParams.h"
    "Source/Core/Fm/FmRegisterType.h"
    "Source/Core/Fm/FmRegisterConverter.h"
    "Source/Core/Fm/FmRegisterConverter.cpp"
    "Source/Core/Fm/FmSliderRegMap.h"
    "Source/Core/Fm/FmSliderRegMap.cpp"
    "Source/Core/Fm/FmMmlFormatter.h"
)

set(GUI_FILES
    "Source/Core/Gui/GuiBase.h"
    "Source/Core/Gui/GuiColor.h"
    "Source/Core/Gui/GuiComponents.h"
    "Source/Core/Gui/GuiComponents.cpp"
    "Source/Core/Gui/GuiContext.h"
    "Source/Core/Gui/GuiHelpers.h"
    "Source/Core/Gui/GuiHelpers.cpp"
    "Source/Core/Gui/GuiLF.h"
    "Source/Core/Gui/GuiLF.cpp"
    "Source/Core/Gui/GuiStructs.h"
    "Source/Core/Gui/GuiText.h"
    "Source/Core/Gui/GuiValues.h"
    "Source/Core/Gui/GuiEnvelopeGraph.h"
    "Source/Core/Gui/GuiEnvelopeGraph.cpp"
)

set(PROCESSOR_FILES
    "Source/Core/Processor/ProcessorKeys.h"
    "Source/Core/Processor/ProcessorNames.h"
    "Source/Core/Processor/ProcessorValues.h"
    "Source/Core/Processor/ProcessorBase.h"
    "Source/Core/Processor/ProcessorBase.cpp"
    "Source/Core/Processor/PluginProcessor.h"
    "Source/Core/Processor/PluginProcessor.cpp"
)

set(EDITOR_FILES
    "Source/Core/Editor/EditorGuiText.h"
    "Source/Core/Editor/EditorGuiValues.h"
    "Source/Core/Editor/PluginEditor.h"
    "Source/Core/Editor/PluginEditor.cpp"
)

set(EFFECT_CORE_FILES
    "Source/Core/Effect/RateLUT.h"
)

set(AMP_ENVELOPE_FILES
    "Source/Effect/Envelope/Amp/Adsr/EnvAmpAdsrParams.h"
    "Source/Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
    "Source/Effect/Envelope/Amp/Adsr/EnvAmpAdsr.cpp"
    "Source/Effect/Envelope/Amp/FmRgAdddr/EnvFmRgAdddrParams.h"
    "Source/Effect/Envelope/Amp/FmRgAdddr/EnvFmRgAdddr.h"
    "Source/Effect/Envelope/Amp/FmRgAdddr/EnvFmRgAdddr.cpp"
    "Source/Effect/Envelope/Amp/FmRgAdssr/EnvFmRgAdssrParams.h"
    "Source/Effect/Envelope/Amp/FmRgAdssr/EnvFmRgAdssr.h"
    "Source/Effect/Envelope/Amp/FmRgAdssr/EnvFmRgAdssr.cpp"
    "Source/Effect/Envelope/Amp/OplAdsr/EnvOplAdsrParams.h"
    "Source/Effect/Envelope/Amp/OplAdsr/EnvOplAdsr.h"
    "Source/Effect/Envelope/Amp/OplAdsr/EnvOplAdsr.cpp"
    "Source/Effect/Envelope/Amp/Opzx7Adddr/EnvOpzx7AdddrParams.h"
    "Source/Effect/Envelope/Amp/Opzx7Adddr/EnvOpzx7Adddr.h"
    "Source/Effect/Envelope/Amp/Opzx7Adddr/EnvOpzx7Adddr.cpp"
    "Source/Effect/Envelope/Amp/SsgSw/EnvSsgSwParams.h"
    "Source/Effect/Envelope/Amp/SsgSw/EnvSsgSw.h"
    "Source/Effect/Envelope/Amp/SsgSw/EnvSsgSw.cpp"
)

set(PITCH_ENVELOPE_FILES
    "Source/Effect/Envelope/Pitch/Adsr/EnvPirchAdsrParams.h"
    "Source/Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
    "Source/Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.cpp"
)

set(DETUNE_FILES
    "Source/Effect/Detune/Opl/DetuneOpl.h"
    "Source/Effect/Detune/Opl/DetuneOpl.cpp"
    "Source/Effect/Detune/Opn/DetuneOpn.h"
    "Source/Effect/Detune/Opn/DetuneOpn.cpp"
    "Source/Effect/Detune/Opm/DetuneOpm.h"
    "Source/Effect/Detune/Opm/DetuneOpm.cpp"
    "Source/Effect/Detune/Opzx7/DetuneOpzx7.h"
    "Source/Effect/Detune/Opzx7/DetuneOpzx7.cpp"
)

set(LFO_FILES
    "Source/Effect/Lfo/N88/LfoN88.h"
    "Source/Effect/Lfo/N88/LfoN88.cpp"
    "Source/Effect/Lfo/Opna/LfoOpna.h"
    "Source/Effect/Lfo/Opna/LfoOpna.cpp"
    "Source/Effect/Lfo/Opl/LfoOpl.h"
    "Source/Effect/Lfo/Opl/LfoOpl.cpp"
    "Source/Effect/Lfo/Opm/LfoOpm.h"
    "Source/Effect/Lfo/Opm/LfoOpm.cpp"
    "Source/Effect/Lfo/Opzx7/LfoOpzx7.h"
    "Source/Effect/Lfo/Opzx7/LfoOpzx7.cpp"
    "Source/Effect/Lfo/Opzx7/LfoOpzx7Unit.h"
    "Source/Effect/Lfo/Opzx7/LfoOpzx7Unit.cpp"
)

set(FX_EFFECT_FILES
    "Source/Effect/Fx/Fx.h"
    "Source/Effect/Fx/Fx.cpp"
)

set(OPNA_SYNTH_FILES
    "Source/Synth/Opna/SynthOpna.h"
    "Source/Synth/Opna/SynthOpna.cpp"
    "Source/Synth/Opna/SynthOpnaParams.h"
    "Source/Synth/Opna/Operator/SynthOpnaOp.h"
    "Source/Synth/Opna/Operator/SynthOpnaOp.cpp"
    "Source/Synth/Opna/Operator/SynthOpnaOpParams.h"
)

set(OPN_SYNTH_FILES
    "Source/Synth/Opn/SynthOpn.h"
    "Source/Synth/Opn/SynthOpn.cpp"
    "Source/Synth/Opn/SynthOpnParams.h"
    "Source/Synth/Opn/Operator/SynthOpnOp.h"
    "Source/Synth/Opn/Operator/SynthOpnOp.cpp"
    "Source/Synth/Opn/Operator/SynthOpnOpParams.h"
)

set(OPL_SYNTH_FILES
    "Source/Synth/Opl/SynthOpl.h"
    "Source/Synth/Opl/SynthOpl.cpp"
    "Source/Synth/Opl/SynthOplParams.h"
    "Source/Synth/Opl/Operator/SynthOplOp.h"
    "Source/Synth/Opl/Operator/SynthOplOp.cpp"
    "Source/Synth/Opl/Operator/SynthOplOpParams.h"
)

set(OPL3_SYNTH_FILES
    "Source/Synth/Opl3/SynthOpl3.h"
    "Source/Synth/Opl3/SynthOpl3.cpp"
    "Source/Synth/Opl3/SynthOpl3Params.h"
    "Source/Synth/Opl3/Operator/SynthOpl3Op.h"
    "Source/Synth/Opl3/Operator/SynthOpl3Op.cpp"
    "Source/Synth/Opl3/Operator/SynthOpl3OpParams.h"
)

set(OPM_SYNTH_FILES
    "Source/Synth/Opm/SynthOpm.h"
    "Source/Synth/Opm/SynthOpm.cpp"
    "Source/Synth/Opm/SynthOpmParams.h"
    "Source/Synth/Opm/Operator/SynthOpmOp.h"
    "Source/Synth/Opm/Operator/SynthOpmOp.cpp"
    "Source/Synth/Opm/Operator/SynthOpmOpParams.h"
)

set(OPZX7_SYNTH_FILES
    "Source/Synth/Opzx7/SynthOpzx7.h"
    "Source/Synth/Opzx7/SynthOpzx7.cpp"
    "Source/Synth/Opzx7/SynthOpzx7Params.h"
    "Source/Synth/Opzx7/Operator/SynthOpzx7Op.h"
    "Source/Synth/Opzx7/Operator/SynthOpzx7Op.cpp"
    "Source/Synth/Opzx7/Operator/SynthOpzx7OpParams.h"
)

set(SSG_SYNTH_FILES
    "Source/Synth/Ssg/SynthSsg.h"
    "Source/Synth/Ssg/SynthSsg.cpp"
    "Source/Synth/Ssg/SynthSsgParams.h"
)

set(WT_SYNTH_FILES
    "Source/Synth/Wavetable/SynthWt.h"
    "Source/Synth/Wavetable/SynthWt.cpp"
    "Source/Synth/Wavetable/SynthWtParams.h"
)

set(RHYTHM_SYNTH_FILES
    "Source/Synth/Rhythm/SynthRhythm.h"
    "Source/Synth/Rhythm/SynthRhythm.cpp"
    "Source/Synth/Rhythm/SynthRhythmParams.h"
)

set(ADPCM_SYNTH_FILES
    "Source/Synth/Adpcm/SynthAdpcm.h"
    "Source/Synth/Adpcm/SynthAdpcm.cpp"
    "Source/Synth/Adpcm/SynthAdpcmParams.h"
)

set(BEEP_SYNTH_FILES
    "Source/Synth/Beep/SynthBeep.h"
    "Source/Synth/Beep/SynthBeep.cpp"
    "Source/Synth/Beep/SynthBeepParams.h"
)

set(CURVE_FILES
    "Source/Advanced/Curve/AdvancedCurve.h"
    "Source/Advanced/Curve/AdvancedCurve.cpp"
    "Source/Advanced/Curve/AdvancedCurveParams.h"
)

set(OPNA_PROCESSOR_FILES
    "Source/Processor/Opna/ProcessorOpnaKeys.h"
    "Source/Processor/Opna/ProcessorOpnaNames.h"
    "Source/Processor/Opna/ProcessorOpnaValues.h"
    "Source/Processor/Opna/ProcessorOpna.h"
    "Source/Processor/Opna/ProcessorOpna.cpp"
)

set(OPN_PROCESSOR_FILES
    "Source/Processor/Opn/ProcessorOpnKeys.h"
    "Source/Processor/Opn/ProcessorOpnNames.h"
    "Source/Processor/Opn/ProcessorOpnValues.h"
    "Source/Processor/Opn/ProcessorOpn.h"
    "Source/Processor/Opn/ProcessorOpn.cpp"
)

set(OPL_PROCESSOR_FILES
    "Source/Processor/Opl/ProcessorOplKeys.h"
    "Source/Processor/Opl/ProcessorOplNames.h"
    "Source/Processor/Opl/ProcessorOplValues.h"
    "Source/Processor/Opl/ProcessorOpl.h"
    "Source/Processor/Opl/ProcessorOpl.cpp"
)

set(OPL3_PROCESSOR_FILES
    "Source/Processor/Opl3/ProcessorOpl3Keys.h"
    "Source/Processor/Opl3/ProcessorOpl3Names.h"
    "Source/Processor/Opl3/ProcessorOpl3Values.h"
    "Source/Processor/Opl3/ProcessorOpl3.h"
    "Source/Processor/Opl3/ProcessorOpl3.cpp"
)

set(OPM_PROCESSOR_FILES
    "Source/Processor/Opm/ProcessorOpmKeys.h"
    "Source/Processor/Opm/ProcessorOpmNames.h"
    "Source/Processor/Opm/ProcessorOpmValues.h"
    "Source/Processor/Opm/ProcessorOpm.h"
    "Source/Processor/Opm/ProcessorOpm.cpp"
)

set(OPZX7_PROCESSOR_FILES
    "Source/Processor/Opzx7/ProcessorOpzx7Keys.h"
    "Source/Processor/Opzx7/ProcessorOpzx7Names.h"
    "Source/Processor/Opzx7/ProcessorOpzx7Values.h"
    "Source/Processor/Opzx7/ProcessorOpzx7.h"
    "Source/Processor/Opzx7/ProcessorOpzx7.cpp"
)

set(SSG_PROCESSOR_FILES
    "Source/Processor/Ssg/ProcessorSsgKeys.h"
    "Source/Processor/Ssg/ProcessorSsgNames.h"
    "Source/Processor/Ssg/ProcessorSsgValues.h"
    "Source/Processor/Ssg/ProcessorSsg.h"
    "Source/Processor/Ssg/ProcessorSsg.cpp"
)

set(WT_PROCESSOR_FILES
    "Source/Processor/Wavetable/ProcessorWtKeys.h"
    "Source/Processor/Wavetable/ProcessorWtNames.h"
    "Source/Processor/Wavetable/ProcessorWtValues.h"
    "Source/Processor/Wavetable/ProcessorWt.h"
    "Source/Processor/Wavetable/ProcessorWt.cpp"
)

set(RHYTHM_PROCESSOR_FILES
    "Source/Processor/Rhythm/ProcessorRhythmKeys.h"
    "Source/Processor/Rhythm/ProcessorRhythmNames.h"
    "Source/Processor/Rhythm/ProcessorRhythmValues.h"
    "Source/Processor/Rhythm/ProcessorRhythm.h"
    "Source/Processor/Rhythm/ProcessorRhythm.cpp"
)

set(ADPCM_PROCESSOR_FILES
    "Source/Processor/Adpcm/ProcessorAdpcmKeys.h"
    "Source/Processor/Adpcm/ProcessorAdpcmNames.h"
    "Source/Processor/Adpcm/ProcessorAdpcmValues.h"
    "Source/Processor/Adpcm/ProcessorAdpcm.h"
    "Source/Processor/Adpcm/ProcessorAdpcm.cpp"
)

set(BEEP_PROCESSOR_FILES
    "Source/Processor/Beep/ProcessorBeepKeys.h"
    "Source/Processor/Beep/ProcessorBeepNames.h"
    "Source/Processor/Beep/ProcessorBeepValues.h"
    "Source/Processor/Beep/ProcessorBeep.h"
    "Source/Processor/Beep/ProcessorBeep.cpp"
)

set(CURVE_PROCESSOR_FILES
    "Source/Processor/Curve/ProcessorCurveKeys.h"
    "Source/Processor/Curve/ProcessorCurveNames.h"
    "Source/Processor/Curve/ProcessorCurveValues.h"
    "Source/Processor/Curve/ProcessorCurve.h"
    "Source/Processor/Curve/ProcessorCurve.cpp"
)

set(FX_PROCESSOR_FILES
    "Source/Processor/Fx/ProcessorFxKeys.h"
    "Source/Processor/Fx/ProcessorFxNames.h"
    "Source/Processor/Fx/ProcessorFxValues.h"
    "Source/Processor/Fx/ProcessorFx.h"
    "Source/Processor/Fx/ProcessorFx.cpp"
)

set(OPNA_GUI_FILES
    "Source/Gui/Opna/GuiOpnaText.h"
    "Source/Gui/Opna/GuiOpnaValues.h"
    "Source/Gui/Opna/GuiOpna.h"
    "Source/Gui/Opna/GuiOpna.cpp"
)

set(OPN_GUI_FILES
    "Source/Gui/Opn/GuiOpnText.h"
    "Source/Gui/Opn/GuiOpnValues.h"
    "Source/Gui/Opn/GuiOpn.h"
    "Source/Gui/Opn/GuiOpn.cpp"
)

set(OPL_GUI_FILES
    "Source/Gui/Opl/GuiOplText.h"
    "Source/Gui/Opl/GuiOplValues.h"
    "Source/Gui/Opl/GuiOpl.h"
    "Source/Gui/Opl/GuiOpl.cpp"
)

set(OPL3_GUI_FILES
    "Source/Gui/Opl3/GuiOpl3Text.h"
    "Source/Gui/Opl3/GuiOpl3Values.h"
    "Source/Gui/Opl3/GuiOpl3.h"
    "Source/Gui/Opl3/GuiOpl3.cpp"
)

set(OPM_GUI_FILES
    "Source/Gui/Opm/GuiOpmText.h"
    "Source/Gui/Opm/GuiOpmValues.h"
    "Source/Gui/Opm/GuiOpm.h"
    "Source/Gui/Opm/GuiOpm.cpp"
)

set(OPZX7_GUI_FILES
    "Source/Gui/Opzx7/GuiOpzx7Helpers.h"
    "Source/Gui/Opzx7/GuiOpzx7Helpers.cpp"
    "Source/Gui/Opzx7/GuiOpzx7Text.h"
    "Source/Gui/Opzx7/GuiOpzx7Values.h"
    "Source/Gui/Opzx7/GuiOpzx7.h"
    "Source/Gui/Opzx7/GuiOpzx7.cpp"
)

set(SSG_GUI_FILES
    "Source/Gui/Ssg/GuiSsgText.h"
    "Source/Gui/Ssg/GuiSsgValues.h"
    "Source/Gui/Ssg/GuiSsg.h"
    "Source/Gui/Ssg/GuiSsg.cpp"
)

set(WT_GUI_FILES
    "Source/Gui/Wavetable/GuiWtHelpers.h"
    "Source/Gui/Wavetable/GuiWtHelpers.cpp"
    "Source/Gui/Wavetable/GuiWtText.h"
    "Source/Gui/Wavetable/GuiWtValues.h"
    "Source/Gui/Wavetable/GuiWt.h"
    "Source/Gui/Wavetable/GuiWt.cpp"
)

set(RHYTHM_GUI_FILES
    "Source/Gui/Rhythm/GuiRhythmHelpers.h"
    "Source/Gui/Rhythm/GuiRhythmHelpers.cpp"
    "Source/Gui/Rhythm/GuiRhythmText.h"
    "Source/Gui/Rhythm/GuiRhythmValues.h"
    "Source/Gui/Rhythm/GuiRhythm.h"
    "Source/Gui/Rhythm/GuiRhythm.cpp"
)

set(ADPCM_GUI_FILES
    "Source/Gui/Adpcm/GuiAdpcmHelpers.h"
    "Source/Gui/Adpcm/GuiAdpcmHelpers.cpp"
    "Source/Gui/Adpcm/GuiAdpcmText.h"
    "Source/Gui/Adpcm/GuiAdpcmValues.h"
    "Source/Gui/Adpcm/GuiAdpcm.h"
    "Source/Gui/Adpcm/GuiAdpcm.cpp"
)

set(BEEP_GUI_FILES
    "Source/Gui/Beep/GuiBeepText.h"
    "Source/Gui/Beep/GuiBeepValues.h"
    "Source/Gui/Beep/GuiBeep.h"
    "Source/Gui/Beep/GuiBeep.cpp"
)

set(FX_GUI_FILES
    "Source/Gui/Fx/GuiFxText.h"
    "Source/Gui/Fx/GuiFxValues.h"
    "Source/Gui/Fx/GuiFx.h"
    "Source/Gui/Fx/GuiFx.cpp"
)

set(PRESET_GUI_FILES
    "Source/Gui/Preset/GuiPresetText.h"
    "Source/Gui/Preset/GuiPresetValues.h"
    "Source/Gui/Preset/PresetKeys.h"
    "Source/Gui/Preset/PresetValues.h"
    "Source/Gui/Preset/GuiPreset.h"
    "Source/Gui/Preset/GuiPreset.cpp"
)

set(SETTINGS_GUI_FILES
    "Source/Gui/Settings/GuiSettingsHelpers.h"
    "Source/Gui/Settings/GuiSettingsHelpers.cpp"
    "Source/Gui/Settings/GuiSettingsText.h"
    "Source/Gui/Settings/GuiSettingsValues.h"
    "Source/Gui/Settings/SettingsKeys.h"
    "Source/Gui/Settings/SettingsValues.h"
    "Source/Gui/Settings/GuiSettings.h"
    "Source/Gui/Settings/GuiSettings.cpp"
)

set(ABOUT_GUI_FILES
    "Source/Gui/About/GuiAboutText.h"
    "Source/Gui/About/GuiAboutValues.h"
    "Source/Gui/About/GuiAbout.h"
    "Source/Gui/About/GuiAbout.cpp"
)

set(CURVE_GUI_FILES
    "Source/Gui/Curve/GuiCurveText.h"
    "Source/Gui/Curve/GuiCurveValues.h"
    "Source/Gui/Curve/GuiCurveGraph.h"
    "Source/Gui/Curve/GuiCurveGraph.cpp"
    "Source/Gui/Curve/GuiCurve.h"
    "Source/Gui/Curve/GuiCurve.cpp"
)

set(GUI_COMPONENT_UNISON_FILES
    "Source/Gui/Components/Unison/Unison.h"
    "Source/Gui/Components/Unison/Unison.cpp"
)

set(GUI_COMPONENT_FIX_FILES
    "Source/Gui/Components/Fix/Fix.h"
    "Source/Gui/Components/Fix/Fix.cpp"
)

set(GUI_COMPONENT_MULDETUNE_FILES
    "Source/Gui/Components/MulDetune/MulDetune.h"
    "Source/Gui/Components/MulDetune/MulDetune.cpp"
)

set(GUI_COMPONENT_AMPENV_FILES
    "Source/Gui/Components/AmpEnv/AmpEnv.h"
    "Source/Gui/Components/AmpEnv/AmpEnv.cpp"
)

set(GUI_COMPONENT_PITCHENV_FILES
    "Source/Gui/Components/PitchEnv/PitchEnv.h"
    "Source/Gui/Components/PitchEnv/PitchEnv.cpp"
)

set(GUI_COMPONENT_SSGSWENV_FILES
    "Source/Gui/Components/SsgSwEnv/SsgSwEnv.h"
    "Source/Gui/Components/SsgSwEnv/SsgSwEnv.cpp"
)

set(GUI_COMPONENT_PITCHBUTTONS_FILES
    "Source/Gui/Components/PitchButtons/PitchButtons.h"
    "Source/Gui/Components/PitchButtons/PitchButtons.cpp"
)

set(GUI_COMPONENT_MIDI_FILES
    "Source/Gui/Components/Midi/Midi.h"
    "Source/Gui/Components/Midi/Midi.cpp"
)

set(GUI_COMPONENT_LFO_OPZX7_FILES
    "Source/Gui/Components/LfoOpzx7/LfoOpzx7.h"
    "Source/Gui/Components/LfoOpzx7/LfoOpzx7.cpp"
)

set(DOCUMENT_FILES "../readme.md" "../readme.en.md")

# ソースグループの設定
source_group("2686V\\Constants" FILES ${CONST_FILES})
source_group("2686V\\Documents" FILES ${DOCUMENT_FILES})
source_group("2686V\\Advanced\\Curve" FILES ${CURVE_FILES})
source_group("2686V\\Generator\\Pcm" FILES ${PCM_GENERATOR_FILES})
source_group("2686V\\Generator\\Noise\\Lfsr" FILES ${LFSR_NOISE_GEN_FILES})
source_group("2686V\\Generator\\Noise\\Ssg" FILES ${SSG_NOISE_GEN_FILES})
source_group("2686V\\Generator\\Fm\\Fix" FILES ${FM_FIX_FILES})
source_group("2686V\\Synth\\Core" FILES ${SYNTH_FILES})
source_group("2686V\\Synth\\Fm" FILES ${FM_FILES})
source_group("2686V\\Synth\\Opna" FILES ${OPNA_SYNTH_FILES})
source_group("2686V\\Synth\\Opn" FILES ${OPN_SYNTH_FILES})
source_group("2686V\\Synth\\Opl" FILES ${OPL_SYNTH_FILES})
source_group("2686V\\Synth\\Opl3" FILES ${OPL3_SYNTH_FILES})
source_group("2686V\\Synth\\Opm" FILES ${OPM_SYNTH_FILES})
source_group("2686V\\Synth\\Opzx7" FILES ${OPZX7_SYNTH_FILES})
source_group("2686V\\Synth\\Ssg" FILES ${SSG_SYNTH_FILES})
source_group("2686V\\Synth\\Wavetable" FILES ${WT_SYNTH_FILES})
source_group("2686V\\Synth\\Rhythm" FILES ${RHYTHM_SYNTH_FILES})
source_group("2686V\\Synth\\Adpcm" FILES ${ADPCM_SYNTH_FILES})
source_group("2686V\\Synth\\Beep" FILES ${BEEP_SYNTH_FILES})
source_group("2686V\\Effect\\Core" FILES ${EFFECT_CORE_FILES})
source_group("2686V\\Effect\\Envelope\\Amp" FILES ${AMP_ENVELOPE_FILES})
source_group("2686V\\Effect\\Envelope\\Pitch" FILES ${PITCH_ENVELOPE_FILES})
source_group("2686V\\Effect\\Detune" FILES ${DETUNE_FILES})
source_group("2686V\\Effect\\Lfo" FILES ${LFO_FILES})
source_group("2686V\\Effect\\Fx" FILES ${FX_EFFECT_FILES})
source_group("2686V\\Processor\\Core" FILES ${PROCESSOR_FILES})
source_group("2686V\\Processor\\Opna" FILES ${OPNA_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opn" FILES ${OPN_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opl" FILES ${OPL_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opl3" FILES ${OPL3_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opm" FILES ${OPM_PROCESSOR_FILES})
source_group("2686V\\Processor\\Opzx7" FILES ${OPZX7_PROCESSOR_FILES})
source_group("2686V\\Processor\\Ssg" FILES ${SSG_PROCESSOR_FILES})
source_group("2686V\\Processor\\Wavetable" FILES ${WT_PROCESSOR_FILES})
source_group("2686V\\Processor\\Rhythm" FILES ${RHYTHM_PROCESSOR_FILES})
source_group("2686V\\Processor\\Adpcm" FILES ${ADPCM_PROCESSOR_FILES})
source_group("2686V\\Processor\\Beep" FILES ${BEEP_PROCESSOR_FILES})
source_group("2686V\\Processor\\Fx" FILES ${FX_PROCESSOR_FILES})
source_group("2686V\\Processor\\Curve" FILES ${CURVE_PROCESSOR_FILES})
source_group("2686V\\Gui\\Core" FILES ${GUI_FILES})
source_group("2686V\\Gui\\Components\\Unison" FILES ${GUI_COMPONENT_UNISON_FILES})
source_group("2686V\\Gui\\Components\\Fix" FILES ${GUI_COMPONENT_FIX_FILES})
source_group("2686V\\Gui\\Components\\MulDetune" FILES ${GUI_COMPONENT_MULDETUNE_FILES})
source_group("2686V\\Gui\\Components\\AmpEnv" FILES ${GUI_COMPONENT_AMPENV_FILES})
source_group("2686V\\Gui\\Components\\PitchEnv" FILES ${GUI_COMPONENT_PITCHENV_FILES})
source_group("2686V\\Gui\\Components\\SsgSwEnv" FILES ${GUI_COMPONENT_SSGSWENV_FILES})
source_group("2686V\\Gui\\Components\\Lfo\\Opzx7" FILES ${GUI_COMPONENT_LFO_OPZX7_FILES})
source_group("2686V\\Gui\\Components\\Midi" FILES ${GUI_COMPONENT_MIDI_FILES})
source_group("2686V\\Gui\\Components\\PitchButtons" FILES ${GUI_COMPONENT_PITCHBUTTONS_FILES})
source_group("2686V\\Gui\\Editor" FILES ${EDITOR_FILES})
source_group("2686V\\Gui\\Tab\\Opna" FILES ${OPNA_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Opn" FILES ${OPN_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Opl" FILES ${OPL_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Opl3" FILES ${OPL3_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Opm" FILES ${OPM_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Opzx7" FILES ${OPZX7_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Ssg" FILES ${SSG_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Wavetable" FILES ${WT_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Rhythm" FILES ${RHYTHM_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Adpcm" FILES ${ADPCM_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Beep" FILES ${BEEP_GUI_FILES})
source_group("2686V\\Gui\\Tab\\Curve" FILES ${CURVE_GUI_FILES})
source_group("2686V\\Gui\\Pane\\Fx" FILES ${FX_GUI_FILES})
source_group("2686V\\Gui\\Dialog\\Preset" FILES ${PRESET_GUI_FILES})
source_group("2686V\\Gui\\Dialog\\Settings" FILES ${SETTINGS_GUI_FILES})
source_group("2686V\\Gui\\Dialog\\About" FILES ${ABOUT_GUI_FILES})

# ==============================================================================
# プラグイン生成関数 (関数化して使い回す)
# ==============================================================================
function(create_cyross_plugin TARGET_NAME)
    set(CY_PLUGIN_CODE "2686")

    juce_add_plugin(${TARGET_NAME}
        COMPANY_NAME ${CY_COMPANY_NAME}
        IS_SYNTH TRUE
        NEEDS_MIDI_INPUT TRUE
        NEEDS_MIDI_OUTPUT FALSE
        IS_MIDI_EFFECT FALSE
        EDITOR_WANTS_KEYBOARD_FOCUS TRUE
        COPY_PLUGIN_AFTER_BUILD FALSE
        PLUGIN_MANUFACTURER_CODE ${CY_MANUFACTURER_CODE}
        PLUGIN_CODE ${CY_PLUGIN_CODE}
        FORMATS VST3 Standalone
        ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/Resources/icon.png"
        PRODUCT_NAME ${TARGET_NAME}
        VST3_AUTO_MANIFEST FALSE
    )

    if(MSVC)
        target_compile_options(${TARGET_NAME} PRIVATE /utf-8 /FS)
    endif()

    target_sources(${TARGET_NAME} PRIVATE ${ALL_SOURCES})

    if(UNIX AND NOT APPLE)
        target_link_libraries(${TARGET_NAME} PRIVATE
            freetype
            fontconfig
            X11
            Xcursor
            Xinerama
            Xrandr
            Xcomposite
            asound
            GL
        )
    endif()

    target_link_libraries(${TARGET_NAME} PRIVATE
        AppIconForAbout
        VstLogoForAbout
        juce::juce_audio_utils
        juce::juce_recommended_config_flags
        juce::juce_dsp
    )

    target_compile_definitions(${TARGET_NAME} PRIVATE
        JUCE_VST3_CAN_REPLACE_VST2=0
        JUCE_WEB_BROWSER=0
        JUCE_USE_CURL=0
        JUCE_USE_CAMERA=0
        JUCE_USE_CDREADER=0
        JUCE_USE_CDBURNER=0
    )

    juce_generate_juce_header(${TARGET_NAME})
endfunction()

create_cyross_plugin("2686V")
