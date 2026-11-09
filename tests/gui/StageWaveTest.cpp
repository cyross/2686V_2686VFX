// ============================================================================
// 生成中の波形の窓 (4.4.0)
// ============================================================================
//   ・打鍵の並び (テンポ・小節数・間隔・音符) どおりに押す・離すが並ぶ
//   ・エンベロープ・LFO などのまとまりを、計算の写しの上だけで切れる
//   ・どのチャンネルでも段ごとの波形が出来る (FM はオペレーター、RHYTHM はパッド)
//   ・エディタのボタンから別の窓を開け閉めできる
//   ・パラメータを触り終えて待ち時間がたったら自動で作る。作っているあいだはローディング画面 (4.4.1)
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
#include "Shared/Gui/Components/Loading/GuiLoading.h"
#include "Shared/Gui/Components/ParamBrowser/ParamBrowser.h"
#include "Shared/Core/Editor/EditorStageWaveText.h"
#include "Shared/Core/Editor/EditorParamBrowserText.h"
#include "Shared/Core/Gui/GuiBase.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Gui/Settings/GuiSettingsText.h"

#include "./EditorTestAccess.h"
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

	// どれか 1 つのパラメータを動かす (どのプラグインにもある、いちばん頭のもの)
	void nudgeParam(juce::AudioProcessor& p)
	{
		auto* param = p.getParameters()[0];
		REQUIRE(param != nullptr);
		param->setValueNotifyingHost(param->getValue() < 0.5f ? 1.0f : 0.0f);
	}

	template <typename T>
	void findAllInto(juce::Component& root, const std::function<bool(T&)>& match, std::vector<T*>& out)
	{
		for (auto* child : root.getChildren())
		{
			if (auto* t = dynamic_cast<T*>(child); t != nullptr && match(*t)) out.push_back(t);

			findAllInto<T>(*child, match, out);
		}

		// 裏のタブ (SETTINGS など) の中身は、表に出るまで子に入っていない
		if (auto* tabs = dynamic_cast<juce::TabbedComponent*>(&root))
		{
			for (int i = 0; i < tabs->getNumTabs(); ++i)
				if (auto* content = tabs->getTabContentComponent(i); content != nullptr && content->getParentComponent() != &root)
					findAllInto<T>(*content, match, out);
		}
	}

	template <typename T>
	std::vector<T*> findAll(juce::Component& root, const std::function<bool(T&)>& match)
	{
		std::vector<T*> out;

		findAllInto<T>(root, match, out);

		return out;
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

TEST_CASE("生成中の波形: 触り終えて待ち時間がたったら自動で作り、作っているあいだはローディング画面で覆う (4.4.1)")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	auto& host = *processor;

	// 既定は自動生成が入りで、待ち時間は 1 秒
	CHECK(host.stageAutoGenerate);
	CHECK(host.stageAutoInterval == doctest::Approx(1.0f));

	processor->prepareToPlay(48000.0, 512);

	// 軽く済ませるための短い並び
	host.stageBpm = 240.0f;
	host.stageBars = 1;

	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	auto* launcher = findFirst<GuiGenWave>(*editor);

	REQUIRE(launcher != nullptr);

	// 窓を閉じているあいだも、触った履歴は溜まる
	const auto before = host.paramTouches.count();

	nudgeParam(host);

	CHECK(host.paramTouches.count() > before);

	// 切っていれば、開いても作らない
	host.stageAutoGenerate = false;

	launcher->setWindowOpen(true);

	REQUIRE(launcher->stageWave() != nullptr);

	auto& stage = *launcher->stageWave();

	CHECK_FALSE(stage.isBusy());
	CHECK_FALSE(stage.autoGenerateDue(juce::Time::getMillisecondCounterHiRes(), true));

	// 入れれば、触った履歴があるので開いた瞬間なら待たずに作る頃合い
	host.stageAutoGenerate = true;

	const double last = host.paramTouches.lastMs();

	CHECK(stage.autoGenerateDue(last, true));

	// 開いた瞬間でなければ、最後に触ってから待ち時間 (0.1〜5.0 秒) がたつまで待つ
	host.stageAutoInterval = 1.0f;

	CHECK_FALSE(stage.autoGenerateDue(last + 500.0, false));
	CHECK(stage.autoGenerateDue(last + 1000.0, false));

	host.stageAutoInterval = 0.1f;

	CHECK(stage.autoGenerateDue(last + 101.0, false));

	// 範囲の外の値は 0.1〜5.0 秒に収める
	host.stageAutoInterval = 60.0f;

	CHECK(stage.autoGenerateDue(last + 5000.0, false));

	host.stageAutoInterval = 1.0f;

	// 作り始めると、作っているあいだはローディング画面で覆い、重ねて作らない
	auto* window = stage.findParentComponentOfClass<juce::ResizableWindow>();

	REQUIRE(window != nullptr);

	const auto loadings = [window] {
		return findAll<GuiLoading>(*window, [](GuiLoading& l) { return l.isVisible(); });
	};

	CHECK(loadings().empty());

	stage.requestGenerate();

	REQUIRE(stage.isBusy());
	CHECK(loadings().size() == 1);
	CHECK_FALSE(stage.autoGenerateDue(last + 10000.0, false));

	// 窓の見えているところいっぱいを覆う
	if (auto* scroller = window->getContentComponent(); scroller != nullptr && !loadings().empty())
		CHECK(loadings()[0]->getBounds() == scroller->getLocalBounds());

	// 作り終えたら覆いを外す。作り始めたあとに触っていなければ、作り直さない
	stage.finishGenerate({});

	CHECK_FALSE(stage.isBusy());
	CHECK(loadings().empty());
	CHECK_FALSE(stage.autoGenerateDue(host.paramTouches.lastMs() + 10000.0, false));

	// 触れば、また待ち時間のあとに作る
	nudgeParam(host);

	CHECK(stage.autoGenerateDue(host.paramTouches.lastMs() + 1000.0, false));

	// 窓の打鍵の並びを触っても履歴に載る
	const auto touched = host.paramTouches.count();
	auto boxes = findAll<juce::ComboBox>(stage, [](juce::ComboBox& b) { return b.getNumItems() == 16 && b.getItemText(15) == "16"; });

	REQUIRE(boxes.size() == 1);

	boxes[0]->setSelectedId(2, juce::sendNotificationSync);

	CHECK(host.stageBars == 2);
	CHECK(host.paramTouches.count() > touched);

	// 作ること自体はパラメータを触らない (触ると、作り終えるたびに作り直してしまう)
	{
		const auto count = host.paramTouches.count();
		auto job = StageWaveRender::prepare(host, stage.currentOptions());

		REQUIRE(job != nullptr);

		StageWaveRender::run(*job);

		CHECK(host.paramTouches.count() == count);
	}

	launcher->setWindowOpen(false);

	editor.reset();
}

TEST_CASE("SETTINGS: 生成過程プレビューの自動生成は入り切りでき、切ると待ち時間は選べない (4.4.1)")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	EditorTestAccess::materializeAll(*static_cast<AudioPlugin2686VEditor*>(editor.get()));

	auto toggles = findAll<GuiToggleButton>(*editor, [](GuiToggleButton& t) { return t.getButtonText() == SettingsGuiText::stageAutoGenerate.get(); });

	REQUIRE(toggles.size() == 1);

	auto* toggle = toggles[0];
	auto* page = toggle->getParentComponent();
	auto* settings = dynamic_cast<GuiBase*>(page);

	REQUIRE(settings != nullptr);

	settings->layout(page->getLocalBounds());

	// 同じ行の右に待ち時間
	auto sliders = findAll<GuiSlider>(*page, [toggle](GuiSlider& s) { return s.isVisible() && s.getY() == toggle->getY() && s.getX() >= toggle->getRight(); });

	REQUIRE(sliders.size() == 1);

	auto* interval = sliders[0];

	CHECK(interval->getRight() <= page->getWidth());
	CHECK(interval->getBottom() <= page->getHeight());

	// 0.1〜5.0 秒、初期値は入・1.0 秒
	CHECK(interval->getMinimum() == doctest::Approx(0.1));
	CHECK(interval->getMaximum() == doctest::Approx(5.0));
	CHECK(interval->getValue() == doctest::Approx(1.0));
	CHECK(toggle->getToggleState());
	CHECK(interval->isEnabled());

	// 頁のいちばん下の部品まで、頁からはみ出さない
	int bottom = 0;

	for (auto* child : page->getChildren())
		if (child->isVisible()) bottom = juce::jmax(bottom, child->getBottom());

	CHECK(bottom <= page->getHeight());

	// 動かすとプロセッサの値が変わる
	interval->setValue(2.5, juce::sendNotificationSync);

	CHECK(processor->stageAutoInterval == doctest::Approx(2.5f));

	// 切ると待ち時間は選べない
	toggle->setToggleState(false, juce::dontSendNotification);

	REQUIRE(toggle->onClick != nullptr);

	toggle->onClick();

	CHECK_FALSE(processor->stageAutoGenerate);
	CHECK_FALSE(interval->isEnabled());

	editor.reset();
}

TEST_CASE("生成中の波形: 起動して最初に開いたときは、パラメータが初期値のままでも作る (4.4.1)")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	auto& host = *processor;

	processor->prepareToPlay(48000.0, 512);

	host.stageBpm = 240.0f;
	host.stageBars = 1;

	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	auto* launcher = findFirst<GuiGenWave>(*editor);

	REQUIRE(launcher != nullptr);
	REQUIRE(host.stageAutoGenerate);
	CHECK_FALSE(host.stageOpenedOnce);

	// 触っていなくても、最初に開いたら作り始める
	const auto touches = host.paramTouches.count();

	launcher->setWindowOpen(true);

	REQUIRE(launcher->stageWave() != nullptr);
	CHECK(launcher->stageWave()->isBusy());
	CHECK(host.stageOpenedOnce);

	// 2 度目からは、触った履歴が無ければ作らない
	launcher->setWindowOpen(false);

	if (host.paramTouches.count() == touches)
	{
		launcher->setWindowOpen(true);

		REQUIRE(launcher->stageWave() != nullptr);
		CHECK_FALSE(launcher->stageWave()->isBusy());

		launcher->setWindowOpen(false);
	}

	editor.reset();
}

TEST_CASE("生成中の波形: 再生の段は「加工」の段の下にある (4.4.1)")
{
	auto processor = std::make_unique<AudioPlugin2686V>();

	processor->stageAutoGenerate = false;

	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	auto* launcher = findFirst<GuiGenWave>(*editor);

	REQUIRE(launcher != nullptr);

	launcher->setWindowOpen(true);

	auto& stage = *launcher->stageWave();

	// 「加工」の段のトグル (見えているもの) のいちばん下より、再生の段 (枠の段の上) が下にある。
	// 再生の段は描くだけの部品なので、枠の段の上端との間で見る
	int switchesBottom = 0;

	// 操作の段のトグル (段の高さのもの) だけを見る。RHYTHM のパッドは 3 段目の枠の見出しに置くので数えない
	for (auto* child : stage.getChildren())
		if (auto* t = dynamic_cast<juce::ToggleButton*>(child); t != nullptr && t->isVisible() && t->getHeight() == GuiStageWave::controlHeight)
			switchesBottom = juce::jmax(switchesBottom, t->getBottom());

	// 操作の 3 段・横線 1 本・(出ていれば横のスクロールバー) の下端。再生の段はその下の横線のさらに下
	const int viewsTop = GuiStageWave::margin + GuiStageWave::controlHeight * 3 + GuiStageWave::separatorGap
		+ GuiStageWave::viewGap + GuiStageWave::viewGap / 2 + GuiStageWave::controlScrollHeight;

	CHECK(switchesBottom <= viewsTop);

	// 見えている部品は、どれも再生の段 (操作の 3 段の下) より上
	for (auto* child : stage.getChildren())
		if (child->isVisible() && dynamic_cast<juce::ScrollBar*>(child) == nullptr && child->getHeight() == GuiStageWave::controlHeight)
			CHECK(child->getBottom() <= viewsTop);

	// 見た目を確かめたいときは、置き場を環境変数で渡すと窓の中身を PNG に書き出す
	if (const auto dir = juce::SystemStats::getEnvironmentVariable("STAGE_WAVE_SNAPSHOT_DIR", {}); dir.isNotEmpty())
	{
		auto* window = stage.findParentComponentOfClass<juce::ResizableWindow>();
		auto* scroller = window->getContentComponent();
		const auto image = scroller->createComponentSnapshot(scroller->getLocalBounds());
		auto file = juce::File(dir).getChildFile("stage_441.png");

		file.deleteFile();

		juce::FileOutputStream out(file);
		juce::PNGImageFormat().writeImageToStream(image, out);
	}

	launcher->setWindowOpen(false);

	editor.reset();
}

TEST_CASE("ファイルブラウザー: 行のプレビューは打鍵の並びどおりに作り、並びごとに別に貯める (4.4.1)")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	auto& host = *processor;

	processor->prepareToPlay(48000.0, 512);

	// 既定は 120 BPM・1 小節・4 分音符ごとに 4 分音符・C3、生成周期 2・x1・30 FPS、自動生成は切
	CHECK(host.browserBpm == doctest::Approx(120.0f));
	CHECK(host.browserBars == 1);
	CHECK(host.browserIntervalDiv == 4);
	CHECK_FALSE(host.browserIntervalTriplet);
	CHECK_FALSE(host.browserIntervalDotted);
	CHECK(host.browserLengthDiv == 4);
	CHECK_FALSE(host.browserLengthTriplet);
	CHECK_FALSE(host.browserLengthDotted);
	CHECK(host.browserMidiNote == 60);
	CHECK(host.browserZoom == 1);
	CHECK(host.browserFps == 30);
	CHECK_FALSE(host.browserAutoGenerate);

	auto pattern = StageWaveRender::browserPattern(host);

	CHECK(pattern.midiNote == 60);
	CHECK(pattern.sigNumerator == 4);
	CHECK(pattern.sigDenominator == 4);

	// 240 BPM・1 小節なら 1 秒 + 余韻
	host.browserBpm = 240.0f;
	pattern = StageWaveRender::browserPattern(host);

	auto rig = host.createRenderRig(GenWaveRender::renderRate);

	REQUIRE(rig != nullptr);

	const auto wave = StageWaveRender::renderPattern(*rig, pattern);

	CHECK(wave.noteHz == doctest::Approx(juce::MidiMessage::getMidiNoteInHertz(60)));
	CHECK(wave.lengthSeconds() >= 1.0);
	CHECK(wave.lengthSeconds() <= 1.0 + StageWaveRender::maxTailSeconds + 0.1);
	CHECK(wave.left.size() == wave.right.size());

	// 間引いた速さどおりに記す (44.1kHz を 3 つおき)
	CHECK(wave.sampleRate == doctest::Approx(GenWaveRender::renderRate / 3.0));

	// 並びが変われば、貯める先も変わる
	const auto dir = juce::File::getSpecialLocation(juce::File::tempDirectory);
	const auto source = dir.getChildFile("x.param.opna.json");
	const auto before = GenWaveRender::cacheFileFor(dir, source, StageWaveRender::patternKey(pattern));

	host.browserMidiNote = 62;

	CHECK(GenWaveRender::cacheFileFor(dir, source, StageWaveRender::patternKey(StageWaveRender::browserPattern(host))) != before);

	host.browserMidiNote = 60;
	host.browserIntervalTriplet = true;

	CHECK(GenWaveRender::cacheFileFor(dir, source, StageWaveRender::patternKey(StageWaveRender::browserPattern(host))) != before);
}

TEST_CASE("ファイルブラウザー: 作り方・見せ方の行は「フォルダ…」の行の下に 1 行で並び、自動生成は欠けているときだけ (4.4.1)")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	auto& host = *processor;

	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	auto* editorHost = dynamic_cast<GuiEditorHost*>(editor.get());

	REQUIRE(editorHost != nullptr);

	GuiContext context(host, *editorHost, host.apvts);

	// パラメータファイルを 1 件置いたフォルダ (中身は空。作るのには使わない)
	juce::TemporaryFile tempDir;
	const auto root = tempDir.getFile();

	root.createDirectory();
	root.getChildFile("tone.param.opna.json").replaceWithText("{}");

	GuiParamBrowser browser(context);

	GuiParamBrowser::Request request;

	request.root = root;

	host.browserAutoGenerate = false;

	browser.open(*editor, request);

	CHECK_FALSE(browser.needsAutoGenerate());

	// 入れれば、貯めてあるものが無いので作る
	host.browserAutoGenerate = true;

	CHECK(browser.needsAutoGenerate());

	host.browserAutoGenerate = false;

	// 行の部品: テンポ・DAW 同期・小節・キーオン周期・音符長・キー・生成周期・表示倍率・FPS
	auto zooms = findAll<juce::ComboBox>(browser, [](juce::ComboBox& b) { return b.getNumItems() == 8 && b.getItemText(0) == "x1"; });
	auto fpss = findAll<juce::ComboBox>(browser, [](juce::ComboBox& b) { return b.getNumItems() == 4 && b.getItemText(0) == "15" && b.getItemText(3) == "120"; });
	auto cycles = findAll<juce::ComboBox>(browser, [](juce::ComboBox& b) { return b.getNumItems() == 4 && b.getItemText(0) == "1" && b.getItemText(3) == "10"; });
	auto folders = findAll<juce::TextButton>(browser, [](juce::TextButton& b) { return b.getButtonText() == EditorGuiText::ParamBrowser::folder.get(); });

	REQUIRE(zooms.size() == 1);
	REQUIRE(fpss.size() == 1);
	REQUIRE(cycles.size() == 1);
	REQUIRE(folders.size() == 1);

	const int rowY = zooms[0]->getY();

	CHECK(rowY > folders[0]->getBottom());
	CHECK(fpss[0]->getY() == rowY);
	CHECK(cycles[0]->getY() == rowY);
	CHECK(cycles[0]->getRight() <= zooms[0]->getX());
	CHECK(zooms[0]->getRight() <= fpss[0]->getX());
	CHECK(fpss[0]->getRight() <= browser.getWidth());

	// 初期値 (生成周期 2・x1・30 FPS)
	CHECK(cycles[0]->getText() == "2");
	CHECK(zooms[0]->getText() == "x1");
	CHECK(fpss[0]->getText() == "30");

	// 選ぶとプロセッサの値が変わる
	zooms[0]->setSelectedId(3, juce::sendNotificationSync);
	fpss[0]->setSelectedId(60, juce::sendNotificationSync);

	CHECK(host.browserZoom == 3);
	CHECK(host.browserFps == 60);

	// 見た目を確かめたいときは、置き場を環境変数で渡すと PNG に書き出す
	if (const auto dir = juce::SystemStats::getEnvironmentVariable("STAGE_WAVE_SNAPSHOT_DIR", {}); dir.isNotEmpty())
	{
		const auto image = browser.createComponentSnapshot(browser.getLocalBounds());
		auto file = juce::File(dir).getChildFile("browser_441.png");

		file.deleteFile();

		juce::FileOutputStream out(file);
		juce::PNGImageFormat().writeImageToStream(image, out);
	}

	browser.close();

	root.deleteRecursively();

	editor.reset();
}

TEST_CASE("ファイルブラウザー: キーオン周期・音符長に全音符 × 2・× 4・× 8 があり、3 連符・付点は切 (4.4.1)")
{
	// 負の値は全音符の何倍か
	CHECK(StageWaveRender::NoteValue{ -2, false, false }.quarters() == doctest::Approx(8.0));
	CHECK(StageWaveRender::NoteValue{ -4, false, false }.quarters() == doctest::Approx(16.0));
	CHECK(StageWaveRender::NoteValue{ -8, false, false }.quarters() == doctest::Approx(32.0));
	CHECK(StageWaveRender::NoteValue{ 1, false, false }.quarters() == doctest::Approx(4.0));

	auto processor = std::make_unique<AudioPlugin2686V>();
	auto& host = *processor;

	CHECK_FALSE(host.browserIntervalTriplet);
	CHECK_FALSE(host.browserIntervalDotted);
	CHECK_FALSE(host.browserLengthTriplet);
	CHECK_FALSE(host.browserLengthDotted);

	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	auto* editorHost = dynamic_cast<GuiEditorHost*>(editor.get());

	REQUIRE(editorHost != nullptr);

	GuiContext context(host, *editorHost, host.apvts);

	juce::TemporaryFile tempDir;
	const auto root = tempDir.getFile();

	root.createDirectory();

	GuiParamBrowser browser(context);
	GuiParamBrowser::Request request;

	request.root = root;
	browser.open(*editor, request);

	// 音符の選択肢は 2 つ (キーオン周期・音符長)。どちらも全音符 × 8 から 32 分音符まで 9 つ
	auto notes = findAll<juce::ComboBox>(browser, [](juce::ComboBox& b) { return b.getNumItems() == 9; });

	REQUIRE(notes.size() == 2);

	for (auto* box : notes)
	{
		CHECK(box->getItemText(0).endsWith("8"));
		CHECK(box->getItemText(1).endsWith("4"));
		CHECK(box->getItemText(2).endsWith("2"));
		CHECK(box->getItemText(3) == EditorGuiText::StageWave::whole.get());
		CHECK(box->getSelectedItemIndex() == 5);	// 4 分音符
	}

	// 全音符 × 8 を選ぶと、プロセッサの値は -8
	notes[0]->setSelectedItemIndex(0, juce::sendNotificationSync);

	CHECK((host.browserIntervalDiv == -8 || host.browserLengthDiv == -8));

	// 3 連符・付点のトグルは切
	int checked = 0;

	for (auto* t : findAll<juce::ToggleButton>(browser, [](juce::ToggleButton& b) {
		return b.getButtonText() == EditorGuiText::StageWave::triplet.get() || b.getButtonText() == EditorGuiText::StageWave::dotted.get(); }))
	{
		CHECK_FALSE(t->getToggleState());
		++checked;
	}

	CHECK(checked == 4);

	browser.close();
	root.deleteRecursively();
	editor.reset();
}

TEST_CASE("ファイルブラウザー: 「すべて停止」は ■ / ▶ の記号で、その左に「すべて先頭へ」(<<) がある (4.4.1)")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	auto& host = *processor;

	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	auto* editorHost = dynamic_cast<GuiEditorHost*>(editor.get());

	REQUIRE(editorHost != nullptr);

	GuiContext context(host, *editorHost, host.apvts);

	juce::TemporaryFile tempDir;
	const auto root = tempDir.getFile();

	root.createDirectory();

	GuiParamBrowser browser(context);
	GuiParamBrowser::Request request;

	request.root = root;
	browser.open(*editor, request);

	auto pauses = findAll<juce::TextButton>(browser, [](juce::TextButton& b) { return b.getButtonText() == EditorGuiText::ParamBrowser::pauseAllIcon.get(); });
	auto rewinds = findAll<juce::TextButton>(browser, [](juce::TextButton& b) { return b.getButtonText() == EditorGuiText::ParamBrowser::rewindAllIcon.get(); });

	REQUIRE(pauses.size() == 1);
	REQUIRE(rewinds.size() == 1);

	// 言葉は吹き出しに
	CHECK(pauses[0]->getTooltip() == EditorGuiText::ParamBrowser::pauseAll.get());
	CHECK(rewinds[0]->getTooltip() == EditorGuiText::ParamBrowser::rewindAll.get());

	// 同じ行で、<< が ■ の左
	CHECK(rewinds[0]->getY() == pauses[0]->getY());
	CHECK(rewinds[0]->getRight() < pauses[0]->getX());

	// 押すと ▶ に変わり、もう一度で ■ に戻る
	pauses[0]->onClick();

	CHECK(pauses[0]->getButtonText() == EditorGuiText::ParamBrowser::playAllIcon.get());

	pauses[0]->onClick();

	CHECK(pauses[0]->getButtonText() == EditorGuiText::ParamBrowser::pauseAllIcon.get());

	// 行が無くても押せる
	rewinds[0]->onClick();

	browser.close();
	root.deleteRecursively();
	editor.reset();
}

TEST_CASE("生成過程プレビュー: 開くボタンの見出しの上に横の区切り線の余裕がある (4.4.1)")
{
	CHECK(GuiGenWave::separatorGap == GuiGenWave::separatorPad * 2 + GuiGenWave::separatorHeight);
	CHECK(GuiGenWave::totalHeight == GuiGenWave::separatorGap + GuiGenWave::labelHeight + GuiGenWave::gap + GuiGenWave::buttonHeight);

	auto processor = std::make_unique<AudioPlugin2686V>();

	processor->stageAutoGenerate = false;

	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	auto* launcher = findFirst<GuiGenWave>(*editor);

	REQUIRE(launcher != nullptr);

	launcher->setBounds(0, 0, 300, GuiGenWave::totalHeight);

	// 見出し (と開くボタン) は区切り線の下
	for (auto* child : launcher->getChildren())
		CHECK(child->getY() >= GuiGenWave::separatorGap);

	// 窓のキーオン周期の見出し
	launcher->setWindowOpen(true);

	auto labels = findAll<juce::Label>(*launcher->stageWave(), [](juce::Label& l) { return l.getText() == EditorGuiText::StageWave::interval.get(); });

	REQUIRE(labels.size() == 1);

	launcher->setWindowOpen(false);

	if (const auto dir = juce::SystemStats::getEnvironmentVariable("STAGE_WAVE_SNAPSHOT_DIR", {}); dir.isNotEmpty())
	{
		// 部品は背景を描かないので、エディタの地の色の上に描く
		juce::Image image(juce::Image::ARGB, launcher->getWidth(), launcher->getHeight(), true);
		{
			juce::Graphics g(image);
			g.fillAll(juce::Colour(0xff202020));
			launcher->paintEntireComponent(g, true);
		}
		auto file = juce::File(dir).getChildFile("launcher_441.png");

		file.deleteFile();

		juce::FileOutputStream out(file);
		juce::PNGImageFormat().writeImageToStream(image, out);
	}

	editor.reset();
}
