#include "./GuiColor.h"

namespace GuiColor
{
	std::map<juce::String, Entry*>& registry()
	{
		static std::map<juce::String, Entry*> table;

		return table;
	}

	juce::ChangeBroadcaster& changeBroadcaster()
	{
		static juce::ChangeBroadcaster broadcaster;

		return broadcaster;
	}

	Entry::Entry(const char* id, juce::Colour defaultColour)
		: m_default(defaultColour), m_current(defaultColour)
	{
		registry()[juce::String(id)] = this;
	}

	void setColour(const juce::String& id, juce::Colour colour)
	{
		auto it = registry().find(id);

		if (it == registry().end()) return;

		it->second->set(colour);

		changeBroadcaster().sendChangeMessage();
	}

	void resetColour(const juce::String& id)
	{
		auto it = registry().find(id);

		if (it == registry().end()) return;

		it->second->reset();

		changeBroadcaster().sendChangeMessage();
	}

	void resetAllColours()
	{
		for (auto& kv : registry()) kv.second->reset();

		changeBroadcaster().sendChangeMessage();
	}
}
