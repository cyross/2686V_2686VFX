#pragma once

#include <JuceHeader.h>

#include <array>
#include <bitset>

#include "../../Core/Processor/ProcessorHelper.h"
#include "../../Core/Processor/ProcessorStructs.h"

#include "../../Effect/Envelope/Amp/Adsr/EnvAmpAdsr.h"
#include "../../Effect/Envelope/Amp/SsgHw/EnvSsgHw.h"
#include "../../Effect/Envelope/Amp/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Envelope/Pitch/Adsr/EnvPirchAdsr.h"
#include "../../Effect/Envelope/Pitch/SsgSw11/EnvSsgSw11.h"
#include "../../Effect/Envelope/Pitch/SsgHw/EnvSsgHw.h"
#include "../../Effect/Lfo/Opzx7/LfoOpzx7.h"
#include "../../Effect/Detune/Opzx7/DetuneOpzx7.h"
#include "Shared/Generator/WtMod/GenWtModulator.h"
#include "Shared/Generator/WtMod/GenWtAmpModulator.h"

#include "../../Core/Synth/UnisonParams.h"
#include "../../Core/Synth/UnisonState.h"

#include "./ModPitchShifter.h"
#include "./ProcessorModKeys.h"
#include "./ProcessorModValues.h"

// ============================================================================
// 出力へ掛ける変調
// ============================================================================
// 音源のプラグインでは、エンベロープや LFO はチャンネルごとに持っていて、
// オペレータの音量や音程を動かしていた。エフェクトにはチャンネルが無いので、
// 出力に対して 1 組だけ持つ。
//
// 動かすきっかけは MIDI。鍵盤を押すとエンベロープが始まり、離すと戻る。
// 鍵盤を触らなければ何も掛からず、そのまま素通しになる。
//
// 音量側はそのまま掛ければよいが、音程側は作りが違う。音源には発振器が
// あって位相の進み方を変えれば済んだのに対し、エフェクトは入ってきた音を
// 溜めてから読み出す速さを変えるしかない。そこは ModPitchShifter が担う。
class ModProcessor
{
	// --- 音量側 ---
	PrPtrsAdsrAmpEnv ptAmpEnv;
	PrPtrsSsgHwEnv ptSsgHwEnv;
	PrPtrsSsgSwEnv11 ptSsgSwEnv11;
	PrPtrsWtAmpMod ptWtAmpMod;
	PrPtrsOpzx7Lfo ptLfo;

	// --- 音程側 ---
	PrPtrsPitchEnv ptPitchEnv;
	PrPtrsSsgSwPEnv11 ptSsgSwPEnv11;
	PrPtrsWtMod ptWtMod;
	PrPtrsSsgHwPEnv ptSsgHwPEnv;

	// --- 音程を一定量ずらすもの ---
	PrPtrsOpzx7Detune ptDetune;
	PrPtrsUnison ptUnison;

	std::atomic<float>* pEnvBypass = nullptr;
	std::atomic<float>* pLfoBypass = nullptr;
	std::atomic<float>* pPitchBypass = nullptr;
	std::atomic<float>* pWtModBaseFreq = nullptr;
	std::atomic<float>* pShiftBypass = nullptr;

	// どの鍵盤で動かすか
	std::atomic<float>* pKeyAssignMode = nullptr;
	std::array<std::atomic<float>*, ModPrKey::KeyAssign::NumTargets> pKeys{};

	// 押さえている鍵盤。カスタマイズのときに、押している間だけ効かせる
	// もの (LFO・MUL/DET・UNISON/HARMONY・アルペジオ) が見る。
	std::bitset<128> heldKeys;

	// 押さえている鍵盤を割り当てた対象 (Target の番号のビット)。
	// 画面が枠の見出しを塗り分けるのに読む。オーディオスレッドが書き、
	// 画面のスレッドが読むので atomic にしてある。
	std::atomic<uint32_t> heldTargetsForGui{ 0 };

	static_assert(ModPrKey::KeyAssign::NumTargets <= 32, "heldTargetsForGui のビットが足りない");

	// LFO の AM / PM を効かせる度合い (0〜1)。押し離しの継ぎ目で音が
	// 飛ばないよう、数ミリ秒かけて寄せる。
	float lfoAmGate = 1.0f;
	float lfoPmGate = 1.0f;

	AmpAdsrEnv ampEnv;
	SsgHwEnv ssgHwEnv;
	SsgSwEnv11 ssgSwEnv11;
	WtAmpModulator wtAmpMod;
	Opzx7LfoCore lfo;

	PitchAdsrEnv pitchEnv;
	SsgSwPEnv11 ssgSwPEnv11;
	WtModulator wtMod;
	SsgHwPEnv ssgHwPEnv;

	Opzx7Detune detune;
	UnisonParams unisonParams;

	// ボイスごと・左右ごとに持つ。ユニゾンはボイスで音程が違い、
	// 音程が違えば溜めた音の読み口も別々になる。共用はできない。
	std::array<std::array<ModPitchShifter, 2>, Global::unisonVoices> shifters;

	// 疑似高速アルペジオ。今どのボイスを鳴らしているかと、
	// 切り替わり目でクリック音を出さないための渡り具合。
	int arpVoice = 0;
	double arpPhase = 0.0;
	std::array<float, Global::unisonVoices> arpGains{};

	// 今の音量。滑らかに動かすため、毎サンプル更新する。
	float ampLevel = 1.0f;

	bool envEnabled = false;
	bool lfoEnabled = false;
	bool pitchEnabled = false;
	bool shiftEnabled = false;

	// 前の塊で音程を動かしていたか。切り替わり目で溜めた音を捨てる。
	bool wasShifting = false;

	double rate = 44100.0;

	// 入り切りの札を読み直す。押し離しは音を作るより先に届くため。
	void refreshSwitches();

	using Targets = std::bitset<ModPrKey::KeyAssign::NumTargets>;

	bool isCustomKeyAssign() const;

	// その鍵盤を割り当てた対象
	Targets targetsOf(int note) const;

	// 対象へ「押した」「離した」を送る。順番はこれまでと同じ。
	void startTargets(const Targets& targets);
	void releaseTargets(const Targets& targets);

	// 押している間だけ効かせる対象が、いま効いているか。
	// シングルキーアサインでは鍵盤に関係なく効く (これまでどおり)。
	bool isHeld(ModPrKey::KeyAssign::Target target) const;

	// heldTargetsForGui を今の鍵盤と割り当てから作り直す
	void publishHeldTargets();
public:
	void createLayout(juce::AudioProcessorValueTreeState::ParameterLayout& layout);

	// WT PITCH MOD は波形の置き場を見に行くので、一緒に受け取る。
	void init(juce::AudioProcessorValueTreeState& apvts, WtModWaveStore& store);

	void prepare(double sampleRate);

	// 鍵盤の押し離し。音を鳴らすためではない。
	//
	// シングルキーアサインでは、どの鍵盤でも全部を動かす (音程は見ない)。
	// カスタマイズでは、その鍵盤を割り当てた対象だけを動かす。
	void noteOn(int note);
	void noteOff(int note);

	// 全部の鍵盤を離したことにする (オールノートオフ)
	void allNotesOff();

	// 出力へ掛ける。何も有効になっていなければ触らない。
	void processBlock(juce::AudioBuffer<float>& buffer, juce::AudioProcessorValueTreeState& apvts);

	// 画面の表示に使う。鳴っている間だけ真になる。
	bool isActive() const;

	// 画面の表示に使う。押さえている鍵盤を割り当てた対象を、
	// Target の番号のビットで返す。キーアサインのモードは見ない。
	uint32_t getHeldTargets() const { return heldTargetsForGui.load(std::memory_order_relaxed); }
};
