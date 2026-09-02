---
title: WT+ channel
description: The channel that switches between loaded waves as it plays
sidebar:
  order: 10
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_wtp.png" alt="WT+ channel" style="width:600px;" />
	<figcaption>WT+ channel</figcaption>
</figure>

WT+ is for **lining up waves you already have and switching between them while you play**. It is new in 3.0.0.

Where WT and WT2 are for *drawing* one cycle, WT+ is for **using** what you drew. Switching wave changes the tone on the spot, so you can have the tone move through a single held note.

:::note[This channel is ours]
Nothing on the hardware corresponds to it directly. It packages up, in a form that is easy to work with, what the sound drivers of the day did when they rewrote wavetable memory mid-performance to change the tone.
:::

## SLOT — the row of waves

You can load **up to 32** waves. Both `.wt` and `.wt2` are accepted.

| Item | What it does | Range |
| --- | --- | --- |
| **SLOT** | Which wave is sounding | 0–31 |
| **WT** / **W2** | Load a `.wt` / `.wt2` into that slot | — |
| **Clear** | Empty that slot | — |

Loaded waves are drawn on screen, so you can see at a glance what is in which slot.

| Knob | What it does | Automation |
| --- | --- | --- |
| **SLOT** | Which slot sounds. 0–31, switchable from automation | [`WTPLUS_SLOT`](/2686V_2686VFX/en/reference/automation/wtplus/#wtplus-slot) |
| **Interpolate** | Join the slots as they switch | [`WTPLUS_INTERPOLATE`](/2686V_2686VFX/en/reference/automation/wtplus/#wtplus-interpolate) |
| **STEP** | The value increment while drawing | [`WTPLUS_STEPS`](/2686V_2686VFX/en/reference/automation/wtplus/#wtplus-steps) |

## How to switch

**Move the SLOT slider and the tone changes as you do.**

The best use for it is **DAW automation**. SLOT is an automation target, so you can draw the wave changes against time in the DAW.

:::tip[One way to use it]

1. Load waves in order, from bright through to dark
2. Move SLOT slowly with automation
3. Hold a note, and only the tone travels

Think about the order and you get a change quite unlike a filter's.
:::

## WAVE MEMORY — how it sounds

| Item | What it does | Range |
| --- | --- | --- |
| **STEP** | Round to a number of steps you name | 0–10 |
| **Interpolate** | Join the points smoothly | on / off (on by default) |

Whatever resolution the loaded waves have, this evens the coarseness out. **Switch Interpolate off and they sound in steps as they are.**

## MOD — wavetable pitch modulation (hardware / ours)

The same modulation WT and WT2 have: the FDS, WonderSwan and HuC6280 families.

| Item | What it does | Range |
| --- | --- | --- |
| **Enable** | Use the modulation | on / off |
| **DEPTH** | How far it reaches | 0.1–1.0 |
| **SPEED** | Rate, as a ratio against the carrier frequency | 0.1–10.0 |
| **SHAPE** | How it modulates | 9 kinds |

For the details, see [the WT page](/2686V_2686VFX/en/chips/wt/).

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
| [**UTILITY**](/2686V_2686VFX/en/chips/common/#utility) | Reading and writing parameters. **Broadcast Level** sends the level out to the other channels |

For the details of QUALITY, see [the reference](/2686V_2686VFX/en/reference/lists-quality/).

## Related pages

- [WT](/2686V_2686VFX/en/chips/wt/) — draws waves as real numbers
- [WT2](/2686V_2686VFX/en/chips/wt2/) — draws waves in steps

## Automation

The full list of what WT+ exposes to the DAW is under
[WT+ automation](/2686V_2686VFX/en/reference/automation/wtplus/), with IDs, types, ranges and
defaults.
