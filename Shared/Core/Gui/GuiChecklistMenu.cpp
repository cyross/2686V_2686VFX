#include "GuiChecklistMenu.h"

#include <memory>

namespace
{
    // 行の丈と、印の箱の大きさ
    constexpr int rowHeight = 24;
    constexpr int boxSize = 12;
    constexpr int paddingX = 8;

    // 押したあとに全部の行を描き直すための、同じポップアップの行の一覧
    struct Rows
    {
        std::vector<juce::Component::SafePointer<juce::Component>> rows;

        void repaintAll()
        {
            for (auto& r : rows) {
                if (r != nullptr) r->repaint();
            }
        }
    };

    class Row : public juce::PopupMenu::CustomComponent
    {
    public:
        Row(juce::String text, std::function<bool()> on, std::function<void()> click, std::shared_ptr<Rows> others, bool check) :
            // false: 押されても PopupMenu に選ばせない (閉じない)
            juce::PopupMenu::CustomComponent(false),
            label(std::move(text)),
            isOn(std::move(on)),
            onClick(std::move(click)),
            rows(std::move(others)),
            isCheck(check)
        {
        }

        void getIdealSize(int& idealWidth, int& idealHeight) override
        {
            const auto font = juce::Font(juce::FontOptions((float)rowHeight * 0.6f));

            idealWidth = paddingX * 3 + boxSize + juce::roundToInt(juce::GlyphArrangement::getStringWidth(font, label)) + paddingX;
            idealHeight = rowHeight;
        }

        void paint(juce::Graphics& g) override
        {
            auto& lf = getLookAndFeel();

            auto area = getLocalBounds();

            if (isItemHighlighted()) {
                g.setColour(lf.findColour(juce::PopupMenu::highlightedBackgroundColourId));
                g.fillRect(area);
            }

            const auto text = lf.findColour(isItemHighlighted() ? juce::PopupMenu::highlightedTextColourId : juce::PopupMenu::textColourId);

            area.removeFromLeft(paddingX);

            auto box = area.removeFromLeft(boxSize).withSizeKeepingCentre(boxSize, boxSize).toFloat();

            area.removeFromLeft(paddingX);

            g.setColour(text);

            if (isCheck) {
                g.drawRect(box, 1.0f);

                if (isOn != nullptr && isOn()) g.fillRect(box.reduced(3.0f));
            }

            g.setFont(juce::Font(juce::FontOptions((float)rowHeight * 0.6f)));
            g.drawText(label, area, juce::Justification::centredLeft, true);
        }

        void mouseUp(const juce::MouseEvent& e) override
        {
            if (!getLocalBounds().contains(e.getPosition())) return;

            if (onClick != nullptr) onClick();

            if (rows != nullptr) rows->repaintAll();
        }

    private:
        juce::String label;
        std::function<bool()> isOn;
        std::function<void()> onClick;
        std::shared_ptr<Rows> rows;
        bool isCheck;
    };
}

void GuiChecklistMenu::show(const juce::String& title, std::vector<Check> checks, std::vector<Action> actions, std::vector<Action> leading)
{
    juce::PopupMenu menu;

    auto rows = std::make_shared<Rows>();

    if (title.isNotEmpty()) menu.addSectionHeader(title);

    // 上に置くふつうの項目 (見出しの右クリックの「グループを非表示」など)。押すと閉じる
    for (auto& a : leading) menu.addItem(a.name, a.run);

    if (!leading.empty() && (!checks.empty() || !actions.empty())) menu.addSeparator();

    for (auto& c : checks) {
        auto isOn = c.isOn;
        auto set = c.set;

        auto row = std::make_unique<Row>(c.name, isOn, [isOn, set] {
            if (isOn != nullptr && set != nullptr) set(!isOn());
            }, rows, true);

        rows->rows.push_back(row.get());

        menu.addCustomItem(-1, std::move(row), nullptr, c.name);
    }

    if (!checks.empty() && !actions.empty()) menu.addSeparator();

    for (auto& a : actions) {
        auto row = std::make_unique<Row>(a.name, nullptr, a.run, rows, false);

        rows->rows.push_back(row.get());

        menu.addCustomItem(-1, std::move(row), nullptr, a.name);
    }

    menu.showMenuAsync(juce::PopupMenu::Options().withMousePosition());
}
