#include "./ParamFile.h"

namespace Io
{
	ParamWriter::ParamWriter(ParamFormat format)
		: m_values(new juce::DynamicObject()), m_format(std::move(format))
	{
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
		auto* root = new juce::DynamicObject();

		root->setProperty(ParamKey::format, m_format.name);
		root->setProperty(ParamKey::version, m_format.version);
		root->setProperty(ParamKey::values, juce::var(m_values.get()));

		return file.replaceWithText(juce::JSON::toString(juce::var(root)));
	}

	std::optional<ParamReader> ParamReader::open(const juce::File& file,
		const ParamFormat& format, const juce::StringArray& legacyOrder)
	{
		if (!file.existsAsFile()) return std::nullopt;

		auto text = file.loadFileAsString();

		ParamReader reader;

		reader.m_legacyOrder = legacyOrder;

		auto parsed = juce::JSON::parse(text);

		if (auto* root = parsed.getDynamicObject())
		{
			// 印が違うものは別のファイル。読み違えるより開かないほうがよい。
			if (root->getProperty(ParamKey::format).toString() != format.name) return std::nullopt;

			auto* values = root->getProperty(ParamKey::values).getDynamicObject();

			if (values == nullptr) return std::nullopt;

			reader.m_values = values;

			return reader;
		}

		// JSON として読めなければ古い形式とみなす
		reader.m_legacy = true;
		reader.m_lines.addLines(text);

		return reader;
	}

	juce::var ParamReader::find(const juce::String& key) const
	{
		if (!m_legacy)
		{
			if (m_values == nullptr) return {};

			return m_values->getProperty(key);
		}

		int index = m_legacyOrder.indexOf(key);

		if (index < 0 || index >= m_lines.size()) return {};

		return m_lines[index];
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

		if (v.isVoid()) return fallback;

		// 古い形式は 0 と 1 の文字で持っていた
		if (m_legacy) return v.toString().getIntValue() != 0;

		return (bool)v;
	}

	juce::String ParamReader::getString(const juce::String& key, const juce::String& fallback) const
	{
		auto v = find(key);

		return v.isVoid() ? fallback : v.toString();
	}

	std::vector<float> ParamReader::getFloatArray(const juce::String& key) const
	{
		std::vector<float> out;

		if (!m_legacy)
		{
			auto v = find(key);

			if (auto* array = v.getArray())
			{
				for (const auto& e : *array) out.push_back((float)(double)e);
			}

			return out;
		}

		int index = m_legacyOrder.indexOf(key);

		if (index < 0) return out;

		for (int i = index; i < m_lines.size(); ++i) out.push_back(m_lines[i].getFloatValue());

		return out;
	}

	std::vector<int> ParamReader::getIntArray(const juce::String& key) const
	{
		std::vector<int> out;

		if (!m_legacy)
		{
			auto v = find(key);

			if (auto* array = v.getArray())
			{
				for (const auto& e : *array) out.push_back((int)e);
			}

			return out;
		}

		int index = m_legacyOrder.indexOf(key);

		if (index < 0) return out;

		for (int i = index; i < m_lines.size(); ++i) out.push_back(m_lines[i].getIntValue());

		return out;
	}
}
