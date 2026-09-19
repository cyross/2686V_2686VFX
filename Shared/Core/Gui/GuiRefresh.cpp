#include "./GuiRefresh.h"

#include <utility>
#include <vector>

namespace
{
	// まとめの入れ子の深さ。0 なら溜めずにその場で作り直す。
	int batchDepth = 0;

	// 溜めた作り直し。部品ごとに 1 つだけ持つ。
	std::vector<std::pair<const void*, std::function<void()>>> pending;
}

namespace GuiRefresh
{
	Batch::Batch()
	{
		++batchDepth;
	}

	Batch::~Batch()
	{
		if (--batchDepth > 0) return;

		// 走らせる前に取り出す。作り直しの中でまた溜められても
		// 取りこぼさないようにするため。
		auto todo = std::move(pending);

		pending.clear();

		for (auto& entry : todo) entry.second();
	}

	bool defer(const void* owner, std::function<void()> update)
	{
		if (batchDepth == 0) return false;

		for (auto& entry : pending)
		{
			if (entry.first == owner)
			{
				entry.second = std::move(update);

				return true;
			}
		}

		pending.push_back({ owner, std::move(update) });

		return true;
	}
}
