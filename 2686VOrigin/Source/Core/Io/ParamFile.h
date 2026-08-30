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
	// 部品ごとに入れ子にできる。オペレータのように同じものが複数あるときは
	// 並びとして持つので、名前に番号を混ぜずに済み、開いたときに何番目かが
	// 構造で分かる。
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
		// 書き出す全体。入れ子の先からでも書き出せるように持っておく。
		juce::DynamicObject::Ptr m_root;

		// このまとまりの値
		juce::DynamicObject::Ptr m_values;

		ParamFormat m_format;

		ParamWriter(juce::DynamicObject::Ptr root, juce::DynamicObject::Ptr values, ParamFormat format);
	public:
		explicit ParamWriter(ParamFormat format);

		// 入れ子のまとまりを作る。部品ごとに区切って持たせるため。
		ParamWriter child(const juce::String& key);

		// 並びの中のひとつ。足りなければそこまで作る。
		ParamWriter arrayItem(const juce::String& key, int index);

		void set(const juce::String& key, int value);
		void set(const juce::String& key, float value);
		void set(const juce::String& key, double value);
		void set(const juce::String& key, bool value);
		void set(const juce::String& key, const juce::String& value);

		void setArray(const juce::String& key, const std::vector<float>& values);
		void setArray(const juce::String& key, const std::vector<int>& values);

		bool writeTo(const juce::File& file) const;
	};

	class ParamReader
	{
		juce::DynamicObject::Ptr m_values;

		explicit ParamReader(juce::DynamicObject::Ptr values);

		juce::var find(const juce::String& key) const;
	public:
		// 読めない、または印が違う場合は空を返す。
		//
		// 3.0.0 より前の形式だったときは、読めないことを画面で伝えてから
		// 空を返す。黙って何も起きないと、壊れたのか使い方を誤ったのかが
		// 分からないため。
		// tellIfLegacy を false にすると、古い形式でも知らせを出さずに空を返す。
		// 起動時のように、こちらから開いたわけではない場面で使う。
		static std::optional<ParamReader> open(const juce::File& file, const ParamFormat& format,
			bool tellIfLegacy = true);

		ParamReader() = default;

		// 入れ子のまとまり。無ければ空のものを返すので、読む側は既定値を
		// 受け取ることになる。
		ParamReader child(const juce::String& key) const;

		// 並びの中のひとつ。範囲の外なら空のものを返す。
		ParamReader arrayItem(const juce::String& key, int index) const;
		int arraySize(const juce::String& key) const;

		int getInt(const juce::String& key, int fallback = 0) const;
		float getFloat(const juce::String& key, float fallback = 0.0f) const;
		bool getBool(const juce::String& key, bool fallback = false) const;
		juce::String getString(const juce::String& key, const juce::String& fallback = {}) const;

		std::vector<float> getFloatArray(const juce::String& key) const;
		std::vector<int> getIntArray(const juce::String& key) const;

		// このまとまりが持つ名前を並べる。中身の決まっていない対応表を
		// 写すときに使う。
		juce::StringArray keys() const;
	};

	// ========================================================================
	// プリセットの中身
	// ========================================================================
	// APVTS の状態は XML で組み立てている。項目が 1 万を超えるため、組み立て
	// 直すのではなく、書き出す形だけを名前式へ置き換える。
	//
	// パラメータは id と値だけの並びなので平らな対応表にする。カーブのように
	// 形のあるまとまりは、そのまま写す。
	void writeStateXml(ParamWriter& writer, const juce::XmlElement& xml);

	// 読み戻して XML を組み立て直す。根の名前は呼び出し側が知っている。
	std::unique_ptr<juce::XmlElement> readStateXml(const ParamReader& reader, const juce::String& rootType);

	namespace StateKey
	{
		static inline const juce::String meta = "meta";
		static inline const juce::String params = "params";
		static inline const juce::String nodes = "nodes";
		static inline const juce::String type = "type";
		static inline const juce::String attributes = "attributes";

		// APVTS がパラメータへ付けている名前
		static inline const juce::String param = "PARAM";
		static inline const juce::String id = "id";
		static inline const juce::String value = "value";
	}

	namespace ParamKey
	{
		static inline const juce::String format = "format";
		static inline const juce::String version = "version";
		static inline const juce::String values = "values";

		// オペレータのように同じものが並ぶときの名前
		static inline const juce::String ops = "ops";
		static inline const juce::String pads = "pads";
	}
}
