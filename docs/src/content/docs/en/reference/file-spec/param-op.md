---
title: A single operator
description: What is inside .param.<chip>.op.json
sidebar:
  order: 43
---

Moves **a single FM operator** in and out. It has the same shape as one entry
of the `ops` list in a channel file.

| Chip | Extension | format | What it holds |
| --- | --- | --- | --- |
| OPNA | `.param.opna.op.json` | `opnaOp` | [OPNA channel](/2686V_2686VFX/en/reference/file-spec/param-opna/#ops- list) |
| OPN | `.param.opn.op.json` | `opnOp` | [OPN channel](/2686V_2686VFX/en/reference/file-spec/param-opn/#ops- list) |
| OPL | `.param.opl.op.json` | `oplOp` | [OPL channel](/2686V_2686VFX/en/reference/file-spec/param-opl/#ops- list) |
| OPL3 | `.param.opl3.op.json` | `opl3Op` | [OPL3 channel](/2686V_2686VFX/en/reference/file-spec/param-opl3/#ops- list) |
| OPM | `.param.opm.op.json` | `opmOp` | [OPM channel](/2686V_2686VFX/en/reference/file-spec/param-opm/#ops- list) |
| OPZX7 | `.param.opzx7s.op.json` | `opzx7Op` | [OPZX7 channel](/2686V_2686VFX/en/reference/file-spec/param-opzx7s/#ops- list) |
| Rhythm pads | `.param.rhythm.pad.json` | `rhythmPad` | [Rhythm channel](/2686V_2686VFX/en/reference/file-spec/param-rhythm/#pads- list) |

## Shape

```json
{
  "format": "opnaOp",
  "version": 1,
  "values": { ... }
}
```

That operator's knobs and nested blocks sit directly under `values`. Unlike a
channel file, they are not inside an `ops` list.

**Every operator has the same shape**, so a setting built on OP1 can be loaded
onto OP3. Across chips, the shared knobs still load.

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
