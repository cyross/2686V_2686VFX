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
