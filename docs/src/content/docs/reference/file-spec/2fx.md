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
| `pcmBitCrusher` | `bypass` / `bits` / `rate` / `interp` / `mix` / `nrResample` |
| `noiseReduction` | `bypass` / `rate` / `rateBypass` / `gate` / `gateLevel` / `lpf` / `mix` |

どの効果にも `bypass` があり、**真で切り**です。`mix` は 0.0 で原音のまま、
1.0 で効果だけになります。

:::note
`pcmBitCrusher` は 2686VFX にしかない効果です。音源で読んだときは、この
まとまりを見に行かないので飛ばされます。逆に 3.1.0 より前に書いた
ファイルにはこのまとまりがありませんが、そのときは今の値をそのまま
使うので、読めなくなることはありません。

`noiseReduction` も 2686VFX にしかない効果で、3.6.3 で足しました。
`pcmBitCrusher` の `nrResample` も同じく 3.6.3 からです。どちらも、無い
ファイルを読んだときは既定（ノイズリダクションはバイパス、`nrResample` は切）
にするので、それより前と同じ音になります。

`bits` / `rate` / `interp` と `noiseReduction` の `rate` / `lpf` は一覧の
**何番目か**を持ちます。効果の順番は
[エフェクトの順番](/2686V_2686VFX/reference/file-spec/fxo/) の側で持ちます。
:::

## 出力の音量（2686VFX）

`output` のまとまりに、出力の LEVEL を持ちます。3.6.2 で足しました。

```json
"output": { "level": 1.0 }
```

`level` は 0.0〜10.0 の倍率で、1.0 がそのままの大きさです。持たない古い
ファイルや音源で書いたファイルを読んだときは、今の値のまま変えません。

## キーアサイン（2686VFX）

`keyAssign` のまとまりに、変調を動かす鍵盤の割り当てを持ちます。3.6.0 で
足しました。

```json
"keyAssign": { "mode": 1, "ampEnv": 60, "pitchEnv": 62, "lfoAm": 64, "…": 60 }
```

| 鍵 | 型 | 範囲 | 初期値 | 内容 |
| --- | --- | --- | ---: | --- |
| `mode` | 整数 | 0 〜 1 | 0 | 0 がシングルキーアサイン、1 がキーアサインのカスタマイズ |
| `ampEnv` | 整数 | 0 〜 127 | 60 | AMP ENV を動かす鍵盤 |
| `ssgHwEnv` | 整数 | 0 〜 127 | 60 | SSG HW AMP ENV を動かす鍵盤 |
| `wtAmpMod` | 整数 | 0 〜 127 | 60 | WT AMP MOD を動かす鍵盤 |
| `ssgSwEnv11` | 整数 | 0 〜 127 | 60 | SSG SW AMP ENV\[11\] を動かす鍵盤 |
| `pitchEnv` | 整数 | 0 〜 127 | 60 | PITCH ENV を動かす鍵盤 |
| `ssgHwPEnv` | 整数 | 0 〜 127 | 60 | SSG HW PITCH ENV を動かす鍵盤 |
| `ssgSwPEnv11` | 整数 | 0 〜 127 | 60 | SSG SW PITCH ENV\[11\] を動かす鍵盤 |
| `wtMod` | 整数 | 0 〜 127 | 60 | WT PITCH MOD を動かす鍵盤 |
| `lfoAm` | 整数 | 0 〜 127 | 60 | LFO AM を動かす鍵盤 |
| `lfoPm` | 整数 | 0 〜 127 | 60 | LFO PM を動かす鍵盤 |
| `mulDet` | 整数 | 0 〜 127 | 60 | MUL/DET を動かす鍵盤 |
| `unison` | 整数 | 0 〜 127 | 60 | UNISON/HARMONY を動かす鍵盤 |
| `arpeggio` | 整数 | 0 〜 127 | 60 | ARPEGGIO を動かす鍵盤 |

鍵盤は MIDI のノート番号で、60 が C3 です。

:::note
`keyAssign` は 2686VFX にしかありません。音源で読んだときは飛ばされます。
3.6.0 より前に書いたファイルにはこのまとまりがありませんが、そのときは
いまの割り当てのまま変わりません。
:::

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
