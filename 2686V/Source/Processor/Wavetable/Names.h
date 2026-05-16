#pragma once

#include<JuceHeader.h>

#include "../../Core/Processor/Names.h"

// UI上に表示するタイトル等を管理
namespace WtPrName
{
	static inline const juce::String custom32 = " Custom32 ";
	static inline const juce::String custom64 = " Custom64 ";
	static inline const juce::String custom128 = " Custom128 ";
	static inline const juce::String custom256 = " Custom256 ";

	static inline const juce::String level = " Level";
	static inline const juce::String sampleSize = " Sample Size";
	static inline const juce::String steps = " Steps";
	static inline const juce::String waveform = " Waveform";
	static inline const juce::String bit = " Bit";
	static inline const juce::String rate = " Rate";

	namespace Mod
	{
		static inline const juce::String enable = " Mod Enable";
		static inline const juce::String depth = " Mod Depth";
		static inline const juce::String speed = " Mod Speed";
	}

	namespace Adsr
	{
		static inline const juce::String bypass = " ADSR Bypass";
	}

	namespace PitchAdsr
	{
		static inline const juce::String bypass = " PitchEnv Bypass";
	};

	static inline const juce::String dt1 = CorePrName::Fm::Op::dt1;
	static inline const juce::String dt2 = CorePrName::Fm::Op::dt2;

	namespace Lfo
	{
		static inline const juce::String pmFreq = " LFO PM Speed";
		static inline const juce::String amFreq = " LFO AM Speed";
		static inline const juce::String pmShape = " LFO PM Shape";
		static inline const juce::String amShape = " LFO AM Shape";
		static inline const juce::String am = " LFO Am Enable";
		static inline const juce::String pm = " LFO Pm Enable";
		static inline const juce::String ams = " LFO AMS";
		static inline const juce::String pms = " LFO PMS";
		static inline const juce::String pmd = " LFO PMD";
		static inline const juce::String amd = " LFO AMD";
		static inline const juce::String amSmoothRatio = " LFO AM Smooth Rate";
		static inline const juce::String syncDelay = " LFO Sync Delay";
	}
}
