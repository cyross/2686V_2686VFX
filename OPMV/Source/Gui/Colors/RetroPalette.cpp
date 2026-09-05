#include "./RetroPalette.h"

namespace
{
	// 等間隔の段を作る。bits ビットなら 0 から 255 までを等分する。
	juce::uint8 evenStep(int value, int bits)
	{
		int maxValue = (1 << bits) - 1;

		return (juce::uint8)juce::roundToInt((double)value * 255.0 / (double)maxValue);
	}

	// メガドライブの段。DAC の出力が等間隔でないので、実測でよく使われる
	// 値を並べてある。等間隔で作ると当時の見た目から外れる。
	constexpr juce::uint8 megaDriveLevels[8] = { 0, 52, 87, 116, 144, 172, 206, 255 };

	std::vector<juce::Colour> makeEven(int bits)
	{
		const int steps = 1 << bits;

		std::vector<juce::Colour> out;
		out.reserve((size_t)(steps * steps * steps));

		for (int r = 0; r < steps; ++r) {
			for (int g = 0; g < steps; ++g) {
				for (int b = 0; b < steps; ++b) {
					out.push_back(juce::Colour::fromRGB(evenStep(r, bits), evenStep(g, bits), evenStep(b, bits)));
				}
			}
		}

		return out;
	}
}

std::vector<juce::Colour> RetroPalette::colours(Machine machine)
{
	switch (machine)
	{
	case Machine::Pc8801:
	case Machine::PcEngine:
		return makeEven(3);

	case Machine::Pc9801:
		return makeEven(4);

	case Machine::MegaDrive:
	{
		std::vector<juce::Colour> out;
		out.reserve(512);

		for (int r = 0; r < 8; ++r) {
			for (int g = 0; g < 8; ++g) {
				for (int b = 0; b < 8; ++b) {
					out.push_back(juce::Colour::fromRGB(megaDriveLevels[r], megaDriveLevels[g], megaDriveLevels[b]));
				}
			}
		}

		return out;
	}

	case Machine::X68000:
	{
		// RGB 各 5bit に輝度 1bit が付く。輝度は各成分の最下位ビットとして
		// 効くので、6bit ぶんの段を 32 段 + 半段で作るのと同じことになる。
		std::vector<juce::Colour> out;
		out.reserve(65536);

		for (int i = 0; i < 2; ++i) {
			for (int r = 0; r < 32; ++r) {
				for (int g = 0; g < 32; ++g) {
					for (int b = 0; b < 32; ++b) {
						int rr = (r << 1) | i;
						int gg = (g << 1) | i;
						int bb = (b << 1) | i;

						out.push_back(juce::Colour::fromRGB(evenStep(rr, 6), evenStep(gg, 6), evenStep(bb, 6)));
					}
				}
			}
		}

		return out;
	}
	}

	return {};
}

juce::String RetroPalette::name(Machine machine)
{
	switch (machine)
	{
	case Machine::Pc8801:    return u8"PC-8801 (512色)";
	case Machine::Pc9801:    return u8"PC-9801 (4096色)";
	case Machine::PcEngine:  return u8"PCエンジン (512色)";
	case Machine::MegaDrive: return u8"メガドライブ (512色)";
	case Machine::X68000:    return u8"X68000 (65536色)";
	}

	return {};
}

int RetroPalette::columns(Machine machine)
{
	// 同じ色相がまとまって見えるよう、青の段数を 1 行の長さにする
	switch (machine)
	{
	case Machine::Pc8801:
	case Machine::PcEngine:
	case Machine::MegaDrive:
		return 8 * 8;

	case Machine::Pc9801:
		return 16 * 8;

	case Machine::X68000:
		return 32 * 8;
	}

	return 32;
}

void RetroPalette::setMachine(Machine machine)
{
	m_machine = machine;
	m_colours = colours(machine);

	repaint();
}

int RetroPalette::preferredHeight(int) const
{
	if (m_colours.empty()) return 0;

	const int cols = columns(m_machine);

	return ((int)m_colours.size() + cols - 1) / cols * cell;
}

void RetroPalette::paint(juce::Graphics& g)
{
	if (m_colours.empty()) return;

	const int cols = columns(m_machine);

	for (int i = 0; i < (int)m_colours.size(); ++i) {
		int x = (i % cols) * cell;
		int y = (i / cols) * cell;

		g.setColour(m_colours[(size_t)i]);
		g.fillRect(x, y, cell, cell);
	}
}

int RetroPalette::indexAt(juce::Point<int> position) const
{
	if (m_colours.empty()) return -1;

	const int cols = columns(m_machine);
	const int col = position.x / cell;

	if (col < 0 || col >= cols) return -1;

	int index = (position.y / cell) * cols + col;

	if (index < 0 || index >= (int)m_colours.size()) return -1;

	return index;
}

void RetroPalette::mouseDown(const juce::MouseEvent& e)
{
	int index = indexAt(e.getPosition());

	if (index < 0 || !onColourPicked) return;

	onColourPicked(m_colours[(size_t)index]);
}
