#pragma once

class FmSampleCore
{
    // DAWやVSTホストで設定されているサンプルレート
	double hostSampleRate = 44100.0;

    // サンプルレートエミュレーションのための加算器
    // 初期状態(キーを押した瞬間)は1.0
    // getSampleが呼ばれるたびにstepSizeずつ加算されていく
    // 1.0以上になったら、音源内部で処理するサンプルレートのタイミングが来たと判断できる
    double rateAcc = 0.0;
	// DAWのサンプルレートと、音源内部で処理するサンプルレートの比率
	// 例えば、hostSampleRate が 48000.0 で、targetSampleRate が 44100.0 の場合、stepSize は約0.91875になる
    double stepSize = 0.0;
    float prevSample = 0.0f;
    float lastSample = 0.0f;
public:
    // 音源内部で処理するサンプルレートを指すインデックス値
    // targetSampleRate の算出に使われる
    // メンバとして管理することで、サンプレートが変わらない時に計算をスルーできるようになる
    int rateIndex = 1;

    // 音源内部で処理するサンプレート
    double targetSampleRate = 44100.0;

    void prepare(double sampleRate);
    void setParameters(int newRateIndex);
    void noteOn();
	void updateAcc();
    bool isActive() const;
    void start();
    void finish(float finalSample);
    float getSample() const;
};
