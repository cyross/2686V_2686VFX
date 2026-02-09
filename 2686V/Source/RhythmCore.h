#pragma once
#include <JuceHeader.h>
#include <vector>
#include <cmath>
#include "Mode.h"

// 1�̃h�������F��\���N���X
struct DrumSample
{
    std::vector<float> data; // PCM�f�[�^
    int position = -1;       // ���݂̍Đ��ʒu (-1�͒�~��)
    bool isPlaying = false;

    void start() { position = 0; isPlaying = true; }

    // �T���v����1���o��
    float getNextSample()
    {
        if (!isPlaying || position < 0 || position >= data.size()) return 0.0f;

        float out = data[position];
        position++;

        if (position >= data.size()) {
            isPlaying = false;
            position = -1;
        }
        return out;
    }
};

class RhythmCore
{
public:
    RhythmCore() {}

    void prepare(double sampleRate)
    {
        if (sampleRate > 0.0) m_sampleRate = sampleRate;
        generateDrumSounds();
    }

    void setParameters(const SynthParams& params)
    {
        m_masterLevel = params.rhythmLevel;
    }

    void noteOn(int midiNote, float velocity)
    {
        // 60(C3): Kick
        // 62(D3): Snare
        // 64(E3): HiHat

        // 36(C1): Kick
        // 38(D1): Snare
        // 42(F#1): HiHat

        if (midiNote == 36 || midiNote == 60) m_kick.start();
        else if (midiNote == 38 || midiNote == 62) m_snare.start();
        else if (midiNote == 42 || midiNote == 64) m_hihat.start();
    }

    void noteOff()
    {
    }

    bool isPlaying() const
    {
        return m_kick.isPlaying || m_snare.isPlaying || m_hihat.isPlaying;
    }

    float getSample()
    {
        float mix = 0.0f;
        mix += m_kick.getNextSample();
        mix += m_snare.getNextSample();
        mix += m_hihat.getNextSample();

        return mix * m_masterLevel;
    }

private:
    void generateDrumSounds()
    {
        // 1. Kick
        int kickLen = (int)(0.3 * m_sampleRate);
        m_kick.data.resize(kickLen);
        for (int i = 0; i < kickLen; ++i)
        {
            float progress = (float)i / kickLen;
            float freq = 150.0f * std::pow(0.01f, progress);
            float phase = freq * 2.0f * juce::MathConstants<float>::pi / m_sampleRate * i;
            float env = 1.0f - progress;
            m_kick.data[i] = std::sin(phase * i * 0.05f) * env * 0.8f;
        }

        // 2. Snare
        int snareLen = (int)(0.2 * m_sampleRate);
        m_snare.data.resize(snareLen);
        for (int i = 0; i < snareLen; ++i)
        {
            float progress = (float)i / snareLen;
            float noise = (float(std::rand()) / RAND_MAX * 2.0f - 1.0f);
            float env = std::pow(1.0f - progress, 2.0f);
            m_snare.data[i] = noise * env * 0.6f;
        }

        // 3. HiHat
        int hhLen = (int)(0.05 * m_sampleRate);
        m_hihat.data.resize(hhLen);
        for (int i = 0; i < hhLen; ++i)
        {
            float progress = (float)i / hhLen;
            float noise = (float(std::rand()) / RAND_MAX * 2.0f - 1.0f);
            if (i % 2 == 0) noise *= -1.0f;
            float env = std::pow(1.0f - progress, 4.0f);
            m_hihat.data[i] = noise * env * 0.4f;
        }
    }

    double m_sampleRate = 44100.0;
    float m_masterLevel = 1.0f;

    DrumSample m_kick;
    DrumSample m_snare;
    DrumSample m_hihat;
};
