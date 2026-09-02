---
title: WT channel
description: The wavetable channel — draw one cycle of a wave and sound it
sidebar:
  order: 8
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_wt.png" alt="WT channel" style="width:600px;" />
	<figcaption>WT channel</figcaption>
</figure>

WT is the **wavetable** channel. It is the approach used by the Famicom Disk System, the PC Engine and the WonderSwan, and also by Namco's arcade hardware (C30) and Konami's MSX cartridges (SCC): **you draw one cycle of a wave yourself** and it is sounded over and over.

Rather than building a sound through modulation the way FM does, **the shape of the wave is the tone.** You can lay out exactly the overtones you want; in exchange, movement has to come from somewhere else — the envelopes and the modulation.

## How things are marked

| Mark | Meaning |
| --- | --- |
| **hardware** | Something the real chip had. It behaves as the hardware did |
| **ours** | An extension with no hardware equivalent |

## How WT and WT2 differ

They are both wavetables; what differs is **how the values are held**.

| | WT | WT2 |
| --- | --- | --- |
| Values | **real numbers** (-1.0 to 1.0) | **steps** (whole-number notches) |
| Good for | smooth waves | waves at the hardware's resolution |
| File | `.wt` | `.wt2` |

Every real chip held its values in steps, so **WT2 is the one to reach for when you want the period sound**, and WT the one for drawing freely.

## FORM — setting up the wave

### SIZE — how many samples

How many points make up one cycle.

| Value | Samples |
| --- | --- |
| 0 | 32 |
| 1 | 64 |
| 2 | 128 |
| 3 | 256 |

**More points mean a finer wave**, but the machines of the day usually had about 32. Keep it low if you are after that feel.

| Knob | What it does | Automation |
| --- | --- | --- |
| **SIZE** | Samples in one cycle. 32 / 64 / 128 / 256 | [`WT_SIZE`](/2686V_2686VFX/en/reference/automation/wt/#wt-size) |
| **STEP** | The value increment while drawing | [`WT_STEPS`](/2686V_2686VFX/en/reference/automation/wt/#wt-steps) |
| **FORM** | A built-in wave, or one you drew | [`WT_WAVE`](/2686V_2686VFX/en/reference/automation/wt/#wt-wave) |

### Drawing the wave

Drag directly on the graph. **Hold a modifier** and the values snap to an increment.

| Key | Increment |
| --- | --- |
| **Shift** + drag | 0.01 |
| **Ctrl** + drag | 0.05 |
| **Alt** + drag | 0.1 |

With nothing held, you draw freely with no snapping. A common way to work is to draw the detail first and then tidy the shape up with a modifier held.

### The helper buttons

| Button | What it does |
| --- | --- |
| `-> 1.0` / `-> 0.0` / `-> -1.0` | Set every point to that value |
| **STEP** | Round to a number of steps you name |
| **Interpolate** | Join the points smoothly (on by default) |

Switch **Interpolate** off and the points sound as they are, in steps, which is coarser. Use it when you want the texture of the hardware.

### WAVE FILE — reading and writing

Waves read and write as `.wt` files. They are plain text, so you can bring in something made with another tool.

:::note
**The `.wt` / `.wt2` formats have not changed in 3.0.0.** Files you made earlier load as they are.
:::

## MOD — wavetable pitch modulation (hardware / ours)

The **pitch modulation** the wavetable chips had. This is the heart of WT.

| Item | What it does | Range |
| --- | --- | --- |
| **Enable** | Use the modulation | on / off |
| **DEPTH** | How far it reaches | 0.1–1.0 (0.2 by default) |
| **SPEED** | Rate, **as a ratio against the carrier frequency** | 0.1–10.0 (1.0 by default) |
| **SHAPE** | How it modulates | see below |

### What SHAPE offers

| Value | Method | Notes |
| --- | --- | --- |
| 0 | Sine | A simplified FDS |
| 1–4 | The **FDS (2C33)** 32-step tables (triangle / saw / reset / trapezoid) | |
| 5–6 | The **WonderSwan ch3** sweep (up / down) | As used on the WonderSwan |
| 7 | **HuC6280** — uses the wavetable itself as the modulator | As used on the PC Engine |
| 8 | **FdsUser** — edit the FDS table in place | |

**HuC6280 mode** can hold up to eight modulation waves and switch between them as it plays. **FdsUser** lets you edit a 32-entry table directly, in the same 3-bit values the hardware used.

SPEED being a ratio against the carrier is exactly as the hardware was. **Whole-number ratios lock in as overtones**; ratios in between give you beating and unsteady wobble.

## Shared sections

The following do not belong to any one chip. They are all described under
[Shared sections](/2686V_2686VFX/en/chips/common/).

| Section | What it does |
| --- | --- |
| [**QUALITY**](/2686V_2686VFX/en/chips/common/#quality) | Drops bit depth and sample rate for texture |
| [**ENVELOPE**](/2686V_2686VFX/en/chips/common/#envelope) | [AMP ENV](/2686V_2686VFX/en/chips/common/#amp-env) / [SSG HW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-amp-env) / [SSG SW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env) / [SSG SW AMP ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env11) / [PITCH ENV](/2686V_2686VFX/en/chips/common/#pitch-env) / [SSG SW PITCH ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-pitch-env11) |
| [**LFO**](/2686V_2686VFX/en/chips/common/#lfo) | Wobbles level and pitch at a steady rate (the OPZX7S one) |
| [**MUL/DET**](/2686V_2686VFX/en/chips/common/#muldet) | Shifts the pitch |
| [**UNISON/HARMONY**](/2686V_2686VFX/en/chips/common/#unisonharmony) | Stacks copies of the note to thicken it |
| [**UTILITY**](/2686V_2686VFX/en/chips/common/#utility) | Reading and writing parameters |

For the details of QUALITY, see [the reference](/2686V_2686VFX/en/reference/lists-quality/).

## Related pages

- [WT2](/2686V_2686VFX/en/chips/wt2/) — the version that holds values in steps
- [WT+](/2686V_2686VFX/en/chips/wtplus/) — switches between several waves as it plays
- [Keyboard shortcuts](/2686V_2686VFX/en/reference/shortcuts/) — the increments while drawing

## Automation

The full list of what WT exposes to the DAW is under
[WT automation](/2686V_2686VFX/en/reference/automation/wt/), with IDs, types, ranges and
defaults.
