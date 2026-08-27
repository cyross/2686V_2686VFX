// カテゴライズできない定数を管理

#pragma once

#include <JuceHeader.h>

namespace Global
{
	static inline const int unisonVoices = 8; // ユニゾン発音数
	// Para-Distance / Para-Detune の設定対象数。
	// ボイス0はメイン(素の音程・定位)なので対象外となり、ボイス数-1 になる。
	static inline const int unisonParaVoices = unisonVoices - 1;
	static inline const int voices = 10; // 最大同時発音数
	static inline const int totalVoices = voices * unisonVoices; // 最大同時発音数 * ユニゾン発音数
	static inline constexpr int floatDecimalPlaces = 6; // パラメータ書き出し時の小数点以下の桁数

	namespace Plugin
	{
		static inline const juce::String name = ProjectInfo::projectName;
		static inline const juce::String version = ProjectInfo::versionString;
		static inline const juce::String author = "Copyright (C) 2026 CYROSS";
		static inline const juce::String parameters = "Parameters";
		static inline const juce::String allowedControlChars = "\r\n\t";
	};

	namespace Audio
	{
		static inline const juce::String input = "Input";
		static inline const juce::String output = "Output";
	};

	namespace About
	{
		namespace Gpl
		{
			static inline const juce::String notice = "This software is released under the GNU General Public License v3.0 (GPLv3).";
			static inline const juce::String navigate = "View Full License (GNU.org)";
			static inline const juce::String linkUrl = "https://www.gnu.org/licenses/gpl-3.0.en.html";
		};

		namespace Vst
		{
			static inline const juce::String notice = "VST is a registered trademark of Steinberg Media Technologies GmbH.";
		}
	};

	namespace RateMinSeconds {
		static inline constexpr float reg = 0.0f; // レジスタレートの最長=0秒
		static inline constexpr float real = 0.0f; // 実数レートの最長=0秒
	}

	namespace RateMaxSeconds {
		static inline constexpr float reg = 5.0f; // レジスタレートの最長=5秒
		static inline constexpr float real = 10.0f; // 実数レートの最長=10秒
	}

	namespace Lfo {
		// AM(トレモロ)の最大減衰量[dB]。全音源で共通の上限値。
		//
		// 実機の AMS テーブルは OPM で最大 95.6dB、OPZ 系でも 96dB 相当まで下がるが、
		// そこまで落とすと LFO の谷で完全に無音になり、音が途切れてプチノイズの
		// 原因になる。そのため実機 OPNA の AMS 最大値 23.9dB を切り上げた 24dB を
		// 全音源共通の上限とし、AMS の段数比だけを各チップ固有のカーブとして残す。
		static inline constexpr float maxAmDepthDb = 24.0f;
	}
};
