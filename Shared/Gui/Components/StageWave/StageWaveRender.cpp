#include "./StageWaveRender.h"

#include <algorithm>
#include <cmath>

#include "Shared/Core/Const/ConstGlobal.h"
#include "Shared/Core/Synth/LivePosition.h"

namespace
{
	// 1 回ぶんの計算で拾った音。間引いたあと
	struct Collected
	{
		std::vector<float> raw;
		std::vector<float> core;
		std::array<std::vector<float>, StageProbe::maxOps> ops;

		std::vector<float> unisonL, unisonR;
		std::vector<float> dacL, dacR;
		std::vector<float> fxL, fxR;

		int unisonSeen = 1;
	};

	float clip(float v) { return juce::jlimit(-1.0f, 1.0f, v); }

	// 打鍵の並びどおりに回す。gate なら、離した音を余韻を待たずに止める
	void runPass(GuiStageRig& rig, const std::vector<StageWaveRender::Event>& events, bool gate, Collected& out)
	{
		using namespace StageWaveRender;

		const int block = blockSize;

		juce::AudioBuffer<float> synth(2, block);
		juce::AudioBuffer<float> dac(2, block);
		juce::AudioBuffer<float> fx(2, block);

		juce::MidiBuffer midi;

		int endAt = 0;

		for (const auto& e : events) endAt = juce::jmax(endAt, e.at);

		const int hardEnd = endAt + (int)(maxTailSeconds * renderRate);
		const int silentLimit = (int)(tailSeconds * renderRate);

		const size_t reserve = (size_t)(hardEnd / storeStep + 1);

		for (auto* v : { &out.raw, &out.core, &out.unisonL, &out.unisonR, &out.dacL, &out.dacR, &out.fxL, &out.fxR }) v->reserve(reserve);

		size_t next = 0;
		int pos = 0;
		int silentFor = 0;

		while (pos < hardEnd)
		{
			midi.clear();

			while (next < events.size() && events[next].at <= pos)
			{
				const auto& e = events[next++];

				if (e.on) {
					// EG を切った音は離しても鳴り続けるので、押し直す前に止める
					if (gate) rig.stopNote(e.note);

					midi.addEvent(juce::MidiMessage::noteOn(1, e.note, (juce::uint8)100), 0);
				}
				else if (gate) {
					rig.stopNote(e.note);
				}
				else {
					midi.addEvent(juce::MidiMessage::noteOff(1, e.note), 0);
				}
			}

			int n = juce::jmin(block, hardEnd - pos);

			if (next < events.size()) n = juce::jmin(n, events[next].at - pos);

			n = juce::jmax(1, n);

			rig.render(midi, n, synth, dac, fx);

			auto& probe = rig.probe();

			out.unisonSeen = juce::jmax(out.unisonSeen, probe.unisonSeen);

			for (int i = 0; i < n; ++i)
			{
				const int at = pos + i;

				// 鳴り終わりを見張るのは、最後に離したあとだけ
				if (at >= endAt) {
					const bool loud = std::abs(fx.getSample(0, i)) > silenceLevel || std::abs(fx.getSample(1, i)) > silenceLevel
						|| std::abs(synth.getSample(0, i)) > silenceLevel || std::abs(synth.getSample(1, i)) > silenceLevel;

					silentFor = loud ? 0 : silentFor + 1;
				}

				if ((at % storeStep) != 0) continue;

				out.raw.push_back(clip(probe.raw[(size_t)i]));
				out.core.push_back(clip(probe.core[(size_t)i]));

				for (int op = 0; op < StageProbe::maxOps; ++op) out.ops[(size_t)op].push_back(clip(probe.ops[(size_t)op][(size_t)i]));

				out.unisonL.push_back(clip(synth.getSample(0, i)));
				out.unisonR.push_back(clip(synth.getSample(1, i)));
				out.dacL.push_back(clip(dac.getSample(0, i)));
				out.dacR.push_back(clip(dac.getSample(1, i)));
				out.fxL.push_back(clip(fx.getSample(0, i)));
				out.fxR.push_back(clip(fx.getSample(1, i)));
			}

			pos += n;

			if (pos >= endAt && silentFor >= silentLimit) break;
		}
	}

	GenWaveRender::Wave makeWave(const std::vector<float>& left, const std::vector<float>& right, double noteHz)
	{
		GenWaveRender::Wave w;

		w.sampleRate = StageWaveRender::renderRate / (double)StageWaveRender::storeStep;
		w.noteHz = noteHz;
		w.left = left;
		w.right = right;

		return w;
	}

	GenWaveRender::Wave makeMono(const std::vector<float>& data, double noteHz)
	{
		return makeWave(data, data, noteHz);
	}

	// RHYTHM で鳴らす鍵 (同じ鍵のパッドは 1 回押せば両方鳴る)
	std::vector<int> padKeys(const GuiStageInfo& info, const std::array<bool, StageProbe::maxOps>* checked)
	{
		std::vector<int> keys;

		for (int i = 0; i < info.opCount; ++i)
		{
			if (checked != nullptr && !(*checked)[(size_t)i]) continue;

			const int key = info.padNotes[(size_t)i];

			if (std::find(keys.begin(), keys.end(), key) == keys.end()) keys.push_back(key);
		}

		return keys;
	}
}

std::vector<StageWaveRender::Event> StageWaveRender::buildEvents(const Pattern& pattern, const std::vector<int>& notes,
	double sampleRate, bool& truncated)
{
	std::vector<Event> events;

	truncated = false;

	if (notes.empty()) return events;

	const double bpm = juce::jlimit(1.0, 999.0, std::isfinite(pattern.bpm) ? pattern.bpm : 120.0);
	const double quarterSeconds = 60.0 / bpm;

	// 1 小節の長さ (4 分音符いくつぶんか)
	const double barQuarters = (double)juce::jmax(1, pattern.sigNumerator) * 4.0 / (double)juce::jmax(1, pattern.sigDenominator);

	double patternSeconds = (double)juce::jmax(1, pattern.bars) * barQuarters * quarterSeconds;

	if (patternSeconds > maxPatternSeconds) {
		patternSeconds = maxPatternSeconds;
		truncated = true;
	}

	const double interval = juce::jmax(1.0e-3, pattern.interval.quarters() * quarterSeconds);
	const double length = juce::jmax(1.0e-3, pattern.length.quarters() * quarterSeconds);

	// 押す時刻
	std::vector<double> starts;

	for (double t = 0.0; t < patternSeconds - 1.0e-9; t += interval) starts.push_back(t);

	for (size_t k = 0; k < starts.size(); ++k)
	{
		const int on = (int)std::lround(starts[k] * sampleRate);

		double offSeconds = starts[k] + length;

		// 上限で切ったときは、そこで離す
		if (truncated) offSeconds = juce::jmin(offSeconds, maxPatternSeconds);

		const int off = (int)std::lround(offSeconds * sampleRate);

		// 次に押すより後で離すなら、離さない (押し直しで前の音は止まる。後から離すと次の音を切ってしまう)
		const bool keepHeld = (k + 1 < starts.size()) && off > (int)std::lround(starts[k + 1] * sampleRate);

		for (int note : notes)
		{
			events.push_back({ on, note, true });

			if (!keepHeld) events.push_back({ juce::jmax(on + 1, off), note, false });
		}
	}

	// 時刻の順。同じ時刻なら、離すのを先に
	std::stable_sort(events.begin(), events.end(), [](const Event& a, const Event& b) {
		if (a.at != b.at) return a.at < b.at;

		return !a.on && b.on;
	});

	return events;
}

StageWaveRender::Pattern StageWaveRender::patternFrom(const GuiProcessorHost& host, bool followDaw)
{
	Pattern p;

	p.bpm = host.stageBpm;
	p.bars = juce::jlimit(1, 16, host.stageBars);
	p.interval = { host.stageIntervalDiv, host.stageIntervalTriplet, host.stageIntervalDotted };
	p.length = { host.stageLengthDiv, host.stageLengthTriplet, host.stageLengthDotted };
	p.midiNote = juce::jlimit(0, 127, host.stageMidiNote);

	if (followDaw)
	{
		const double bpm = host.hostBpm.load(std::memory_order_relaxed);

		if (bpm > 0.0) p.bpm = bpm;

		p.sigNumerator = juce::jlimit(1, 32, host.hostTimeSigNumerator.load(std::memory_order_relaxed));
		p.sigDenominator = juce::jlimit(1, 32, host.hostTimeSigDenominator.load(std::memory_order_relaxed));
	}

	return p;
}

GuiStageInfo StageWaveRender::inspect(GuiProcessorHost& host)
{
	auto rig = host.createStageRig(renderRate, 0, false, 1);

	return rig != nullptr ? rig->info() : GuiStageInfo{};
}

std::shared_ptr<StageWaveRender::Job> StageWaveRender::prepare(GuiProcessorHost& host, const Options& options)
{
	auto job = std::make_shared<Job>();

	job->options = options;
	job->main = host.createStageRig(renderRate, options.off, false, Global::totalVoices);

	if (job->main == nullptr) return nullptr;

	job->info = job->main->info();

	// オペレーターの EG を切ると、押しているあいだもコアは鳴り終わったと見ることがある
	const bool hold = job->info.kind == StageProbe::Kind::fm && (options.off & StageSwitch::bit(StageSwitch::opEnv)) != 0;

	job->main->setHoldWhileKeyDown(hold);

	switch (job->info.kind)
	{
	case StageProbe::Kind::fm:
		job->extra = host.createStageRig(renderRate, options.off, true, Global::totalVoices);
		if (job->extra != nullptr) job->extra->setHoldWhileKeyDown(hold);
		break;
	case StageProbe::Kind::rhythm:
		job->extra = host.createStageRig(renderRate, options.off, false, Global::totalVoices);
		break;
	default:
		break;
	}

	return job;
}

StageWaveRender::Result StageWaveRender::run(Job& job)
{
	// 計算で鳴らした音は、グラフの鳴っている位置に出さない
	const LivePosition::Mute mute;

	Result out;

	out.info = job.info;

	const auto& info = job.info;
	const auto& pattern = job.options.pattern;

	const bool isFm = info.kind == StageProbe::Kind::fm;
	const bool isRhythm = info.kind == StageProbe::Kind::rhythm;

	// オペレーターの EG を切ったときは、離したところで止める (EG を切ると離しても鳴り続けるため)
	const bool gate = isFm && (job.options.off & StageSwitch::bit(StageSwitch::opEnv)) != 0;

	const double noteHz = isRhythm ? 0.0 : juce::MidiMessage::getMidiNoteInHertz(pattern.midiNote);

	const std::vector<int> mainKeys = isRhythm ? padKeys(info, &job.options.padChecked) : std::vector<int>{ pattern.midiNote };

	bool truncated = false;

	Collected main;

	runPass(*job.main, buildEvents(pattern, mainKeys, renderRate, truncated), gate, main);

	out.truncated = truncated;

	Collected extra;

	if (job.extra != nullptr)
	{
		const std::vector<int> keys = isRhythm ? padKeys(info, nullptr) : mainKeys;

		bool cut = false;

		runPass(*job.extra, buildEvents(pattern, keys, renderRate, cut), gate, extra);
	}

	const int ops = juce::jlimit(0, viewsPerRow, info.opCount);

	if (isFm)
	{
		for (int i = 0; i < ops; ++i)
		{
			out.row1[(size_t)i] = makeMono(extra.ops[(size_t)i], noteHz);
			out.row2[(size_t)i] = makeMono(main.ops[(size_t)i], noteHz);
		}
	}
	else if (isRhythm)
	{
		for (int i = 0; i < ops; ++i) out.row1[(size_t)i] = makeMono(extra.ops[(size_t)i], noteHz);
	}
	else
	{
		out.row1[0] = makeMono(main.raw, noteHz);
	}

	// ① は FM だけ (それ以外は第 1 段に出している)
	out.stageOn[stageRaw] = isFm;
	out.stageOn[stageCore] = true;
	out.stageOn[stageUnison] = main.unisonSeen > 1;
	out.stageOn[stageDac] = info.chipDac;
	out.stageOn[stageFx] = true;

	if (out.stageOn[stageRaw]) out.stages[stageRaw] = makeMono(main.raw, noteHz);

	out.stages[stageCore] = makeMono(main.core, noteHz);

	if (out.stageOn[stageUnison]) out.stages[stageUnison] = makeWave(main.unisonL, main.unisonR, noteHz);
	if (out.stageOn[stageDac]) out.stages[stageDac] = makeWave(main.dacL, main.dacR, noteHz);

	out.stages[stageFx] = makeWave(main.fxL, main.fxR, noteHz);

	return out;
}
