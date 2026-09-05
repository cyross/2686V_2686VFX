#pragma once

#include <JuceHeader.h>
#include <vector>

#include "../../Core/Gui/GuiStructs.h"
#include "../../Core/Io/ParamFile.h"

// ============================================================================
// お気に入りと履歴
// ============================================================================
// プリセットそのものはフォルダの中のファイルだが、どれを気に入ったか・
// どれをいつ読んだかは、ファイルの外側の話になる。プラグインのフォルダへ
// 1 つのファイルとして持つ。
//
// 見出しも一緒に覚えておく。お気に入りと履歴は今見ているフォルダの外を
// 指すことがあり、表示のたびに実ファイルを開き直すと、数が増えたときに
// 待たされるため。
//
// どのファイルかは絶対パスで見分ける。ファイルを動かされると結び付きは
// 切れるが、名前で見分けると別物を同じものとみなしてしまう。
class PresetLibrary
{
	juce::File m_file;

	// 新しい順に並べる。先頭が最後に読んだもの。
	std::vector<PresetItem> m_history;

	// 登録した順に並べる
	std::vector<PresetItem> m_favourites;

	static int indexOf(const std::vector<PresetItem>& items, const juce::String& path);
public:
	// 置き場を決めて読み込む。無ければ空のまま始める。
	void openIn(const juce::File& directory, const juce::String& baseName);

	bool save() const;

	bool isFavourite(const juce::String& path) const;

	// お気に入りの入り切りを変える。入れるときは見出しも覚える。
	void setFavourite(const PresetItem& item, bool on);

	// 読み込んだときに呼ぶ。同じものが既にあれば先頭へ動かす。
	void addHistory(const PresetItem& item);

	void clearHistory();

	const std::vector<PresetItem>& getHistory() const { return m_history; }
	const std::vector<PresetItem>& getFavourites() const { return m_favourites; }
};
