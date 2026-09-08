#include "./GenWaveRender.h"

#include <cmath>
#include <cstring>

#include "../../../Core/Processor/PluginProcessor.h"
#include "../../../Core/Const/ConstFileValues.h"

// 音源一式。ヘッダへ出すと PluginProcessor.h を読むことになり、
// 画面側から辿ったときに循環するので、ここへ隠しておく。
struct GenWaveRender::Rig::Impl
{
    RetroSynthesiser synth;
    SynthParams params;
};

GenWaveRender::Rig::Rig() : m_impl(std::make_unique<Impl>())
{
}

GenWaveRender::Rig::~Rig() = default;

void GenWaveRender::Rig::build(AudioPlugin2686V& processor)
{
    auto& impl = *m_impl;

    impl.params = processor.buildRenderParams();

    impl.synth.clearVoices();
    impl.synth.clearSounds();
    impl.synth.addSound(new SynthSound());

    // 1 音ぶんなので、ユニゾンの最大数だけあれば足りる
    for (int i = 0; i < Global::unisonVoices; ++i)
    {
        auto* voice = new SynthVoice();

        impl.synth.addVoice(voice);

        processor.prepareRenderVoice(*voice, renderRate);

        voice->setParameters(impl.params);
    }

    impl.synth.setCurrentPlaybackSampleRate(renderRate);

    // 本体と同じ鳴らし方にするため、シンセ側の設定も写す
    impl.synth.currentParams = &impl.params;
    impl.synth.isMonoMode = impl.params.monoMode;
    impl.synth.useVelocity = impl.params.useVelocity;
    impl.synth.pitchResetOnLegato = impl.params.pitchResetOnLegato;
    impl.synth.fixedVelocity = impl.params.fixedVelocity;
}

GenWaveRender::Wave GenWaveRender::render(Rig& rig)
{
    Wave out;

    out.sampleRate = storeRate;
    out.noteHz = juce::MidiMessage::getMidiNoteInHertz(midiNote);

    const int total = (int)(seconds * renderRate);
    const int offAt = (int)(total * noteOffAt);

    // 間引きの間隔。割り切れる値にしてあるので位相はずれない。
    const int step = juce::jmax(1, (int)std::lround(renderRate / storeRate));

    out.left.reserve((size_t)(total / step) + 1);
    out.right.reserve((size_t)(total / step) + 1);

    juce::AudioBuffer<float> buffer(2, blockSize);
    juce::MidiBuffer midi;

    midi.addEvent(juce::MidiMessage::noteOn(1, midiNote, (juce::uint8)100), 0);

    bool released = false;

    // 鳴り終わりを見張る。一度も鳴っていないうちは数えない
    // (立ち上がりの遅い音を頭で切ってしまわないため)。
    bool started = false;
    int silentFor = 0;

    const int silentLimit = (int)(tailSeconds * renderRate);

    for (int done = 0; done < total; done += blockSize)
    {
        const int num = juce::jmin(blockSize, total - done);

        if (!released && offAt >= done && offAt < done + num)
        {
            midi.addEvent(juce::MidiMessage::noteOff(1, midiNote), offAt - done);

            released = true;
        }

        buffer.clear();
        rig.impl().synth.renderNextBlock(buffer, midi, 0, num);
        midi.clear();

        const float* left = buffer.getReadPointer(0);
        const float* right = buffer.getNumChannels() > 1 ? buffer.getReadPointer(1) : left;

        for (int i = 0; i < num; ++i)
        {
            if (std::abs(left[i]) > silenceLevel || std::abs(right[i]) > silenceLevel)
            {
                started = true;
                silentFor = 0;
            }
            else if (started)
            {
                ++silentFor;
            }

            if (((done + i) % step) != 0) continue;

            out.left.push_back(juce::jlimit(-1.0f, 1.0f, left[i]));
            out.right.push_back(juce::jlimit(-1.0f, 1.0f, right[i]));
        }

        // 鳴り終わって tailSeconds ぶん経ったら、そこで切り上げる
        if (started && silentFor >= silentLimit) break;
    }

    return out;
}

// ============================================================================
// 貯めておくファイル
// ============================================================================
// 中身は 8 ビットに落とした波形。100px ほどの帯へ描くだけなので 256 段
// あれば足りる。1 件 160KB ほどで、フォルダ 1 つぶんを貯めても現実的な
// 大きさに収まる。作り直せるものなので、消えても困らない。
namespace
{
    const char* const genWaveMagic = "2686VGWV";
    constexpr int genWaveMagicLength = 8;
}

bool GenWaveRender::write(const juce::File& file, const Wave& wave)
{
    if (wave.isEmpty()) return false;

    file.getParentDirectory().createDirectory();

    juce::TemporaryFile temp(file);

    {
        juce::FileOutputStream stream(temp.getFile());

        if (!stream.openedOk()) return false;

        stream.write(genWaveMagic, genWaveMagicLength);
        stream.writeInt(fileVersion);
        stream.writeInt((int)wave.sampleRate);
        stream.writeInt((int)wave.left.size());
        stream.writeFloat((float)wave.noteHz);

        std::vector<juce::int8> bytes;

        bytes.reserve(wave.left.size() * 2);

        // L をひとまとめ、そのあと R。読むときに前から順に取れる。
        for (float v : wave.left)
        {
            bytes.push_back((juce::int8)juce::jlimit(-127, 127, (int)std::lround(v * 127.0f)));
        }

        for (float v : wave.right)
        {
            bytes.push_back((juce::int8)juce::jlimit(-127, 127, (int)std::lround(v * 127.0f)));
        }

        stream.write(bytes.data(), bytes.size());
        stream.flush();
    }

    return temp.overwriteTargetFileWithTemporary();
}

GenWaveRender::Wave GenWaveRender::read(const juce::File& file)
{
    Wave out;

    if (!file.existsAsFile()) return out;

    juce::FileInputStream stream(file);

    if (!stream.openedOk()) return out;

    char head[genWaveMagicLength] = {};

    if (stream.read(head, genWaveMagicLength) != genWaveMagicLength) return out;
    if (std::memcmp(head, genWaveMagic, genWaveMagicLength) != 0) return out;

    const int version = stream.readInt();

    // 1 のものは混ざったものしか入っていない。読めるだけ読んで、
    // 左右へ同じものを入れる。
    if (version != 1 && version != fileVersion) return out;

    const int rate = stream.readInt();
    const int count = stream.readInt();
    const float hz = stream.readFloat();

    if (rate <= 0 || count <= 0 || count > (int)(seconds * renderRate)) return out;

    const int channels = (version == 1) ? 1 : 2;
    const int stored = count * channels;

    std::vector<juce::int8> bytes((size_t)stored);

    if (stream.read(bytes.data(), stored) != stored) return out;

    out.sampleRate = rate;
    out.noteHz = hz;
    out.left.reserve((size_t)count);
    out.right.reserve((size_t)count);

    for (int i = 0; i < count; ++i)
    {
        out.left.push_back((float)bytes[(size_t)i] / 127.0f);
        out.right.push_back((float)bytes[(size_t)(channels == 1 ? i : count + i)] / 127.0f);
    }

    return out;
}

juce::File GenWaveRender::cacheDirectory(const AudioPlugin2686V& processor)
{
    auto dir = processor.getPluginDirectory().getChildFile(Io::Folder::wavePreview);

    if (!dir.exists()) dir.createDirectory();

    return dir;
}

juce::File GenWaveRender::cacheFileFor(const juce::File& cacheDir, const juce::File& source)
{
    // 元の場所から短い符号を作る。同じ名前が別のフォルダにあっても
    // 混ざらないようにするためで、中身を見分けるためのものではない。
    const juce::String tag =
        juce::String::toHexString((juce::int64)source.getFullPathName().toLowerCase().hashCode64());

    return cacheDir.getChildFile(source.getFileName() + "." + tag + fileExtension);
}
