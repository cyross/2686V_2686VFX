#include "./ProcessorPattern.h"

std::mutex& PatternStore::registryLock()
{
	static std::mutex m;
	return m;
}

std::map<const juce::AudioProcessorValueTreeState*, PatternStore*>& PatternStore::registry()
{
	static std::map<const juce::AudioProcessorValueTreeState*, PatternStore*> r;
	return r;
}

PatternStore::~PatternStore()
{
	stopTimer();

	if (apvts != nullptr) {
		std::lock_guard<std::mutex> lock(registryLock());

		auto& r = registry();
		auto it = r.find(apvts);

		if (it != r.end() && it->second == this) r.erase(it);
	}
}

int PatternStore::Section::indexOf(const juce::String& id) const
{
	for (int i = 0; i < (int)params.size(); ++i) {
		if (params[(size_t)i]->getParameterID() == id) return i;
	}

	return -1;
}

void PatternStore::attach(juce::AudioProcessorValueTreeState& state, StepTableStore& tables)
{
	apvts = &state;
	tableStore = &tables;

	// ID の順に並べ、区分の頭で始まるものを前から拾う
	std::map<juce::String, juce::RangedAudioParameter*> byId;

	for (auto* p : state.processor.getParameters()) {
		if (auto* ranged = dynamic_cast<juce::RangedAudioParameter*>(p)) byId[ranged->getParameterID()] = ranged;
	}

	for (const auto& [id, param] : byId) {
		if (!id.endsWith(EnvPattern::patternsKey)) continue;

		auto s = std::make_unique<Section>();

		s->base = id.dropLastCharacters(EnvPattern::patternsKey.length());
		s->patterns = state.getRawParameterValue(s->base + EnvPattern::patternsKey);
		s->pattern = state.getRawParameterValue(s->base + EnvPattern::patternKey);
		s->resetFlag = state.getRawParameterValue(s->base + EnvPattern::resetKey);

		if (s->pattern == nullptr || s->resetFlag == nullptr) continue;

		const auto head = s->base + "_";

		for (auto it = byId.lower_bound(head); it != byId.end() && it->first.startsWith(head); ++it) {
			const auto tail = it->first.substring(s->base.length());

			if (EnvPattern::isExcludedTail(tail)) continue;

			s->params.push_back(it->second);
			s->apvtsRaw.push_back(state.getRawParameterValue(it->first));
			s->defaults.push_back(it->second->convertFrom0to1(it->second->getDefaultValue()));
		}

		const auto n = s->params.size();

		s->live = std::make_unique<std::atomic<float>[]>(n);

		for (int p = 1; p <= EnvPattern::maxPatterns; ++p) s->slots[(size_t)p] = std::make_unique<std::atomic<float>[]>(n);

		for (size_t i = 0; i < n; ++i) {
			const float now = s->apvtsRaw[i]->load(std::memory_order_relaxed);

			s->live[i].store(now, std::memory_order_relaxed);

			for (int p = 1; p <= EnvPattern::maxPatterns; ++p) s->slots[(size_t)p][i].store(s->defaults[i], std::memory_order_relaxed);

			s->slots[1][i].store(now, std::memory_order_relaxed);
		}

		for (size_t i = 0; i < n; ++i) members[s->params[i]->getParameterID()] = { s.get(), (int)i };

		byBase[s->base] = s.get();

		sections.push_back(std::move(s));
	}

	{
		std::lock_guard<std::mutex> lock(registryLock());
		registry()[apvts] = this;
	}

	// 組の値を APVTS へ戻すのと、組の表を作るのはメッセージスレッドで
	startTimerHz(30);
}

PatternStore* PatternStore::find(const juce::AudioProcessorValueTreeState& state)
{
	std::lock_guard<std::mutex> lock(registryLock());

	auto& r = registry();
	auto it = r.find(&state);

	return it == r.end() ? nullptr : it->second;
}

std::atomic<float>* PatternStore::raw(juce::AudioProcessorValueTreeState& state, const juce::String& id)
{
	if (auto* store = find(state)) {
		auto it = store->members.find(id);

		if (it != store->members.end()) return &it->second.first->live[(size_t)it->second.second];
	}

	return state.getRawParameterValue(id);
}

const std::atomic<uint32_t>* PatternStore::restartOf(juce::AudioProcessorValueTreeState& state, const juce::String& base)
{
	if (auto* store = find(state)) {
		if (auto* s = store->sectionOf(base)) return &s->restart;
	}

	return nullptr;
}

PatternStore::Section* PatternStore::sectionOf(const juce::String& base) const
{
	auto it = byBase.find(base);

	return it == byBase.end() ? nullptr : it->second;
}

PatternStore::Section* PatternStore::sectionOfTable(const juce::String& key) const
{
	// 表の鍵は区分の頭で始まる (code + CPK::ssgSwEnv256 = code + "_SSGSW256" など)。
	// いちばん長く合う頭を選ぶ
	Section* best = nullptr;

	for (auto& s : sections) {
		if (!key.startsWith(s->base)) continue;

		if (best == nullptr || s->base.length() > best->base.length()) best = s.get();
	}

	return best;
}

void PatternStore::registerTable(const juce::String& key, float defaultR, float defaultL, StepTable* table, void** slot)
{
	auto* s = sectionOfTable(key);

	if (s == nullptr) return;

	TableBind* bind = nullptr;

	for (auto& t : s->tables) {
		if (t->key == key) bind = t.get();
	}

	if (bind == nullptr) {
		auto b = std::make_unique<TableBind>();

		b->key = key;
		b->defaultR = defaultR;
		b->defaultL = defaultL;

		for (auto& t : b->tables) t.store(nullptr, std::memory_order_relaxed);

		b->tables[1].store(table, std::memory_order_relaxed);

		bind = b.get();
		s->tables.push_back(std::move(b));

		byTable[key] = { s, bind };
	}

	if (slot != nullptr) bind->slots.push_back(slot);
}

void PatternStore::process()
{
	lastProcess.store(juce::Time::getMillisecondCounter(), std::memory_order_relaxed);

	for (auto& sp : sections) {
		auto& s = *sp;
		const auto n = s.params.size();

		const int p = EnvPattern::clampPattern(s.patterns->load(std::memory_order_relaxed), s.pattern->load(std::memory_order_relaxed));
		const int e = s.editing.load(std::memory_order_acquire);

		// 画面で選んでいる組へ、APVTS の値 (つまみ・オートメーション) を写す
		if (e >= 1) {
			auto* slot = s.slots[(size_t)e].get();

			for (size_t i = 0; i < n; ++i) slot[i].store(s.apvtsRaw[i]->load(std::memory_order_relaxed), std::memory_order_relaxed);
		}

		const int old = s.active.load(std::memory_order_relaxed);

		if (p != old) {
			if (s.resetFlag->load(std::memory_order_relaxed) > 0.5f) {
				s.restart.fetch_add(1, std::memory_order_release);

				// TAP SEQUENCER は開始前へ
				for (auto& t : s.tables) {
					if (auto* table = t->tables[(size_t)p].load(std::memory_order_acquire)) table->clearCursor();
				}
			}
			else {
				// 段の位置を新しい組へ引き継ぐ
				for (auto& t : s.tables) {
					auto* from = t->tables[(size_t)old].load(std::memory_order_acquire);
					auto* to = t->tables[(size_t)p].load(std::memory_order_acquire);

					if (from == nullptr) from = t->tables[1].load(std::memory_order_acquire);
					if (to == nullptr) to = t->tables[1].load(std::memory_order_acquire);

					if (from != nullptr && to != nullptr && from != to) to->cursor.store(from->cursor.load(std::memory_order_relaxed), std::memory_order_relaxed);
				}
			}

			s.active.store(p, std::memory_order_release);
		}

		// 使う組の値を音の側へ
		auto* use = s.slots[(size_t)p].get();

		for (size_t i = 0; i < n; ++i) s.live[i].store(use[i].load(std::memory_order_relaxed), std::memory_order_relaxed);

		// 使う組の表。まだ作っていなければ 1 組目
		for (auto& t : s.tables) {
			auto* table = t->tables[(size_t)p].load(std::memory_order_acquire);

			if (table == nullptr) table = t->tables[1].load(std::memory_order_acquire);

			for (auto* slot : t->slots) *slot = (void*)table;
		}
	}
}

int PatternStore::editingPattern(const juce::String& base) const
{
	if (auto* s = sectionOf(base)) {
		const int e = s->editing.load(std::memory_order_acquire);

		return e >= 1 ? e : EnvPattern::clampPattern(s->patterns->load(std::memory_order_relaxed), s->pattern->load(std::memory_order_relaxed));
	}

	return 1;
}

juce::String PatternStore::tableKeyFor(const juce::String& key) const
{
	auto it = byTable.find(key);

	if (it == byTable.end()) return key;

	const int p = editingPattern(it->second.first->base);

	if (p <= 1 || it->second.second->tables[(size_t)p].load(std::memory_order_acquire) == nullptr) return key;

	return EnvPattern::tableKey(key, p);
}

void PatternStore::load(Section& s, int pattern)
{
	// 戻しているあいだは、APVTS の値を組へ写さない
	s.editing.store(0, std::memory_order_release);

	auto* slot = s.slots[(size_t)pattern].get();

	for (size_t i = 0; i < s.params.size(); ++i) {
		auto* param = s.params[i];
		const float v = slot[i].load(std::memory_order_relaxed);
		const float norm = param->convertTo0to1(v);

		if (std::abs(param->getValue() - norm) > 1.0e-7f) param->setValueNotifyingHost(norm);
	}

	s.editing.store(pattern, std::memory_order_release);
}

void PatternStore::ensureTables()
{
	if (tableStore == nullptr) return;

	for (auto& sp : sections) {
		const int n = std::clamp((int)sp->patterns->load(std::memory_order_relaxed), 1, EnvPattern::maxPatterns);

		for (auto& t : sp->tables) {
			for (int p = 2; p <= n; ++p) {
				if (t->tables[(size_t)p].load(std::memory_order_acquire) != nullptr) continue;

				auto* table = &tableStore->get(EnvPattern::tableKey(t->key, p), t->defaultR, t->defaultL);

				t->tables[(size_t)p].store(table, std::memory_order_release);
			}
		}
	}
}

void PatternStore::timerCallback()
{
	update();
}

void PatternStore::update()
{
	ensureTables();

	// 音が止まっている (process が呼ばれていない) あいだは、ここで写す
	if (juce::Time::getMillisecondCounter() - lastProcess.load(std::memory_order_relaxed) > 200u) process();

	for (auto& sp : sections) {
		const int a = sp->active.load(std::memory_order_acquire);

		if (sp->editing.load(std::memory_order_acquire) != a) load(*sp, a);
	}
}

void PatternStore::write(juce::XmlElement& parent) const
{
	auto* node = parent.createNewChildElement(xmlTag);

	for (auto& sp : sections) {
		const auto& s = *sp;
		const int n = std::clamp((int)s.patterns->load(std::memory_order_relaxed), 1, EnvPattern::maxPatterns);
		const int e = s.editing.load(std::memory_order_acquire);

		// 組が 1 つで、選んでいる組が 1 組目なら APVTS だけで足りる
		if (n <= 1 && e <= 1) continue;

		auto* sec = node->createNewChildElement(xmlSection);

		sec->setAttribute("base", s.base);

		for (int p = 1; p <= n; ++p) {
			auto* pe = sec->createNewChildElement(xmlPattern);

			pe->setAttribute("n", p);

			const auto* slot = s.slots[(size_t)p].get();

			for (size_t i = 0; i < s.params.size(); ++i) {
				const float v = slot[i].load(std::memory_order_relaxed);

				if (v == s.defaults[i]) continue;

				// 区分の頭は付けない (同じ区分の中で一意)
				pe->setAttribute(s.params[i]->getParameterID().substring(s.base.length() + 1), v);
			}
		}
	}
}

void PatternStore::prepareTables(const juce::XmlElement& state)
{
	if (tableStore == nullptr) return;

	auto* tables = state.getChildByName(StepTableIo::xmlTag);

	if (tables == nullptr) return;

	for (auto* item : tables->getChildIterator()) {
		const auto key = item->getStringAttribute("key");
		const int mark = key.lastIndexOf("#P");

		if (mark < 0) continue;

		const auto baseKey = key.substring(0, mark);
		const int p = key.substring(mark + 2).getIntValue();

		if (p < 2 || p > EnvPattern::maxPatterns) continue;

		auto* s = sectionOfTable(baseKey);

		if (s == nullptr) continue;

		for (auto& t : s->tables) {
			if (t->key != baseKey || t->tables[(size_t)p].load(std::memory_order_acquire) != nullptr) continue;

			t->tables[(size_t)p].store(&tableStore->get(key, t->defaultR, t->defaultL), std::memory_order_release);
		}
	}
}

void PatternStore::read(const juce::XmlElement& state)
{
	// 読み終えて settle するまで、APVTS (古い state) を写しへ書かない
	for (auto& sp : sections) sp->editing.store(0, std::memory_order_release);

	// 書いていない組は初期値
	reset();

	if (auto* node = state.getChildByName(xmlTag)) {
		for (auto* sec : node->getChildWithTagNameIterator(xmlSection)) {
			auto* s = sectionOf(sec->getStringAttribute("base"));

			if (s == nullptr) continue;

			for (auto* pe : sec->getChildWithTagNameIterator(xmlPattern)) {
				const int p = pe->getIntAttribute("n");

				if (p < 1 || p > EnvPattern::maxPatterns) continue;

				auto* slot = s->slots[(size_t)p].get();

				for (int a = 0; a < pe->getNumAttributes(); ++a) {
					const auto name = pe->getAttributeName(a);

					if (name == "n") continue;

					const int i = s->indexOf(s->base + "_" + name);

					if (i >= 0) slot[(size_t)i].store((float)pe->getAttributeValue(a).getDoubleValue(), std::memory_order_relaxed);
				}
			}
		}
	}
}

void PatternStore::settle()
{
	// APVTS (いま選んでいる組) がいちばん新しい。音の側もその組から始める
	for (auto& sp : sections) {
		auto& s = *sp;
		const int p = EnvPattern::clampPattern(s.patterns->load(std::memory_order_relaxed), s.pattern->load(std::memory_order_relaxed));
		auto* slot = s.slots[(size_t)p].get();

		for (size_t i = 0; i < s.params.size(); ++i) slot[i].store(s.apvtsRaw[i]->load(std::memory_order_relaxed), std::memory_order_relaxed);

		s.active.store(p, std::memory_order_release);
		s.editing.store(p, std::memory_order_release);
	}

	ensureTables();
}

uint32_t PatternStore::restartSum(const juce::String& token) const
{
	uint32_t sum = 0;

	for (auto& s : sections) {
		if (s->base.endsWith(token)) sum += s->restart.load(std::memory_order_acquire);
	}

	return sum;
}

void PatternStore::reset(const juce::String& code)
{
	for (auto& sp : sections) {
		auto& s = *sp;

		if (code.isNotEmpty() && !s.base.startsWith(code)) continue;

		for (int p = 1; p <= EnvPattern::maxPatterns; ++p) {
			auto* slot = s.slots[(size_t)p].get();

			for (size_t i = 0; i < s.params.size(); ++i) slot[i].store(s.defaults[i], std::memory_order_relaxed);
		}
	}
}
