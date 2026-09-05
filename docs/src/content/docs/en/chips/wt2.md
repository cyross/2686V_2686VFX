---
title: WT2 channel
description: The wavetable channel that holds its values in steps
sidebar:
  order: 9
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_wt2.png" alt="WT2 channel" style="width:600px;" />
	<figcaption>WT2 channel</figcaption>
</figure>

WT2 is the same kind of wavetable channel as [WT](/2686V_2686VFX/en/chips/wt/), except that **it holds its values in steps — whole-number notches.**

Every real wavetable chip held its values at a fixed bit depth: 16 notches at 4 bits, 32 at 5. WT2 copies that arrangement straight across, so you can build **waves at the resolution the hardware had**.

## How things are marked

| Mark | Meaning |
| --- | --- |
| **hardware** | Something the real chip had. It behaves as the hardware did |
| **ours** | An extension with no hardware equivalent |

## Choosing between WT and WT2

| | WT | WT2 |
| --- | --- | --- |
| Values | real numbers (-1.0 to 1.0) | **steps (whole-number notches)** |
| Good for | smooth waves | **waves at the hardware's resolution** |
| File | `.wt` | `.wt2` |

**When you want the period feel, WT2 is the one.** The fewer the notches, the more of that old coarseness comes through. For a fine, smooth shape, use WT instead.

## FORM — setting up the wave

### SIZE — how many samples

How many points make up one cycle.

| Value | Samples |
| --- | --- |
| 0 | 32 |
| 1 | 64 |
| 2 | 128 |
| 3 | 256 |

| Knob | What it does | Automation |
| --- | --- | --- |
| **SIZE** | Samples in one cycle | [`WT2_SIZE`](/2686V_2686VFX/en/reference/automation/wt2/#wt2-size) |
| **RESO** | How many notches the values have. 16 / 32 / 64 / 128 / 256 | [`WT2_RES`](/2686V_2686VFX/en/reference/automation/wt2/#wt2-res) |
| **FORM** | A built-in wave, or one you drew | [`WT2_WAVE`](/2686V_2686VFX/en/reference/automation/wt2/#wt2-wave) |

### Drawing the wave

Drag directly on the graph. **Unlike WT, the values are already in steps, so there is no modifier-key snapping.** You pick the number of notches on screen instead.

### The helper buttons

| Button | What it does |
| --- | --- |
| `-> Max` / `-> Center` / `-> 0` | Set every point to that value |
| **STEP** | Name the number of notches |
| **Interpolate** | Join the points smoothly (on by default) |

**Switch Interpolate off and it sounds in steps as it is.** Few notches plus Interpolate off gets you close to the hardware's coarseness.

### WAVE FILE — reading and writing

Waves read and write as `.wt2` files. They are plain text.

:::note
**The `.wt` / `.wt2` formats have not changed in 3.0.0.** Files you made earlier load as they are.
:::

## MOD — wavetable pitch modulation (hardware / ours)

The same thing WT has. You can pick from the FDS, WonderSwan and HuC6280 families of modulation.

| Item | What it does | Range |
| --- | --- | --- |
| **Enable** | Use the modulation | on / off |
| **DEPTH** | How far it reaches | 0.1–1.0 (0.2 by default) |
| **SPEED** | Rate, as a ratio against the carrier frequency | 0.1–10.0 (1.0 by default) |
| **SHAPE** | How it modulates | 9 kinds |

For the details, see [the WT page](/2686V_2686VFX/en/chips/wt/).

## Shared sections

The following do not belong to any one chip. They are all described under
[Shared sections](/2686V_2686VFX/en/chips/common/).

| Section | What it does |
| --- | --- |
| [**QUALITY**](/2686V_2686VFX/en/chips/common/#quality) | Drops bit depth and sample rate for texture |
| [**ENVELOPE**](/2686V_2686VFX/en/chips/common/#envelope) | [AMP ENV](/2686V_2686VFX/en/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env) / [SSG SW AMP ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env11) / [PITCH ENV](/2686V_2686VFX/en/chips/common/#pitch-env) / [SSG HW PITCH ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-pitch-env) / [SSG SW PITCH ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-pitch-env11) |
| [**LFO**](/2686V_2686VFX/en/chips/common/#lfo) | Wobbles level and pitch at a steady rate (the OPZX7S one) |
| [**MUL/DET**](/2686V_2686VFX/en/chips/common/#muldet) | Shifts the pitch |
| [**UNISON/HARMONY**](/2686V_2686VFX/en/chips/common/#unisonharmony) | Stacks copies of the note to thicken it |
| [**UTILITY**](/2686V_2686VFX/en/chips/common/#utility) | Reading and writing parameters |

For the details of QUALITY, see [the reference](/2686V_2686VFX/en/reference/lists-quality/).

## Related pages

- [WT](/2686V_2686VFX/en/chips/wt/) — the version that holds values as real numbers
- [WT+](/2686V_2686VFX/en/chips/wtplus/) — switches between several waves as it plays

## Automation

The full list of what WT2 exposes to the DAW is under
[WT2 automation](/2686V_2686VFX/en/reference/automation/wt2/), with IDs, types, ranges and
defaults.
