---
title: Choosing a plugin
description: How the six plugins differ
sidebar:
  order: 1
---

The series has six plugins. **Five are instruments** that make sound; the
remaining one, **2686VFX, is an effect** that processes sound you feed it.

## The five instruments

Among the instruments, the plugins differ **only in which tabs they carry.**
Everything else — the shared screens and the way you work — is identical. On the instruments FX is not a tab but a pane that stays
put at the right ([Reading the screen](/2686V_2686VFX/en/guide/screen/)).

| Tab | <img src="/2686V_2686VFX/logos/2686V.png" alt="" width="28" height="28" style="vertical-align:middle;" /><br />2686V | <img src="/2686V_2686VFX/logos/2686VLight.png" alt="" width="28" height="28" style="vertical-align:middle;" /><br />2686VLight | <img src="/2686V_2686VFX/logos/26V.png" alt="" width="28" height="28" style="vertical-align:middle;" /><br />26V | <img src="/2686V_2686VFX/logos/86V.png" alt="" width="28" height="28" style="vertical-align:middle;" /><br />86V | <img src="/2686V_2686VFX/logos/OPZX7S.png" alt="" width="28" height="28" style="vertical-align:middle;" /><br />OPZX7S |
| --- | :---: | :---: | :---: | :---: | :---: |
| OPNA | ✔ | ✔ | — | ✔ | — |
| OPN | ✔ | ✔ | ✔ | — | — |
| OPL | ✔ | ✔ | — | — | — |
| OPL3 | ✔ | ✔ | — | — | — |
| OPM | ✔ | ✔ | — | — | — |
| OPZX7 | ✔ | ✔ | — | — | ✔ |
| SSG | ✔ | ✔ | ✔ | ✔ | — |
| WAVETABLE | ✔ | ✔ | — | — | — |
| WT2 | ✔ | ✔ | — | — | — |
| WTPLUS | ✔ | ✔ | — | — | — |
| RHYTHM | ✔ | ✔ | — | ✔ (6 pads) | — |
| ADPCM | ✔ | ✔ | — | ✔ (PCM) | — |
| BEEP | ✔ | ✔ | — | — | — |
| FX (pane) | ✔ | ✔ | ✔ | ✔ | ✔ |
| CURVE | ✔ | — | — | — | ✔ |
| PRESET / SETTINGS / COLORS / ABOUT | ✔ | ✔ | ✔ | ✔ | ✔ |

### Which one to pick

- **When in doubt, 2686V** — it has everything
- **2686VLight** — 2686V with curve editing removed. If you don't use curves,
  this one runs lighter
- **26V** — matched to the PC-9801-26(K) layout. It carries **only OPN and
  SSG**, which suits writing in the sound of the earliest PC-98 machines
- **86V** — matched to the PC-9801-86 layout. ADPCM is called **PCM**, and the
  rhythm section has **6 pads instead of 8**. When you want to work within the
  original hardware's limits, nothing extra gets in the way
- **OPZX7S** — when you want to focus on OPZX7. **Curve editing is standard**

## The one effect

**2686VFX** is the odd one out. It makes no sound of its own; it **processes
audio you feed into it.** It is the FX tab from the instruments, lifted out so
you can use it on its own.

| | The five instruments | <img src="/2686V_2686VFX/logos/2686VFX.png" alt="" width="28" height="28" style="vertical-align:middle;" /><br />2686VFX |
| --- | :---: | :---: | :---: |
| Kind | Instrument (synth) | Effect |
| Audio input | none | stereo |
| Sound-chip tabs | yes | none |
| FX | 8 kinds | **9 kinds** |
| Modulation of the output | — | ✔ |
| PRESET tab | ✔ | — |

See [2686VFX (the effect)](/2686V_2686VFX/en/plugins/fx-plugin/) for details.

:::note
Preset extensions differ per plugin, so another plugin's presets don't appear in
the list. **FX order and FX parameter files are shared**, though, and move
freely between the instruments and 2686VFX. See
[File formats and locations](/2686V_2686VFX/en/files/format/).
:::
