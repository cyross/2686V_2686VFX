// [SimpleEffects.h]
#pragma once
#include <JuceHeader.h>

// --- Simple Stereo Delay ---
class SimpleDelay
{
public:
    void prepare(double sampleRate, int maxDelayMs = 2000)
    {
        fs = sampleRate;
        int maxSamples = (int)(fs * maxDelayMs / 1000.0);
        delayBuffer.setSize(2, maxSamples);
        delayBuffer.clear();
        writePos = 0;
    }

    void setParameters(float timeMs, float feedback, float mix)
    {
        // Smooth parameter changes could be added here
        delayTimeSamples = (int)(fs * timeMs / 1000.0);
        fb = juce::jlimit(0.0f, 0.95f, feedback);
        wetLevel = juce::jlimit(0.0f, 1.0f, mix);
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        if (wetLevel < 0.01f) return; // Skip if mix is 0

        int numSamples = buffer.getNumSamples();
        int delayBufLen = delayBuffer.getNumSamples();

        for (int ch = 0; ch < buffer.getNumChannels(); ++ch)
        {
            if (ch >= 2) break; // Only support stereo

            auto* channelData = buffer.getWritePointer(ch);
            auto* delayData = delayBuffer.getWritePointer(ch);

            // Channel independent read pos (Simple)
            // For Ping-Pong, we would need cross-feedback logic
            int rPos = writePos - delayTimeSamples;
            if (rPos < 0) rPos += delayBufLen;

            for (int i = 0; i < numSamples; ++i)
            {
                float dry = channelData[i];

                // Read from delay buffer
                float wet = delayData[rPos];

                // Write back to delay buffer (with feedback)
                delayData[writePos] = dry + (wet * fb);

                // Mix
                channelData[i] = (dry * (1.0f - wetLevel)) + (wet * wetLevel);

                // Advance positions
                writePos++;
                if (writePos >= delayBufLen) writePos = 0;

                rPos++;
                if (rPos >= delayBufLen) rPos = 0;
            }

            // Sync writePos for next channel (reset for stereo loop consistency in this block)
            if (ch < buffer.getNumChannels() - 1) {
                // writePos needs to be effectively rewound or synced.
                // However, simpler approach: update writePos only once per sample frame?
                // Let's use simpler block-based logic for stereo sync:
                // Actually, circular buffer state 'writePos' is shared.
                // We must handle channels carefully.
                // Correct way for interleaved loop:
                // Wait, writePos is member variable. It changes during ch0 loop.
                // We need to backup writePos for ch1.
                writePos -= numSamples;
                while (writePos < 0) writePos += delayBufLen;
            }
        }
        // Advance writePos properly after all channels processed
        writePos += numSamples;
        while (writePos >= delayBufLen) writePos -= delayBufLen;
    }

private:
    juce::AudioBuffer<float> delayBuffer;
    double fs = 44100.0;
    int writePos = 0;
    int delayTimeSamples = 0;
    float fb = 0.0f;
    float wetLevel = 0.0f;
};

// --- Effect Manager ---
class EffectChain
{
public:
    void prepare(double sampleRate)
    {
        delay.prepare(sampleRate);
        reverb.setSampleRate(sampleRate);
    }

    // Parameters
    // Delay: Time(ms), Feedback(0-1), Mix(0-1)
    // Reverb: Size(0-1), Damping(0-1), Mix(0-1)
    void setDelayParams(float time, float fb, float mix) { delay.setParameters(time, fb, mix); }

    void setReverbParams(float size, float damp, float width, float mix)
    {
        juce::Reverb::Parameters p;
        p.roomSize = size;
        p.damping = damp;
        p.width = width;
        p.wetLevel = mix;
        p.dryLevel = 1.0f - mix;
        p.freezeMode = 0;
        reverb.setParameters(p);
    }

    void process(juce::AudioBuffer<float>& buffer)
    {
        // 1. Apply Delay
        delay.process(buffer);

        // 2. Apply Reverb (Stereo)
        if (buffer.getNumChannels() == 2)
        {
            reverb.processStereo(buffer.getWritePointer(0), buffer.getWritePointer(1), buffer.getNumSamples());
        }
        else if (buffer.getNumChannels() == 1)
        {
            reverb.processMono(buffer.getWritePointer(0), buffer.getNumSamples());
        }
    }

    void reset()
    {
        reverb.reset();
        // Delay reset logic if needed
    }

private:
    SimpleDelay delay;
    juce::Reverb reverb;
};
