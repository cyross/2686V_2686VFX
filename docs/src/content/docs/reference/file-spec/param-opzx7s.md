---
title: OPZX7 チャンネル
description: .param.opzx7s.json の中身
sidebar:
  order: 35
---

チャンネル 1 つぶんの設定をまとめたファイルです。

| | |
| --- | --- |
| **拡張子** | `.param.opzx7s.json` / `.param.opzx7s.yaml` |
| **表記（format）** | `opzx7` |
| **版（version）** | `1` |
| **書き出し・読み込み** | UTILITY の [EX] / [IM] |
| **音源の説明** | [opzx7](/2686V_2686VFX/chips/opzx7/) |

## かたち

```json
{
  "format": "opzx7",
  "version": 1,
  "values": { ... }
}
```

読む側は根の `format` を見て、合わないファイルは開きません。**書かれていない
項目はそのままの値が残ります**（初期値へは戻りません）。

## チャンネルのつまみ

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `algMode` | int | — | — |
| `alg` | 整数 | 1 〜 130（選択肢の番号 + 1） | 16 |
| `feedback1` | 小数 | -8 〜 8 | 0 |
| `feedback2` | 小数 | -8 〜 8 | 0 |
| `feedback3` | 小数 | -8 〜 8 | 0 |
| `feedback4` | 小数 | -8 〜 8 | 0 |
| `feedback5` | 小数 | -8 〜 8 | 0 |
| `feedback6` | 小数 | -8 〜 8 | 0 |
| `feedback7` | 小数 | -8 〜 8 | 0 |
| `feedback8` | 小数 | -8 〜 8 | 0 |
| `panpotEnable` | 真偽 | false / true | false |
| `panpot` | 整数 | 0 〜 31 | 15 |

## チャンネルの中の入れ子

| 鍵 | 中身 |
| --- | --- |
| `level` | チャンネルの音量。`level` ひとつだけを持ちます |
| `algMatrixComp` | アルゴリズムマトリックス。`ops` の並びに、`isCarrier` と `mod` / `fbMod` の並びを持ちます |
| `ssgHwEnv` | [ssgHwEnv](/2686V_2686VFX/reference/file-spec/ssghwenv/) と同じ |
| `ssgSwEnv11` | [ssgSwEnv11](/2686V_2686VFX/reference/file-spec/ssgswenv11/) と同じ |
| `glLfo` | [glLfo](/2686V_2686VFX/reference/file-spec/lfoopzx7/) と同じ |
| `quality` | [quality](/2686V_2686VFX/reference/file-spec/quality/) と同じ |
| `unison` | [unison](/2686V_2686VFX/reference/file-spec/unison/) と同じ |
| `ampEnv` | [ampEnv](/2686V_2686VFX/reference/file-spec/ampenv/) と同じ |
| `ssgSwPEnv11` | [ssgSwPEnv11](/2686V_2686VFX/reference/file-spec/ssgswpenv11/) と同じ |
| `wtMod` | [wtMod](/2686V_2686VFX/reference/file-spec/wtmod/) と同じ |

## `ops` の並び

オペレータ 8 つぶんが並びます。それぞれ次を持ちます。

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `rgEn` | 真偽 | false / true | true |
| `rgAr` | 整数 | 0 〜 31 | 31 |
| `rgD1r` | 整数 | 0 〜 31 | 31 |
| `rgD1l` | 整数 | 0 〜 15 | 0 |
| `rgD2r` | 整数 | 0 〜 31 | 0 |
| `rgRr` | 整数 | 0 〜 31 | 31 |
| `rgTl` | 整数 | 0 〜 63 | 0 |
| `ar` | 小数 | 0.001 〜 10 | 0.001 |
| `d1r` | 小数 | 0.001 〜 10 | 0.001 |
| `d1l` | 小数 | 0 〜 1 | 1 |
| `d2r` | 小数 | 0.001 〜 10 | 0.001 |
| `rr` | 小数 | 0.001 〜 10 | 0.001 |
| `tl` | 小数 | 0 〜 1 | 1 |
| `ksEn` | 真偽 | false / true | false |
| `ksMode` | 整数 | 1 〜 3（選択肢の番号 + 1） | 1 |
| `ksrMA7` | 真偽 | false / true | false |
| `kslMA7` | 整数 | 1 〜 4（選択肢の番号 + 1） | 1 |
| `ksrOPZ` | 整数 | 0 〜 3 | 0 |
| `kslOPZ` | 整数 | 0 〜 99 | 0 |
| `ksBp` | 整数 | 0 〜 127 | 60 |
| `ksLc` | 整数 | 1 〜 4（選択肢の番号 + 1） | 1 |
| `ksRc` | 整数 | 1 〜 4（選択肢の番号 + 1） | 1 |
| `ksLd` | 小数 | 0 〜 1 | 0 |
| `ksRd` | 小数 | 0 〜 1 | 0 |
| `ksRs` | 整数 | 0 〜 7 | 0 |
| `bypass` | 真偽 | false / true | false |
| `sus` | 真偽 | false / true | false |
| `xof` | 真偽 | false / true | false |
| `kor` | 真偽 | false / true | false |
| `mask` | 真偽 | false / true | false |
| `ws` | 整数 | 1 〜 73（選択肢の番号 + 1） | 1 |
| `pcmFile` | ? | — | — |
| `wtFile` | ? | — | — |
| `wt2File` | ? | — | — |
| `pcmOffset` | 小数 | 0 〜 2000 | 0 |
| `pcmRatio` | 小数 | 0.05 〜 1 | 1 |
| `loopPointEnable` | 真偽 | false / true | false |
| `loopPointStart` | 小数 | 0 〜 0.999999 | 0 |
| `loopPointEnd` | 小数 | 1e-06 〜 1 | 1 |
| `se` | 整数 | 1 〜 16（選択肢の番号 + 1） | 1 |
| `seFreq` | 小数 | 0.1 〜 30 | 1 |

さらに、次の入れ子を持ちます。

| 鍵 | 中身 |
| --- | --- |
| `mulDetune` | [mulDetune](/2686V_2686VFX/reference/file-spec/detune/) と同じ |
| `fix` | 音程の固定。`enable` と `freq` を持ちます |
| `lfo` | [lfo](/2686V_2686VFX/reference/file-spec/lfoopzx7/) と同じ |
| `pitchEnv` | [pitchEnv](/2686V_2686VFX/reference/file-spec/pitchenv/) と同じ |
| `ssgSwEnv` | [ssgSwEnv](/2686V_2686VFX/reference/file-spec/ssgswenv/) と同じ |
| `ssgSwEnv11` | [ssgSwEnv11](/2686V_2686VFX/reference/file-spec/ssgswenv11/) と同じ |
| `ssgSwPEnv11` | [ssgSwPEnv11](/2686V_2686VFX/reference/file-spec/ssgswpenv11/) と同じ |

:::note
`ops` の中の**ファイルの場所**（`filePath` / `pcmFile` /
`wtFile` / `wt2File`）は、`Samples` フォルダからの相対でも書けます。
絶対パスで書かれていればそのまま使います。
:::

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
