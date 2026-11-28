#include "./Wallpaper.h"

#include "Shared/Core/Gui/GuiColor.h"

bool GuiWallpaper::load(const juce::String& path)
{
    if (m_loaded && path == m_path) return false;

    m_loaded = true;
    m_path = path;
    m_image = {};
    m_blurred = {};

    if (path.isEmpty()) return true;

    const juce::File file(path);

    if (!file.existsAsFile()) return true;

    m_image = juce::ImageFileFormat::loadFrom(file);

    if (m_image.isValid()) {
        // 余白に敷くぼかし。軽くするため 1/16 に縮めてから、5x5 のガウシアンを掛ける
        const int w = juce::jmax(1, m_image.getWidth() / 16);
        const int h = juce::jmax(1, m_image.getHeight() / 16);
        const juce::Image small = m_image.rescaled(w, h, juce::Graphics::lowResamplingQuality);

        juce::ImageConvolutionKernel kernel(5);
        kernel.createGaussianBlur(2.0f);

        m_blurred = juce::Image(small.getFormat(), w, h, true);
        kernel.applyToImage(m_blurred, small, small.getBounds());
    }

    return true;
}

bool GuiWallpaper::draw(juce::Graphics& g, juce::Rectangle<float> area, int mode) const
{
    if (!m_image.isValid()) return false;

    // Fit・Original は余白が出るので、ぼかした画像を敷いて少し暗く落とす
    if ((mode == 2 || mode == 3) && m_blurred.isValid()) {
        g.drawImage(m_blurred, area, juce::RectanglePlacement::fillDestination);
        g.setColour(GuiColor::Editor::blurWallpaperBg);
        g.fillRect(area);
    }

    int placement = juce::RectanglePlacement::stretchToFit;

    switch (mode) {
    case 1: placement = juce::RectanglePlacement::fillDestination; break;
    case 2: placement = juce::RectanglePlacement::centred; break;
    case 3: placement = juce::RectanglePlacement::centred | juce::RectanglePlacement::doNotResize; break;
    default: break;
    }

    g.drawImage(m_image, area, placement);

    // 手前の部品が読めるよう、薄く暗い膜を掛ける
    g.setColour(GuiColor::Editor::wallpaperBg);
    g.fillRect(area);

    return true;
}
