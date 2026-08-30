// ============================================================================
// パラメータの一覧を書き出す
// ============================================================================
// オートメーションの一覧と、各チャンネルの説明に載せる型・範囲・初期値は、
// 2686V だけで 9,000 を超える。手で書き写すのは現実的でないうえ、パラメータ
// を足すたびに古くなる。
//
// プロセッサを 1 つ作って、登録されているものをそのまま吐き出す。マニュアル
// 側はこの結果から表を組み立てる。パラメータを増減したら、これを流し直せば
// 一覧が追いつく。
//
// 出すのは JSON。マニュアルの生成で読みやすく、差分も見やすい。

#include <JuceHeader.h>

#include <cmath>

#include "Core/Processor/PluginProcessor.h"

namespace
{
	// パラメータの種類。マニュアルでは Int / Float / Bool の 3 つで説明する。
	juce::String typeOf(const juce::AudioProcessorParameter& param)
	{
		if (dynamic_cast<const juce::AudioParameterBool*>(&param) != nullptr) return "Bool";
		if (dynamic_cast<const juce::AudioParameterInt*>(&param) != nullptr) return "Int";
		if (dynamic_cast<const juce::AudioParameterChoice*>(&param) != nullptr) return "Choice";
		if (dynamic_cast<const juce::AudioParameterFloat*>(&param) != nullptr) return "Float";

		return "Other";
	}

	// 0.0〜1.0 で持っている値を、表に出す実際の値へ戻す
	juce::var actualValue(const juce::AudioProcessorParameter& param, float normalised)
	{
		if (auto* b = dynamic_cast<const juce::AudioParameterBool*>(&param))
		{
			juce::ignoreUnused(b);

			return normalised >= 0.5f;
		}

		// 整数は整数のまま出す。小数で出すと表に 0.0 と並んで読みにくい。
		if (auto* i = dynamic_cast<const juce::AudioParameterInt*>(&param))
		{
			return (int)std::lround(i->getNormalisableRange().convertFrom0to1(normalised));
		}

		if (auto* c = dynamic_cast<const juce::AudioParameterChoice*>(&param))
		{
			return (int)std::lround(c->getNormalisableRange().convertFrom0to1(normalised));
		}

		if (auto* f = dynamic_cast<const juce::AudioParameterFloat*>(&param))
		{
			return (double)f->getNormalisableRange().convertFrom0to1(normalised);
		}

		return (double)normalised;
	}

	void describeRange(const juce::AudioProcessorParameter& param, juce::DynamicObject& out)
	{
		if (dynamic_cast<const juce::AudioParameterBool*>(&param) != nullptr)
		{
			// 真偽は 0.0〜0.49 が false、0.50〜1.00 が true。
			// オートメーションで扱うときのために、そのまま書いておく。
			out.setProperty("min", false);
			out.setProperty("max", true);

			return;
		}

		if (auto* i = dynamic_cast<const juce::AudioParameterInt*>(&param))
		{
			out.setProperty("min", i->getRange().getStart());
			out.setProperty("max", i->getRange().getEnd());

			return;
		}

		if (auto* c = dynamic_cast<const juce::AudioParameterChoice*>(&param))
		{
			juce::Array<juce::var> choices;

			for (const auto& name : c->choices) choices.add(name);

			out.setProperty("choices", choices);

			return;
		}

		if (auto* f = dynamic_cast<const juce::AudioParameterFloat*>(&param))
		{
			const auto& range = f->getNormalisableRange();

			out.setProperty("min", (double)range.start);
			out.setProperty("max", (double)range.end);

			if (range.interval > 0.0f) out.setProperty("step", (double)range.interval);

			return;
		}
	}
}

int main(int argc, char* argv[])
{
	// juce::File などを使うので、先に土台を立てておく
	juce::ScopedJuceInitialiser_GUI juceInit;

	juce::File target = argc > 1
		? juce::File::getCurrentWorkingDirectory().getChildFile(juce::String(argv[1]))
		: juce::File::getCurrentWorkingDirectory().getChildFile("parameters.json");

	// スタックには置けない。9,000 を超えるパラメータを抱えるので、実体が
	// 大きく、そのまま置くとスタックがあふれる。
	auto processor = std::make_unique<AudioPlugin2686V>();

	juce::Array<juce::var> list;

	for (auto* param : processor->getParameters())
	{
		auto* withId = dynamic_cast<juce::AudioProcessorParameterWithID*>(param);

		if (withId == nullptr) continue;

		auto* entry = new juce::DynamicObject();

		// id は保存されるファイルの中の名前、name は DAW のオートメーションに
		// 出てくる名前。マニュアルではどちらも要る。
		entry->setProperty("id", withId->paramID);
		entry->setProperty("name", param->getName(1024));
		entry->setProperty("type", typeOf(*param));

		describeRange(*param, *entry);

		entry->setProperty("default", actualValue(*param, param->getDefaultValue()));

		auto label = param->getLabel();

		if (label.isNotEmpty()) entry->setProperty("unit", label);

		list.add(juce::var(entry));
	}

	auto* root = new juce::DynamicObject();

	root->setProperty("plugin", processor->getName());
	root->setProperty("version", JucePlugin_VersionString);
	root->setProperty("count", list.size());
	root->setProperty("parameters", list);

	if (!target.replaceWithText(juce::JSON::toString(juce::var(root))))
	{
		std::fprintf(stderr, "書き出せませんでした: %s\n", target.getFullPathName().toRawUTF8());

		return 1;
	}

	std::printf("%d 件を書き出しました: %s\n", list.size(), target.getFullPathName().toRawUTF8());

	return 0;
}
