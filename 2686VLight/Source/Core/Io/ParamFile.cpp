#include "./ParamFile.h"

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
}

namespace Io
{
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

	bool ParamWriter::writeTo(const juce::File& file) const
	{
		return file.replaceWithText(juce::JSON::toString(juce::var(m_root.get())));
	}

	ParamReader::ParamReader(juce::DynamicObject::Ptr values) : m_values(std::move(values))
	{
	}

	std::optional<ParamReader> ParamReader::open(const juce::File& file, const ParamFormat& format, bool tellIfLegacy)
	{
		if (!file.existsAsFile()) return std::nullopt;

		auto parsed = juce::JSON::parse(file.loadFileAsString());
		auto* root = parsed.getDynamicObject();

		if (root == nullptr)
		{
			// JSON として読めないものは古い形式とみなす
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
