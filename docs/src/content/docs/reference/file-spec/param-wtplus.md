---
title: 波形メモリ + チャンネル
description: .param.wtplus.json の中身
sidebar:
  order: 39
---

チャンネル 1 つぶんの設定をまとめたファイルです。

| | |
| --- | --- |
| **拡張子** | `.param.wtplus.json` / `.param.wtplus.yaml` |
| **表記（format）** | `wtPlus` |
| **版（version）** | `1` |
| **書き出し・読み込み** | UTILITY の [EX] / [IM] |
| **音源の説明** | [wtplus](/2686V_2686VFX/chips/wtplus/) |

## かたち

```json
{
  "format": "wtPlus",
  "version": 1,
  "values": { ... }
}
```

読む側は根の `format` を見て、合わないファイルは開きません。**書かれていない
項目はそのままの値が残ります**（初期値へは戻りません）。

## チャンネルのつまみ

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `slot` | 整数 | 0 〜 31 | 0 |
| `interpolate` | 真偽 | false / true | true |
| `steps` | 整数 | 0 〜 10（選択肢の番号） | 0 |

## チャンネルの中の入れ子

| 鍵 | 中身 |
| --- | --- |
| `level` | チャンネルの音量。`level` ひとつだけを持ちます |
| `wtMod` | [wtMod](/2686V_2686VFX/reference/file-spec/wtmod/) と同じ |
| `fix` | 音程の固定。`enable` と `freq` を持ちます |
| `ampEnv` | [ampEnv](/2686V_2686VFX/reference/file-spec/ampenv/) と同じ |
| `pitchEnv` | [pitchEnv](/2686V_2686VFX/reference/file-spec/pitchenv/) と同じ |
| `ssgHwEnv` | [ssgHwEnv](/2686V_2686VFX/reference/file-spec/ssghwenv/) と同じ |
| `ssgSwEnv` | [ssgSwEnv](/2686V_2686VFX/reference/file-spec/ssgswenv/) と同じ |
| `ssgSwEnv11` | [ssgSwEnv11](/2686V_2686VFX/reference/file-spec/ssgswenv11/) と同じ |
| `ssgSwPEnv11` | [ssgSwPEnv11](/2686V_2686VFX/reference/file-spec/ssgswpenv11/) と同じ |
| `mulDetune` | [mulDetune](/2686V_2686VFX/reference/file-spec/detune/) と同じ |
| `lfo` | [lfo](/2686V_2686VFX/reference/file-spec/lfoopzx7/) と同じ |
| `quality` | [quality](/2686V_2686VFX/reference/file-spec/quality/) と同じ |
| `unison` | [unison](/2686V_2686VFX/reference/file-spec/unison/) と同じ |

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
