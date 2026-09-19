// 画面のテストの入口。
//
// 部品は JUCE のメッセージスレッドの上でしか作れないので、doctest を
// 回す前に JUCE を立ち上げておく。テストはこのスレッドで走るので、
// 束縛 (Attachment) は値の変化をその場で部品へ届ける。
#define DOCTEST_CONFIG_IMPLEMENT
#include "doctest/doctest.h"

#include <JuceHeader.h>

int main(int argc, char** argv)
{
    juce::ScopedJuceInitialiser_GUI juce;

    doctest::Context context;

    context.applyCommandLine(argc, argv);

    return context.run();
}
