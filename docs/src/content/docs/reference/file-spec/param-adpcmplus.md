---
title: ADPCM+ チャンネル
description: .param.adpcmplus.json の中身
sidebar:
  order: 42
---

チャンネル 1 つぶんの設定をまとめたファイルです。**PCM 32 本ぶんの設定が
そのまま入ります**。

| | |
| --- | --- |
| **拡張子** | `.param.adpcmplus.json` / `.param.adpcmplus.yaml` |
| **表記（format）** | `adpcmPlus` |
| **版（version）** | `1` |
| **書き出し・読み込み** | UTILITY の [EX] / [IM] |
| **音源の説明** | [adpcmplus](/2686V_2686VFX/chips/adpcmplus/) |

## かたち

```json
{
  "format": "adpcmPlus",
  "version": 1,
  "values": {
    "slot": 0,
    "slot0": { "filePath": "...", "pan": 0.5, ... },
    "slot1": { ... },
    "slot31": { ... }
  }
}
```

読む側は根の `format` を見て、合わないファイルは開きません。**書かれていない
項目はそのままの値が残ります**（初期値へは戻りません）。

## チャンネルのつまみ

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `slot` | 整数 | 0 〜 31 | 0 |

鳴らす PCM の番号です。画面で選んでいる番号（TGT）は、音に関わらないので
入りません。

## PCM 1 本ぶん — `slot0` 〜 `slot31`

`slot` に続けて番号を付けた鍵が 32 個並びます。中身はどれも同じ形です。

| 鍵 | 型 | 範囲 | 初期値 |
| --- | --- | --- | ---: |
| `filePath` | 文字列 | — | — |
| `tone` | 小数 | 0 〜 1 | 1 |
| `noise` | 小数 | 0 〜 1 | 1 |
| `noiseFreq` | 小数 | 0 〜 30000 | 12000 |
| `mix` | 小数 | 0 〜 1 | 0 |
| `pan` | 小数 | 0 〜 1 | 0.5 |
| `speed` | 小数 | 0.0001 〜 100 | 1 |
| `pcmOffset` | 小数 | 0 〜 2000 | 0 |
| `pcmRatio` | 小数 | 0.05 〜 1 | 1 |
| `loop` | 真偽 | false / true | true |
| `loopPointEnable` | 真偽 | false / true | false |
| `loopPointStart` | 小数 | 0 〜 0.999999 | 0 |
| `loopPointEnd` | 小数 | 1e-06 〜 1 | 1 |
| `loopCount` | 整数 | 0 〜 3000 | 0 |

`filePath` は素材そのものではなく、**その場所**です。読み直すときはここから
読みます。空なら、その枠は空のままになります。

## チャンネルの中の入れ子

| 鍵 | 中身 |
| --- | --- |
| `level` | チャンネルの音量と再生遅延。`level` と `delay` を持ちます |
| `fix` | 音程の固定。`enable` と `freq` を持ちます |
| `ampEnv` | [ampEnv](/2686V_2686VFX/reference/file-spec/ampenv/) と同じ |
| `pitchEnv` | [pitchEnv](/2686V_2686VFX/reference/file-spec/pitchenv/) と同じ |
| `ssgHwEnv` | [ssgHwEnv](/2686V_2686VFX/reference/file-spec/ssghwenv/) と同じ |
| `ssgHwPEnv` | [ssgHwEnv](/2686V_2686VFX/reference/file-spec/ssghwenv/) と同じ形 |
| `ssgSwEnv` | [ssgSwEnv](/2686V_2686VFX/reference/file-spec/ssgswenv/) と同じ |
| `ssgSwEnv11` | [ssgSwEnv11](/2686V_2686VFX/reference/file-spec/ssgswenv11/) と同じ |
| `ssgSwPEnv11` | [ssgSwPEnv11](/2686V_2686VFX/reference/file-spec/ssgswpenv11/) と同じ |
| `mulDetune` | [mulDetune](/2686V_2686VFX/reference/file-spec/detune/) と同じ |
| `lfo` | [lfo](/2686V_2686VFX/reference/file-spec/lfoopzx7/) と同じ |
| `qualityPcm` | [qualityPcm](/2686V_2686VFX/reference/file-spec/pcmquality/) と同じ |
| `unison` | [unison](/2686V_2686VFX/reference/file-spec/unison/) と同じ |
| `wtMod` | [wtMod](/2686V_2686VFX/reference/file-spec/wtmod/) と同じ |
| `wtAmpMod` | [wtMod](/2686V_2686VFX/reference/file-spec/wtmod/) と同じ形 |

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
