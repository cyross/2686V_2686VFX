#include "./GuiPluginContext.h"

#include "Shared/Core/Gui/GuiContext.h"
#include "../Editor/PluginEditor.h"
#include "../Processor/PluginProcessor.h"

AudioPlugin2686V& pluginOf(const GuiContext& ctx)
{
    return static_cast<AudioPlugin2686V&>(ctx.audioProcessor);
}

AudioPlugin2686VEditor& editorOf(const GuiContext& ctx)
{
    return static_cast<AudioPlugin2686VEditor&>(ctx.editor);
}
