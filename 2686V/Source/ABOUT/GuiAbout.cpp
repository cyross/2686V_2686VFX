#include "GuiAbout.h"
#include "../core/Global.h"
#include "../gui/GuiValues.h"

void GuiAbout::setup()
{
    mainGroup.setup(*this, "");

    // 0. App Icon
    iconImage.setup(*this);

    auto iconImg = juce::ImageCache::getFromMemory(
        AppIconAboutForAboutData::icon_png,
        AppIconAboutForAboutData::icon_pngSize
    );
    iconImage.setImage(iconImg);

    // 1. Plugin Name
    pluginNameLabel.setup({ .parent = *this, .title = Global::Plugin::name });
    pluginNameLabel.setFont(juce::Font(GuiValue::About::PluginName::fontFamily, GuiValue::About::PluginName::fontSize, juce::Font::bold | juce::Font::italic));
    pluginNameLabel.setJustificationType(juce::Justification::centred);

    // 2. Version
    versionLabel.setup({ .parent = *this, .title = Global::Plugin::version });
    versionLabel.setFont(juce::Font(GuiValue::About::PluginVerion::fontSize));
    versionLabel.setJustificationType(juce::Justification::centred);

    // 3. Copyright
    copyrightLabel.setup({ .parent = *this, .title = Global::Plugin::author });
    copyrightLabel.setFont(juce::Font(GuiValue::About::PluginCioyright::fontSize));
    copyrightLabel.setJustificationType(juce::Justification::centred);

    // 4. Logo (BinaryDataから読み込み)
    // ファイルパスではなく、メモリ(バイナリ)から読み込む
    // 名前空間: VstLogoForAboutData (CMakeで指定したもの)
    // 変数名: ファイル名のドットがアンダースコアになったもの (vst3_logo_png)
    vst3Logo.setup(*this);

    auto logoImg = juce::ImageCache::getFromMemory(
        VstLogoForAboutData::vst3_logo_png,
        VstLogoForAboutData::vst3_logo_pngSize
    );
    vst3Logo.setImage(logoImg);

    // 5. VST Trademark Notice (必須表記)
    vstGuidelineLabel.setup({ .parent = *this, .title = Global::About::Vst::notice });
    vstGuidelineLabel.setFont(juce::Font(GuiValue::About::VstNotice::fontSize));
    vstGuidelineLabel.setJustificationType(juce::Justification::centred);

    // --- GPLv3ライセンス表示 ---
    // 1. 通知テキスト
    gplNoticeLabel.setup({ .parent = *this, .title = Global::About::Gpl::notice });
    gplNoticeLabel.setFont(juce::Font(GuiValue::About::GplNotice::fontSize));
    gplNoticeLabel.setJustificationType(juce::Justification::centred);

    // 2. リンクボタン
    gplLinkButton.setup({ .parent = *this, .title = Global::About::Gpl::navigate, .url = juce::URL(Global::About::Gpl::linkUrl), .color = juce::Colours::lightblue });
    gplLinkButton.setFont(juce::Font(GuiValue::About::GplLink::fontSize), false, juce::Justification::centred);
}

void GuiAbout::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();
    pageArea.removeFromTop(GuiValue::Group::TitlePaddingTop);

    auto mainArea = pageArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mainGroup.setBounds(pageArea);

    auto iconArea = mainArea.removeFromTop(GuiValue::About::LogoImage::areaHeight); 
    iconImage.setBounds(iconArea.withSizeKeepingCentre(GuiValue::About::LogoImage::imageSize, GuiValue::About::LogoImage::imageSize));

    // Name & Ver
    pluginNameLabel.setBounds(mainArea.removeFromTop(GuiValue::About::PluginName::areaHeight));
    versionLabel.setBounds(mainArea.removeFromTop(GuiValue::About::PluginVerion::areaHeight));
    copyrightLabel.setBounds(mainArea.removeFromTop(GuiValue::About::PluginCioyright::areaHeight));

    // VST Logo Row
    auto vstLogoArea = mainArea.removeFromTop(GuiValue::About::VstLogo::areaHeight);
    vst3Logo.setBounds(vstLogoArea.withSizeKeepingCentre(GuiValue::About::VstLogo::width, GuiValue::About::VstLogo::height));

    // VST Guideline Text
    vstGuidelineLabel.setBounds(mainArea.removeFromTop(GuiValue::About::VstNotice::areaHeight));

    auto gplNoticeArea = mainArea.removeFromTop(GuiValue::About::GplNotice::areaHeight);

    gplNoticeLabel.setBounds(gplNoticeArea);

    auto gplLinkArea = mainArea.removeFromTop(GuiValue::About::GplLink::areaHeight);

    gplLinkButton.setBounds(gplLinkArea);
}
