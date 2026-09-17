// UI のラベルで使用する文字列を管理

#pragma once

#include<JuceHeader.h>

#include "../../Core/Gui/GuiI18n.h"

namespace CurveGuiText
{
	static inline const I18n::Text curveGroup{ u8"カーブ編集", u8"Curve editing" };
	static inline const juce::String position = u8"Position";
	static inline const juce::String target = u8"Target";
	static inline const juce::String param = u8"Param";
	static inline const I18n::Text enable{ u8"カーブモード", u8"Curve mode" };
	static inline const I18n::Text logic{ u8"ロジック", u8"Logic" };

	// ロジックの名前。組み合わせのものは、この部品をつないで作る
	// (GuiCurve.cpp の curveLogicItems)。31 通りを別々に持つと、訳を
	// 直すときに取りこぼす。
	namespace Logic
	{
		static inline const I18n::Text linear{ u8"線形", u8"Linear" };
		static inline const I18n::Text arcExp{ u8"円弧(指数的)", u8"Arc (exponential)" };
		static inline const I18n::Text arcLog{ u8"円弧(対数的)", u8"Arc (logarithmic)" };
		static inline const I18n::Text exp{ u8"指数関数", u8"Exponential" };
		static inline const I18n::Text log{ u8"対数関数", u8"Logarithmic" };
		static inline const I18n::Text spline1{ u8"1点スプライン", u8"1-point spline" };
		static inline const I18n::Text spline2{ u8"2点スプライン", u8"2-point spline" };
	}
	static inline const juce::String k = u8"K";
	static inline const juce::String rate = u8"Rate";
	static inline const juce::String posX = u8"PX";
	static inline const juce::String posY = u8"PY";
	static inline const juce::String pos1X = u8"P1X";
	static inline const juce::String pos1Y = u8"P1Y";
	static inline const juce::String pos2X = u8"P2X";
	static inline const juce::String pos2Y = u8"P2Y";
	static inline const juce::String cpX = u8"CX";
	static inline const juce::String cpY = u8"CY";
	static inline const juce::String cp1X = u8"C1X";
	static inline const juce::String cp1Y = u8"C1Y";
	static inline const juce::String cp2X = u8"C2X";
	static inline const juce::String cp2Y = u8"C2Y";
	static inline const juce::String cp3X = u8"C3X";
	static inline const juce::String cp3Y = u8"C3Y";
	static inline const juce::String cp4X = u8"C4X";
	static inline const juce::String cp4Y = u8"C4Y";
	static inline const juce::String cp5X = u8"C5X";
	static inline const juce::String cp5Y = u8"C5Y";
	static inline const juce::String cp6X = u8"C6X";
	static inline const juce::String cp6Y = u8"C6Y";
}
