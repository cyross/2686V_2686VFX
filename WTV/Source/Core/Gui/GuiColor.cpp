#include "./GuiColor.h"

#include "./GuiColorJuceNames.h"

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

	// 既定値はここでは作らない。名前を控えるだけにして、色を作るのは
	// 最初に使うときまで遅らせる。
	Entry::Entry(const char* id, Maker make) : m_make(make)
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

	Refresher::Refresher(std::function<void()> apply) : m_apply(std::move(apply))
	{
		changeBroadcaster().addChangeListener(this);
	}

	Refresher::~Refresher()
	{
		changeBroadcaster().removeChangeListener(this);
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

	// 数値の表から juce::Colour を作る。最初に呼ばれたときだけ作れば
	// よいので、関数の中の静的変数に置く。
	const std::vector<std::pair<juce::String, juce::Colour>>& juceColours()
	{
		static const std::vector<std::pair<juce::String, juce::Colour>> table = [] {
			std::vector<std::pair<juce::String, juce::Colour>> out;

			for (const auto& e : juceColourTable) {
				out.push_back({ juce::String(e.name), juce::Colour(e.argb) });
			}

			return out;
		}();

		return table;
	}

	// 名前が付いていればそれを、無ければ 16 進で返す。
	//
	// こちらで付けた名前を先に見る。同じ色に JUCE の名前も当たる場合、
	// この画面ではこちらの名前のほうが分かりやすいため。
	juce::String describe(juce::Colour colour)
	{
		for (const auto& kv : namedColours()) {
			if (kv.second == colour) return kv.first;
		}

		for (const auto& kv : juceColours()) {
			if (kv.second == colour) return kv.first;
		}

		return "#" + colour.toDisplayString(true);
	}
}
