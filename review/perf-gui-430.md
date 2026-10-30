# v4.3.0 GUI パフォーマンス 敵対的検証

- 対象: `feat/adversarial-review-430` (aefdf5fd 時点)
- 見方: コードを読んだだけ。ビルド・計測はしていない。効き具合の見積もりは、特に断りが無いかぎりすべて **推測**。「確実」は「そのコードがその頻度で走ること」をコードで確かめた、という意味で、重さの大きさまで測ったという意味ではない。
- JUCE は 8.0.12 (`external/JUCE/CMakeLists.txt`) として JUCE 側の挙動を確かめた。

効き具合: 高 = マウスを動かすだけで毎回走り、部品数・タブ数に比例して膨らむもの / 中 = 操作のたびに広く走るもの / 低 = 気にするほどではないが直せるもの。

---

## 第 1 段階: 差分の部分 (d454db96 / 4909ed2c / aefdf5fd)

### P1-1. 区分の板のホバー判定が、マウス移動のたびに全部の板を回り、板ごとに画面全体の当たり判定を 2 回やる 【高・確実 (頻度) / 推測 (重さ)】

- 場所: `Shared/Core/Gui/GuiComponents.cpp:1529` (`isUnder`)、`:1550` (`update`)、`:1577-1583` (`refresh` と各 mouse コールバック)、`:1586` (コンストラクタで登録)
- 何が重いか:
  1. `Desktop::addGlobalMouseListener` で受けているので、**エディタのどこでマウスを動かしても** (区分の板の外でも、つまみをドラッグ中でも、ホイールでも) `update()` が走る。
  2. `update()` は登録済みの **全部の** `GuiCategoryBackdrop` を回る。板は `GuiCategoryLabel` が 1 つずつ持つメンバ (`GuiComponents.h:1570`) なので、共有部品 (AmpEnv / PitchEnv / TapSeq / StepEnv … 各 1 つ) × オペレーター数 × 作ったタブの数だけある。タブは遅延生成だが一度作ると残る (`PluginEditor.cpp` の `GuiLazy` / `peek()`) ので、触ったタブが増えるほど増える。FM タブ 1 枚で数十〜百程度 (推測)。
  3. 板ごとに `getLocalPoint(nullptr, …)` で親を根までたどる。
  4. 範囲に入った板では、さらに `Desktop::findComponentAt()` (全トップレベル窓 → 部品木を降りる) と `top->getComponentAt()` (部品木をもう一度降りる) をやる。部品木を降りる途中で、畳める枠の `hitTest` (下の P1-2) が文字の整形まで走らせる。
  5. `isVisible()` は自分の札しか見ないので、**裏に隠れたタブの板も範囲判定を通ってしまい**、4 の重い 2 回の当たり判定まで進んでから捨てられる。タブはどれも同じ場所に重なっているので、作ったタブの数だけ重い判定が増える。
  6. JUCE のグローバルマウスリスナーは、登録されているあいだ `Desktop` のタイマーでマウス位置を見張る (`juce_Desktop.cpp:238-246, 268-285`。止まっていれば 100ms、動いていれば 20ms ごとに偽の `mouseMove` を送る)。本物のマウスイベントにも乗るので、動かしているあいだは **ほぼ 2 倍** 走る。
  7. 実体は `static` の 1 個で、同じ DLL の別インスタンスのエディタの板も同じ表に入る。複数トラックでエディタを開いていると、全部の板を回る。
- 直し方の案 (効く順):
  - `isUnder` の最初で `b.isShowing()` を見る (隠れたタブ・畳んだ枠の中身を一発で落とせる)。**これだけでも大半が消える見込み (推測)**。
  - 重い当たり判定は「範囲に入った板」ごとではなく、**1 回のイベントにつき 1 回だけ** やる。`under = top->getComponentAt(...)` を先に一度求め、`under` から親をたどって最初に見つかった `GuiCategoryLabel`/板の親で決める、という逆向きの探し方にすると、板の数に比例しなくなる。
  - グローバルリスナーをやめ、各タブ (または `contentCanvas`) に `addMouseListener(this, true)` を付けて、自分の子の中のイベントだけ受ける。Desktop タイマーも止まる。
  - 表をエディタごとに持つ (static をやめる)。
  - `mouseDrag` では探さない (ドラッグ中は板が変わらなくても困らない)。

### P1-2. 畳める枠の `hitTest` / `mouseMove` / `paint` が、毎回 Font を作って文字を整形し直す 【中〜高・確実 (頻度) / 推測 (重さ)】

- 場所: `Shared/Core/Gui/GuiComponents.cpp:193` (`titleChipBounds`)、呼び元 `:238` (`hitTest`)、`:269` → `:283` (`mouseMove` → `updateTitleHover`)、`mouseDown`、`:132` (`paint`)
- 何が重いか: `titleChipBounds()` は呼ばれるたびに `juce::Font(FontOptions(...)).withStyle(bold)` を作り、`GlyphArrangement::getStringWidthInt()` を呼ぶ。JUCE 8 のこれは `GlyphArrangement` を作って `addLineOfText` で **整形 (shaping) をやり直す** (`juce_GlyphArrangement.h:323-345`)。配列の確保も伴う。
  - `hitTest` は JUCE がマウス移動のたびに部品木を降りるとき、枠の矩形に入っていれば必ず呼ばれる。畳める枠 (FM のオペレーターの列・RHYTHM のパッド・2686VFX の効果と変調) は列いっぱいの大きさなので、その上でマウスを動かすあいだずっと呼ばれる。P1-1 が同じ木をさらに 2 回 × 板の数だけ降りるので、それにも掛け算で乗る。
  - 枠の上 (子の無いところ) では `mouseMove` → `updateTitleHover` でもう一度。
- 直し方の案: 見出しの文字幅 (または帯の矩形そのもの) を、`setText` / `setFolded` / `resized` のときに計算してメンバへ持つ。Font も `static const` かメンバに。`hitTest` は矩形の比較だけにする。

### P1-3. 区分の板のホバーが変わるたび、区分の中身を丸ごと描き直す 【中・確実】

- 場所: `Shared/Core/Gui/GuiComponents.cpp:1564-1574` (`current->repaint()` を 2 回)
- 何が重いか: 板は区分の中身全体を覆う大きさで、その上にスライダー・コンボボックス・グラフ (エンベロープのグラフなど) が乗っている。板を `repaint()` すると、その矩形に重なる **上の部品も全部** 描き直しになる。大区分の板は小区分をまとめて覆うので、大区分と小区分の境をまたぐだけで大きな範囲が 2 回分 (離れた板と乗った板) 無効化される。区分の境を横切るたびなので、マウスを大きく動かすと連続して起きる。
- 直し方の案:
  - 色の差は小さい (`brighter(0.3f)`) ので、板全体ではなく **縁だけ** 明るくする (縁の帯 4 本だけ `repaint(rect)` する) 見せ方にすれば、中の部品は描き直さずに済む。
  - それでも面で見せたいなら、少なくとも大区分の板 (外側) の描き直しを避ける。いまは「いちばん小さい板」だけ色が変わるので、大→小の移動では大の板全体と小の板の両方を描き直している。

### P1-4. 枠を 1 つ畳む・開くたびに、エディタ全体の `resized()` が走る 【中・確実】

- 場所: `Shared/Core/Gui/GuiGroupFolds.cpp:40` (見出しクリック)、`:52` (全部開く/閉じる)、`:131` (状態の差し替え)。行き先は `2686V/Source/Core/Editor/PluginEditor.cpp:592-` の `resized()`
- 何が重いか: 畳んで変わるのは 1 タブの列の並びだけなのに、`ctx.editor.resized()` はエディタ全体を並べ直す。中では
  - 表に出ているタブの `layout()` (コメントのとおり「中でエンベロープのグラフまで作り直す」)、
  - **FX 欄の `fxGui->layout(content)` を毎回** (`:852`)、
  - プレビューの見出し 3 つに `Font` を作り直して `setFont` (`:655-660`。`setFont` は Label の描き直しを伴う)、
  - 全タブへ `tabNeedsLayout = true` (`:846`)。これで、畳んだのと関係ない裏のタブも、次に前へ出たときにもう一度全部並べ直しになる。
  が走る。2686VFX 側も同じ形 (`2686VFX/Source/Core/Editor/PluginEditor.cpp` の `resized()`)。
- クリック 1 回に 1 度なので体感に出るかはタブの重さ次第 (推測)。既存の区分の開閉も同じ経路なので 4.3.0 で新しく生まれた問題ではないが、畳みは「列を次々畳む」使い方になりやすく、回数が増える。
- 直し方の案: `GuiGroupFolds` にタブ自身の並べ直しを呼ぶコールバック (`onLayoutNeeded`) を渡し、タブの `layout(lastBounds)` だけを呼ぶ。2686VFX の FX 欄なら `fxGui->layout()` だけ。

### P1-5. 2686VFX の畳んだ枠の中身は、隠さずに 100000px 左へ置いているだけ 【低・確実】

- 場所: `2686VFX/Source/Gui/Fx/GuiFx.cpp:1269-1276`
- 何が重いか: 畳んだ枠の中身は `setVisible(false)` にせず、送り台の外へ動かしているだけ。描画は親の切り抜きで飛ばされるので描く重さは無いが、
  - 中身の `layoutBody` は畳んでいても毎回走る、
  - 中身にタイマーで描き直す部品 (グラフ・プレビュー) があれば、見えないまま更新を続ける (`repaint()` 自体は切り抜きで空になるが、値の計算は残る)、
  - 中身の区分の板は P1-1 の表に残り、毎回の範囲判定の対象になる (範囲外なので重い判定までは行かない)。
- 直し方の案: 2686V 側の `GuiScrollGroup::foldedChanged` (`GuiComponents.h:340`、`viewport.setVisible(!isFolded())`) と同じく、中身を `setVisible(false)` にする。並べ方を残したいなら、`layoutBody` は開いたときだけ走らせる。

### P1-6. 畳み状態の見張りが、パラメータが動くたびに呼ばれる 【低・確実】

- 場所: `Shared/Core/Gui/GuiGroupFolds.cpp:19` (`apvts.state.addListener`)、`:118`
- 何が重いか: `apvts.state` の根に付けているので、APVTS がパラメータの値を木へ書き戻すたび (オートメーション再生中はほぼ常時、メッセージスレッドのタイマー) に、作ったタブの数 + FX 欄ぶんの `GuiGroupFolds` が呼ばれる。中身は `Identifier` の比較だけ (コメントどおり、そのために `keyId` を持っている) なので軽い。音声スレッドを止める経路は見当たらない (`valueTreeRedirected` からは `triggerAsyncUpdate` だけ)。
- 直し方の案: 気になるなら、エディタ側で 1 つだけ付けて各 `GuiGroupFolds` へ配る。現状のままでも問題は小さい。

### P1-7. 部品のホバー色 (`GuiColor::Hover::face/border`) と `GuiHoverWatch` 【低・確実 — 問題なし寄り】

- 場所: `Shared/Core/Gui/GuiColor.h:229-239`、`Shared/Core/Gui/GuiComponents.h:144` (`GuiHoverWatch`)、各 LookAndFeel の `isMouseOver(true)`
- 見たこと:
  - `face()` / `border()` は `getPerceivedBrightness()` と `brighter/darker` (HSB 変換) を描くたびにやるが、乗っている部品 1 つぶんだけなので無視できる。
  - `GuiColor::Entry::get()` は `ensure()` の分岐 1 つ + 値の写しで、表 (`std::map`) は引かない。`ContentBgHover` の `brighter(0.3f)` も最初の 1 回だけ。**色の参照コストは問題ない。**
  - `GuiHoverWatch` は出入りのときに部品 1 つを描き直すだけで、範囲は適切。子 (スライダーの値の枠) への出入りで `mouseExit` + `mouseEnter` が続けて来て 2 回 `repaint()` になるが、JUCE がまとめるので実害は無い。
  - `isMouseOver(true)` はマウスソースを数個見るだけ。
- 直し方の案: 不要。

### P1-8. 押せる見出し (`GuiCategoryLabel`) と枠の見出しのホバー描き直し範囲 【低・確実】

- 場所: `Shared/Core/Gui/GuiComponents.cpp:1435` の `GuiCategoryLabel::mouseEnter/mouseExit` (見出し自身だけ `repaint()` — 適切)、`ColoredGroupComponent::updateTitleHover` (`:283`) の `repaint()`
- 何が重いか: 枠の見出しは帯 (縦 16px 程度) だけ色が変わるのに、`repaint()` が **枠全体** (列いっぱい) を無効化する。枠の上に並ぶ子 (スクロール欄の中身全部) も描き直しになる。見出しの出入りのたびなので頻度は低い。
- 直し方の案: `repaint(titleChipBounds().expanded(2).getSmallestIntegerContainer())` (影のぶんも含める) にする。P1-2 で矩形をメンバに持てばそのまま使える。

---

## 第 1 段階のまとめ (優先順)

1. **P1-1** — `isShowing()` を足す、当たり判定を 1 イベント 1 回にする。最も効く見込み。
2. **P1-2** — 見出しの矩形をキャッシュする。P1-1 の当たり判定の中にも乗っているので、両方直すと掛け算で減る。
3. **P1-3 / P1-8** — 描き直し範囲を縁・帯だけにする。
4. **P1-4** — 畳みではタブだけ並べ直す。

測り方の提案: Windows の Standalone で、FM タブ (OPM) と RHYTHM を開いた状態で、オペレーターの列の上をマウスで往復させ、Visual Studio の CPU 使用率プロファイラで `GuiCategoryBackdropHover::update` と `GlyphArrangement::addLineOfText` の占める割合を見る。次にタブを 5〜6 枚開いてから同じことをして、増え方 (タブ数に比例するか) を見ると P1-1 の 5 番を確かめられる。

---

## 第 2 段階: 共有 GUI ライブラリの描画全般

### P2-1. エンベロープのグラフは、鳴っている位置が動くたびに線をすべて組み直して描く (既定で 30fps) 【高・確実 (頻度) / 推測 (重さ)】

- 場所: `Shared/Core/Gui/GuiEnvelopeGraph.cpp:91-111` (`timerCallback` → `repaint()`)、`:325-539` (`paint`)、既定値 `Shared/Core/Gui/GuiHost.h:101` (`envLiveGraph = true`)、`:95` (`tapLiveGraph = true`)
- 何が重いか:
  - 鳴っている位置 (`phase` / `frac`) が変わると、グラフ **全体** を `repaint()` する。動いているのは縦線 1 本 (またはサスティンの帯) だけ。
  - `paint` は段 (phase) ごとに `juce::Path` を新しく作り、`resolution = max(10, 幅/2)` 点ぶん `curveFunc` (`std::function`。カーブを使うときは `CurveCore::process`) を呼んで線を組み、段ごとに `strokePath` (点線なら `createDashedStroke` も) する。SSG SW ENV[256] / PARAM ENV[256] のように段が多いものは、1 回の描画で段の数 (最大 256) だけ Path の確保と stroke が走る。
  - これが鍵を弾いているあいだ、FPS の設定 (既定 30、最大 120) で、**表に見えているグラフの数** (オペレーター 4 つ × 区分。さらに TARGET の大きな枠の写し、P2-2) だけ繰り返される。
  - 不透明な部品が 1 つも無い (P2-4) ので、グラフの矩形ぶん、背後のエディタの背景 (壁紙の拡大描画を含む)・枠・区分の板もそのたびに描かれる。
- 直し方の案:
  - 線 (段の Path を全部つないだもの) を、`currentPhases` / 大きさ / 拡大率 / 送り具合が変わったときだけ作ってメンバに持つ。`paint` での `strokePath` は色・線種ごとにまとめて数回にする。さらに軽くするなら、線をまるごと `juce::Image` へ焼いておき、`paint` は画像を置いて縦線を足すだけにする。
  - 鳴っている位置が変わったときは、前の縦線と新しい縦線の細い矩形 2 つだけ `repaint(rect)` する (サスティンの帯は帯の矩形だけ)。
  - `timerCallback` の頭で `isShowing()` を見て、見えていなければ何もしない (P2-5)。

### P2-2. TARGET の大きな枠は、10Hz で小さな枠の絵を写し、変わっていなくても毎回描き直す 【中・確実】

- 場所: `Shared/Gui/Components/TargetCell/TargetCell.cpp:101` (`startTimerHz(10)`)、`:105-113` (`timerCallback`)。写す側は `Shared/Core/Gui/GuiEnvelopeGraph.h:191-201` と `Shared/Gui/Components/WavePreview/WavePreview.h:74-86` (`copyFrom` の最後で無条件に `repaint()`)
- 何が重いか: 毎秒 10 回、見出しの `setText`、エンベロープの段の並び (`std::vector<PhaseDef>`。段ごとの `curveFunc` の `std::function` も写す) の丸写し、`setLiveSource` (`std::function` の写しと `timerCallback` の即時呼び出し)、波形の点列 (`m_points` / `m_mins` / `m_maxs` / `m_markers`) の丸写しをして、**変化の有無にかかわらず** グラフとプレビューを `repaint()` する。グラフの `paint` は P2-1 のとおり重いので、何も触っていなくても TARGET の枠の全描画が 10Hz で回り続ける。
- 直し方の案: 写し元に「版」(更新のたびに増やす整数) を持たせ、版が変わったときだけ写して描き直す。あるいはタイマーをやめ、写し元の更新 (`setEnvelope` / `setPoints`) から TARGET の枠へ知らせる形にする。

### P2-3. TAP SEQUENCER / TAP PARAM SEQUENCER は、いまの段が 1 つ進むたびに全部の段の表示を作り直す 【中・確実】

- 場所: `Shared/Gui/Components/TapParam/TapParam.cpp:525-541` (`timerCallback`)、`:289-332` (`rebindValue` / `rebindPair`)、`:478-518` (`refresh`)。`Shared/Gui/Components/TapSeq/TapSeq.cpp:361-373`、`:225-237`、`:314-` (`refresh`)
- 何が重いか: 鍵を弾いていまの段 (`cursor`) が変わっただけでも、値が書き換わったとき (`version`) と同じ `rebindValue()` を通る。中では
  - TAP PARAM は **全部の段 (最大 256)** について `pairList()` (段ごとに `std::vector` を確保) を取り、指し先の名前と値の文字列 (`valueText`) を `juce::String` で組み立て、`StringArray::joinIntoString` で 1 行にする。
  - P.TARGET のつまみの `setRange` / `setValue`、`textFromValueFunction` / `valueFromTextFunction` への `std::function` の作り直し、`updateText()`、コンボボックスの `setSelectedItemIndex` も毎回やり直す。段が進んでも選んでいる段 (`target`) は変わらないので、これらは要らない。
  - TAP SEQUENCER も全段ぶん `"S" + String(s)` の文字列を作り直す。
  - 4.3.0 で TAP PARAM SEQUENCER はオペレーター・パッドごとに分かれた (478e67ad) ので、1 音ごとに、オペレーターの数 × 作ったタブの数だけこれが走る。**隠れたタブ・閉じた区分でも走る** (`isShowing()` を見ていない)。
- 直し方の案: `cursor` だけが変わったとき (`version` が同じとき) は `list.cursor` / `bars.cursor` だけ書き換え、前の行と新しい行の矩形だけ `repaint` する。`rebindValue()` と全段の作り直しは `version` が変わったときだけにする。見えていないときは後回しでよい。

### P2-4. 不透明な部品が無く、小さな描き直しのたびにエディタの背景 (壁紙の拡大描画) から描き直す 【中・確実 (仕組み) / 推測 (重さ)】

- 場所: `setOpaque(true)` はリポジトリのどこにも無い (`setOpaque(false)` だけ)。背景は `2686V/Source/Core/Editor/PluginEditor.cpp:866-919` (`drawBg`)。2686VFX も同じ形。
- 何が重いか: JUCE は描き直す矩形について、いちばん近い不透明な祖先から下へ全部描く。不透明な部品が無いので、スライダー 1 本のホバー、グラフの 30fps、プレビューの 30Hz (P2-6) のような小さな描き直しのたびに、
  - エディタの `paint` → `drawBg` の `g.drawImage(backgroundImage, baseArea, placement)` (壁紙を **拡大・縮小しながら** 描く。Fit / Original では、ぼかし画像も同じく拡大描画) と、`fillAll` での半透明の重ね、
  - 透かしのロゴ (`mainIconImage`、アルファ付き)、タブ、枠 (`ColoredGroupComponent::paint` の角丸の塗り)、区分の板 (半透明の角丸)
  が、その矩形ぶん毎回描かれる。拡大しながらの画像描画は、等倍の転送よりかなり重い (推測)。
- 直し方の案:
  - 壁紙は、壁紙・置き方・エディタの大きさが変わったときに、エディタの大きさの `juce::Image` へ一度だけ (暗い重ねも含めて) 焼いておき、`drawBg` は `drawImageAt` で等倍に置くだけにする。これだけでも効く見込み (推測)。
  - 動きの多い部品 (エンベロープのグラフ、リアルタイムの波形、GENWAVE) は、矩形全体を塗りつぶせるなら `setOpaque(true)` にできる。いまは角丸・半透明で塗っているので、そのままでは不透明にできない (見た目が変わるので要相談)。

### P2-5. 見えていない部品のタイマーも全部回り続ける 【低〜中・確実】

- 場所: `Shared/Core/Gui/GuiEnvelopeGraph.cpp:91` (グラフ 1 枚ごと、10Hz〜FPS)、`Shared/Gui/Components/TapParam/TapParam.cpp:246` / `Shared/Gui/Components/TapSeq/TapSeq.cpp:198` (区分ごと、10Hz〜FPS)、`Shared/Gui/Components/StepEnv/StepEnv.cpp:331` (区分ごと、10Hz)、`Shared/Gui/Components/TargetCell/TargetCell.cpp:101` (10Hz)
- 何が重いか: どれも `juce::Timer` を部品ごとに持ち、作られてから消えるまで止まらない。タブは遅延生成だが一度作ると残るので、触ったタブが増えるほど裏で回るタイマーが増える (FM タブ 1 枚でグラフ・TAP 系が数十、推測)。鍵を弾いていないあいだの 1 回の中身は原子変数の読み比べだけで軽いが、合わせると毎秒数百〜千回単位のコールバックがメッセージスレッドに積まれる (推測)。弾いているあいだは P2-3 の重い処理が隠れた部品でも走る (確実)。P2-1 の `repaint()` は隠れた親で止まるので、こちらは重くない。
- 直し方の案: タブ (またはエディタ) に 1 つだけタイマーを持ち、表に出ているタブの部品だけを回す。最低でも各 `timerCallback` の頭で `if (!isShowing()) return;` とし、`visibilityChanged()` で止め・再開する。

### P2-6. リアルタイムの波形 3 本と GENWAVE は、変化が無くても 30Hz で描き直す 【低〜中・確実】

- 場所: `2686V/Source/Core/Editor/PluginEditor.cpp:1830-1871` (30Hz、`previewHz`) → `Shared/Core/Gui/GuiComponents.cpp:44-49` (`pushBuffer` は無条件に `repaint()`)、`:21-42` (`strokeWave`: 毎回 `juce::Path` を作って 200 点、補助線 10 本も毎回)。`Shared/Gui/Components/GenWave/GenWave.h:101` (`timerCallback() { repaint(); }`、33ms)
- 何が重いか:
  - 音が出ていない (中身がずっと 0) あいだも、L / M / R の 3 つを毎秒 30 回、背景から描き直す (P2-4 により壁紙の拡大描画込み)。
  - GENWAVE は波形を読み込んでいれば、**再生を止めていても** (`drawWave` のコメントどおり「止めていればそのコマのまま」) 33ms ごとに 3 段の Path を組み直して描く。
- 直し方の案:
  - `pushBuffer` で、前回と中身が同じ (無音なら全部 0) なら `repaint()` しない。200 点を `std::equal` で比べるのは描くよりずっと安い。
  - GENWAVE は再生中だけタイマーを回し、止めたら `stopTimer()`。
  - Path はメンバに持って `clear()` で使い回すと確保が減る (小さい)。

### P2-7. 2686VFX: 鍵盤の割り当てを自分で決めているとき、鍵を押すたびに変調の枠を丸ごと描き直す 【低・確実】

- 場所: `2686VFX/Source/Gui/Fx/GuiFx.cpp:2280-2322` (30Hz、`keyAssignTitleHz`) → `Shared/Core/Gui/GuiComponents.cpp:124-130` (`setTitleIdle` → 枠全体の `repaint()`)
- 何が重いか: 変化したときだけ描き直す作りなのは良いが、変わるのは見出しの帯の色だけなのに、縦長の枠全体 (中のスクロール欄の部品すべて) を描き直す。鍵の上げ下げのたびなので、速いフレーズでは連続する。P1-8 と同じ根。
- 直し方の案: P1-8 と同じく、見出しの帯の矩形だけ `repaint(rect)` する。

### P2-8. 波形プレビューの再計算は、値が変わったときだけ・読み込み中はまとめて 1 回 — 問題なし 【—・確実】

- 場所: `Shared/Core/Gui/GuiRefresh.h`、例 `Shared/Gui/Components/SsgHwEnv/SsgHwEnv.cpp:428-445`
- 見たこと: プレビューは `paint` では計算せず、値が変わったときに `WavePreviewSource` (出荷する DSP を回す) で点列を作り、プリセットの読み込み中は `GuiRefresh::defer` で溜めて 1 回にまとめている。つまみをドラッグしているあいだはドラッグの頻度で再計算が走るが、1 回の点数は小さい (推測)。見えていない区分でも計算するが、値が変わったときだけなので許容範囲。
- 直し方の案: 不要。気になるなら、見えていないときは「要再計算」の札だけ立て、`visibilityChanged()` で計算する。

### P2-9. GUI スレッドから音声スレッドを止めかねないロック — 見つからない 【—・確実】

- 見たこと: `CriticalSection` / `ScopedLock` / `std::mutex` / `SpinLock` / `MessageManagerLock` / `suspendProcessing` / `getCallbackLock` は、GUI・エディタ・共有 GUI ライブラリ・プロセッサのどこにも無い (`2686V/Source/Core/Processor/PluginProcessor.h:1084` のコメントのとおり、以前の `CriticalSection` は 3 枚回しの受け渡しへ置き換え済み)。
  - GUI ↔ 音声の受け渡しは、`StepTable` (`Shared/Core/Synth/StepTable.h`) の原子変数 + `version`、リアルタイム波形のリングバッファ + `realTimeWritePos` (acquire)、`LivePosition` の原子変数で、どれも待ちが無い。
  - `GuiGroupFolds` の `valueTreeRedirected` は、別スレッドから来ても `triggerAsyncUpdate` するだけ。
  - エディタの `parameterChanged` (音声スレッドから来ることがある) は、原子変数へ預けて `triggerAsyncUpdate` するだけ (`2686V/Source/Core/Editor/PluginEditor.cpp:1898-1915`)。
- 性能の観点では問題なし。リングバッファを読む途中で書き手と重なると波形が一部ちぎれて見えることはあり得るが、見た目だけの話。

### P2-10. コンボボックスは 1 個ごとに LookAndFeel を作る 【低・確実 (タブを開く時間のみ)】

- 場所: `Shared/Core/Gui/GuiComponents.h:896` (`class ComboBoxLF : public juce::LookAndFeel_V4`)、`:986` (`ComboBoxLF customLF;` を部品ごとのメンバに持つ)
- 何が重いか: `LookAndFeel_V4` は作るときに色の表を組むので、部品ごとに作るとタブを開く時間に効く。スライダーは同じ理由で `SharedResourcePointer<SliderLF>` に変えてある (コメント「画面 1 枚で数百個できるので、そこが起動時間として出ていた」) が、コンボボックスは部品ごとのまま。部品ごとにフォント (`selectedFont` / `dropdownFont`) を変えられるようにしているのが理由と思われる。動いているあいだの重さには関係しない。
- 直し方の案: フォントを LookAndFeel ではなく `GuiComboBox` 側に持ち、`getComboBoxFont(box)` で部品から引く形にして、LookAndFeel を共有にする。

---

## 全体の優先順 (第 1・第 2 段階を合わせて)

1. **P1-1** (区分の板のホバー判定) — マウスを動かすだけで、作ったタブの数に比例して重くなる。`isShowing()` を足すだけでも大きく減る見込み。
2. **P2-1** (エンベロープのグラフの描き直し) — 既定の設定のまま、鍵を弾くだけで 30fps × 見えているグラフの数だけ、線の組み直しと stroke が走る。線のキャッシュと、縦線の矩形だけの描き直し。
3. **P1-2** (枠の見出しの文字幅を毎回測る) — P1-1 の当たり判定にも乗っている。
4. **P2-4** (不透明な部品が無く、壁紙を毎回拡大描画) — P2-1 / P2-6 / ホバーのすべてに掛け算で効く。壁紙を等倍の画像に焼いておくだけでも効く見込み。
5. **P2-3** (TAP 系の全段作り直し) — 1 音ごと × オペレーター数 × タブ数。`cursor` だけの変化を分ける。
6. **P2-2** (TARGET の枠の 10Hz 無条件写し) — 何もしていなくても回る。版を持たせる。
7. **P1-3 / P1-8 / P2-7** (描き直し範囲を帯・縁だけに)、**P1-4** (畳みでタブだけ並べ直す)、**P2-5 / P2-6** (見えていない・変化の無いときに止める)。

測り方の提案 (第 2 段階ぶん):
- P2-1: SETTINGS で ENV のライブ表示を 30 / 120 fps / OFF に切り替え、OPM で 4 オペレーターのエンベロープを開いた状態で鍵を押しっぱなしにしたときの、メッセージスレッドの CPU を比べる。プロファイラでは `GuiEnvelopeGraph::paint` と stroke の割合を見る。
- P2-4: 壁紙あり (Fit) と無しで、上と同じ操作の CPU を比べる。差が大きければ P2-4 が効いている。
- P2-3: TAP PARAM SEQUENCER の STEPS を 256 にして各段に組を入れ、FM タブを数枚開いてから速いフレーズを弾き、`GuiComponentTapParam::refresh` の割合を見る。
