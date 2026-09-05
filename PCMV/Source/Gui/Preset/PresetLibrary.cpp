#include "./PresetLibrary.h"

namespace
{
	// ファイルの中身を見分ける印
	const Io::ParamFormat libraryFormat{ "presetLibrary", 1 };

	namespace Key
	{
		static inline const juce::String favourites = "favourites";
		static inline const juce::String history = "history";

		static inline const juce::String path = "path";
		static inline const juce::String name = "name";
		static inline const juce::String author = "author";
		static inline const juce::String version = "version";
		static inline const juce::String comment = "comment";
		static inline const juce::String mode = "mode";
		static inline const juce::String genre = "genre";
		static inline const juce::String format = "format";
		static inline const juce::String lastModified = "lastModified";
		static inline const juce::String fileSize = "fileSize";
	}

	void writeItems(Io::ParamWriter& writer, const juce::String& key, const std::vector<PresetItem>& items)
	{
		for (int i = 0; i < (int)items.size(); ++i)
		{
			const auto& item = items[(size_t)i];

			auto entry = writer.arrayItem(key, i);

			entry.set(Key::path, item.fullPath);
			entry.set(Key::name, item.name);
			entry.set(Key::author, item.author);
			entry.set(Key::version, item.version);
			entry.set(Key::comment, item.comment);
			entry.set(Key::mode, item.modeName);
			entry.set(Key::genre, item.genre);
			entry.set(Key::format, item.format);
			entry.set(Key::lastModified, item.lastModificationTime.toMilliseconds());
			entry.set(Key::fileSize, item.fileSize);
		}
	}

	std::vector<PresetItem> readItems(const Io::ParamReader& reader, const juce::String& key)
	{
		std::vector<PresetItem> out;

		int count = reader.arraySize(key);

		for (int i = 0; i < count; ++i)
		{
			auto entry = reader.arrayItem(key, i);

			PresetItem item;

			item.fullPath = entry.getString(Key::path);

			// 置き場の分からないものは覚えていても使えない
			if (item.fullPath.isEmpty()) continue;

			item.file = juce::File(item.fullPath);
			item.fileName = item.file.getFileName();
			item.name = entry.getString(Key::name);
			item.author = entry.getString(Key::author);
			item.version = entry.getString(Key::version);
			item.comment = entry.getString(Key::comment);
			item.modeName = entry.getString(Key::mode);
			item.genre = entry.getString(Key::genre);
			item.format = entry.getString(Key::format);
			item.lastModificationTime = juce::Time(entry.getInt64(Key::lastModified));
			item.fileSize = entry.getInt64(Key::fileSize);

			out.push_back(item);
		}

		return out;
	}
}

int PresetLibrary::indexOf(const std::vector<PresetItem>& items, const juce::String& path)
{
	for (int i = 0; i < (int)items.size(); ++i)
	{
		if (items[(size_t)i].fullPath == path) return i;
	}

	return -1;
}

void PresetLibrary::openIn(const juce::File& directory, const juce::String& baseName)
{
	m_file = Io::fileToWrite(directory, baseName);

	m_history.clear();
	m_favourites.clear();

	// 読むのは今あるほう。書き出すのは今選ばれている形なので、形を
	// 変えた直後は読みと書きで名前が違うことがある。
	auto existing = Io::resolveFile(directory, baseName);

	auto reader = Io::ParamReader::open(existing, libraryFormat, false);

	if (!reader.has_value()) return;

	m_favourites = readItems(*reader, Key::favourites);
	m_history = readItems(*reader, Key::history);
}

bool PresetLibrary::save() const
{
	if (m_file == juce::File{}) return false;

	Io::ParamWriter writer(libraryFormat);

	writeItems(writer, Key::favourites, m_favourites);
	writeItems(writer, Key::history, m_history);

	return writer.writeTo(m_file);
}

bool PresetLibrary::isFavourite(const juce::String& path) const
{
	return indexOf(m_favourites, path) >= 0;
}

void PresetLibrary::setFavourite(const PresetItem& item, bool on)
{
	int index = indexOf(m_favourites, item.fullPath);

	if (on)
	{
		if (index >= 0)
		{
			// 見出しが変わっていることがあるので入れ替える
			m_favourites[(size_t)index] = item;
		}
		else
		{
			m_favourites.push_back(item);
		}
	}
	else if (index >= 0)
	{
		m_favourites.erase(m_favourites.begin() + index);
	}

	save();
}

void PresetLibrary::addHistory(const PresetItem& item)
{
	if (item.fullPath.isEmpty()) return;

	int index = indexOf(m_history, item.fullPath);

	// 同じものを二重に持たない。読み直したら先頭へ来るようにする。
	if (index >= 0) m_history.erase(m_history.begin() + index);

	m_history.insert(m_history.begin(), item);

	save();
}

void PresetLibrary::clearHistory()
{
	m_history.clear();

	save();
}
