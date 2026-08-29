#pragma once

#include <JuceHeader.h>
#include <optional>
#include <vector>

namespace Io
{
	// ========================================================================
	// パラメータファイル
	// ========================================================================
	// 値を名前で持つ。
	//
	// 3.0.0 より前は値を行の順番だけで並べていた。そのため項目を足すと古い
	// ファイルが読めなくなり、並べ間違えると黙って別の値が入る。名前で
	// 持てばどちらも起きない。
	//
	// 古い形式は読まない。並び順を全部覚えておく必要があり、項目を足す
	// たびにその重みが増えるため。代わりに、古いファイルを選んだときは
	// 黙って何も起きるのではなく、読めないことを画面で伝える。
	//
	// 中身は JSON。将来 YAML へ移すときも、読み書きする側はこの入れ物を
	// 通しているので手を入れずに済む。

	// 何のファイルかを表す印と、その版
	struct ParamFormat
	{
		juce::String name;
		int version = 1;
	};

	class ParamWriter
	{
		juce::DynamicObject::Ptr m_values;
		ParamFormat m_format;

		// 入れ子にした部品の置き場。名前の頭に部品の名を付けて区別する。
		juce::String m_prefix;
	public:
		explicit ParamWriter(ParamFormat format);

		// 部品ごとに頭を付けたい場合に使う。同じ種類の部品が複数あっても
		// 名前がぶつからない。
		ParamWriter(const ParamWriter& parent, const juce::String& prefix);

		void set(const juce::String& key, int value);
		void set(const juce::String& key, float value);
		void set(const juce::String& key, double value);
		void set(const juce::String& key, bool value);
		void set(const juce::String& key, const juce::String& value);

		void setArray(const juce::String& key, const std::vector<float>& values);
		void setArray(const juce::String& key, const std::vector<int>& values);

		bool writeTo(const juce::File& file) const;
	private:
		juce::String full(const juce::String& key) const;
	};

	class ParamReader
	{
		juce::DynamicObject::Ptr m_values;
		juce::String m_prefix;

		juce::var find(const juce::String& key) const;
	public:
		// 読めない、または印が違う場合は空を返す。
		//
		// 3.0.0 より前の形式だったときは、読めないことを画面で伝えてから
		// 空を返す。黙って何も起きないと、壊れたのか使い方を誤ったのかが
		// 分からないため。
		static std::optional<ParamReader> open(const juce::File& file, const ParamFormat& format);

		ParamReader() = default;

		// 部品ごとに頭を付けて読む
		ParamReader(const ParamReader& parent, const juce::String& prefix);

		int getInt(const juce::String& key, int fallback = 0) const;
		float getFloat(const juce::String& key, float fallback = 0.0f) const;
		bool getBool(const juce::String& key, bool fallback = false) const;
		juce::String getString(const juce::String& key, const juce::String& fallback = {}) const;

		std::vector<float> getFloatArray(const juce::String& key) const;
		std::vector<int> getIntArray(const juce::String& key) const;
	private:
		juce::String full(const juce::String& key) const;
	};

	namespace ParamKey
	{
		static inline const juce::String format = "format";
		static inline const juce::String version = "version";
		static inline const juce::String values = "values";
	}
}
