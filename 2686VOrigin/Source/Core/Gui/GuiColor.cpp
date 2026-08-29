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

	std::vector<std::pair<juce::String, juce::Colour>> namedColours()
	{
		std::vector<std::pair<juce::String, juce::Colour>> out;

		for (const auto& kv : registry()) {
			if (!kv.first.startsWith("Palette.")) continue;

			// 表示名は名前空間を外したもの
			out.push_back({ kv.first.fromLastOccurrenceOf(".", false, false), kv.second->defaultColour() });
		}

		return out;
	}

	juce::String describe(juce::Colour colour)
	{
		for (const auto& kv : namedColours()) {
			if (kv.second == colour) return kv.first;
		}

		return "#" + colour.toDisplayString(true);
	}
}
