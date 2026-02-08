#include "PluginProcessor.h"
#include "PluginEditor.h"

AudioPlugin2686VFX::AudioPlugin2686VFX()
    : AudioProcessor(BusesProperties()
        .withInput("Input", juce::AudioChannelSet::stereo(), true)
        .withOutput("Output", juce::AudioChannelSet::stereo(), true))
{}

AudioPlugin2686VFX::~AudioPlugin2686VFX() {}

void AudioPlugin2686VFX::prepareToPlay(double sampleRate, int samplesPerBlock) {}
void AudioPlugin2686VFX::releaseResources() {}
void AudioPlugin2686VFX::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    // エフェクト処理は後で実装
}

juce::AudioProcessorEditor* AudioPlugin2686VFX::createEditor()
{
    return new juce::GenericAudioProcessorEditor(*this);
}
bool AudioPlugin2686VFX::hasEditor() const { return true; }

const juce::String AudioPlugin2686VFX::getName() const { return "2686VFX"; }
bool AudioPlugin2686VFX::acceptsMidi() const { return false; }
bool AudioPlugin2686VFX::producesMidi() const { return false; }
bool AudioPlugin2686VFX::isMidiEffect() const { return false; }
double AudioPlugin2686VFX::getTailLengthSeconds() const { return 0.0; }
int AudioPlugin2686VFX::getNumPrograms() { return 1; }
int AudioPlugin2686VFX::getCurrentProgram() { return 0; }
void AudioPlugin2686VFX::setCurrentProgram(int index) {}
const juce::String AudioPlugin2686VFX::getProgramName(int index) { return {}; }
void AudioPlugin2686VFX::changeProgramName(int index, const juce::String& newName) {}
void AudioPlugin2686VFX::getStateInformation(juce::MemoryBlock& destData) {}
void AudioPlugin2686VFX::setStateInformation(const void* data, int sizeInBytes) {}

// ★ エントリーポイント
juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter()
{
    return new AudioPlugin2686VFX();
}
