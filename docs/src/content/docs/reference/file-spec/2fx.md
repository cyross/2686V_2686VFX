---
title: エフェクトの設定
description: .2fx.json の中身
sidebar:
  order: 50
---

FX の設定をまとめたファイルです。

| | |
| --- | --- |
| **拡張子** | `.2fx.json` / `.2fx.yaml` |
| **表記（format）** | `fxParam` |
| **版（version）** | `1` |
| **書き出し・読み込み** | FX の [EX] / [IM] |

## かたち

```json
{
  "format": "fxParam",
  "version": 1,
  "values": {
    "bypass": false,
    "tremolo": { "bypass": true, "rate": 5.0, "depth": 1.0, "mix": 0.0 },
    "…": {}
  }
}
```

根の `bypass` は FX 全体の入り切りです。その下に、効果ごとの入れ子が並びます。

## 効果ごとの鍵

| `tremolo` | `bypass` / `rate` / `depth` / `mix` |
| `vibrato` | `bypass` / `rate` / `depth` / `mix` |
| `bitCrusher` | `bypass` / `rate` / `bits` / `mix` |
| `delay` | `bypass` / `time` / `fb` / `mix` |
| `reverb` | `bypass` / `size` / `damp` / `mix` |
| `filter` | `bypass` / `type` / `freq` / `q` / `mix` |
| `eq3band` | `bypass` / `lowGainDb` / `midFreq` / `midGainDb` / `highGainDb` / `mix` |
| `sfcEcho` | `bypass` / `time` / `fb` / `firCoef0` / `firCoef1` / `firCoef2` / `firCoef3` / `firCoef4` / `firCoef5` / `firCoef6` / `firCoef7` / `mix` |
| `pcmBitCrusher` | `bypass` / `bits` / `rate` / `interp` / `mix` |

どの効果にも `bypass` があり、**真で切り**です。`mix` は 0.0 で原音のまま、
1.0 で効果だけになります。

:::note
`pcmBitCrusher` は 2686VFX にしかない効果です。音源で読んだときは、この
まとまりを見に行かないので飛ばされます。逆に 3.1.0 より前に書いた
ファイルにはこのまとまりがありませんが、そのときは今の値をそのまま
使うので、読めなくなることはありません。

`bits` / `rate` / `interp` は一覧の**何番目か**を持ちます。効果の順番は
[エフェクトの順番](/2686V_2686VFX/reference/file-spec/fxo/) の側で持ちます。
:::

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
