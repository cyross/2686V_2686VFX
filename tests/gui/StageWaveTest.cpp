// ============================================================================
// 生成中の波形の窓 (4.4.0)
// ============================================================================
//   ・打鍵の並び (テンポ・小節数・間隔・音符) どおりに押す・離すが並ぶ
//   ・エンベロープ・LFO などのまとまりを、計算の写しの上だけで切れる
//   ・どのチャンネルでも段ごとの波形が出来る (FM はオペレーター、RHYTHM はパッド)
//   ・エディタのボタンから別の窓を開け閉めできる
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <cmath>
#include <functional>
#include <vector>

#include "Core/Processor/PluginProcessor.h"
#include "Core/Editor/PluginEditor.h"
#include "Shared/Core/Synth/StageSwitch.h"
#include "Shared/Gui/Components/GenWave/GenWave.h"
#include "Shared/Gui/Components/GenWave/GenWavePlayer.h"
#include "Shared/Gui/Components/StageWave/StageWaveRender.h"

#include "./GuiTestChips.h"

namespace
{
	float peakOf(const std::vector<float>& v)
	{
		float peak = 0.0f;

		for (float x : v) peak = std::max(peak, std::abs(x));

		return peak;
	}

	void setParam(AudioPlugin2686V& p, const juce::String& id, int value)
	{
		auto* param = p.apvts.getParameter(id);
		REQUIRE(param != nullptr);
		param->setValueNotifyingHost(param->convertTo0to1((float)value));
	}

	template <typename T>
	T* findFirst(juce::Component& root)
	{
		for (auto* child : root.getChildren())
		{
			if (auto* t = dynamic_cast<T*>(child)) return t;
			if (auto* t = findFirst<T>(*child)) return t;
		}

		return nullptr;
	}

	bool isFmName(const juce::String& name)
	{
		return name == "OPNA" || name == "OPN" || name == "OPL" || name == "OPL3" || name == "OPM" || name == "OPZX7";
	}

	// 既定では素材 (PCM・波形メモリ) が空で、鳴らないチャンネル
	bool isSilentByDefault(const juce::String& name)
	{
		return name == "RHYTHM" || name == "ADPCM" || name == "ADPCM+" || name == "WAVETABLE" || name == "WTPLUS";
	}

	// テストを軽くするための短い並び (240 BPM・1 小節・4 分音符ごとに 8 分音符)
	StageWaveRender::Options shortOptions()
	{
		StageWaveRender::Options o;

		o.pattern.bpm = 240.0;
		o.pattern.bars = 1;
		o.pattern.interval = { 4, false, false };
		o.pattern.length = { 8, false, false };
		o.pattern.midiNote = 60;

		return o;
	}
}

TEST_CASE("生成中の波形: 打鍵の並びどおりに押す・離すが並ぶ")
{
	using namespace StageWaveRender;

	const double rate = 1000.0;

	SUBCASE("既定は 1 小節に 4 分音符で 4 回押し、それぞれ 4 分音符で離す")
	{
		Pattern p;
		bool truncated = true;

		const auto events = buildEvents(p, { 60 }, rate, truncated);

		CHECK_FALSE(truncated);
		REQUIRE(events.size() == 8);

		std::vector<int> ons;
		std::vector<int> offs;

		for (const auto& e : events) (e.on ? ons : offs).push_back(e.at);

		// 120 BPM の 1 拍 = 0.5 秒
		CHECK(ons == std::vector<int>{ 0, 500, 1000, 1500 });
		CHECK(offs == std::vector<int>{ 500, 1000, 1500, 2000 });
	}

	SUBCASE("音符が間隔より長ければ、押し直すまで離さない (最後の音だけ離す)")
	{
		Pattern p;
		p.interval = { 4, false, false };
		p.length = { 2, false, false };

		bool truncated = false;

		const auto events = buildEvents(p, { 60 }, rate, truncated);

		int ons = 0, offs = 0;

		for (const auto& e : events) (e.on ? ons : offs)++;

		CHECK(ons == 4);
		CHECK(offs == 1);
		CHECK(events.back().at == 1500 + 1000); // 4 拍目に押して 2 拍
	}

	SUBCASE("3 連符・付点の長さ")
	{
		CHECK(NoteValue{ 4, true, false }.quarters() == doctest::Approx(2.0 / 3.0));
		CHECK(NoteValue{ 8, false, true }.quarters() == doctest::Approx(0.75));
		CHECK(NoteValue{ 1, false, false }.quarters() == doctest::Approx(4.0));
	}

	SUBCASE("30 秒を超える並びは打ち切る")
	{
		Pattern p;
		p.bpm = 60.0;
		p.bars = 16; // 64 秒

		bool truncated = false;

		const auto events = buildEvents(p, { 60 }, rate, truncated);

		CHECK(truncated);

		for (const auto& e : events) CHECK(e.at <= (int)(maxPatternSeconds * rate));
	}

	SUBCASE("DAW の拍子 (3/4) なら 1 小節は 3 拍")
	{
		Pattern p;
		p.sigNumerator = 3;
		p.interval = { 4, false, false };
		p.length = { 8, false, false };

		bool truncated = false;

		const auto events = buildEvents(p, { 60 }, rate, truncated);

		int ons = 0;

		for (const auto& e : events) ons += e.on ? 1 : 0;

		CHECK(ons == 3);
	}
}

TEST_CASE("生成中の波形: 1 小節の拍 (音符と数) で小節の長さが決まる")
{
	using namespace StageWaveRender;

	auto holder = std::make_unique<AudioPlugin2686V>();
	auto& host = *holder;

	host.stageBars = 1;
	host.stageIntervalDiv = 8;
	host.stageIntervalTriplet = false;
	host.stageIntervalDotted = false;
	host.stageLengthDiv = 8;
	host.stageLengthTriplet = false;
	host.stageLengthDotted = false;

	const auto countOns = [](const Pattern& p) {
		bool truncated = false;
		int ons = 0;

		for (const auto& e : buildEvents(p, { 60 }, 1000.0, truncated)) ons += e.on ? 1 : 0;

		return ons;
	};

	SUBCASE("4 分音符 × 4 で 8 分音符ごとなら 8 回")
	{
		host.stageBeatDiv = 4;
		host.stageBeatCount = 4;

		const auto p = patternFrom(host, false);

		CHECK(p.sigNumerator == 4);
		CHECK(p.sigDenominator == 4);
		CHECK(countOns(p) == 8);
	}

	SUBCASE("8 分音符 × 6 で 8 分音符ごとなら 6 回")
	{
		host.stageBeatDiv = 8;
		host.stageBeatCount = 6;

		const auto p = patternFrom(host, false);

		CHECK(p.sigNumerator == 6);
		CHECK(p.sigDenominator == 8);
		CHECK(countOns(p) == 6);
	}

	SUBCASE("数は 1〜12 に収める")
	{
		host.stageBeatDiv = 4;
		host.stageBeatCount = 40;

		CHECK(patternFrom(host, false).sigNumerator == 12);
	}
}

TEST_CASE("生成中の波形: 最後のリリースを切ると、打鍵の並びのちょうどの長さになる")
{
	auto holder = std::make_unique<AudioPlugin2686V>();
	auto& plugin = *holder;

	plugin.prepareToPlay(48000.0, 512);

	StageWaveRender::Options options;

	options.pattern.bpm = 240.0; // 1 小節 = 1 秒
	options.pattern.bars = 1;
	options.pattern.interval = { 4, false, false };
	options.pattern.length = { 4, false, false };

	CHECK(StageWaveRender::patternSeconds(options.pattern) == doctest::Approx(1.0));

	const auto lengthOf = [&](bool tail) {
		options.releaseTail = tail;

		auto job = StageWaveRender::prepare(plugin, options);

		REQUIRE(job != nullptr);

		return StageWaveRender::run(*job).stages[StageWaveRender::stageFx].lengthSeconds();
	};

	// 切れば 1 秒ちょうど (間引いた 1 点ぶんの誤差まで)
	CHECK(lengthOf(false) == doctest::Approx(1.0).epsilon(0.001));

	// 入れれば、離したあとの余韻のぶん長い
	CHECK(lengthOf(true) > 1.0 + StageWaveRender::tailSeconds * 0.5);
}

TEST_CASE("生成中の波形: ループを切ると最後で止まり、押すと頭から流れる")
{
	GenWaveRender::Wave wave;

	wave.sampleRate = 1000.0;
	wave.left.assign(1000, 0.0f); // 1 秒
	wave.right = wave.left;

	GenWavePlayer player;

	player.setFramesPerSecond(10.0); // 10 コマ
	player.setLoopInRow(true);
	player.restart(0.0);

	REQUIRE(player.frameCount(wave) == 10);

	SUBCASE("ループ (既定) なら頭へ戻る")
	{
		CHECK(player.isLooping());
		CHECK(player.frame(wave, 1050.0) == 0);
		CHECK_FALSE(player.stopAtEnd(&wave, 1050.0));
	}

	SUBCASE("切れば最後のコマで止まる")
	{
		player.setLooping(false);

		CHECK(player.frame(wave, 950.0) == 9);
		CHECK(player.frame(wave, 1500.0) == 9);
		CHECK(player.stopAtEnd(&wave, 1500.0));
		CHECK(player.isPaused());
		CHECK(player.frame(wave, 5000.0) == 9);

		// 再生のボタン (段の左端) を押すと頭から
		const juce::Rectangle<int> area(0, 0, 300, GenWavePlayer::height);

		player.mouseDown(area, { 5, 5 }, &wave, 6000.0, false);

		CHECK_FALSE(player.isPaused());
		CHECK(player.frame(wave, 6000.0) == 0);
	}

	SUBCASE("ループのボタン (再生のボタンの右) で入り切りする")
	{
		const juce::Rectangle<int> area(0, 0, 300, GenWavePlayer::height);

		player.mouseDown(area, { 24 + 4 + 5, 5 }, &wave, 100.0, false);

		CHECK_FALSE(player.isLooping());

		player.mouseDown(area, { 24 + 4 + 5, 5 }, &wave, 200.0, false);

		CHECK(player.isLooping());
	}
}

TEST_CASE("生成中の波形: まとまりは計算の写しの上だけで切る")
{
	using namespace StageSwitch;

	// FM はオペレーターの EG を持ち、SSG は持たない
	CHECK((present<OpnaParams>() & bit(opEnv)) != 0);
	CHECK((present<SsgParams>() & bit(opEnv)) == 0);
	CHECK((present<SsgParams>() & bit(stepEnv)) != 0);   // DUTY ENV・PEAK ENV
	CHECK((present<RhythmParams>() & bit(ampEnv)) != 0); // パッドの ADSR

	OpnaParams p;

	p.algEnvEnable = true;
	p.op[0].pitchEnvEnable = true;
	p.ssgHwEnv.enable = true;
	p.glLfo.pm = true;

	apply(p, bit(pitchEnv) | bit(lfo));

	CHECK_FALSE(p.op[0].pitchEnvEnable);
	CHECK(p.op[0].pitchAdsr.bypass);
	CHECK_FALSE(p.glLfo.pm);

	// 切っていないものは触らない
	CHECK(p.algEnvEnable);
	CHECK(p.ssgHwEnv.enable);
	CHECK_FALSE(p.op[0].m_adsrParams.bypass);
}

TEST_CASE("生成中の波形: どのチャンネルでも段ごとの波形が出来る")
{
	auto holder = std::make_unique<AudioPlugin2686V>();
	auto& plugin = *holder;

	plugin.prepareToPlay(48000.0, 512);

	int checked = 0;

	for (int m = 0; m < (int)OscMode::Count; ++m)
	{
		const juce::String name = getModeName((OscMode)m);

		if (GuiTestChips::byModeName(name) == nullptr) continue;

		INFO("チャンネル: ", name);

		setParam(plugin, "MODE", m);

		const auto info = StageWaveRender::inspect(plugin);

		const bool fm = isFmName(name);
		const bool rhythm = name == "RHYTHM";

		CHECK((info.kind == StageProbe::Kind::fm) == fm);
		CHECK((info.kind == StageProbe::Kind::rhythm) == rhythm);
		CHECK(info.modules != 0);

		for (StageSwitch::Mask off : { (StageSwitch::Mask)0, StageSwitch::all })
		{
			INFO("切ったまとまり: ", (int)off);

			auto options = shortOptions();
			options.off = off;

			auto job = StageWaveRender::prepare(plugin, options);

			REQUIRE(job != nullptr);

			const auto result = StageWaveRender::run(*job);

			REQUIRE_FALSE(result.isEmpty());

			// 1 小節 (1 秒) と余韻の上限を足した長さより短く終わる。EG を切っても鳴り続けない
			CHECK(result.stages[StageWaveRender::stageFx].lengthSeconds() < 1.0 + StageWaveRender::maxTailSeconds);

			CHECK(result.stageOn[StageWaveRender::stageRaw] == fm);
			CHECK(result.stageOn[StageWaveRender::stageCore]);
			CHECK(result.stageOn[StageWaveRender::stageFx]);

			// 素材を読ませていないチャンネルは鳴らないので、波形の中身は見ない
			if (isSilentByDefault(name)) continue;

			CHECK(peakOf(result.stages[StageWaveRender::stageCore].left) > 1.0e-3f);
			CHECK(peakOf(result.stages[StageWaveRender::stageFx].left) > 1.0e-3f);

			if (fm)
			{
				REQUIRE(info.opCount > 0);

				float row1 = 0.0f, row2 = 0.0f;

				for (int i = 0; i < info.opCount; ++i)
				{
					CHECK_FALSE(result.row1[(size_t)i].isEmpty());
					CHECK_FALSE(result.row2[(size_t)i].isEmpty());

					row1 = std::max(row1, peakOf(result.row1[(size_t)i].left));
					row2 = std::max(row2, peakOf(result.row2[(size_t)i].left));
				}

				CHECK(row1 > 1.0e-3f);
				CHECK(row2 > 1.0e-3f);
			}
			else
			{
				CHECK(peakOf(result.row1[0].left) > 1.0e-3f);
				CHECK(result.row2[0].isEmpty());
			}
		}

		++checked;
	}

	CHECK(checked > 0);
}

TEST_CASE("生成中の波形: エディタのボタンから別の窓を開け閉めする")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	auto* launcher = findFirst<GuiGenWave>(*editor);

	REQUIRE(launcher != nullptr);
	CHECK_FALSE(launcher->isWindowOpen());

	launcher->setWindowOpen(true);

	REQUIRE(launcher->isWindowOpen());
	REQUIRE(launcher->stageWave() != nullptr);

	// 画面の値から作る条件を組む。設定ファイルの値 (既定は C3・120 BPM) が入る
	const auto options = launcher->stageWave()->currentOptions();

	CHECK(options.pattern.midiNote == 60);
	CHECK(options.pattern.bpm == doctest::Approx(120.0));
	CHECK(options.off == 0);

	// 間隔・音符の初期値はどちらも 4 分音符で、3 連・付点は切
	CHECK(options.pattern.interval.division == 4);
	CHECK_FALSE(options.pattern.interval.triplet);
	CHECK_FALSE(options.pattern.interval.dotted);
	CHECK(options.pattern.length.division == 4);
	CHECK_FALSE(options.pattern.length.triplet);
	CHECK_FALSE(options.pattern.length.dotted);

	// 枠は窓に合わせて伸び縮みし、最初の大きさの縦横半分より小さくはならない
	{
		auto& stage = *launcher->stageWave();

		// 窓の最小は最初の大きさの縦横半分
		auto* window = stage.findParentComponentOfClass<juce::ResizableWindow>();

		REQUIRE(window != nullptr);
		CHECK(window->getConstrainer()->getMinimumWidth() == GuiStageWave::contentWidth / 2);
		CHECK(window->getConstrainer()->getMinimumHeight() == GuiStageWave::contentHeight / 2);

		CHECK(stage.currentViewWidth() == GuiStageWave::viewWidth);
		CHECK(stage.currentViewHeight() == GuiStageWave::viewHeight);
		CHECK_FALSE(stage.isControlScrollShown());

		stage.setSize(GuiStageWave::contentWidth + StageWaveRender::viewsPerRow * 50, GuiStageWave::contentHeight + 3 * 40);

		CHECK(stage.currentViewWidth() == GuiStageWave::viewWidth + 50);
		CHECK(stage.currentViewHeight() == GuiStageWave::viewHeight + 40);

		stage.setSize(100, 100);

		CHECK(stage.currentViewWidth() == GuiStageWave::minViewWidth);
		CHECK(stage.currentViewHeight() == GuiStageWave::minViewHeight);

		// 操作の段が収まらなければ横のスクロールバーが出て、操作の段だけがずれる
		stage.setSize(stage.minimumWidth(), stage.minimumHeight(stage.minimumWidth()));

		REQUIRE(stage.isControlScrollShown());
		CHECK(stage.controlsWidth() > stage.getWidth());

		const int playerRight = stage.getWidth();
		const auto before = stage.getChildComponent(0)->getX();

		stage.setControlScroll(100);

		CHECK(stage.controlScroll() == 100);
		CHECK(stage.getChildComponent(0)->getX() == before - 100);
		CHECK(stage.getWidth() == playerRight);

		// ずらしすぎは詰める
		stage.setControlScroll(100000);

		CHECK(stage.controlScroll() == stage.controlsWidth() - stage.getWidth());

		// 広げて収まれば、スクロールバーは消えて、ずらしも戻る
		stage.setSize(GuiStageWave::contentWidth, GuiStageWave::contentHeight);

		CHECK_FALSE(stage.isControlScrollShown());
		CHECK(stage.controlScroll() == 0);

		// 見た目を確かめたいときは、置き場を環境変数で渡すと窓の中身を PNG に書き出す
		if (const auto dir = juce::SystemStats::getEnvironmentVariable("STAGE_WAVE_SNAPSHOT_DIR", {}); dir.isNotEmpty())
		{
			auto* scroller = window->getContentComponent();

			REQUIRE(scroller != nullptr);

			const std::array<juce::Point<int>, 3> sizes{ {
				{ GuiStageWave::contentWidth, GuiStageWave::contentHeight },
				{ 1200, 520 },
				{ GuiStageWave::minWindowWidth, GuiStageWave::minWindowHeight },
			} };

			for (const auto& size : sizes)
			{
				scroller->setSize(size.x, size.y);

				const auto image = scroller->createComponentSnapshot(scroller->getLocalBounds());
				auto file = juce::File(dir).getChildFile("stage_" + juce::String(size.x) + "x" + juce::String(size.y) + ".png");

				file.deleteFile();

				juce::FileOutputStream out(file);
				juce::PNGImageFormat().writeImageToStream(image, out);
			}
		}
	}

	launcher->setWindowOpen(false);

	CHECK_FALSE(launcher->isWindowOpen());

	editor.reset();
}
