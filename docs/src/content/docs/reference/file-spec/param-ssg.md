---
title: SSG チャンネル
description: .param.ssg.json の中身
sidebar:
  order: 36
---

チャンネル 1 つぶんの設定をまとめたファイルです。

| | |
| --- | --- |
| **拡張子** | `.param.ssg.json` / `.param.ssg.yaml` |
| **表記（format）** | `ssg` |
| **版（version）** | `1` |
| **書き出し・読み込み** | UTILITY の [EX] / [IM] |
| **音源の説明** | [ssg](/2686V_2686VFX/chips/ssg/) |

## かたち

```json
{
  "format": "ssg",
  "version": 1,
  "values": { ... }
}
```

読む側は根の `format` を見て、合わないファイルは開きません。**書かれていない
項目はそのままの値が残ります**（初期値へは戻りません）。

## チャンネルのつまみ

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `wave` | 整数 | 0 〜 1（選択肢の番号） | 0 |
| `tone` | 小数 | 0 〜 1 | 1 |
| `noise` | 小数 | 0 〜 1 | 1 |
| `noiseFreq` | 小数 | 0 〜 30000 | 12000 |
| `noiseOnNote` | 真偽 | false / true | false |
| `mix` | 小数 | 0 〜 1 | 0 |
| `dutyMode` | 整数 | 0 〜 1（選択肢の番号） | 0 |
| `dutyPreset` | 整数 | 0 〜 8（選択肢の番号） | 0 |
| `dutyVar` | 小数 | 0 〜 0.5 | 0.5 |
| `dutyInvert` | 真偽 | false / true | false |
| `dutyFc` | 真偽 | false / true | false |
| `dutyFcFluc` | 小数 | 0 〜 1 | 0.2 |
| `dutyMoreFc` | 真偽 | false / true | false |
| `dutyMoreNes` | 真偽 | false / true | false |
| `triKeyTrack` | 真偽 | false / true | true |
| `triFreq` | 小数 | 0 〜 8000 | 440 |
| `triPeak` | 小数 | 0 〜 1 | 0.5 |

## チャンネルの中の入れ子

| 鍵 | 中身 |
| --- | --- |
| `level` | チャンネルの音量と再生遅延。`level` と `delay` を持ちます |
| `ssgHwEnv` | [ssgHwEnv](/2686V_2686VFX/reference/file-spec/ssghwenv/) と同じ |
| `fix` | 音程の固定。`enable` と `freq` を持ちます |
| `ampEnv` | [ampEnv](/2686V_2686VFX/reference/file-spec/ampenv/) と同じ |
| `pitchEnv` | [pitchEnv](/2686V_2686VFX/reference/file-spec/pitchenv/) と同じ |
| `ssgSwEnv` | 3.6.3 までの 6 段の SSG SW AMP ENV。4.0.0 では書き出さない。読み込むと `ssgSwEnv256` へ写す（[ssgSwEnv](/2686V_2686VFX/reference/file-spec/ssgswenv/)） |
| `ssgSwEnv256` | [ssgSwEnv256](/2686V_2686VFX/reference/file-spec/ssgswenv256/) と同じ |
| `ssgSwPEnv256` | [ssgSwPEnv256](/2686V_2686VFX/reference/file-spec/ssgswpenv256/) と同じ |
| `dutyEnv` | DUTY RATIO ENVELOPE。鍵は [ssgSwEnv256](/2686V_2686VFX/reference/file-spec/ssgswenv256/) と同じで、値はデューティ比 (%)（4.0.0 から） |
| `peakEnv` | PEAK RATIO ENVELOPE。鍵は [ssgSwEnv256](/2686V_2686VFX/reference/file-spec/ssgswenv256/) と同じで、値は三角波の頂点 0〜1（4.0.0 から） |
| `mulDetune` | [mulDetune](/2686V_2686VFX/reference/file-spec/detune/) と同じ |
| `lfo` | [lfo](/2686V_2686VFX/reference/file-spec/lfoopzx7/) と同じ |
| `quality` | [quality](/2686V_2686VFX/reference/file-spec/quality/) と同じ |
| `unison` | [unison](/2686V_2686VFX/reference/file-spec/unison/) と同じ |
| `wtMod` | [wtMod](/2686V_2686VFX/reference/file-spec/wtmod/) と同じ |

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
