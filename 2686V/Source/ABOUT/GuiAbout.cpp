#include "GuiAbout.h"
#include "../const/GlobalConstants.h"

void GuiAbout::setup()
{
    mainGroup.setup(*this, "");

    // 1. Plugin Name
    pluginNameLabel.setup({ .parent = *this, .title = pluginName });
    pluginNameLabel.setFont(juce::Font(FontFamily, 64.0f, juce::Font::bold | juce::Font::italic));
    pluginNameLabel.setJustificationType(juce::Justification::centred);

    // 2. Version
    versionLabel.setup({ .parent = *this, .title = pluginVersion });
    versionLabel.setFont(juce::Font(24.0f));
    versionLabel.setJustificationType(juce::Justification::centred);

    // 3. Copyright
    copyrightLabel.setup({ .parent = *this, .title = pluginAuthor });
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
    vstGuidelineLabel.setup({ .parent = *this, .title = vstNotice });
    vstGuidelineLabel.setFont(juce::Font(14.0f)); // 小さめでOK
    vstGuidelineLabel.setJustificationType(juce::Justification::centred);

    // --- GPLv3ライセンス表示 ---
    // 1. 通知テキスト
    gplNoticeLabel.setup({ .parent = *this, .title = gplv3Notice });
    gplNoticeLabel.setFont(juce::Font(14.0f));
    gplNoticeLabel.setJustificationType(juce::Justification::centred);

    // 2. リンクボタン
    gplLinkButton.setup({ .parent = *this, .title = gplv3Navigate, .url = juce::URL(gplLinkUrl), .color = juce::Colours::lightblue }); // リンクっぽく青色にする
}

void GuiAbout::layout(juce::Rectangle<int> content)
{
    auto pageArea = content.withZeroOrigin();
    pageArea.removeFromTop(TitlePaddingTop);

    auto mainArea = pageArea.reduced(GlobalPaddingWidth, GlobalPaddingHeight);

    mainGroup.setBounds(pageArea);

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
