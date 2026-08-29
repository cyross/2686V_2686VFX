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
	// 従来の形式は値を行の順番だけで並べていた。そのため項目を足すと古い
	// ファイルが読めなくなり、並べ間違えると黙って別の値が入る。名前で
	// 持てばどちらも起きない。
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
	public:
		explicit ParamWriter(ParamFormat format);

		void set(const juce::String& key, int value);
		void set(const juce::String& key, float value);
		void set(const juce::String& key, double value);
		void set(const juce::String& key, bool value);
		void set(const juce::String& key, const juce::String& value);

		void setArray(const juce::String& key, const std::vector<float>& values);
		void setArray(const juce::String& key, const std::vector<int>& values);

		bool writeTo(const juce::File& file) const;
	};

	// 新しい形式と古い形式のどちらも読む。
	//
	// 古いファイルは行の順番でしか意味が決まらないので、開くときに並び順を
	// 渡してもらう。以後は新旧どちらでも名前で読めるようになる。
	class ParamReader
	{
		juce::DynamicObject::Ptr m_values;

		juce::StringArray m_lines;
		juce::StringArray m_legacyOrder;

		bool m_legacy = false;

		juce::var find(const juce::String& key) const;
	public:
		// 読めなければ空を返す。中身が期待した印でなくても空を返す。
		static std::optional<ParamReader> open(const juce::File& file,
			const ParamFormat& format, const juce::StringArray& legacyOrder);

		bool isLegacy() const { return m_legacy; }

		// 古いファイルで行数を確かめたいときに使う
		int legacyLineCount() const { return m_lines.size(); }

		int getInt(const juce::String& key, int fallback = 0) const;
		float getFloat(const juce::String& key, float fallback = 0.0f) const;
		bool getBool(const juce::String& key, bool fallback = false) const;
		juce::String getString(const juce::String& key, const juce::String& fallback = {}) const;

		// 古いファイルでは、その名前の位置から終わりまでを並びとして読む。
		// 従来は末尾へまとめて並べていたため。
		std::vector<float> getFloatArray(const juce::String& key) const;
		std::vector<int> getIntArray(const juce::String& key) const;
	};

	namespace ParamKey
	{
		static inline const juce::String format = "format";
		static inline const juce::String version = "version";
		static inline const juce::String values = "values";
	}
}
