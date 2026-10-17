#include "./GuiParamEdit.h"
#include "Shared/Core/Synth/StepTableIo.h"

#include "./GuiComponents.h"
#include "./GuiI18n.h"
#include "./GuiRefresh.h"

#include "Shared/Gui/Components/Edit/TargetsDialog.h"

#include <set>

namespace ParamEdit
{
	namespace
	{
		// 一覧に出す名前
		namespace Text
		{
			inline juce::String channel() { return I18n::pick(u8"グローバル", u8"Global"); }
			inline juce::String cut() { return I18n::pick(u8"切り取り (CUT)", u8"Cut"); }
			inline juce::String copy() { return I18n::pick(u8"コピー (COPY)", u8"Copy"); }
			inline juce::String paste() { return I18n::pick(u8"貼り付け (PASTE)", u8"Paste"); }
			inline juce::String clear() { return I18n::pick(u8"初期化 (CLEAR)", u8"Clear"); }
			inline juce::String duplicate() { return I18n::pick(u8"複写 (DUPLICATE)", u8"Duplicate"); }
			inline juce::String importParams() { return I18n::pick(u8"読み込み (IMPORT)", u8"Import"); }
			inline juce::String exportParams() { return I18n::pick(u8"書き出し (EXPORT)", u8"Export"); }
			inline juce::String duplicating() { return I18n::pick(u8"Duplicating…", u8"Duplicating…"); }
		}

		// BusyScope の入れ子の深さ (メッセージスレッドだけで触る)
		int busyDepth = 0;

		// 貼る・消すときに読み飛ばすキー。(APVTS, チャンネル) → キーの集まり
		std::map<std::pair<const void*, juce::String>, std::set<juce::String>>& skippedKeys()
		{
			static std::map<std::pair<const void*, juce::String>, std::set<juce::String>> keys;

			return keys;
		}

		// 変調波形の素材のキー。"wave:" + 置き場のキー + ":" + 何枚目か
		const juce::String wavePrefix = "wave:";

		// タブの素材のキー。"asset:" + 持ち主のキー + ":" + 素材の名前
		const juce::String assetPrefix = "asset:";

		// 段の表のキー。"table:" + 表のキー (チャンネル・単位の名前を外したもの)
		const juce::String tablePrefix = "table:";

		// ID を "_" で切った頭の 2 つ
		std::pair<juce::String, juce::String> firstTokens(const juce::String& id)
		{
			const int a = id.indexOfChar('_');

			if (a < 0) return { id, {} };

			const int b = id.indexOfChar(a + 1, '_');

			return { id.substring(0, a), b < 0 ? id.substring(a + 1) : id.substring(a + 1, b) };
		}

		// 置き場のキー (OPM / OPM_AMPMOD / OPM_OP2 / OPM_OP2_AMPMOD) を、
		// 単位に合わせた相対のキーにする
		juce::String relativeWaveKey(const juce::String& key, const Target& target)
		{
			if (target.scope == Scope::Category) return keyOf(key + "_X", Scope::Category).dropLastCharacters(2);

			const auto prefix = target.prefix();

			if (key == prefix) return {};

			return key.substring(prefix.length() + 1);
		}

		juce::String absoluteWaveKey(const juce::String& relative, const juce::String& prefix)
		{
			return relative.isEmpty() ? prefix : prefix + "_" + relative;
		}

		// 単位に入っている段の表
		std::vector<juce::String> tablesOf(const GuiContext& ctx, const Target& target)
		{
			if (target.scope == Scope::Category) return target.stepTables;

			std::vector<juce::String> keys;
			const auto prefix = target.prefix();

			ctx.audioProcessor.stepTables.forEach([&](const juce::String& key, StepTable&) {
				if (!key.startsWith(prefix + "_")) return;

				// グローバルにはオペレーター・パッドの持ち物を入れない
				if (target.scope == Scope::Channel && isUnitToken(firstTokens(key).second)) return;

				keys.push_back(key);
			});

			return keys;
		}

		// 表のキーを、写し先でも同じ名前になるよう相対にする
		juce::String relativeTableKey(const juce::String& key, const Target& target)
		{
			if (target.scope == Scope::Category) return keyOf(key + "_X", Scope::Category).dropLastCharacters(2);

			return key.substring(target.prefix().length() + 1);
		}

		// 単位に入っている変調波形の置き場
		std::vector<juce::String> waveKeysOf(const GuiContext& ctx, const Target& target)
		{
			if (target.scope == Scope::Category) return target.waveKeys;

			std::vector<juce::String> keys;
			const auto prefix = target.prefix();

			for (const auto& [key, paths] : ctx.audioProcessor.modWavePaths)
			{
				juce::ignoreUnused(paths);

				if (key != prefix && !key.startsWith(prefix + "_")) continue;

				// グローバルにはオペレーター・パッドの持ち物を入れない
				if (target.scope == Scope::Channel && isUnitToken(firstTokens(key).second)) continue;

				keys.push_back(key);
			}

			return keys;
		}

		// 単位に入っているパラメータ
		std::vector<juce::RangedAudioParameter*> paramsOf(const GuiContext& ctx, const Target& target)
		{
			std::vector<juce::RangedAudioParameter*> out;

			if (target.scope == Scope::Category)
			{
				for (const auto& id : target.ids)
				{
					if (auto* p = ctx.apvts.getParameter(id)) out.push_back(p);
				}

				return out;
			}

			const auto prefix = target.prefix() + "_";

			for (auto* raw : ctx.apvts.processor.getParameters())
			{
				auto* p = dynamic_cast<juce::RangedAudioParameter*>(raw);

				if (p == nullptr) continue;

				const auto id = p->getParameterID();

				if (!id.startsWith(prefix)) continue;

				// グローバルにはオペレーター・パッドを入れない (WT+ などの枠は入れる)
				if (target.scope == Scope::Channel)
				{
					const auto second = firstTokens(id).second;

					if (isUnitToken(second) && !second.startsWith("SLOT")) continue;
				}

				out.push_back(p);
			}

			return out;
		}

		void setReal(juce::RangedAudioParameter& p, float value)
		{
			const auto range = p.getNormalisableRange();
			const float clamped = juce::jlimit(range.start, range.end, value);

			p.beginChangeGesture();
			p.setValueNotifyingHost(p.convertTo0to1(clamped));
			p.endChangeGesture();
		}

		// 素材を読み直したあとの見た目を描き直す
		void repaintEditor(const GuiContext& ctx);

		Clipboard* g_clipboard = nullptr;

		// 終わりまで持つ。静的な破棄の順番に巻き込まれないよう、壊さない
		juce::ChangeBroadcaster& waveBroadcaster()
		{
			static auto* b = new juce::ChangeBroadcaster();

			return *b;
		}
	}

	WaveWatcher::WaveWatcher(std::function<void()> onChange) : m_onChange(std::move(onChange))
	{
		waveBroadcaster().addChangeListener(this);
	}

	WaveWatcher::~WaveWatcher()
	{
		waveBroadcaster().removeChangeListener(this);
	}

	void notifyWavesChanged()
	{
		waveBroadcaster().sendChangeMessage();
	}

	namespace
	{
		void repaintEditor(const GuiContext& ctx)
		{
			notifyWavesChanged();

			if (auto* c = dynamic_cast<juce::Component*>(&ctx.editor)) c->repaint();
		}
	}

	// ------------------------------------------------------------------ ID
	juce::String channelOf(const juce::String& id)
	{
		return firstTokens(id).first;
	}

	// OP0 / PAD3 / SLOT12 のような番号付きの単位
	bool isUnitToken(const juce::String& token)
	{
		for (const char* head : { "OP", "PAD", "SLOT" })
		{
			if (!token.startsWith(head)) continue;

			const auto rest = token.substring((int)std::strlen(head));

			if (rest.isNotEmpty() && rest.containsOnly("0123456789")) return true;
		}

		return false;
	}

	juce::String keyOf(const juce::String& id, Scope scope)
	{
		const int a = id.indexOfChar('_');

		if (a < 0) return id;

		auto rest = id.substring(a + 1);

		if (scope == Scope::Channel) return rest;

		// オペレーター・パッド・枠の番号を外す
		const int b = rest.indexOfChar('_');
		const auto head = b < 0 ? rest : rest.substring(0, b);

		if (isUnitToken(head)) return b < 0 ? juce::String() : rest.substring(b + 1);

		return rest;
	}

	juce::String Snapshot::scopeName() const
	{
		switch (scope)
		{
		case Scope::Channel:  return Text::channel();
		case Scope::Unit:     return unit;
		case Scope::Category: return unit.isEmpty() ? Text::channel() : unit;
		}

		return {};
	}

	// ------------------------------------------------------------------ 写している間の覆い
	BusyScope::BusyScope(const GuiContext& ctx) : m_editor(ctx.editor)
	{
		if (busyDepth++ > 0) return;

		m_editor.showLoading(Text::duplicating());

		// 処理の間はメッセージが回らないので、覆いをここで描かせておく
		if (auto* c = dynamic_cast<juce::Component*>(&m_editor))
		{
			if (auto* peer = c->getPeer()) peer->performAnyPendingRepaintsNow();
		}
	}

	BusyScope::~BusyScope()
	{
		if (--busyDepth > 0) return;

		m_editor.hideLoading();
	}

	// ------------------------------------------------------------------ 写す・貼る・消す
	Snapshot capture(const GuiContext& ctx, const Target& target)
	{
		Snapshot s;

		s.scope = target.scope;
		s.tab = target.tab;
		s.unit = target.unit;
		s.category = target.category;
		s.time = juce::Time::getCurrentTime();

		const auto keyScope = target.scope == Scope::Unit ? Scope::Unit : target.scope;

		for (auto* p : paramsOf(ctx, target))
		{
			const auto key = keyOf(p->getParameterID(), keyScope);

			if (key.isEmpty()) continue;

			s.values[key] = p->convertFrom0to1(p->getValue());
		}

		// 変調波形
		for (const auto& key : waveKeysOf(ctx, target))
		{
			auto it = ctx.audioProcessor.modWavePaths.find(key);

			if (it == ctx.audioProcessor.modWavePaths.end()) continue;

			const auto rel = relativeWaveKey(key, target);

			for (int i = 0; i < (int)it->second.size(); ++i)
			{
				if (it->second[(size_t)i].isNotEmpty())
					s.assets[wavePrefix + rel + ":" + juce::String(i)] = it->second[(size_t)i];
			}
		}

		// 段の表
		for (const auto& key : tablesOf(ctx, target))
		{
			if (auto* table = ctx.audioProcessor.stepTables.find(key))
				s.assets[tablePrefix + relativeTableKey(key, target)] = StepTableIo::toText(*table);
		}

		// タブが持つ素材
		auto collect = [&](const juce::String& owner, const juce::String& relOwner, bool whole) {
			std::map<juce::String, juce::String> found;

			ctx.editor.collectClipAssets(owner, whole, found);

			for (const auto& [k, v] : found) s.assets[assetPrefix + relOwner + ":" + k] = v;
		};

		if (target.scope == Scope::Category)
		{
			for (const auto& owner : target.assetPrefixes) collect(owner, keyOf(owner + "_X", Scope::Category).dropLastCharacters(2), false);
		}
		else
		{
			collect(target.prefix(), {}, target.scope == Scope::Channel);
		}

		return s;
	}

	int paste(const GuiContext& ctx, const Target& target, const Snapshot& snapshot, bool withAssets)
	{
		BusyScope busy(ctx);
		GuiRefresh::Batch batch;

		ctx.audioProcessor.undoManager.beginNewTransaction();

		int count = 0;

		for (auto* p : paramsOf(ctx, target))
		{
			const auto id = p->getParameterID();
			const auto key = keyOf(id, target.scope);

			auto it = snapshot.values.find(key);

			if (key.isEmpty() || it == snapshot.values.end()) continue;

			// With Note を切っているときの NOTE など
			if (isEditSkipped(ctx, channelOf(id), keyOf(id, Scope::Category))) continue;

			setReal(*p, it->second);
			++count;
		}

		// 段の表。写し元に無い表は、同じ単位で写したものなら初期の値へ戻す
		for (const auto& key : tablesOf(ctx, target))
		{
			auto* table = ctx.audioProcessor.stepTables.find(key);

			if (table == nullptr) continue;

			auto it = snapshot.assets.find(tablePrefix + relativeTableKey(key, target));

			if (it != snapshot.assets.end()) StepTableIo::fromText(*table, it->second);
			else if (snapshot.scope == target.scope && target.scope != Scope::Category) table->reset();
		}

		if (!withAssets)
		{
			repaintEditor(ctx);

			return count;
		}

		// 変調波形。貼り先の置き場を同じ規則で相対にして、同じものへ入れる
		for (const auto& key : waveKeysOf(ctx, target))
		{
			const auto rel = relativeWaveKey(key, target);
			const auto head = wavePrefix + rel + ":";

			bool any = false;

			for (const auto& [k, v] : snapshot.assets) any = any || k.startsWith(head);

			if (!any) continue;

			for (int i = 0; i < Global::WtMod::slots; ++i)
			{
				auto it = snapshot.assets.find(head + juce::String(i));

				if (it == snapshot.assets.end())
					ctx.audioProcessor.unloadWtModWaveFile(key, i);
				else if (juce::File(it->second).existsAsFile())
					ctx.audioProcessor.loadWtModWaveFile(key, i, juce::File(it->second));
			}
		}

		// グローバルやオペレーター単位で写したものを貼るとき、写し元に
		// 変調波形が無ければ、貼り先の波形も外す (写し元と同じにする)
		if (target.scope != Scope::Category)
		{
			bool sourceHasWave = false;

			for (const auto& [k, v] : snapshot.assets) sourceHasWave = sourceHasWave || k.startsWith(wavePrefix);

			if (!sourceHasWave && snapshot.scope == target.scope)
			{
				for (const auto& key : waveKeysOf(ctx, target))
				{
					for (int i = 0; i < Global::WtMod::slots; ++i) ctx.audioProcessor.unloadWtModWaveFile(key, i);
				}
			}
		}

		// タブが持つ素材
		auto apply = [&](const juce::String& owner, const juce::String& relOwner, bool whole) {
			std::map<juce::String, juce::String> found;
			const auto head = assetPrefix + relOwner + ":";

			for (const auto& [k, v] : snapshot.assets)
			{
				if (k.startsWith(head)) found[k.substring(head.length())] = v;
			}

			// 同じ単位で写したものなら、写し元に無い素材は外す
			const bool clearOthers = snapshot.scope == target.scope;

			if (found.empty() && !clearOthers) return;

			ctx.editor.applyClipAssets(owner, whole, found, clearOthers);
		};

		if (target.scope == Scope::Category)
		{
			for (const auto& owner : target.assetPrefixes) apply(owner, keyOf(owner + "_X", Scope::Category).dropLastCharacters(2), false);
		}
		else
		{
			apply(target.prefix(), {}, target.scope == Scope::Channel);
		}

		repaintEditor(ctx);

		return count;
	}

	void clear(const GuiContext& ctx, const Target& target)
	{
		BusyScope busy(ctx);
		GuiRefresh::Batch batch;

		ctx.audioProcessor.undoManager.beginNewTransaction();

		for (auto* p : paramsOf(ctx, target))
		{
			const auto id = p->getParameterID();

			// With Note を切っているときの NOTE など
			if (isEditSkipped(ctx, channelOf(id), keyOf(id, Scope::Category))) continue;

			p->beginChangeGesture();
			p->setValueNotifyingHost(p->getDefaultValue());
			p->endChangeGesture();
		}

		for (const auto& key : waveKeysOf(ctx, target))
		{
			for (int i = 0; i < Global::WtMod::slots; ++i) ctx.audioProcessor.unloadWtModWaveFile(key, i);
		}

		for (const auto& key : tablesOf(ctx, target))
		{
			if (auto* table = ctx.audioProcessor.stepTables.find(key)) table->reset();
		}

		const std::map<juce::String, juce::String> none;

		if (target.scope == Scope::Category)
		{
			for (const auto& owner : target.assetPrefixes) ctx.editor.applyClipAssets(owner, false, none, true);
		}
		else
		{
			ctx.editor.applyClipAssets(target.prefix(), target.scope == Scope::Channel, none, true);
		}

		repaintEditor(ctx);
	}

	void duplicateUnits(const GuiContext& ctx, const juce::String& channel, const juce::String& head,
		int from, int to, int count)
	{
		auto unitTarget = [&](int index) {
			Target t;

			t.scope = Scope::Unit;
			t.channel = channel;
			t.unit = head + juce::String(index);

			return t;
		};

		BusyScope busy(ctx);

		// 先に全部写してから貼る。写し元と貼り先が近くても取り違えない
		std::vector<Snapshot> snaps;

		for (int i = 0; i < count; ++i) snaps.push_back(capture(ctx, unitTarget(from + i)));

		for (int i = 0; i < count; ++i) paste(ctx, unitTarget(to + i), snaps[(size_t)i]);
	}

	namespace
	{
		Target unitTarget(const juce::String& channel, const juce::String& head, int index)
		{
			Target t;

			t.scope = Scope::Unit;
			t.channel = channel;
			t.unit = head + juce::String(index);

			return t;
		}
	}

	void duplicateUnitTo(const GuiContext& ctx, const juce::String& channel, const juce::String& head,
		int from, const std::vector<int>& to, bool withParams, bool withAssets)
	{
		BusyScope busy(ctx);

		auto snap = capture(ctx, unitTarget(channel, head, from));

		// 素材だけ。パラメータは貼らない
		if (!withParams) snap.values.clear();

		for (int index : to)
		{
			if (index == from) continue;

			paste(ctx, unitTarget(channel, head, index), snap, withAssets);
		}
	}

	namespace
	{
		// "_" で区切った語のうち、from と同じものを to へ置き換える
		juce::String replaceToken(const juce::String& s, const juce::String& from, const juce::String& to)
		{
			auto tokens = juce::StringArray::fromTokens(s, "_", "");

			for (auto& t : tokens) if (t == from) t = to;

			return tokens.joinIntoString("_");
		}

		// 単位の名前の頭 (OP3 なら OP)
		juce::String unitHead(const juce::String& unit)
		{
			return unit.trimCharactersAtEnd("0123456789");
		}
	}

	void duplicateCategoryTo(const GuiContext& ctx, const Target& source, const std::vector<juce::String>& units)
	{
		if (source.unit.isEmpty()) return;

		BusyScope busy(ctx);

		const auto snap = capture(ctx, source);

		for (const auto& unit : units)
		{
			if (unit == source.unit) continue;

			Target t = source;

			t.unit = unit;

			for (auto& id : t.ids) id = replaceToken(id, source.unit, unit);
			for (auto& key : t.waveKeys) key = replaceToken(key, source.unit, unit);
			for (auto& owner : t.assetPrefixes) owner = replaceToken(owner, source.unit, unit);
			for (auto& key : t.stepTables) key = replaceToken(key, source.unit, unit);

			// 写し先に無いパラメータ (相手の単位が持たないもの) は外す
			t.ids.erase(std::remove_if(t.ids.begin(), t.ids.end(),
				[&](const juce::String& id) { return ctx.apvts.getParameter(id) == nullptr; }), t.ids.end());

			paste(ctx, t, snap);
		}
	}

	std::vector<int> unitNumbers(const GuiContext& ctx, const juce::String& channel, const juce::String& head)
	{
		std::set<int> found;
		const auto lead = channel + "_" + head;

		for (auto* raw : ctx.apvts.processor.getParameters())
		{
			auto* p = dynamic_cast<juce::RangedAudioParameter*>(raw);

			if (p == nullptr) continue;

			const auto id = p->getParameterID();

			if (!id.startsWith(lead)) continue;

			const auto rest = id.substring(lead.length());
			const auto digits = rest.initialSectionContainingOnly("0123456789");

			if (digits.isEmpty() || !rest.substring(digits.length()).startsWith("_")) continue;

			found.insert(digits.getIntValue());
		}

		return { found.begin(), found.end() };
	}

	void clearUnits(const GuiContext& ctx, const juce::String& channel, const juce::String& head,
		const std::vector<int>& units)
	{
		BusyScope busy(ctx);

		for (int index : units) clear(ctx, unitTarget(channel, head, index));
	}

	void setEditSkipped(const GuiContext& ctx, const juce::String& channel, const juce::String& key, bool skipped)
	{
		auto& keys = skippedKeys()[{ &ctx.apvts, channel }];

		if (skipped) keys.insert(key);
		else keys.erase(key);
	}

	bool isEditSkipped(const GuiContext& ctx, const juce::String& channel, const juce::String& key)
	{
		auto it = skippedKeys().find({ &ctx.apvts, channel });

		return it != skippedKeys().end() && it->second.count(key) > 0;
	}

	void copy(const GuiContext& ctx, const Target& target)
	{
		Clipboard::get().push(capture(ctx, target));
	}

	void cut(const GuiContext& ctx, const Target& target)
	{
		BusyScope busy(ctx);

		copy(ctx, target);
		clear(ctx, target);
	}

	bool pasteClipboard(const GuiContext& ctx, const Target& target)
	{
		const auto& current = Clipboard::get().current();

		if (!current.has_value()) return false;

		paste(ctx, target, *current);

		return true;
	}

	// ------------------------------------------------------------------ タブが持つ素材
	void collectAssets(const std::vector<AssetSlot>& slots, std::map<juce::String, juce::String>& out)
	{
		for (const auto& slot : slots)
		{
			const auto path = slot.get();

			if (path.isNotEmpty()) out[slot.key] = path;
		}
	}

	void applyAssets(const std::vector<AssetSlot>& slots, const std::map<juce::String, juce::String>& assets, bool clearOthers)
	{
		for (const auto& slot : slots)
		{
			auto it = assets.find(slot.key);

			if (it != assets.end())
			{
				// 同じものなら読み直さない
				if (it->second == slot.get()) continue;

				const juce::File file(it->second);

				slot.set(file.existsAsFile() ? file : juce::File());
			}
			else if (clearOthers && slot.get().isNotEmpty())
			{
				slot.set(juce::File());
			}
		}
	}

	int unitIndexOf(const juce::String& prefix, const juce::String& channel, const juce::String& head)
	{
		const auto start = channel + "_" + head;

		if (!prefix.startsWith(start)) return -1;

		const auto rest = prefix.substring(start.length());

		if (rest.isEmpty() || !rest.containsOnly("0123456789")) return -1;

		return rest.getIntValue();
	}

	// ------------------------------------------------------------------ クリップボード
	Clipboard& Clipboard::get()
	{
		// 終わりまで持つ。静的な破棄の順番に巻き込まれないよう、壊さない
		if (g_clipboard == nullptr) g_clipboard = new Clipboard();

		return *g_clipboard;
	}

	void Clipboard::push(Snapshot snapshot)
	{
		snapshot.serial = m_nextSerial++;

		m_current = snapshot;
		m_history.push_front(std::move(snapshot));

		while ((int)m_history.size() > maxHistory) m_history.pop_back();

		sendChangeMessage();
	}

	void Clipboard::select(int serial)
	{
		for (const auto& s : m_history)
		{
			if (s.serial != serial) continue;

			m_current = s;

			sendChangeMessage();

			return;
		}
	}

	// ------------------------------------------------------------------ 区分
	juce::String tabNameOf(juce::Component& component)
	{
		for (auto* c = &component; c != nullptr; c = c->getParentComponent())
		{
			auto* parent = c->getParentComponent();

			if (parent == nullptr) break;

			// TabbedComponent の中身は、TabbedComponent の直下に置かれる
			if (auto* tabs = dynamic_cast<juce::TabbedComponent*>(parent))
			{
				for (int i = 0; i < tabs->getNumTabs(); ++i)
				{
					if (tabs->getTabContentComponent(i) == c) return tabs->getTabNames()[i];
				}
			}
		}

		return {};
	}

	namespace
	{
		// 板の中にある部品 (孫も含む) から、パラメータと素材の印を集める
		void collectMembers(const GuiContext& ctx, juce::Component& c, Target& t)
		{
			const auto id = c.getComponentID();

			if (id.isNotEmpty() && ctx.apvts.getParameter(id) != nullptr
				&& std::find(t.ids.begin(), t.ids.end(), id) == t.ids.end())
			{
				t.ids.push_back(id);
			}

			const auto& props = c.getProperties();

			if (props.contains(Property::waveKey))
			{
				const auto key = props[Property::waveKey].toString();

				if (std::find(t.waveKeys.begin(), t.waveKeys.end(), key) == t.waveKeys.end()) t.waveKeys.push_back(key);
			}

			if (props.contains(Property::stepTable))
			{
				const auto key = props[Property::stepTable].toString();

				if (std::find(t.stepTables.begin(), t.stepTables.end(), key) == t.stepTables.end()) t.stepTables.push_back(key);
			}

			if (props.contains(Property::assetPrefix))
			{
				const auto key = props[Property::assetPrefix].toString();

				if (std::find(t.assetPrefixes.begin(), t.assetPrefixes.end(), key) == t.assetPrefixes.end()) t.assetPrefixes.push_back(key);
			}

			for (auto* child : c.getChildren()) collectMembers(ctx, *child, t);
		}

		// いちばん多いチャンネル・単位を、区分の持ち主として一覧に出す
		void nameOwner(Target& t)
		{
			std::map<juce::String, int> channels, units;

			for (const auto& id : t.ids)
			{
				const auto [first, second] = firstTokens(id);

				++channels[first];

				if (isUnitToken(second)) ++units[second];
			}

			auto top = [](const std::map<juce::String, int>& m) {
				juce::String best;
				int n = 0;

				for (const auto& [k, v] : m) if (v > n) { best = k; n = v; }

				return best;
			};

			t.channel = top(channels);
			t.unit = top(units);

			// タブの外 (音源のプラグインの FX など) は、チャンネルの名前で出す
			if (t.tab.isEmpty()) t.tab = t.channel;
		}
	}

	Target categoryTarget(GuiCategoryLabel& label)
	{
		Target t;

		t.scope = Scope::Category;
		t.category = label.getText();
		t.tab = tabNameOf(label);

		auto* parent = label.getParentComponent();

		if (parent == nullptr) return t;

		// 閉じている区分は、いったん開いて並べ直す。板の場所が中身の場所。
		const bool wasOpen = label.isDetailVisible();

		if (!wasOpen)
		{
			label.setDetailVisible(true);
			label.context().editor.resized();
		}

		const auto area = label.backdropBounds();

		if (!area.isEmpty())
		{
			for (auto* c : parent->getChildren())
			{
				if (c == &label || !c->isVisible()) continue;

				if (!area.contains(c->getBounds().getCentre())) continue;

				collectMembers(label.context(), *c, t);
			}
		}

		if (!wasOpen)
		{
			label.setDetailVisible(false);
			label.context().editor.resized();
		}

		nameOwner(t);

		// 区分と同じ持ち主の素材
		for (auto& owner : t.assetPrefixes)
		{
			if (owner == Property::sameOwner) owner = t.channel.isEmpty() ? juce::String() : t.prefix();
		}

		t.assetPrefixes.erase(std::remove_if(t.assetPrefixes.begin(), t.assetPrefixes.end(),
			[](const juce::String& s) { return s.isEmpty(); }), t.assetPrefixes.end());

		return t;
	}

	namespace
	{
		// 区分の複写のダイアログ。区分の見出しから開くので持ち主の部品が無い。
		// 自分で自分を持ち、閉じたら片付く。出した先 (エディタ) が先に消える
		// ときも一緒に消える。
		class OwnedDuplicateDialog : public GuiTargetsDialog, private juce::ComponentListener
		{
		public:
			static void open(const GuiContext& ctx, juce::Component& anchor, const Target& source)
			{
				auto* top = anchor.getTopLevelComponent();

				if (top == nullptr || source.unit.isEmpty()) return;

				const auto head = unitHead(source.unit);
				const auto numbers = unitNumbers(ctx, source.channel, head);

				if (numbers.size() < 2) return;

				// 画面の番号は TARGET のつまみに合わせる。OP / PAD は 1 から、
				// WT+ / ADPCM+ の TARGET (SLOT) は 0 から
				const bool slot = head == "SLOT";

				Config c;

				c.mode = Mode::DuplicateTo;
				c.count = (int)numbers.size();
				c.current = 0;
				c.title = Text::duplicate() + ": " + source.category;

				std::vector<juce::String> units;

				for (int i = 0; i < (int)numbers.size(); ++i)
				{
					const auto unit = head + juce::String(numbers[(size_t)i]);

					if (unit == source.unit) c.current = i;

					units.push_back(unit);
					c.names.push_back(slot ? juce::String(numbers[(size_t)i]) : head + juce::String(numbers[(size_t)i] + 1));
				}

				c.onDuplicateTo = [ctx, source, units](const std::vector<int>& targets) {
					std::vector<juce::String> to;

					for (int i : targets) to.push_back(units[(size_t)i]);

					duplicateCategoryTo(ctx, source, to);
				};

				auto* dialog = new OwnedDuplicateDialog(ctx, *top);

				c.onClosed = [dialog] {
					juce::MessageManager::callAsync([dialog] { delete dialog; });
				};

				constexpr int width = 380;

				const auto area = top->getLocalBounds().withSizeKeepingCentre(
					juce::jmin(width, top->getWidth() - 20), juce::jmax(100, top->getHeight() - 40));

				dialog->GuiTargetsDialog::open(*top, area, c);

				// 中身の高さに合わせて縦も真ん中へ寄せる
				dialog->setCentrePosition(top->getLocalBounds().getCentre());
			}

			~OwnedDuplicateDialog() override
			{
				if (auto* owner = m_owner.getComponent()) owner->removeComponentListener(this);
			}
		private:
			OwnedDuplicateDialog(const GuiContext& ctx, juce::Component& owner) : GuiTargetsDialog(ctx), m_owner(&owner)
			{
				owner.addComponentListener(this);
			}

			void componentBeingDeleted(juce::Component& c) override
			{
				if (&c != m_owner.getComponent()) return;

				c.removeComponentListener(this);
				m_owner = nullptr;

				delete this;
			}

			juce::Component::SafePointer<juce::Component> m_owner;
		};

		// 読み込み・書き出し (区分の見出しから。空なら押せない)
		struct ImportExport
		{
			std::function<void()> onImport;
			std::function<void()> onExport;
		};

		// CUT / COPY / PASTE / CLEAR / DUPLICATE / IMPORT / EXPORT のメニュー
		void showMenu(juce::Component& anchor, const GuiContext& ctx, const Target& target, const ImportExport& io = {})
		{
			const bool has = !target.ids.empty() || !target.waveKeys.empty() || !target.assetPrefixes.empty() || !target.stepTables.empty();
			const bool canPaste = has && Clipboard::get().current().has_value();

			// 複写はオペレーター・パッド・TARGET の区分だけ。写し先が要る
			const bool canDuplicate = has && target.unit.isNotEmpty()
				&& unitNumbers(ctx, target.channel, unitHead(target.unit)).size() > 1;

			juce::PopupMenu menu;

			menu.addItem(1, Text::cut(), has);
			menu.addItem(2, Text::copy(), has);
			menu.addItem(3, Text::paste(), canPaste);
			menu.addItem(4, Text::clear(), has);
			menu.addItem(5, Text::duplicate(), canDuplicate);
			menu.addSeparator();
			menu.addItem(6, Text::importParams(), io.onImport != nullptr);
			menu.addItem(7, Text::exportParams(), io.onExport != nullptr);

			juce::Component::SafePointer<juce::Component> safe(&anchor);

			menu.showMenuAsync(juce::PopupMenu::Options().withTargetComponent(&anchor),
				[safe, target, ctx, io](int result) {
					if (safe == nullptr || result == 0) return;

					switch (result)
					{
					case 1: cut(ctx, target); break;
					case 2: copy(ctx, target); break;
					case 3: pasteClipboard(ctx, target); break;
					case 4: clear(ctx, target); break;
					case 5: OwnedDuplicateDialog::open(ctx, *safe, target); break;
					case 6: if (io.onImport) io.onImport(); break;
					case 7: if (io.onExport) io.onExport(); break;
					default: break;
					}
				});
		}
	}

	void showCategoryMenu(GuiCategoryLabel& label)
	{
		showMenu(label, label.context(), categoryTarget(label), { label.onImport, label.onExport });
	}

	Target groupTarget(const GuiContext& ctx, juce::GroupComponent& group)
	{
		Target t;

		t.scope = Scope::Category;
		t.category = group.getText();
		t.tab = tabNameOf(group);

		auto* parent = group.getParentComponent();

		if (parent == nullptr) return t;

		const auto area = group.getBounds();

		for (auto* c : parent->getChildren())
		{
			if (c == &group || !c->isVisible()) continue;

			if (!area.contains(c->getBounds().getCentre())) continue;

			collectMembers(ctx, *c, t);
		}

		nameOwner(t);

		for (auto& owner : t.assetPrefixes)
		{
			if (owner == Property::sameOwner) owner = t.channel.isEmpty() ? juce::String() : t.prefix();
		}

		t.assetPrefixes.erase(std::remove_if(t.assetPrefixes.begin(), t.assetPrefixes.end(),
			[](const juce::String& s) { return s.isEmpty(); }), t.assetPrefixes.end());

		return t;
	}

	void showGroupMenu(const GuiContext& ctx, juce::GroupComponent& group)
	{
		showMenu(group, ctx, groupTarget(ctx, group));
	}
}
