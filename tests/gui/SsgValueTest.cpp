// ============================================================================
// SSG の設定値 (VALUE) で決める: SSG HW AMP ENV / SSG HW PITCH ENV / FORM
// ============================================================================
#include "doctest/doctest.h"

#include <JuceHeader.h>

#include <functional>
#include <vector>

#include "Core/Processor/PluginProcessor.h"
#include "Core/Editor/PluginEditor.h"
#include "Shared/Core/Gui/GuiComponents.h"
#include "Shared/Core/Processor/ProcessorHelper.h"
#include "Shared/Core/Synth/SsgRegister.h"
#include "./EditorTestAccess.h"

namespace
{
	void setReal(juce::AudioProcessorValueTreeState& apvts, const juce::String& id, float value)
	{
		auto* p = apvts.getParameter(id);

		REQUIRE(p != nullptr);

		p->setValueNotifyingHost(p->convertTo0to1(value));
	}

	template <typename T>
	std::vector<T*> findAll(juce::Component& root, std::function<bool(T&)> pred)
	{
		std::vector<T*> out;

		std::function<void(juce::Component&)> walk = [&](juce::Component& c) {
			for (auto* child : c.getChildren())
			{
				if (auto* t = dynamic_cast<T*>(child); t != nullptr && pred(*t)) out.push_back(t);

				walk(*child);
			}

			if (auto* tabs = dynamic_cast<juce::TabbedComponent*>(&c))
			{
				for (int i = 0; i < tabs->getNumTabs(); ++i)
					if (auto* content = tabs->getTabContentComponent(i); content != nullptr && content->getParentComponent() != &c) walk(*content);
			}
		};

		walk(root);

		return out;
	}
}

TEST_CASE("SSG の設定値: 式どおりに Hz へ直し、0 は 1 として扱う")
{
	// Fenv = 667000 / (512 * Te)
	CHECK(SsgRegister::envHz(1303) == doctest::Approx(667000.0 / (512.0 * 1303.0)));
	CHECK(SsgRegister::envHz(1303) == doctest::Approx(1.0).epsilon(0.001));
	CHECK(SsgRegister::envHz(0) == doctest::Approx(SsgRegister::envHz(1)));

	// Fnois = 667000 / (32 * Tn)
	CHECK(SsgRegister::noiseHz(2) == doctest::Approx(667000.0 / 64.0));
	CHECK(SsgRegister::noiseHz(0) == doctest::Approx(SsgRegister::noiseHz(1)));

	// Fnote = 667000 / (32 * To)。440 Hz は To = 47 (443.48 Hz) へ寄る
	CHECK(SsgRegister::toneValue(440.0f) == 47);
	CHECK(SsgRegister::quantizeTone(440.0f) == doctest::Approx(667000.0 / (32.0 * 47.0)));
	CHECK(SsgRegister::quantizeToneRatio(440.0f) == doctest::Approx((667000.0 / (32.0 * 47.0)) / 440.0));

	// 範囲の外は端へ (To は 1〜4095)
	CHECK(SsgRegister::toneValue(1.0f) == SsgRegister::toneMax);
	CHECK(SsgRegister::toneValue(100000.0f) == 1);
}

TEST_CASE("SSG の設定値: VALUE を入れると PERIOD / N.FREQ の代わりになり、切ればこれまでどおり")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	auto& apvts = processor->apvts;

	// SSG を持たないプラグイン (2686VFX など) は飛ばす
	if (apvts.getParameter("SSG" + CPK::SsgHwEnv::period) == nullptr) return;

	// ---- SSG HW AMP ENV / SSG HW PITCH ENV
	PrPtrsSsgHwEnv env;
	PrPtrsSsgHwPEnv penv;
	PrHelper::setupSsgHwEnv(apvts, "SSG", env);
	PrHelper::setupSsgHwPEnv(apvts, "SSG", penv);

	setReal(apvts, "SSG" + CPK::SsgHwEnv::period, 5.0f);
	setReal(apvts, "SSG" + CPK::SsgHwEnv::value, 100.0f);
	setReal(apvts, "SSG" + CPK::SsgHwPEnv::period, 7.0f);
	setReal(apvts, "SSG" + CPK::SsgHwPEnv::value, 200.0f);

	SsgHwEnvParams ep;
	SsgHwPEnvParams pp;

	// 初めは切。これまでどおり PERIOD
	PrHelper::applySsgHwEnv(env, ep);
	PrHelper::applySsgHwPEnv(penv, pp);
	CHECK(ep.period == doctest::Approx(5.0f));
	CHECK(pp.period == doctest::Approx(7.0f));

	setReal(apvts, "SSG" + CPK::SsgHwEnv::valueOn, 1.0f);
	setReal(apvts, "SSG" + CPK::SsgHwPEnv::valueOn, 1.0f);

	PrHelper::applySsgHwEnv(env, ep);
	PrHelper::applySsgHwPEnv(penv, pp);
	CHECK(ep.period == doctest::Approx(SsgRegister::envHz(100)));
	CHECK(pp.period == doctest::Approx(SsgRegister::envHz(200)));

	// ---- FORM の NOISE / TONE
	PrPtrsToneNoise tn;
	PrHelper::setupToneNoise(apvts, "SSG", tn);

	setReal(apvts, "SSG" + CPK::Tn::freq, 9000.0f);
	setReal(apvts, "SSG" + CPK::Tn::noiseValue, 5.0f);

	ToneNoiseParams np;

	PrHelper::applyToneNoise(tn, np);
	CHECK(np.noiseFreq == doctest::Approx(9000.0f));
	CHECK_FALSE(np.toneValueOn);

	setReal(apvts, "SSG" + CPK::Tn::noiseValueOn, 1.0f);
	setReal(apvts, "SSG" + CPK::Tn::toneValueOn, 1.0f);

	PrHelper::applyToneNoise(tn, np);
	CHECK(np.noiseFreq == doctest::Approx(SsgRegister::noiseHz(5)));
	CHECK(np.toneValueOn);
}

TEST_CASE("SSG の設定値: 画面では、効いている方 (PERIOD / VALUE、N.FREQ / N.VALUE) だけ押せる")
{
	auto processor = std::make_unique<AudioPlugin2686V>();
	std::unique_ptr<juce::AudioProcessorEditor> editor(processor->createEditor());

	EditorTestAccess::materializeAll(*static_cast<AudioPlugin2686VEditor*>(editor.get()));

	auto& apvts = processor->apvts;

	if (apvts.getParameter("SSG" + CPK::SsgHwEnv::period) == nullptr) return;

	auto byId = [&](const juce::String& id) {
		auto found = findAll<GuiSlider>(*editor, [&](GuiSlider& s) { return s.getComponentID() == id; });
		return found.empty() ? nullptr : found.front();
	};

	auto toggleById = [&](const juce::String& id) {
		auto found = findAll<GuiToggleButton>(*editor, [&](GuiToggleButton& t) { return t.getComponentID() == id; });
		return found.empty() ? nullptr : found.front();
	};

	// SSG の SSG HW AMP ENV を使えるようにしておく
	setReal(apvts, "SSG" + CPK::SsgHwEnv::enable, 1.0f);

	auto* period = byId("SSG" + CPK::SsgHwEnv::period);
	auto* value = byId("SSG" + CPK::SsgHwEnv::value);
	auto* useValue = toggleById("SSG" + CPK::SsgHwEnv::valueOn);

	REQUIRE(period != nullptr);
	REQUIRE(value != nullptr);
	REQUIRE(useValue != nullptr);

	useValue->setToggleState(false, juce::sendNotificationSync);
	CHECK(period->isEnabled());
	CHECK_FALSE(value->isEnabled());

	useValue->setToggleState(true, juce::sendNotificationSync);
	CHECK_FALSE(period->isEnabled());
	CHECK(value->isEnabled());

	// FORM: NOISE の VALUE を入れると、N.FREQ が止まって N.VALUE が押せる
	auto* nfreq = byId("SSG" + CPK::Tn::freq);
	auto* nvalue = byId("SSG" + CPK::Tn::noiseValue);
	auto* noiseUse = toggleById("SSG" + CPK::Tn::noiseValueOn);

	REQUIRE(nfreq != nullptr);
	REQUIRE(nvalue != nullptr);
	REQUIRE(noiseUse != nullptr);
	REQUIRE(toggleById("SSG" + CPK::Tn::toneValueOn) != nullptr);

	noiseUse->setToggleState(false, juce::sendNotificationSync);
	CHECK(nfreq->isEnabled());
	CHECK_FALSE(nvalue->isEnabled());

	noiseUse->setToggleState(true, juce::sendNotificationSync);
	CHECK_FALSE(nfreq->isEnabled());
	CHECK(nvalue->isEnabled());

	// ほかのチャンネルの FORM にも置いてある (RHYTHM / ADPCM / ADPCM+ を持つもの)
	for (const char* id : { "RHYTHM_PAD0_NOISE_VALUE_ON", "ADPCM_NOISE_VALUE_ON", "ADPCMP_SLOT0_NOISE_VALUE_ON" })
	{
		if (apvts.getParameter(id) == nullptr) continue;

		INFO(id);
		CHECK(toggleById(id) != nullptr);
	}

	editor.reset();
}
