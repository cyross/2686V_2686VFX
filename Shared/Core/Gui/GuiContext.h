#pragma once

#include <JuceHeader.h>

#include "Shared/Core/Gui/GuiHost.h"

// 画面の部品へ渡すもの。プロセッサとエディタは窓口 (GuiHost.h) として持つ。
// タブなどプラグインの側のコードは、pluginOf / editorOf (GuiPluginContext.h)
// で具体的なクラスへ戻して使う。
struct GuiContext
{
	GuiProcessorHost& audioProcessor;
	GuiEditorHost& editor;
	juce::AudioProcessorValueTreeState& apvts;

	GuiContext(GuiProcessorHost& p, GuiEditorHost& e, juce::AudioProcessorValueTreeState& vts) :
		audioProcessor(p),
		editor(e),
		apvts(vts)
	{
	}
};
