---
title: OPL チャンネル
description: .param.opl.json の中身
sidebar:
  order: 32
---

チャンネル 1 つぶんの設定をまとめたファイルです。

| | |
| --- | --- |
| **拡張子** | `.param.opl.json` / `.param.opl.yaml` |
| **表記（format）** | `opl` |
| **版（version）** | `1` |
| **書き出し・読み込み** | UTILITY の [EX] / [IM] |
| **音源の説明** | [opl](/2686V_2686VFX/chips/opl/) |

## かたち

```json
{
  "format": "opl",
  "version": 1,
  "values": { ... }
}
```

読む側は根の `format` を見て、合わないファイルは開きません。**書かれていない
項目はそのままの値が残ります**（初期値へは戻りません）。

## チャンネルのつまみ

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `alg` | 整数 | 1 〜 2（選択肢の番号 + 1） | 1 |
| `feedback` | 整数 | 0 〜 7 | 0 |

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

オペレータ 2 つぶんが並びます。それぞれ次を持ちます。

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `mul` | 整数 | 1 〜 16（選択肢の番号 + 1） | 2 |
| `ar` | 整数 | 0 〜 15 | 15 |
| `dr` | 整数 | 0 〜 15 | 15 |
| `sl` | 整数 | 0 〜 15 | 0 |
| `rr` | 整数 | 0 〜 15 | 15 |
| `tl` | 整数 | 0 〜 63 | 0 |
| `ksr` | 真偽 | false / true | false |
| `ksl` | 整数 | 1 〜 4（選択肢の番号 + 1） | 1 |
| `egType` | 真偽 | false / true | true |
| `bypass` | 真偽 | false / true | false |
| `sus` | 真偽 | false / true | false |
| `kor` | 真偽 | false / true | false |
| `xof` | 真偽 | false / true | false |
| `mask` | 真偽 | false / true | false |
| `eg` | 整数 | 1 〜 4（選択肢の番号 + 1） | 1 |
| `am` | 真偽 | false / true | false |
| `amd` | 小数 | 0.1 〜 10 | 1 |
| `ams` | 小数 | 1 〜 20 | 3.7 |
| `vib` | 真偽 | false / true | false |
| `pmd` | 小数 | 1 〜 100 | 7 |
| `pms` | 小数 | 1 〜 20 | 6.4 |

さらに、次の入れ子を持ちます。

| 鍵 | 中身 |
| --- | --- |
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
