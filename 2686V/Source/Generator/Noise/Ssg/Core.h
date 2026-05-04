class SsgNoiseGen
{
    unsigned int lfsr = 0x1FFFF;
    float level = 0.0f;
public:
    void prepare(double newTargetRate);
    void setParameters(float newLevel, float newBaseFreq, bool noiseOnNote);
	void updateTargetRate(double newTargetRate) { this->targetSampleRate = newTargetRate; }
    void generate();
    void updateDelta();
	void updateFrequency(float currentFreq);
    float generateSample(float gain);

    bool noiseOnNote = false;
    float phase = 0.0f;
    float delta = 0.0f;
    float currentSample = 0.0f;
    float baseFreq = 12000.0f; // Slider Value
    float targetFreq = 12000.0f; // Active Frequency
	double targetSampleRate = 44100.0;
};
