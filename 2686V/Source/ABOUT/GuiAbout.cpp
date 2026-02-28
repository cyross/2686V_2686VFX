#include "GuiAbout.h"
#include "../core/Global.h"
#include "../core/GuiValues.h"

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
    pluginNameLabel.setFont(juce::Font(GuiValue::About::Logo::fontFamily, GuiValue::About::Logo::fontSize, juce::Font::bold | juce::Font::italic));
    pluginNameLabel.setJustificationType(juce::Justification::centred);

    // 2. Version
    versionLabel.setup({ .parent = *this, .title = Global::Plugin::version });
    versionLabel.setFont(juce::Font(24.0f));
    versionLabel.setJustificationType(juce::Justification::centred);

    // 3. Copyright
    copyrightLabel.setup({ .parent = *this, .title = Global::Plugin::author });
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
    vstGuidelineLabel.setFont(juce::Font(14.0f)); // 小さめでOK
    vstGuidelineLabel.setJustificationType(juce::Justification::centred);

    // --- GPLv3ライセンス表示 ---
    // 1. 通知テキスト
    gplNoticeLabel.setup({ .parent = *this, .title = Global::About::Gpl::notice });
    gplNoticeLabel.setFont(juce::Font(14.0f));
    gplNoticeLabel.setJustificationType(juce::Justification::centred);

    // 2. リンクボタン
    gplLinkButton.setup({ .parent = *this, .title = Global::About::Gpl::navigate, .url = juce::URL(Global::About::Gpl::linkUrl), .color = juce::Colours::lightblue }); // リンクっぽく青色にする
}

void GuiAbout::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();
    pageArea.removeFromTop(GuiValue::Group::TitlePaddingTop);

    auto mainArea = pageArea.reduced(GuiValue::Group::Padding::width, GuiValue::Group::Padding::height);

    mainGroup.setBounds(pageArea);

    auto iconArea = mainArea.removeFromTop(160); 
    iconImage.setBounds(iconArea.withSizeKeepingCentre(128, 128));

    // Name & Ver
    pluginNameLabel.setBounds(mainArea.removeFromTop(80));
    versionLabel.setBounds(mainArea.removeFromTop(40));
    copyrightLabel.setBounds(mainArea.removeFromTop(30));

    // Logo Row
    // ロゴの元サイズ比率を維持して中央配置
    auto logoArea = mainArea.removeFromTop(80);
    vst3Logo.setBounds(logoArea.withSizeKeepingCentre(150, 60)); // 枠内で最大150x60に収める等の調整

    // VST Guideline Text
    vstGuidelineLabel.setBounds(mainArea.removeFromTop(20));

    auto gplNoticeArea = mainArea.removeFromTop(40);

    gplNoticeLabel.setBounds(gplNoticeArea);

    auto gplLinkArea = mainArea.removeFromTop(40);

    gplLinkButton.setBounds(gplLinkArea);

}
