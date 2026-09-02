---
title: OPNA チャンネル
description: .param.opna.json の中身
sidebar:
  order: 30
---

チャンネル 1 つぶんの設定をまとめたファイルです。

| | |
| --- | --- |
| **拡張子** | `.param.opna.json` / `.param.opna.yaml` |
| **印（format）** | `opna` |
| **版（version）** | `1` |
| **書き出し・読み込み** | UTILITY の [EX] / [IM] |
| **音源の説明** | [opna](/2686V_2686VFX/chips/opna/) |

## かたち

```json
{
  "format": "opna",
  "version": 1,
  "values": { ... }
}
```

読む側は根の `format` を見て、合わないファイルは開きません。**書かれていない
項目はそのままの値が残ります**（初期値へは戻りません）。

## チャンネルのつまみ

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `alg` | 整数 | 1 〜 8（選択肢の番号 + 1） | 1 |
| `feedback` | 整数 | 0 〜 7 | 0 |
| `pan` | 整数 | -1 〜 1 | 0 |
| `lfoFreq` | 整数 | 0 〜 16383 | 0 |
| `lfoShape` | 整数 | 1 〜 6（選択肢の番号 + 1） | 1 |
| `lfoAmSmRt` | 小数 | 0.005 〜 0.5 | 0.005 |
| `lfoSyncDelay` | 整数 | 0 〜 255 | 0 |
| `lfoPm` | 真偽 | false / true | false |
| `lfoPms` | 整数 | 0 〜 15 | 0 |
| `lfoPmd` | 整数 | -127 〜 127 | 0 |
| `lfoAm` | 真偽 | false / true | false |
| `lfoAmd` | 整数 | -127 〜 127 | 0 |

## チャンネルの中の入れ子

| 鍵 | 中身 |
| --- | --- |
| `level` | チャンネルの音量。`level` ひとつだけを持ちます |
| `ssgHwEnv` | [ssgHwEnv](/2686V_2686VFX/reference/file-spec/ssghwenv/) と同じ |
| `ssgSwEnv11` | [ssgSwEnv11](/2686V_2686VFX/reference/file-spec/ssgswenv11/) と同じ |
| `quality` | [quality](/2686V_2686VFX/reference/file-spec/quality/) と同じ |
| `unison` | [unison](/2686V_2686VFX/reference/file-spec/unison/) と同じ |
| `ampEnv` | [ampEnv](/2686V_2686VFX/reference/file-spec/ampenv/) と同じ |
| `ssgSwPEnv11` | [ssgSwPEnv11](/2686V_2686VFX/reference/file-spec/ssgswpenv11/) と同じ |
| `wtMod` | [wtMod](/2686V_2686VFX/reference/file-spec/wtmod/) と同じ |

## `ops` の並び

オペレータ 4 つぶんが並びます。それぞれ次を持ちます。

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `mul` | 整数 | 1 〜 16（選択肢の番号 + 1） | 2 |
| `dt` | 整数 | 1 〜 8（選択肢の番号 + 1） | 1 |
| `ar` | 整数 | 0 〜 31 | 31 |
| `dr` | 整数 | 0 〜 31 | 31 |
| `sl` | 整数 | 0 〜 15 | 0 |
| `sr` | 整数 | 0 〜 31 | 0 |
| `rr` | 整数 | 0 〜 15 | 15 |
| `tl` | 整数 | 0 〜 127 | 0 |
| `ks` | 整数 | 1 〜 4（選択肢の番号 + 1） | 1 |
| `freq` | 整数 | 1 〜 8（選択肢の番号 + 1） | 1 |
| `syncDelay` | 整数 | 0 〜 255 | 0 |
| `pm` | 真偽 | false / true | false |
| `pms` | 整数 | 1 〜 8（選択肢の番号 + 1） | 1 |
| `am` | 真偽 | false / true | false |
| `ams` | 整数 | 1 〜 4（選択肢の番号 + 1） | 1 |
| `n88Ams` | 整数 | 0 〜 15 | 0 |
| `ssgEnv` | 整数 | 1 〜 16（選択肢の番号 + 1） | 1 |
| `ssgEnvFreq` | 小数 | 0.1 〜 30 | 1 |
| `bypass` | 真偽 | false / true | false |
| `xof` | 真偽 | false / true | false |
| `kor` | 真偽 | false / true | false |
| `mask` | 真偽 | false / true | false |

さらに、次の入れ子を持ちます。

| 鍵 | 中身 |
| --- | --- |
| `fix` | 音程の固定。`enable` と `freq` を持ちます |
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
