#include "./GuiI18n.h"

namespace I18n
{
	namespace
	{
		// 静的初期化の順番に左右されないよう、実体は関数の中に置く。
		Lang& currentRef()
		{
			static Lang lang = Lang::ja;

			return lang;
		}
	}

	Lang current() { return currentRef(); }

	void setCurrent(Lang lang)
	{
		if (currentRef() == lang) return;

		currentRef() = lang;

		// 知らせは後回しで届く。切り替えを頼んだのは SETTINGS タブの
		// 選択で、受けた側はそのタブごと作り直す。いま呼ばれている場所を
		// その場で消さないよう、間を空けるのが要る。
		changeBroadcaster().sendChangeMessage();
	}

	juce::String toCode(Lang lang) { return lang == Lang::ja ? jaCode : enCode; }

	Lang fromCode(const juce::String& code)
	{
		// 知らない名前は英語として扱う。「日本語以外は英語」という決まりに
		// 合わせてある。
		return code.equalsIgnoreCase(jaCode) ? Lang::ja : Lang::en;
	}

	Lang detect()
	{
		// getUserLanguage() は "ja" のような ISO 639-1 の名前を返す。
		// 地域まで付くことがあるので、頭だけを見る。
		return juce::SystemStats::getUserLanguage().startsWithIgnoreCase(jaCode)
			? Lang::ja
			: Lang::en;
	}

	juce::String pick(const char8_t* ja, const char8_t* en)
	{
		return Text{ ja, en }.get();
	}

	juce::ChangeBroadcaster& changeBroadcaster()
	{
		static juce::ChangeBroadcaster broadcaster;

		return broadcaster;
	}
}
