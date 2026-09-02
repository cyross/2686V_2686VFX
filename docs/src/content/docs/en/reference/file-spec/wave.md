---
title: Wavetables (.wt / .wt2)
description: The text files that hold a wavetable
sidebar:
  order: 55
---

What is inside a wavetable. **It is not JSON — it is text, one number per
line**, so that it is easy to produce from a spreadsheet or a script.

## .wt — real numbers

| Line | What it is |
| --- | --- |
| Line 1 | Sample count. `32` / `64` / `128` / `256` |
| Line 2 onwards | A value from `-1.0` to `1.0` |

```plaintext
32
0.000000
0.195090
0.382683
…
```

There must be as many values as the sample count on the first line.

## .wt2 — steps

| Line | What it is |
| --- | --- |
| Line 1 | Sample count. `32` / `64` / `128` / `256` |
| Line 2 | Step count. `16` / `32` / `64` / `128` / `256` |
| Line 3 onwards | A whole number from `0` to the step count minus 1 |

```plaintext
32
16
8
10
12
…
```

The centre is half the number of steps (`8` for 16 steps). **The coarseness of
the steps is the point**, so the same shape sounds different at a different
step count.

## Where they are used

- WAVE FILE on the WT / WT2 channels
- WT+ slots (up to 32)
- OPZX7 operators (when WS is set to `.wt` / `.wt2`)
- The modulator wave for [WT PITCH MOD](/2686V_2686VFX/en/chips/common/#wt-pitch-mod)

For where the files live, see [File formats and locations](/2686V_2686VFX/en/files/format/).
