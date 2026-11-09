#pragma once

#include <array>
#include <memory>
#include <vector>

#include <JuceHeader.h>

#include "Shared/Core/Gui/GuiHost.h"
#include "Shared/Gui/Components/GenWave/GenWaveRender.h"

// ============================================================================
// 生成中の波形の計算 (4.4.0)
// ============================================================================
// 打鍵の並び (テンポ・小節数・間隔・音符) どおりに鍵を押して音源を回し、
// 音源の中の途中の音を段ごとに拾う。計算するだけで、音は出さない
// (GenWaveRender と同じく、出荷される音源のクラスをそのまま回す)。
//
// 音源一式はメッセージスレッドで組み立て (prepare)、回すのは別のスレッドで行う (run)。
namespace StageWaveRender
{
	inline constexpr double renderRate = 44100.0;

	// 貯めるときは 1 つおきに間引く (22.05kHz)。30 秒を 21 枠ぶん持っても重くならないように
	inline constexpr int storeStep = 2;

	// 打鍵の並びの長さの上限。超えたぶんは打ち切る
	inline constexpr double maxPatternSeconds = 30.0;

	// 最後に離してから、音が消えてここまで見届けて打ち切る
	inline constexpr double tailSeconds = 0.5;

	// 余韻を待つ上限 (鳴り続ける音でも、ここで打ち切る)
	inline constexpr double maxTailSeconds = 10.0;

	inline constexpr int blockSize = 512;

	// 「鳴っていない」とみなす大きさ
	inline constexpr float silenceLevel = 1.0e-4f;

	// 枠の数 (1 段あたり)
	inline constexpr int viewsPerRow = 8;

	// 第 3 段の段
	enum Stage { stageRaw = 0, stageCore, stageUnison, stageDac, stageFx, stageCount };

	// 音符の長さ。division は 1 = 全音符, 2, 4, 8, 16, 32
	struct NoteValue
	{
		int division = 4;
		bool triplet = false;
		bool dotted = false;

		// 4 分音符を 1 とした長さ
		double quarters() const
		{
			double q = 4.0 / (double)juce::jmax(1, division);

			if (triplet) q *= 2.0 / 3.0;
			if (dotted) q *= 1.5;

			return q;
		}
	};

	inline constexpr std::array<int, 6> divisions{ 1, 2, 4, 8, 16, 32 };

	struct Pattern
	{
		double bpm = 120.0;
		int bars = 1;
		int sigNumerator = 4;
		int sigDenominator = 4;
		NoteValue interval{ 4, false, false };
		NoteValue length{ 4, false, false };

		// 鳴らす鍵 (RHYTHM 以外)
		int midiNote = 60;
	};

	struct Options
	{
		Pattern pattern;

		// 切るまとまり (StageSwitch::Module)
		StageSwitch::Mask off = 0;

		// RHYTHM の第 3 段に混ぜるパッド
		std::array<bool, StageProbe::maxOps> padChecked{ true, true, true, true, true, true, true, true };

		// 最後に離したあとの余韻 (リリース) まで作るか。切ると打鍵の並びのちょうどの長さで止める
		bool releaseTail = true;
	};

	// 押す・離すの時刻 (サンプル)
	struct Event
	{
		int at = 0;
		int note = 0;
		bool on = false;
	};

	// 打鍵の並びの長さ (秒)。小節数 × 1 小節。上限 (maxPatternSeconds) で切る
	double patternSeconds(const Pattern& pattern);

	// 打鍵の並びから、押す・離すの一覧を作る。truncated には上限で切ったかを返す
	std::vector<Event> buildEvents(const Pattern& pattern, const std::vector<int>& notes, double sampleRate, bool& truncated);

	struct Result
	{
		GuiStageInfo info;

		// 第 1 段・第 2 段 (FM はオペレーター、RHYTHM はパッド、その他は第 1 段の 1 枠だけ)
		std::array<GenWaveRender::Wave, viewsPerRow> row1;
		std::array<GenWaveRender::Wave, viewsPerRow> row2;

		// 第 3 段。① ② は真ん中だけ、③ から L / R を持つ
		std::array<GenWaveRender::Wave, stageCount> stages;

		// 段を使ったか (使わない段は枠を残して OFF と出す)
		std::array<bool, stageCount> stageOn{};

		// 打鍵の並びを上限で切ったか
		bool truncated = false;

		bool isEmpty() const { return stages[stageFx].isEmpty(); }
	};

	// 回す前の支度。メッセージスレッドで呼ぶ (音源一式をプロセッサから組み立てるため)
	struct Job
	{
		Options options;
		GuiStageInfo info;

		// 通常の音 (第 2 段・第 3 段)。RHYTHM はチェックしたパッドだけを鳴らす
		std::unique_ptr<GuiStageRig> main;

		// FM の変調を入れない音 (第 1 段)。RHYTHM は全パッドを鳴らす音
		std::unique_ptr<GuiStageRig> extra;
	};

	// 音源を持たないプラグインでは nullptr を返す
	std::shared_ptr<Job> prepare(GuiProcessorHost& host, const Options& options);

	// 回す。時間が掛かるので、メッセージスレッドから直に呼ばないこと
	Result run(Job& job);

	// 様子だけを見る (どの種類の音源か・持っているまとまり)。メッセージスレッドで呼ぶ
	GuiStageInfo inspect(GuiProcessorHost& host);

	// 設定から打鍵の並びを作る。followDaw なら DAW のテンポ・拍子を使う (分からなければ設定の値)
	Pattern patternFrom(const GuiProcessorHost& host, bool followDaw);

	// ---- ファイルブラウザーの行のプレビュー (4.4.1)
	// ブラウザーの設定 (browserBpm など) から打鍵の並びを作る。拍子は 4/4。
	// DAW 同期なら DAW のテンポ・拍子を使う (分からなければ設定の値)
	Pattern browserPattern(const GuiProcessorHost& host);

	// 打鍵の並びの符号。貯めておくファイルの名前に付け、並びが違えば別物として扱う
	juce::String patternKey(const Pattern& pattern);

	// 打鍵の並びどおりに鳴らした出来上がり (L / R) を、GenWaveRender::storeRate へ間引いて返す。
	// 最後に離したあとは、音が消えて tailSeconds たつまで (長くても maxTailSeconds) 見届ける。
	// 時間が掛かるので、メッセージスレッドから直に呼ばないこと
	GenWaveRender::Wave renderPattern(GuiRenderRig& rig, const Pattern& pattern);
}
