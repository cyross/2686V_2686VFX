#include "./GenWaveRender.h"

#include <cmath>
#include <cstring>

#include "../../../Core/Const/ConstFileValues.h"

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

    if (rate <= 0 || count <= 0 || count > (int)(maxSeconds * renderRate)) return out;

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

juce::File GenWaveRender::cacheDirectory(const GuiProcessorHost& processor)
{
    auto dir = processor.getPluginDirectory().getChildFile(Io::Folder::wavePreview);

    if (!dir.exists()) dir.createDirectory();

    return dir;
}

juce::File GenWaveRender::cacheFileFor(const juce::File& cacheDir, const juce::File& source, const juce::String& patternKey)
{
    // 元の場所から短い符号を作る。同じ名前が別のフォルダにあっても
    // 混ざらないようにするためで、中身を見分けるためのものではない。
    const juce::String tag =
        juce::String::toHexString((juce::int64)source.getFullPathName().toLowerCase().hashCode64());

    // 打鍵の並びの符号も付ける。並びを変えたら、前に作ったものは別物として扱う
    return cacheDir.getChildFile(source.getFileName() + "." + tag + "." + patternKey + fileExtension);
}
