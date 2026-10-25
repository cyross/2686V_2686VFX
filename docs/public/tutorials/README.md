# チュートリアルの動画・絵の置き場所

マニュアルの「チュートリアル」の章から見に行くフォルダです。

| 種類 | 拡張子 | 貼ったときの出方 |
| --- | --- | --- |
| 動画 | .mp4 / .webm | 再生ボタン付き。自動では流れない |
| 動く絵 | .gif | そのまま動く |
| 静止画 | .png / .jpg / .webp | そのまま出す |

## 貼り方

1. このフォルダへファイルを置く (例: `opna-first-sound.mp4`)
2. チュートリアルのページ (`docs/src/content/docs/tutorials/` の .mdx) で、
   名前だけを書いて貼る

```mdx
import TutorialMedia from '../../../components/TutorialMedia.astro';

<TutorialMedia src="opna-first-sound.mp4" caption="OPNA で最初の音を出す" />
```

英語版のページ (`docs/src/content/docs/en/tutorials/`) は 1 段深いので、
読み込みは `'../../../../components/TutorialMedia.astro'` になります。
動画や絵は同じものを使えます。

ページのひな形は `docs/src/content/docs/tutorials/_template.mdx` にあります
(名前が _ で始まるのでサイトには出ません)。

## 大きさの目安

GitHub Pages で配るので、1 ファイル 100 MB を超えるものは置けません。
動画は 1280×720・30fps 程度で、数 MB〜数十 MB に収めるのが目安です。
