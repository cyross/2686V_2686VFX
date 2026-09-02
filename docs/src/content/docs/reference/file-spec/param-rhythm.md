---
title: リズム チャンネル
description: .param.rhythm.json の中身
sidebar:
  order: 42
---

チャンネル 1 つぶんの設定をまとめたファイルです。

| | |
| --- | --- |
| **拡張子** | `.param.rhythm.json` / `.param.rhythm.yaml` |
| **印（format）** | `rhythm` |
| **版（version）** | `1` |
| **書き出し・読み込み** | UTILITY の [EX] / [IM] |
| **音源の説明** | [rhythm](/2686V_2686VFX/chips/rhythm/) |

## かたち

```json
{
  "format": "rhythm",
  "version": 1,
  "values": { ... }
}
```

読む側は根の `format` を見て、合わないファイルは開きません。**書かれていない
項目はそのままの値が残ります**（初期値へは戻りません）。

## チャンネルの中の入れ子

| 鍵 | 中身 |
| --- | --- |
| `level` | チャンネルの音量。`level` ひとつだけを持ちます |
| `unison` | [unison](/2686V_2686VFX/reference/file-spec/unison/) と同じ |

## `pads` の並び

パッド 8 つぶんが並びます。それぞれ次を持ちます。

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `filePath` | ? | — | — |
| `vol` | 小数 | 0 〜 10 | 1 |
| `pan` | 小数 | 0 〜 1 | 0.5 |
| `oneShot` | 真偽 | false / true | true |
| `note` | 整数 | 0 〜 127 | 61 |
| `tone` | 小数 | 0 〜 1 | 1 |
| `noise` | 小数 | 0 〜 1 | 1 |
| `noiseFreq` | 小数 | 0 〜 30000 | 12000 |
| `mix` | 小数 | 0 〜 1 | 0 |
| `pcmOffset` | 小数 | 0 〜 2000 | 0 |
| `pcmRatio` | 小数 | 0.05 〜 1 | 1 |
| `loopPointEnable` | 真偽 | false / true | false |
| `loopPointStart` | 小数 | 0 〜 0.999999 | 0 |
| `loopPointEnd` | 小数 | 1e-06 〜 1 | 1 |

さらに、次の入れ子を持ちます。

| 鍵 | 中身 |
| --- | --- |
| `fix` | 音程の固定。`enable` と `freq` を持ちます |
| `ampEnv` | [ampEnv](/2686V_2686VFX/reference/file-spec/ampenv/) と同じ |
| `pitchEnv` | [pitchEnv](/2686V_2686VFX/reference/file-spec/pitchenv/) と同じ |
| `ssgHwEnv` | [ssgHwEnv](/2686V_2686VFX/reference/file-spec/ssghwenv/) と同じ |
| `ssgSwEnv` | [ssgSwEnv](/2686V_2686VFX/reference/file-spec/ssgswenv/) と同じ |
| `ssgSwEnv11` | [ssgSwEnv11](/2686V_2686VFX/reference/file-spec/ssgswenv11/) と同じ |
| `ssgSwPEnv11` | [ssgSwPEnv11](/2686V_2686VFX/reference/file-spec/ssgswpenv11/) と同じ |
| `mulDetune` | [mulDetune](/2686V_2686VFX/reference/file-spec/detune/) と同じ |
| `lfo` | [lfo](/2686V_2686VFX/reference/file-spec/lfoopzx7/) と同じ |
| `qualityPcm` | [qualityPcm](/2686V_2686VFX/reference/file-spec/pcmquality/) と同じ |

:::note
`pads` の中の**ファイルの場所**（`filePath` / `pcmFile` /
`wtFile` / `wt2File`）は、`Samples` フォルダからの相対でも書けます。
絶対パスで書かれていればそのまま使います。
:::

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
