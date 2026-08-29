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
}

namespace Io
{
	ParamWriter::ParamWriter(ParamFormat format)
		: m_values(new juce::DynamicObject()), m_format(std::move(format))
	{
	}

	ParamWriter::ParamWriter(const ParamWriter& parent, const juce::String& prefix)
		: m_values(parent.m_values), m_format(parent.m_format), m_prefix(parent.full(prefix))
	{
	}

	juce::String ParamWriter::full(const juce::String& key) const
	{
		return m_prefix.isEmpty() ? key : m_prefix + "." + key;
	}

	void ParamWriter::set(const juce::String& key, int value)
	{
		m_values->setProperty(full(key), value);
	}

	void ParamWriter::set(const juce::String& key, float value)
	{
		m_values->setProperty(full(key), (double)value);
	}

	void ParamWriter::set(const juce::String& key, double value)
	{
		m_values->setProperty(full(key), value);
	}

	void ParamWriter::set(const juce::String& key, bool value)
	{
		m_values->setProperty(full(key), value);
	}

	void ParamWriter::set(const juce::String& key, const juce::String& value)
	{
		m_values->setProperty(full(key), value);
	}

	void ParamWriter::setArray(const juce::String& key, const std::vector<float>& values)
	{
		juce::Array<juce::var> out;

		for (float v : values) out.add((double)v);

		m_values->setProperty(full(key), out);
	}

	void ParamWriter::setArray(const juce::String& key, const std::vector<int>& values)
	{
		juce::Array<juce::var> out;

		for (int v : values) out.add(v);

		m_values->setProperty(full(key), out);
	}

	bool ParamWriter::writeTo(const juce::File& file) const
	{
		auto* root = new juce::DynamicObject();

		root->setProperty(ParamKey::format, m_format.name);
		root->setProperty(ParamKey::version, m_format.version);
		root->setProperty(ParamKey::values, juce::var(m_values.get()));

		return file.replaceWithText(juce::JSON::toString(juce::var(root)));
	}

	std::optional<ParamReader> ParamReader::open(const juce::File& file, const ParamFormat& format)
	{
		if (!file.existsAsFile()) return std::nullopt;

		auto parsed = juce::JSON::parse(file.loadFileAsString());
		auto* root = parsed.getDynamicObject();

		if (root == nullptr)
		{
			// JSON として読めないものは古い形式とみなす
			tellLegacyNotSupported(file);

			return std::nullopt;
		}

		// 印が違うものは別の種類のファイル。読み違えるより開かないほうがよい。
		if (root->getProperty(ParamKey::format).toString() != format.name) return std::nullopt;

		auto* values = root->getProperty(ParamKey::values).getDynamicObject();

		if (values == nullptr) return std::nullopt;

		ParamReader reader;

		reader.m_values = values;

		return reader;
	}

	ParamReader::ParamReader(const ParamReader& parent, const juce::String& prefix)
		: m_values(parent.m_values), m_prefix(parent.full(prefix))
	{
	}

	juce::String ParamReader::full(const juce::String& key) const
	{
		return m_prefix.isEmpty() ? key : m_prefix + "." + key;
	}

	juce::var ParamReader::find(const juce::String& key) const
	{
		if (m_values == nullptr) return {};

		return m_values->getProperty(full(key));
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
}
