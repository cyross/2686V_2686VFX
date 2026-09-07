#pragma once

#include <deque>
#include <functional>
#include <memory>
#include <vector>

#include <JuceHeader.h>

#include "../GenWave/GenWaveRender.h"
#include "../../../Core/Gui/GuiContext.h"

// ============================================================================
// パラメータ・波形ファイルのブラウザ
// ============================================================================
// パラメータファイルと波形ファイルを一覧して選ぶための画面。画面のほぼ全体を
// 覆うポップアップとして出し、外を押すと選ばずに閉じる。
//
// 行は juce の部品を並べず、直に描いている。フォルダによっては何百件にも
// なるため、1 行につき部品をいくつも作ると開くだけで重くなる。部品を置いて
// あるのは、文字を打ち込む欄と絞り込み、それにボタンだけ。
//
// 今いるフォルダの中身を出す。フォルダも行として並び、開けば中へ入れる。
// 一番上の「上へ」で親フォルダへ戻る。キーワードを打っている間だけは、
// 下のフォルダまで潜って探す。
//
// 波形プレビューは「生成波形のプレビュー」と同じ見せ方で、10 秒ぶんの上を
// 窓が動く。作るのは押したときだけで、作ったものはファイルへ貯めるので、
// 次に開いたときは作り直さない。
class GuiParamBrowser : public juce::Component, private juce::Timer
{
public:
    // --- 見た目の寸法 ---
    static constexpr int margin = 32;          // 画面の縁から空ける幅
    static constexpr int padding = 12;
    static constexpr int searchRowHeight = 26;
    static constexpr int pathRowHeight = 20;
    static constexpr int headerHeight = 26;
    static constexpr int rowHeight = 44;
    static constexpr int scrollBarWidth = 10;

    static constexpr int categoryWidth = 110;
    static constexpr int formatWidth = 80;
    static constexpr int previewWidth = 220;
    static constexpr int actionWidth = 110;

    static constexpr int filterWidth = 150;
    static constexpr int folderButtonWidth = 110;
    static constexpr int newFolderButtonWidth = 120;
    static constexpr int deleteFolderButtonWidth = 120;
    static constexpr int bulkButtonWidth = 220;
    static constexpr int deleteButtonWidth = 130;
    static constexpr int saveButtonWidth = 110;
    static constexpr int nameLabelWidth = 64;

    static constexpr int frameMs = 33;

    // プレビューを持っておく行数。
    //
    // 1 件で 10 秒ぶん (16kHz なら 16 万点) を抱えるので、スクロールした
    // ぶんだけ溜めていくと、数百件のフォルダでメモリを使い切ってしまう。
    // 出した順に並べておき、あふれたものから捨てる。
    static constexpr int previewKeep = 64;

    // 並べ替えの向き。押すたびにこの順で回る。
    enum class Sort { none = 0, ascending = 1, descending = 2 };

    // 並べ替えできる列
    enum class Column { name = 0, category = 1, format = 2 };

    // 読むために開くのか、書き出す先を決めるために開くのか
    enum class Mode { open = 0, save = 1 };

    // 何のファイルか。プレビューの作り方がそれぞれ違う。
    //
    //   param … チャンネルの設定。鳴らして 10 秒ぶんを作り、動かして見せる
    //   wave  … 波形そのもの。中身をそのまま線にする (止め絵)
    //   audio … 音声ファイル。上下の幅で形を出す (止め絵)
    enum class Kind { param = 0, wave = 1, audio = 2 };

    struct Item
    {
        juce::File file;
        juce::String name;      // 拡張子を落とした表示名
        juce::String category;  // 区分 (OPNA / OPM / 波形(WT) …)
        juce::String format;    // JSON / YAML / (無印)

        // フォルダの行。isParent は一番上の「上へ」。
        bool isFolder = false;
        bool isParent = false;

        Kind kind = Kind::param;

        // param のときだけ使う。作って貯めてある 10 秒ぶん。
        GenWaveRender::Wave wave;
        bool hasWave = false;

        // wave の止め絵 (-1.0〜1.0 の折れ線)
        std::vector<float> points;

        // audio の止め絵 (区間ごとの最小値と最大値)
        std::vector<float> mins;
        std::vector<float> maxs;

        // プレビューを用意したかどうか。開いた時点で全部を用意すると、
        // 件数が多いフォルダで待たされる。出す行のぶんだけ用意する。
        bool previewLoaded = false;
    };

    // 開くときの注文。増えてきたのでまとめてある。
    struct Request
    {
        juce::File root;

        // 呼んだ側が扱える区分の名前。空なら制限なし。
        juce::StringArray allowed;

        Mode mode = Mode::open;

        // 書き出すときの初期の名前と、付ける拡張子 (".wt" など)
        juce::String defaultName;
        juce::String extension;

        // 名前にこれが入っていないものは選べない。
        //
        // CURVE のように、同じ区分の中で置き場所と対象ごとに中身が違う
        // ものがある。区分だけでは取り違えるので、名前でも縛れるように
        // してある。空なら縛らない。
        juce::String nameMustContain;

        // これより上のフォルダへは行かせない。空なら縛らない。
        //
        // パラメータやプリセットは、このプラグインの置き場の中でしか
        // 意味を持たない。外へ出られると、選べないファイルばかりの
        // フォルダを延々と辿ることになる。音の素材 (wav) だけは、
        // どこから持ってきても構わないので縛らない。
        juce::File limit;

        std::function<void(const juce::File&)> onChoose;
    };

    explicit GuiParamBrowser(const GuiContext& context);
    ~GuiParamBrowser() override;

    void open(juce::Component& parent, const Request& request);
    void close();

    // フォルダを移ったときに呼ばれる。呼んだ側が次に開くときの根を
    // 覚えておくために使う。
    std::function<void(const juce::File&)> onRootChanged;

    // ファイル名から区分を読む。扱えないファイルなら空を返す。
    static juce::String categoryOf(const juce::File& file);

    // ファイル名から形式を読む (JSON / YAML / 無印)
    static juce::String formatOf(const juce::File& file);

    void paint(juce::Graphics& g) override;
    void resized() override;
    void mouseDown(const juce::MouseEvent& event) override;
    void mouseDoubleClick(const juce::MouseEvent& event) override;
    void mouseWheelMove(const juce::MouseEvent& event, const juce::MouseWheelDetails& wheel) override;
private:
    GuiContext ctx;

    juce::TextEditor keyword;
    juce::ComboBox categoryFilter;
    juce::ComboBox formatFilter;
    juce::TextButton folderBtn;
    juce::TextButton newFolderBtn;
    juce::TextButton deleteFolderBtn;
    juce::TextButton bulkGenerateBtn;
    juce::TextButton bulkDeleteBtn;

    // 書き出す先を決めるときだけ出す
    juce::TextEditor nameEditor;
    juce::TextButton saveBtn;

    std::function<void(const juce::File&)> m_onChoose;

    juce::File m_root;
    juce::File m_limit;
    juce::StringArray m_allowed;
    Mode m_mode = Mode::open;
    juce::String m_extension;
    juce::String m_nameMustContain;

    std::vector<Item> m_items;      // 集めたもの全部
    std::vector<int> m_view;        // 絞り込みと並べ替えの結果 (m_items の番号)

    int m_selected = -1;            // m_view の中の位置
    int m_scroll = 0;               // 一番上に出ている行

    Column m_sortColumn = Column::name;
    Sort m_sort = Sort::ascending;

    int m_cycleIndex = 1;           // 生成波形のプレビューと同じ選択肢

    double m_startMs = 0.0;

    // 一括生成の途中経過
    std::vector<int> m_queue;
    size_t m_queueAt = 0;
    juce::MemoryBlock m_savedState;
    bool m_busy = false;
    bool m_cancelled = false;

    juce::ThreadPool m_pool{ 1 };

    std::unique_ptr<juce::FileChooser> m_fileChooser;

    // プレビューを持っている行 (m_items の番号)。出した順に積む。
    std::deque<int> m_previewOrder;

    // --- 区画 ---
    juce::Rectangle<int> panelArea() const;
    juce::Rectangle<int> innerArea() const;
    int topRowsHeight() const;
    juce::Rectangle<int> pathArea() const;
    juce::Rectangle<int> listArea() const;
    juce::Rectangle<int> headerArea() const;
    juce::Rectangle<int> rowArea(int viewIndex) const;
    juce::Rectangle<int> columnArea(juce::Rectangle<int> row, Column column) const;
    juce::Rectangle<int> previewArea(juce::Rectangle<int> row) const;
    juce::Rectangle<int> actionArea(juce::Rectangle<int> row) const;
    juce::Rectangle<int> cycleCell(int index) const;

    int visibleRows() const;

    // --- 中身 ---
    void collect();
    void rebuildView();
    void refillCategoryFilter();
    void loadPreview(int itemIndex);
    void dropOldPreviews();
    void loadWavePoints(Item& item);
    void loadAudioShape(Item& item);

    bool isAllowed(const Item& item) const;

    // そのフォルダへ移ってよいか (m_limit の中かどうか)
    bool canGoTo(const juce::File& dir) const;

    void goTo(const juce::File& dir);
    void chooseAt(int viewIndex);
    void toggleSort(Column column);

    void chooseRoot();
    void createFolder();
    void deleteSelectedFolder();
    void commitSave();

    void startBulkGenerate();
    void generateNext();
    void finishBulk();
    void deleteAllPreviews();
    void generateOne(int itemIndex);

    // --- 描く ---
    void drawPath(juce::Graphics& g);
    void drawHeader(juce::Graphics& g);
    void drawRows(juce::Graphics& g);
    void drawRow(juce::Graphics& g, int viewIndex);
    void drawWave(juce::Graphics& g, juce::Rectangle<int> area, const Item& item);
    void drawStill(juce::Graphics& g, juce::Rectangle<int> area, const Item& item);
    void drawCycles(juce::Graphics& g);
    void drawScrollBar(juce::Graphics& g);

    void timerCallback() override { repaint(listArea()); }
};
