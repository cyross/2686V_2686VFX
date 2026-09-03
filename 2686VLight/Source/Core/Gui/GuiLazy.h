#pragma once

#include <JuceHeader.h>

#include <functional>
#include <memory>

// ============================================================================
// タブの中身を後から作るための仕掛け
// ============================================================================
// 画面を開いた時点ではタブの中身を作らず、そのタブが選ばれたときに初めて
// 作る。区分が増えて 1 つのタブが数百の部品を抱えるようになり、全部の
// タブを先に作ると開くまで待たされるようになったため。
//
// 呼ぶ側から見た形は std::unique_ptr と同じにしてある。矢印で触れば
// そのとき作られる。まだ作っていないものを触りたくない場面 (全タブへ
// 配って回るような処理) では peek() を使い、nullptr なら飛ばす。

// 中身を入れる器。
//
// juce::TabbedComponent は後から中身を差し替える口を持たないので、
// 空の器だけ先に登録しておき、中身が出来たらこの器へ入れる。
class GuiTabHost : public juce::Component
{
public:
	void setContent(juce::Component& newContent)
	{
		content = &newContent;

		addAndMakeVisible(newContent);

		newContent.setBounds(getLocalBounds());
	}

	void resized() override
	{
		if (content != nullptr) content->setBounds(getLocalBounds());
	}

private:
	juce::Component* content = nullptr;
};

template <typename T>
class GuiLazy
{
public:
	// 実体を作るところ。
	std::function<std::unique_ptr<T>()> onConstruct;

	// 作った直後にやること。setup と、器へ入れて位置を決めるところまで。
	std::function<void(T&)> onSetup;

	// 触られたら作る。std::unique_ptr の get() とは戻り値が違うので、
	// 名前を分けてある。
	T& ref()
	{
		if (ptr == nullptr)
		{
			ptr = onConstruct();

			// ここから onSetup の間は peek() に見せない。
			// 出来上がる前に位置決めなどが走ると、中身が揃っていない。
			if (onSetup) onSetup(*ptr);

			ready = true;
		}

		return *ptr;
	}

	T* operator->() { return &ref(); }
	T& operator*() { return ref(); }

	// まだ作っていなければ nullptr。作っている最中も nullptr を返す。
	T* peek() const { return ready ? ptr.get() : nullptr; }

	bool isMade() const { return ready; }

private:
	std::unique_ptr<T> ptr;
	bool ready = false;
};
