#include "./ParamFile.h"

#include <yaml-cpp/yaml.h>

#include <limits>

namespace
{
	// 古いファイルを選んだときの知らせ
	void tellLegacyNotSupported(const juce::File& file)
	{
		juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
			juce::String("") + "読み込めないファイル",
			juce::String("") + "3.0.0 より前の形式のファイルは読み込めません。\n\n"
			+ file.getFileName());
	}

	// 名前の下にある入れ物を取り出す。無ければ作る。
	juce::DynamicObject* ensureObject(juce::DynamicObject& parent, const juce::String& key)
	{
		auto existing = parent.getProperty(key);

		if (auto* obj = existing.getDynamicObject()) return obj;

		auto* created = new juce::DynamicObject();

		parent.setProperty(key, juce::var(created));

		return created;
	}

	// 名前の下にある並びを取り出す。無ければ作る。
	juce::Array<juce::var>* ensureArray(juce::DynamicObject& parent, const juce::String& key)
	{
		auto existing = parent.getProperty(key);

		if (!existing.isArray())
		{
			parent.setProperty(key, juce::var(juce::Array<juce::var>()));
			existing = parent.getProperty(key);
		}

		return existing.getArray();
	}

	// 今どちらの形で書き出すか。設定から変えるだけなので素の値で持つ。
	Io::FileFormat g_fileFormat = Io::FileFormat::json;

	// ====================================================================
	// YAML
	// ====================================================================
	// 文字列は必ず引用符で囲む。囲まないと "123" や "true" が数や真偽に
	// 見えてしまい、読み戻したときに元の文字列へ戻らない。数と真偽は
	// 引用符を付けずに書くので、読むときは引用符の有無で見分けられる。
	void emitYaml(YAML::Emitter& out, const juce::var& value)
	{
		if (auto* obj = value.getDynamicObject())
		{
			out << YAML::BeginMap;

			for (const auto& kv : obj->getProperties())
			{
				out << YAML::Key << kv.name.toString().toStdString() << YAML::Value;

				emitYaml(out, kv.value);
			}

			out << YAML::EndMap;

			return;
		}

		if (auto* array = value.getArray())
		{
			out << YAML::BeginSeq;

			for (const auto& e : *array) emitYaml(out, e);

			out << YAML::EndSeq;

			return;
		}

		if (value.isVoid() || value.isUndefined())
		{
			out << YAML::Null;

			return;
		}

		if (value.isBool())
		{
			out << (bool)value;

			return;
		}

		if (value.isInt() || value.isInt64())
		{
			out << (long long)(juce::int64)value;

			return;
		}

		if (value.isDouble())
		{
			// yaml-cpp は元の値へ戻る最短の書き方を選ぶので、そのまま渡す
			out << (double)value;

			return;
		}

		out << YAML::DoubleQuoted << value.toString().toStdString();
	}

	juce::String toYamlText(const juce::var& root)
	{
		YAML::Emitter out;

		emitYaml(out, root);

		return juce::String(juce::CharPointer_UTF8(out.c_str()));
	}

	// 引用符の付いていない値が数かどうか。手で書き足した値も読めるよう、
	// 書き出す側が使わない書き方 (指数など) も通す。
	bool looksLikeNumber(const juce::String& text, bool& isInteger)
	{
		if (text.isEmpty()) return false;

		int i = (text[0] == '+' || text[0] == '-') ? 1 : 0;

		bool digits = false;
		bool dot = false;
		bool exponent = false;

		for (; i < text.length(); ++i)
		{
			auto c = text[i];

			if (c >= '0' && c <= '9')
			{
				digits = true;

				continue;
			}

			if (c == '.' && !dot && !exponent)
			{
				dot = true;

				continue;
			}

			if ((c == 'e' || c == 'E') && digits && !exponent)
			{
				exponent = true;

				if (i + 1 < text.length() && (text[i + 1] == '+' || text[i + 1] == '-')) ++i;

				continue;
			}

			return false;
		}

		isInteger = digits && !dot && !exponent;

		return digits;
	}

	// 引用符の付いていない値を見た目から決める
	juce::var plainToVar(const std::string& raw)
	{
		juce::String text(juce::CharPointer_UTF8(raw.c_str()));

		auto trimmed = text.trim();
		auto lower = trimmed.toLowerCase();

		// 手で書いたときのために YAML でよく使われる書き方も受ける
		if (lower == "true" || lower == "yes" || lower == "on") return true;
		if (lower == "false" || lower == "no" || lower == "off") return false;
		if (lower.isEmpty() || lower == "~" || lower == "null") return {};

		bool isInteger = false;

		if (looksLikeNumber(trimmed, isInteger))
		{
			if (!isInteger) return trimmed.getDoubleValue();

			auto n = trimmed.getLargeIntValue();

			if (n >= std::numeric_limits<int>::min() && n <= std::numeric_limits<int>::max())
			{
				return (int)n;
			}

			return juce::var(n);
		}

		return text;
	}

	juce::var fromYaml(const YAML::Node& node)
	{
		if (node.IsMap())
		{
			auto* obj = new juce::DynamicObject();

			for (const auto& kv : node)
			{
				juce::String key(juce::CharPointer_UTF8(kv.first.Scalar().c_str()));

				if (key.isEmpty()) continue;

				obj->setProperty(key, fromYaml(kv.second));
			}

			return juce::var(obj);
		}

		if (node.IsSequence())
		{
			juce::Array<juce::var> out;

			for (const auto& e : node) out.add(fromYaml(e));

			return out;
		}

		if (node.IsScalar())
		{
			// 引用符が付いていたものは文字列。書くときに必ず囲んでいる。
			if (node.Tag() == "!") return juce::String(juce::CharPointer_UTF8(node.Scalar().c_str()));

			return plainToVar(node.Scalar());
		}

		return {};
	}

	// 先頭だけを見て、読む価値があるかを決める。
	//
	// 一覧を作るときはフォルダの中を 1 件ずつ開く。プリセットは 1 つが
	// 数百 KB あるので、3.0.0 より前の XML が並んでいると、読んでから
	// 捨てるだけで待たされる。
	bool looksLikeOurFile(const juce::File& file)
	{
		juce::FileInputStream in(file);

		if (!in.openedOk()) return false;

		char buffer[64] = {};

		auto read = in.read(buffer, (int)sizeof(buffer) - 1);

		if (read <= 0) return false;

		auto head = juce::String::fromUTF8(buffer, (int)read);

		// 書き出す側は付けないが、手で用意されたものには付いていることがある
		if (head.startsWithChar((juce::juce_wchar)0xfeff)) head = head.substring(1);

		head = head.trimStart();

		return head.isNotEmpty() && head[0] != '<';
	}

	// JSON か YAML か。名前ではなく中身で見分ける。
	juce::var parseAny(const juce::String& text)
	{
		// JSON のほうが速いので先に試す
		auto parsed = juce::JSON::parse(text);

		if (parsed.getDynamicObject() != nullptr) return parsed;

		// 明らかに違うものへ YAML 解析をかけない。
		//
		// 一覧を作るときはフォルダの中を 1 件ずつ開くので、3.0.0 より前の
		// XML が並んでいると、その数だけ解析と例外が起きる。読めないと
		// 分かっているものにその手間をかける意味はない。
		auto head = text.trimStart();

		if (head.isEmpty() || head[0] == '<') return {};

		try
		{
			auto node = YAML::Load(text.toStdString());

			if (node.IsMap()) return fromYaml(node);
		}
		catch (const std::exception&)
		{
			// 読めなければ古い形式として扱う。ここで落とさない。
		}

		return {};
	}
}

namespace Io
{
	FileFormat getFileFormat()
	{
		return g_fileFormat;
	}

	void setFileFormat(FileFormat format)
	{
		g_fileFormat = format;
	}

	juce::String fileFormatExtension(FileFormat format)
	{
		return format == FileFormat::yaml ? "yaml" : "json";
	}

	juce::String fileFormatExtension()
	{
		return fileFormatExtension(g_fileFormat);
	}

	juce::String defaultFileName(const juce::String& base)
	{
		return "default." + base + "." + fileFormatExtension();
	}

	juce::String saveGlob(const juce::String& base)
	{
		return "*." + base + "." + fileFormatExtension();
	}

	juce::String openGlob(const juce::String& base)
	{
		return "*." + base + ".json;*." + base + ".yaml";
	}

	bool writeValueTo(const juce::File& file, const juce::var& value)
	{
		if (g_fileFormat == FileFormat::yaml) return file.replaceWithText(toYamlText(value));

		return file.replaceWithText(juce::JSON::toString(value));
	}

	juce::var readValueFrom(const juce::File& file)
	{
		if (!file.existsAsFile()) return {};

		return parseAny(file.loadFileAsString());
	}

	juce::File resolveFile(const juce::File& directory, const juce::String& baseName)
	{
		auto asJson = directory.getChildFile(baseName + ".json");
		auto asYaml = directory.getChildFile(baseName + ".yaml");

		if (asJson.existsAsFile() && asYaml.existsAsFile())
		{
			return asJson.getLastModificationTime() >= asYaml.getLastModificationTime() ? asJson : asYaml;
		}

		if (asYaml.existsAsFile()) return asYaml;
		if (asJson.existsAsFile()) return asJson;

		return fileToWrite(directory, baseName);
	}

	juce::File fileToWrite(const juce::File& directory, const juce::String& baseName)
	{
		return directory.getChildFile(baseName + "." + fileFormatExtension());
	}

	bool isLegacyFile(const juce::File& file)
	{
		if (!file.existsAsFile()) return false;

		auto text = file.loadFileAsString();

		// 名前式として読めるものは古い形式ではない
		if (parseAny(text).getDynamicObject() != nullptr) return false;

		// 古い形式は数の並びだった。最初の中身のある行で見分ける。
		// これを見ないと、壊れたファイルまで変換にかけてしまう。
		for (const auto& line : juce::StringArray::fromLines(text))
		{
			auto trimmed = line.trim();

			if (trimmed.isEmpty()) continue;

			bool isInteger = false;

			return looksLikeNumber(trimmed, isInteger);
		}

		return false;
	}

	juce::File convertedFileFor(const juce::File& file)
	{
		return file.getSiblingFile(file.getFileName() + "." + fileFormatExtension());
	}

	bool writeConverted(const juce::File& file, const ParamWriter& writer)
	{
		auto target = convertedFileFor(file);

		// 同じファイルを読み込むたびに変換が走るので、既にあるものは
		// 触らない。書き換えたあとの内容を戻してしまわないため。
		if (target.existsAsFile()) return false;

		if (!writer.writeTo(target))
		{
			juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::WarningIcon,
				juce::String("") + "変換できませんでした",
				juce::String("") + "読み込みはできましたが、新しい形式で保存できませんでした。\n\n"
				+ target.getFullPathName());

			return false;
		}

		juce::AlertWindow::showMessageBoxAsync(juce::MessageBoxIconType::InfoIcon,
			juce::String("") + "形式を変換しました",
			juce::String("") + "3.0.0 より前の形式のファイルを読み込み、新しい形式で保存しました。\n\n"
			+ juce::String("") + "元のファイル: " + file.getFileName() + "\n"
			+ juce::String("") + "保存先: " + target.getFileName());

		return true;
	}

	ParamWriter::ParamWriter(ParamFormat format)
		: m_root(new juce::DynamicObject()), m_values(new juce::DynamicObject()), m_format(std::move(format))
	{
		m_root->setProperty(ParamKey::format, m_format.name);
		m_root->setProperty(ParamKey::version, m_format.version);
		m_root->setProperty(ParamKey::values, juce::var(m_values.get()));
	}

	ParamWriter::ParamWriter(juce::DynamicObject::Ptr root, juce::DynamicObject::Ptr values, ParamFormat format)
		: m_root(std::move(root)), m_values(std::move(values)), m_format(std::move(format))
	{
	}

	ParamWriter ParamWriter::child(const juce::String& key)
	{
		return ParamWriter(m_root, ensureObject(*m_values, key), m_format);
	}

	ParamWriter ParamWriter::arrayItem(const juce::String& key, int index)
	{
		auto* array = ensureArray(*m_values, key);

		// 飛び番で置かれても形が崩れないよう、そこまで埋めておく
		while (array->size() <= index) array->add(juce::var(new juce::DynamicObject()));

		auto item = (*array)[index];
		auto* obj = item.getDynamicObject();

		if (obj == nullptr)
		{
			obj = new juce::DynamicObject();

			array->set(index, juce::var(obj));
		}

		return ParamWriter(m_root, obj, m_format);
	}

	void ParamWriter::set(const juce::String& key, int value)
	{
		m_values->setProperty(key, value);
	}

	void ParamWriter::set(const juce::String& key, juce::int64 value)
	{
		m_values->setProperty(key, value);
	}

	void ParamWriter::set(const juce::String& key, float value)
	{
		m_values->setProperty(key, (double)value);
	}

	void ParamWriter::set(const juce::String& key, double value)
	{
		m_values->setProperty(key, value);
	}

	void ParamWriter::set(const juce::String& key, bool value)
	{
		m_values->setProperty(key, value);
	}

	void ParamWriter::set(const juce::String& key, const juce::String& value)
	{
		m_values->setProperty(key, value);
	}

	void ParamWriter::setArray(const juce::String& key, const std::vector<float>& values)
	{
		juce::Array<juce::var> out;

		for (float v : values) out.add((double)v);

		m_values->setProperty(key, out);
	}

	void ParamWriter::setArray(const juce::String& key, const std::vector<int>& values)
	{
		juce::Array<juce::var> out;

		for (int v : values) out.add(v);

		m_values->setProperty(key, out);
	}

	void ParamWriter::hoist(const juce::String& key)
	{
		auto* obj = m_values->getProperty(key).getDynamicObject();

		if (obj == nullptr) return;

		// 先に外してから移す。同じ名前が残っていると入れ子のままになる。
		juce::DynamicObject::Ptr held(obj);

		m_values->removeProperty(key);

		for (const auto& kv : held->getProperties())
		{
			m_values->setProperty(kv.name, kv.value);
		}
	}

	bool ParamWriter::writeTo(const juce::File& file) const
	{
		juce::var root(m_root.get());

		if (g_fileFormat == FileFormat::yaml) return file.replaceWithText(toYamlText(root));

		return file.replaceWithText(juce::JSON::toString(root));
	}

	ParamReader::ParamReader(juce::DynamicObject::Ptr values) : m_values(std::move(values))
	{
	}

	std::optional<ParamReader> ParamReader::open(const juce::File& file, const ParamFormat& format, bool tellIfLegacy)
	{
		if (!file.existsAsFile()) return std::nullopt;

		if (!looksLikeOurFile(file))
		{
			if (tellIfLegacy) tellLegacyNotSupported(file);

			return std::nullopt;
		}

		auto parsed = parseAny(file.loadFileAsString());
		auto* root = parsed.getDynamicObject();

		if (root == nullptr)
		{
			// JSON としても YAML としても読めないものは古い形式とみなす
			if (tellIfLegacy) tellLegacyNotSupported(file);

			return std::nullopt;
		}

		// 印が違うものは別の種類のファイル。読み違えるより開かないほうがよい。
		if (root->getProperty(ParamKey::format).toString() != format.name) return std::nullopt;

		auto* values = root->getProperty(ParamKey::values).getDynamicObject();

		if (values == nullptr) return std::nullopt;

		return ParamReader(values);
	}

	ParamReader ParamReader::child(const juce::String& key) const
	{
		if (m_values == nullptr) return {};

		return ParamReader(m_values->getProperty(key).getDynamicObject());
	}

	ParamReader ParamReader::arrayItem(const juce::String& key, int index) const
	{
		if (m_values == nullptr) return {};

		auto* array = m_values->getProperty(key).getArray();

		if (array == nullptr || index < 0 || index >= array->size()) return {};

		return ParamReader((*array)[index].getDynamicObject());
	}

	int ParamReader::arraySize(const juce::String& key) const
	{
		if (m_values == nullptr) return 0;

		auto* array = m_values->getProperty(key).getArray();

		return array == nullptr ? 0 : array->size();
	}

	juce::var ParamReader::find(const juce::String& key) const
	{
		if (m_values == nullptr) return {};

		return m_values->getProperty(key);
	}

	int ParamReader::getInt(const juce::String& key, int fallback) const
	{
		auto v = find(key);

		return v.isVoid() ? fallback : (int)v;
	}

	juce::int64 ParamReader::getInt64(const juce::String& key, juce::int64 fallback) const
	{
		auto v = find(key);

		return v.isVoid() ? fallback : (juce::int64)v;
	}

	float ParamReader::getFloat(const juce::String& key, float fallback) const
	{
		auto v = find(key);

		return v.isVoid() ? fallback : (float)(double)v;
	}

	bool ParamReader::getBool(const juce::String& key, bool fallback) const
	{
		auto v = find(key);

		return v.isVoid() ? fallback : (bool)v;
	}

	juce::String ParamReader::getString(const juce::String& key, const juce::String& fallback) const
	{
		auto v = find(key);

		return v.isVoid() ? fallback : v.toString();
	}

	std::vector<float> ParamReader::getFloatArray(const juce::String& key) const
	{
		std::vector<float> out;

		if (auto* array = find(key).getArray())
		{
			for (const auto& e : *array) out.push_back((float)(double)e);
		}

		return out;
	}

	std::vector<int> ParamReader::getIntArray(const juce::String& key) const
	{
		std::vector<int> out;

		if (auto* array = find(key).getArray())
		{
			for (const auto& e : *array) out.push_back((int)e);
		}

		return out;
	}

	juce::StringArray ParamReader::keys() const
	{
		juce::StringArray out;

		if (m_values == nullptr) return out;

		for (const auto& kv : m_values->getProperties())
		{
			out.add(kv.name.toString());
		}

		return out;
	}

	namespace
	{
		// まとまりを 1 つ写す。中に子があれば同じ形で潜る。
		void writeNode(ParamWriter& writer, const juce::XmlElement& xml)
		{
			writer.set(StateKey::type, xml.getTagName());

			auto attributes = writer.child(StateKey::attributes);

			for (int i = 0; i < xml.getNumAttributes(); ++i)
			{
				attributes.set(xml.getAttributeName(i), xml.getAttributeValue(i));
			}

			int index = 0;

			for (auto* child : xml.getChildIterator())
			{
				auto item = writer.arrayItem(StateKey::nodes, index++);

				writeNode(item, *child);
			}
		}

		void readNode(const ParamReader& reader, juce::XmlElement& xml)
		{
			auto attributes = reader.child(StateKey::attributes);

			for (const auto& key : attributes.keys())
			{
				xml.setAttribute(key, attributes.getString(key));
			}

			int count = reader.arraySize(StateKey::nodes);

			for (int i = 0; i < count; ++i)
			{
				auto item = reader.arrayItem(StateKey::nodes, i);
				auto type = item.getString(StateKey::type);

				if (type.isEmpty()) continue;

				readNode(item, *xml.createNewChildElement(type));
			}
		}
	}

	void writeStateXml(ParamWriter& writer, const juce::XmlElement& xml)
	{
		auto meta = writer.child(StateKey::meta);

		for (int i = 0; i < xml.getNumAttributes(); ++i)
		{
			meta.set(xml.getAttributeName(i), xml.getAttributeValue(i));
		}

		auto params = writer.child(StateKey::params);

		int index = 0;

		for (auto* child : xml.getChildIterator())
		{
			if (child->hasTagName(StateKey::param))
			{
				params.set(child->getStringAttribute(StateKey::id),
					child->getStringAttribute(StateKey::value));

				continue;
			}

			auto item = writer.arrayItem(StateKey::nodes, index++);

			writeNode(item, *child);
		}
	}

	std::unique_ptr<juce::XmlElement> readStateXml(const ParamReader& reader, const juce::String& rootType)
	{
		auto xml = std::make_unique<juce::XmlElement>(rootType);

		auto meta = reader.child(StateKey::meta);

		for (const auto& key : meta.keys())
		{
			xml->setAttribute(key, meta.getString(key));
		}

		auto params = reader.child(StateKey::params);

		for (const auto& key : params.keys())
		{
			auto* param = xml->createNewChildElement(StateKey::param);

			param->setAttribute(StateKey::id, key);
			param->setAttribute(StateKey::value, params.getString(key));
		}

		int count = reader.arraySize(StateKey::nodes);

		for (int i = 0; i < count; ++i)
		{
			auto item = reader.arrayItem(StateKey::nodes, i);
			auto type = item.getString(StateKey::type);

			if (type.isEmpty()) continue;

			readNode(item, *xml->createNewChildElement(type));
		}

		return xml;
	}

}
