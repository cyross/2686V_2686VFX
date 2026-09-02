---
title: オペレータ 1 つぶん
description: .param.<音源>.op.json の中身
sidebar:
  order: 43
---

FM 音源の**オペレータ 1 つぶん**を出し入れするファイルです。チャンネルの
ファイルの `ops` の並びの、1 つぶんと同じかたちをしています。

| 音源 | 拡張子 | 表記（format） | 中身 |
| --- | --- | --- | --- |
| OPNA | `.param.opna.op.json` | `opnaOp` | [OPNA チャンネル](/2686V_2686VFX/reference/file-spec/param-opna/#ops-の並び) |
| OPN | `.param.opn.op.json` | `opnOp` | [OPN チャンネル](/2686V_2686VFX/reference/file-spec/param-opn/#ops-の並び) |
| OPL | `.param.opl.op.json` | `oplOp` | [OPL チャンネル](/2686V_2686VFX/reference/file-spec/param-opl/#ops-の並び) |
| OPL3 | `.param.opl3.op.json` | `opl3Op` | [OPL3 チャンネル](/2686V_2686VFX/reference/file-spec/param-opl3/#ops-の並び) |
| OPM | `.param.opm.op.json` | `opmOp` | [OPM チャンネル](/2686V_2686VFX/reference/file-spec/param-opm/#ops-の並び) |
| OPZX7 | `.param.opzx7s.op.json` | `opzx7Op` | [OPZX7 チャンネル](/2686V_2686VFX/reference/file-spec/param-opzx7s/#ops-の並び) |
| リズムのパッド | `.param.rhythm.pad.json` | `rhythmPad` | [リズム チャンネル](/2686V_2686VFX/reference/file-spec/param-rhythm/#pads-の並び) |

## かたち

```json
{
  "format": "opnaOp",
  "version": 1,
  "values": { ... }
}
```

`values` の下に、そのオペレータのつまみと入れ子がそのまま並びます。
チャンネルのファイルと違い、`ops` の並びには入りません。

**オペレータ同士は同じかたち**なので、OP1 で作った設定を OP3 へ読ませる、
といった使い方ができます。音源をまたいでも、共通のつまみは読めます。

置き場所は [ファイルの形式と置き場所](/2686V_2686VFX/files/format/) を参照してください。
