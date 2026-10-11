#pragma once

#include <JuceHeader.h>

#include "../Const/ConstFileValues.h"

// ============================================================================
// プリセットやチャンネルパラメータに書く、音声・波形ファイルの場所
// ============================================================================
// 場所は相対パスで書くことがある。その基準 (ルート) を 2 つから選べる。
//
//   settingsFolder  設定の置き場 (Samples / Wavetable)。3.6.2 までの動き
//   documentFolder  読み書きしているプリセット・チャンネルパラメータの置き場
//
// documentFolder は、プリセットと音声ファイルを 1 つのフォルダにまとめて
// 配ったり持ち運んだりするためのもの。設定は環境設定に 1 つだけ持つ。
//
// どちらを選んでいても、読むときは両方の基準を試す。もう一方の設定で
// 書いたファイルも読めるようにするため。
namespace Io
{
	namespace PathRoot
	{
		inline constexpr int settingsFolder = 0;
		inline constexpr int documentFolder = 1;
	}

	// 読み書きしているファイルを基準にするか。ファイルを伴わない保存
	// (DAW のプロジェクト) は、選んでいても設定の置き場を使う。
	inline bool usesDocumentRoot(int root, const juce::File& document)
	{
		return root == PathRoot::documentFolder && document != juce::File();
	}

	// 書くときの形。
	//
	// relativeToSettings は、設定の置き場を基準にするときに相対で書くか。
	// プリセットは相対 (3.6.2 まで同じ)、チャンネルパラメータは絶対で
	// 書いてきたので、それぞれこれまでの形を保つ。
	inline juce::String toStoredPath(const juce::String& path, int root, const juce::File& document,
		const juce::String& settingsDir, bool relativeToSettings)
	{
		if (!isFileName(path)) return {};

		// 相対で持っているもの (読めなかったものなど) は、そのまま書き戻す
		if (!juce::File::isAbsolutePath(path)) return path;

		const juce::File file(path);

		if (usesDocumentRoot(root, document)) return file.getRelativePathFrom(document.getParentDirectory());

		if (relativeToSettings && juce::File::isAbsolutePath(settingsDir))
		{
			return file.getRelativePathFrom(juce::File(settingsDir));
		}

		return file.getFullPathName();
	}

	// 書かれた場所を、開ける形へ直す。
	//
	// 相対パスは、選んでいる基準を先に、もう一方を後に試す。どちらにも
	// 無ければ、選んでいる基準で組み立てたものを返す (呼ぶ側が見つからない
	// ことを扱う)。
	inline juce::File fromStoredPath(const juce::String& text, int root, const juce::File& document,
		const juce::String& settingsDir)
	{
		if (!isFileName(text)) return {};

		if (juce::File::isAbsolutePath(text)) return juce::File(text);

		juce::Array<juce::File> bases;

		const bool hasDocument = document != juce::File();
		const bool hasSettings = juce::File::isAbsolutePath(settingsDir);

		if (usesDocumentRoot(root, document))
		{
			bases.add(document.getParentDirectory());

			if (hasSettings) bases.add(juce::File(settingsDir));
		}
		else
		{
			if (hasSettings) bases.add(juce::File(settingsDir));
			if (hasDocument) bases.add(document.getParentDirectory());
		}

		for (const auto& base : bases)
		{
			auto file = base.getChildFile(text);

			if (file.existsAsFile()) return file;
		}

		return bases.isEmpty() ? juce::File() : bases.getFirst().getChildFile(text);
	}
}
