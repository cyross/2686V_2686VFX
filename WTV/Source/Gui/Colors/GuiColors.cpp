#include "./GuiColors.h"

#include "./RetroPalette.h"

#include "../../Core/Const/ConstFileValues.h"

#include <optional>

#include "../../Core/Editor/PluginEditor.h"

// ============================================================================
// 行ごとの「既定へ戻す」
// ============================================================================
// ボタンをセルいっぱいに広げると窮屈なので、入れ物を挟んで余白を取る。
// テーブルはセルの大きさしか決めないため、余白はこちらで持つしかない。
class ResetCell : public juce::Component
{
public:
	GuiTextButton button;

	// どの行のものかは差し替わる。押されたときに読むので、値を
	// 捕まえずここを見ること。
	int row = -1;

	explicit ResetCell(const GuiContext& context) : button(context)
	{
		addAndMakeVisible(button);
		button.setButtonText(ColorsGuiText::resetRow);
	}

	// 色は部品へ写るので、行を差し替えるたびに写し直す
	void applyColours()
	{
		button.setColour(juce::TextButton::textColourOffId, GuiColor::TextButton::Text);
		button.setColour(juce::TextButton::textColourOnId, GuiColor::TextButton::TextOn);
		button.setColour(juce::TextButton::buttonColourId, GuiColor::TextButton::Bg);
	}

	void resized() override
	{
		button.setBounds(getLocalBounds().reduced(ColorsGuiValue::Table::CellPadding));
	}
};
// ============================================================================
// 色名のドロップダウン
// ============================================================================
// 名前だけでは何色か分からないので、左へ色見本を出す。閉じているときの
// 表示にも同じものを出す。
//
// 見本の色は名前から引く。引けない名前 (見出しなど) では何も足さず、
// そのまま JUCE に描かせる。
class ColourNameLF : public juce::LookAndFeel_V4
{
public:
	std::function<std::optional<juce::Colour>(const juce::String&)> lookup;

	void drawPopupMenuItem(juce::Graphics& g, const juce::Rectangle<int>& area,
		bool isSeparator, bool isActive, bool isHighlighted, bool isTicked, bool hasSubMenu,
		const juce::String& text, const juce::String& shortcutKeyText,
		const juce::Drawable* icon, const juce::Colour* textColour) override
	{
		auto colour = lookup ? lookup(text) : std::nullopt;

		if (!colour.has_value()) {
			juce::LookAndFeel_V4::drawPopupMenuItem(g, area, isSeparator, isActive, isHighlighted,
				isTicked, hasSubMenu, text, shortcutKeyText, icon, textColour);

			return;
		}

		auto rest = area;
		auto gutter = rest.removeFromLeft(ColorsGuiValue::Editor::swatchWidth);

		// 強調の帯は JUCE が渡された範囲にしか塗らない。見本の脇だけ
		// 色が抜けて見えるので、先に全体を塗っておく。
		if (isHighlighted && isActive) {
			g.setColour(findColour(juce::PopupMenu::highlightedBackgroundColourId));
			g.fillRect(area);
		}

		juce::LookAndFeel_V4::drawPopupMenuItem(g, rest, isSeparator, isActive, isHighlighted,
			isTicked, hasSubMenu, text, shortcutKeyText, icon, textColour);

		drawSwatch(g, gutter, *colour);
	}

	// 見本のぶんだけ幅を足す
	void getIdealPopupMenuItemSize(const juce::String& text, bool isSeparator,
		int standardMenuItemHeight, int& idealWidth, int& idealHeight) override
	{
		juce::LookAndFeel_V4::getIdealPopupMenuItemSize(text, isSeparator, standardMenuItemHeight,
			idealWidth, idealHeight);

		idealWidth += ColorsGuiValue::Editor::swatchWidth;
	}

	void drawComboBox(juce::Graphics& g, int width, int height, bool isButtonDown,
		int buttonX, int buttonY, int buttonW, int buttonH, juce::ComboBox& box) override
	{
		juce::LookAndFeel_V4::drawComboBox(g, width, height, isButtonDown,
			buttonX, buttonY, buttonW, buttonH, box);

		auto colour = lookup ? lookup(box.getText()) : std::nullopt;

		if (!colour.has_value()) return;

		drawSwatch(g, { 0, 0, ColorsGuiValue::Editor::swatchWidth, height }, *colour);
	}

	// 閉じているときの文字も見本のぶんだけ右へ寄せる。
	//
	// 見本の有無で場所を変えてはいけない。JUCE がこれを呼ぶのは
	// 大きさが変わったときだけで、選び直しても呼ばれない。選ぶ前に
	// 寄せていないと、あとから見本と名前が重なる。
	void positionComboBoxText(juce::ComboBox& box, juce::Label& label) override
	{
		juce::LookAndFeel_V4::positionComboBoxText(box, label);

		label.setBounds(label.getBounds().withTrimmedLeft(ColorsGuiValue::Editor::swatchWidth));
	}
private:
	static void drawSwatch(juce::Graphics& g, juce::Rectangle<int> area, juce::Colour colour)
	{
		auto box = area.reduced(ColorsGuiValue::Editor::swatchPadding).toFloat();

		g.setColour(colour);
		g.fillRoundedRectangle(box, guiCornerRadius);

		// 明るい色でも輪郭が分かるように縁を引く
		g.setColour(GuiColor::Outline);
		g.drawRoundedRectangle(box.reduced(0.5f), guiCornerRadius, 1.0f);
	}
};
// ============================================================================
// 色の変更ダイアログの中身
// ============================================================================
// 色を決める道を 3 つ並べる。
//
//   1. 色名から選ぶ    … 名前の付いた色をそのまま当てる
//   2. 実機のパレット  … 当時の機種が出せた色から選ぶ
//   3. ColourSelector … RGB と色空間で好きに作る
//
// どれで選んでも行き先は ColourSelector なので、最後は必ずそこの色が
// 採用される。上の 2 つは入口を増やしているだけ。
class ColorEditorPanel : public juce::Component
{
	juce::ColourSelector selector;

	juce::Label nameLabel;
	juce::ComboBox nameBox;
	ColourNameLF nameBoxLF;

	juce::Label paletteLabel;
	juce::ComboBox paletteBox;

	juce::Viewport paletteView;
	RetroPalette palette;

	// 名前の一覧。選択肢の番号から引くために持っておく。
	std::vector<std::pair<juce::String, juce::Colour>> names;
public:
	ColorEditorPanel(juce::Colour initial) :
		selector(juce::ColourSelector::showColourAtTop
			| juce::ColourSelector::showSliders
			| juce::ColourSelector::showColourspace)
	{
		addAndMakeVisible(selector);
		selector.setCurrentColour(initial);

		// ---- 色名から選ぶ ----
		addAndMakeVisible(nameLabel);
		nameLabel.setText(ColorsGuiText::Dialog::nameList, juce::dontSendNotification);
		nameLabel.setJustificationType(juce::Justification::centredLeft);

		addAndMakeVisible(nameBox);
		nameBox.addItem(ColorsGuiText::Dialog::nameNone, 1);

		// こちらで付けた名前と JUCE の名前は見出しで分ける。
		// 数が違いすぎて混ぜると探しにくいため。
		names = GuiColor::namedColours();

		nameBox.addSectionHeading(ColorsGuiText::Dialog::ownNames);

		for (int i = 0; i < (int)names.size(); ++i) {
			nameBox.addItem(names[(size_t)i].first, i + 2);
		}

		nameBox.addSectionHeading(ColorsGuiText::Dialog::juceNames);

		for (const auto& kv : GuiColor::juceColours()) {
			names.push_back(kv);
			nameBox.addItem(kv.first, (int)names.size() + 1);
		}

		nameBox.setSelectedId(1, juce::dontSendNotification);

		// 見本を出すのに名前から色を引けるようにする
		nameBoxLF.lookup = [this](const juce::String& text) -> std::optional<juce::Colour> {
			for (const auto& kv : names) {
				if (kv.first == text) return kv.second;
			}

			return std::nullopt;
			};




		nameBox.setLookAndFeel(&nameBoxLF);

		nameBox.onChange = [this] {
			int index = nameBox.getSelectedId() - 2;

			if (index < 0 || index >= (int)names.size()) return;

			selector.setCurrentColour(names[(size_t)index].second);
			};

		// ---- 実機のパレットから選ぶ ----
		addAndMakeVisible(paletteLabel);
		paletteLabel.setText(ColorsGuiText::Dialog::palette, juce::dontSendNotification);
		paletteLabel.setJustificationType(juce::Justification::centredLeft);

		addAndMakeVisible(paletteBox);

		for (int i = 0; i < (int)machines.size(); ++i) {
			paletteBox.addItem(RetroPalette::name(machines[(size_t)i]), i + 1);
		}

		paletteBox.setSelectedId(1, juce::dontSendNotification);

		paletteBox.onChange = [this] {
			int index = paletteBox.getSelectedId() - 1;

			if (index < 0 || index >= (int)machines.size()) return;

			setPalette(machines[(size_t)index]);
			};

		addAndMakeVisible(paletteView);
		paletteView.setViewedComponent(&palette, false);
		paletteView.setScrollBarsShown(true, true);

		palette.onColourPicked = [this](juce::Colour colour) {
			selector.setCurrentColour(colour);
			};

		setPalette(machines[0]);

		setSize(ColorsGuiValue::Editor::width,
			ColorsGuiValue::Row::height * 2
			+ ColorsGuiValue::Row::padding * 3
			+ ColorsGuiValue::Editor::selectorHeight
			+ ColorsGuiValue::Editor::paletteHeight);
	}

	~ColorEditorPanel() override
	{
		nameBox.setLookAndFeel(nullptr);
	}

	juce::Colour currentColour() const { return selector.getCurrentColour(); }

	void resized() override
	{
		auto area = getLocalBounds();

		auto nameRow = area.removeFromTop(ColorsGuiValue::Row::height);
		nameLabel.setBounds(nameRow.removeFromLeft(ColorsGuiValue::Editor::labelWidth));
		nameBox.setBounds(nameRow);

		area.removeFromTop(ColorsGuiValue::Row::padding);

		auto paletteRow = area.removeFromTop(ColorsGuiValue::Row::height);
		paletteLabel.setBounds(paletteRow.removeFromLeft(ColorsGuiValue::Editor::labelWidth));
		paletteBox.setBounds(paletteRow);

		area.removeFromTop(ColorsGuiValue::Row::padding);

		paletteView.setBounds(area.removeFromTop(ColorsGuiValue::Editor::paletteHeight));

		area.removeFromTop(ColorsGuiValue::Row::padding);

		selector.setBounds(area);
	}
private:
	// 並べる順番。色数の少ないものから。
	static inline const std::vector<RetroPalette::Machine> machines = {
		RetroPalette::Machine::Pc8801,
		RetroPalette::Machine::PcEngine,
		RetroPalette::Machine::MegaDrive,
		RetroPalette::Machine::Pc9801,
		RetroPalette::Machine::X68000,
	};

	void setPalette(RetroPalette::Machine machine)
	{
		palette.setMachine(machine);

		// 窓より大きいときは中で送れるよう、実寸で置く
		int width = RetroPalette::columns(machine) * 10;

		palette.setSize(width, palette.preferredHeight(width));
	}
};

void GuiColors::setup()
{
	int tabOrder = 0;

	titleLabel.setup({ .parent = *this, .title = ColorsGuiText::title,
		.font = juce::Font(juce::FontOptions(16.0f)).withStyle(juce::Font::bold),
		.justification = juce::Justification::centredLeft });

	hintLabel.setup({ .parent = *this, .title = ColorsGuiText::hint,
		.justification = juce::Justification::centredLeft });

	// 断り書きは常に見えるところへ置く。ダイアログを開かないと
	// 読めないのでは意味が無い。
	noticeLabel.setup({ .parent = *this, .title = ColorsGuiText::Dialog::notice,
		.justification = juce::Justification::topLeft });

	resetAllButton.setup({ .parent = *this, .title = ColorsGuiText::resetAll, .isReset = false });
	resetAllButton.setExplicitFocusOrder(++tabOrder);
	resetAllButton.onClick = [this] {
		GuiColor::resetAllColours();

		table.updateContent();
		};

	loadButton.setup({ .parent = *this, .title = ColorsGuiText::load, .isReset = false });
	loadButton.setExplicitFocusOrder(++tabOrder);
	loadButton.onClick = [this] { loadFromFile(); };

	saveButton.setup({ .parent = *this, .title = ColorsGuiText::save, .isReset = false });
	saveButton.setExplicitFocusOrder(++tabOrder);
	saveButton.onClick = [this] { saveToFile(); };

	table.setup({ .parent = *this, .title = ColorsGuiText::title });
	table.setWantsKeyboardFocus(false);
	table.addColumn(ColorsGuiText::Table::swatch, 1, ColorsGuiValue::Table::SwatchWidth);
	table.addColumn(ColorsGuiText::Table::item, 2, ColorsGuiValue::Table::ItemWidth);
	table.addColumn(ColorsGuiText::Table::value, 3, ColorsGuiValue::Table::ValueWidth);
	table.addColumn(ColorsGuiText::Table::reset, 4, ColorsGuiValue::Table::ResetWidth);

	rebuildRows();

	table.onGetNumRows = [this]() { return (int)ids.size(); };

	table.onGetCellText = [this](int row, int columnId) -> juce::String {
		if (row < 0 || row >= (int)ids.size()) return {};

		const juce::String& id = ids[(size_t)row];

		if (columnId == 2) return id;

		if (columnId == 3) {
			auto it = GuiColor::registry().find(id);

			if (it == GuiColor::registry().end()) return {};

			return GuiColor::describe(it->second->get());
		}

		return {};
		};

	// 1 列目は文字ではなく色そのものを見せる
	table.onPaintCell = [this](juce::Graphics& g, int row, int columnId, int width, int height, bool) {
		if (columnId != 1) return false;

		if (row < 0 || row >= (int)ids.size()) return true;

		auto it = GuiColor::registry().find(ids[(size_t)row]);

		if (it == GuiColor::registry().end()) return true;

		auto area = juce::Rectangle<float>(2.0f, 2.0f, (float)width - 4.0f, (float)height - 4.0f);

		g.setColour(it->second->get());
		g.fillRoundedRectangle(area, guiCornerRadius);

		// 明るい色でも輪郭が分かるように縁を引く
		g.setColour(GuiColor::Outline);
		g.drawRoundedRectangle(area.reduced(0.5f), guiCornerRadius, 1.0f);

		return true;
		};

	// 4 列目は行ごとの「既定へ戻す」。ダイアログを開かなくても
	// 1 色だけ戻せるようにする。
	table.onRefreshCellComponent = [this](int row, int columnId, bool, juce::Component* existing) -> juce::Component* {
		if (columnId != 4) {
			delete existing;

			return nullptr;
		}

		auto* cell = dynamic_cast<ResetCell*>(existing);

		if (cell == nullptr) {
			delete existing;

			cell = new ResetCell(ctx);

			cell->button.onClick = [this, cell] {
				if (cell->row < 0 || cell->row >= (int)ids.size()) return;

				GuiColor::resetColour(ids[(size_t)cell->row]);

				table.updateContent();
				};
		}

		cell->row = row;
		cell->applyColours();

		// 既定のままの色は戻す先が無いので押せなくする。
		// どの色を変えたかがひと目で分かる利点もある。
		cell->button.setEnabled(isChanged(row));

		return cell;
		};

	table.onDoubleClicked = [this](int row) { openEditor(row); };
}

bool GuiColors::isChanged(int row) const
{
	if (row < 0 || row >= (int)ids.size()) return false;

	auto it = GuiColor::registry().find(ids[(size_t)row]);

	if (it == GuiColor::registry().end()) return false;

	return it->second->get() != it->second->defaultColour();
}

void GuiColors::rebuildRows()
{
	ids.clear();

	for (const auto& kv : GuiColor::registry()) ids.push_back(kv.first);

	table.updateContent();
}

// 1 行ぶんの色を変えるダイアログ。
void GuiColors::openEditor(int row)
{
	if (row < 0 || row >= (int)ids.size()) return;

	const juce::String id = ids[(size_t)row];

	auto it = GuiColor::registry().find(id);

	if (it == GuiColor::registry().end()) return;

	auto* window = new juce::AlertWindow(
		ColorsGuiText::Dialog::title + " : " + id,
		juce::String(),
		juce::MessageBoxIconType::NoIcon);

	auto panel = std::make_shared<ColorEditorPanel>(it->second->get());

	window->addCustomComponent(panel.get());

	window->addButton(ColorsGuiText::Dialog::apply, 1, juce::KeyPress(juce::KeyPress::returnKey, 0, 0));
	window->addButton(ColorsGuiText::Dialog::reset, 2);
	window->addButton(ColorsGuiText::Dialog::cancel, 0, juce::KeyPress(juce::KeyPress::escapeKey, 0, 0));

	GuiDialog::styleButtons(*window);

	// 画面が閉じたあとに答えが返ってくることがあるので、表を触る前に生死を見る。
	// 色そのものは共有の入れ物なので、画面が無くなっていても入れて構わない。
	juce::Component::SafePointer<GuiColors> safeThis(this);

	// 所有権はダイアログへ渡らないので、閉じるまでこちらで抱える
	window->enterModalState(true, juce::ModalCallbackFunction::create(
		[safeThis, id, panel](int result) {
			if (result == 1) GuiColor::setColour(id, panel->currentColour());
			else if (result == 2) GuiColor::resetColour(id);

			if (safeThis != nullptr) safeThis->table.updateContent();
		}), true);
}

void GuiColors::layout(juce::Rectangle<int> content)
{
	auto area = content.withZeroOrigin();

	// タブの下辺と詰まって見えるので、少しだけ離す
	area.removeFromTop(EditorGuiValue::Group::gapFromTabBar);

	area = area.reduced(ColorsGuiValue::Table::PaddingWidth, ColorsGuiValue::Table::PaddingHeight);

	// 見出し・案内・断り書きをまとめて板の上へ載せる
	headerArea = area.removeFromTop(
		ColorsGuiValue::Row::height * 2
		+ ColorsGuiValue::Row::noticeHeight
		+ ColorsGuiValue::Row::padding * 2
		+ ColorsGuiValue::Row::boardPadding * 2);

	auto header = headerArea.reduced(ColorsGuiValue::Row::boardPadding);

	titleLabel.setBounds(header.removeFromTop(ColorsGuiValue::Row::height));
	header.removeFromTop(ColorsGuiValue::Row::padding);

	auto hintRow = header.removeFromTop(ColorsGuiValue::Row::height);

	resetAllButton.setBounds(hintRow.removeFromRight(ColorsGuiValue::Button::width));
	hintRow.removeFromRight(ColorsGuiValue::Row::padding);

	saveButton.setBounds(hintRow.removeFromRight(ColorsGuiValue::Button::fileWidth));
	hintRow.removeFromRight(ColorsGuiValue::Row::padding);

	loadButton.setBounds(hintRow.removeFromRight(ColorsGuiValue::Button::fileWidth));
	hintRow.removeFromRight(ColorsGuiValue::Row::padding);

	hintLabel.setBounds(hintRow);

	header.removeFromTop(ColorsGuiValue::Row::padding);

	noticeLabel.setBounds(header.removeFromTop(ColorsGuiValue::Row::noticeHeight));

	area.removeFromTop(ColorsGuiValue::Row::padding);

	table.setBounds(area);
}

// 壁紙が透けて文字が読みにくくなるのを防ぐ板。
// カテゴリを開いたときに敷くものと同じ色を使う。
void GuiColors::paint(juce::Graphics& g)
{
	if (headerArea.isEmpty()) return;

	g.setColour(GuiColor::Category::ContentBg);
	g.fillRoundedRectangle(headerArea.toFloat(), guiCornerRadius);
}

// ============================================================================
// ファイルへの読み書き
// ============================================================================
// 形式は JSON。新しく作るものなので、値の並び順に頼る古い形式は使わない。
//
// 書き出すのは既定から変えた色だけにしてある。全部書くと、後から色を
// 足したときに古いファイルが「既定のまま」を上書きしてしまい、新しい
// 既定が反映されなくなるため。
void GuiColors::saveToFile()
{
	juce::File defaultDir(ctx.audioProcessor.defaultColorSettingDir);

	if (!defaultDir.isDirectory()) {
		defaultDir = ctx.audioProcessor.getPluginDirectory();
	}

	fileChooser = std::make_unique<juce::FileChooser>(ColorsGuiText::File::saveTitle,
		defaultDir.getChildFile(ColorsGuiText::File::defaultName()),
		Io::ExtensionGlob::ColorSetting);

	fileChooser->launchAsync(juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::warnAboutOverwriting,
		[this](const juce::FileChooser& fc) {
			auto file = fc.getResult();

			if (file == juce::File{}) return;

			// 次回のダイアログ用にディレクトリを保存
			ctx.audioProcessor.defaultColorSettingDir = file.getParentDirectory().getFullPathName();

			auto* colors = new juce::DynamicObject();

			for (const auto& kv : GuiColor::registry()) {
				if (kv.second->get() == kv.second->defaultColour()) continue;

				colors->setProperty(kv.first, kv.second->get().toDisplayString(true));
			}

			auto* root = new juce::DynamicObject();

			root->setProperty(ColorsGuiText::File::Key::format, ColorsGuiText::File::format);
			root->setProperty(ColorsGuiText::File::Key::version, ColorsGuiText::File::version);
			root->setProperty(ColorsGuiText::File::Key::colors, juce::var(colors));

			// 書き出す形は設定で決まる
			Io::writeValueTo(file, juce::var(root));
		});
}

// 知らない名前は読み飛ばす。色を減らした版で書いたファイルでも
// 読めるようにするため。
void GuiColors::loadFromFile()
{
	juce::File defaultDir(ctx.audioProcessor.defaultColorSettingDir);

	if (!defaultDir.isDirectory()) {
		defaultDir = ctx.audioProcessor.getPluginDirectory();
	}

	fileChooser = std::make_unique<juce::FileChooser>(ColorsGuiText::File::openTitle,
		defaultDir, Io::ExtensionGlob::ColorSetting);

	fileChooser->launchAsync(juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles,
		[this](const juce::FileChooser& fc) {
			auto file = fc.getResult();

			if (!file.existsAsFile()) return;

			ctx.audioProcessor.defaultColorSettingDir = file.getParentDirectory().getFullPathName();

			// JSON でも YAML でも読める
			auto parsed = Io::readValueFrom(file);
			auto* root = parsed.getDynamicObject();

			if (root == nullptr) return;

			// 印が違うものは色の設定ではないので触らない
			if (root->getProperty(ColorsGuiText::File::Key::format).toString() != ColorsGuiText::File::format) return;

			auto* colors = root->getProperty(ColorsGuiText::File::Key::colors).getDynamicObject();

			if (colors == nullptr) return;

			// 書かれていない色は既定へ戻す。読み込んだ結果が、書き出した
			// ときの見た目とそろうようにするため。
			GuiColor::resetAllColours();

			for (const auto& kv : colors->getProperties()) {
				GuiColor::setColour(kv.name.toString(), juce::Colour::fromString(kv.value.toString()));
			}

			table.updateContent();
		});
}
