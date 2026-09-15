---
title: What changed in v3.4.0
description: What was added and what changed in 3.4.0
sidebar:
  order: 5
---

A summary of what changed between 3.3.0 and 3.4.0. **No file has become unreadable.** Presets and parameter files made up to 3.3.0 still load as they are.

## Passing sounds between the FM chips and OPZX7S

**A sound built on an FM chip can now be carried over as an OPZX7S sound.** There are two buttons, and both convert in exactly the same way.

| Button | Where | In which plugins |
| --- | --- | --- |
| **[EX]OPZX7S Params** | UTILITY on OPNA / OPN / OPL / OPL3 / OPM | 2686V, 2686VLight, 26V, 86V, OPNV, OPLV, OPMV |
| **[IM]FM Params** | UTILITY on OPZX7S | 2686V, 2686VLight, OPZX7S |

- **[EX]OPZX7S Params** writes the current channel out as an OPZX7S CH Params file
- **[IM]FM Params** picks an OPNA / OPN / OPL / OPL3 / OPM CH Params file, converts it to OPZX7S values and loads it onto the screen. **The OPZX7S plugin has no FM chips of its own, but it can read files saved by the other plugins**
- After loading, the result can be **saved straight away as an OPZX7S CH Params file.** The name comes from the original file

The conversion in brief:

- The algorithm becomes an **algorithm matrix** with the same routing
- The AMP ENV of each operator in use is put into **real mode**, with times and levels worked out from the hardware rate table
- The LFOs go to OPZX7S's LFOs at the same rate and depth
- Operators the original chip does not have are left unconnected and given defaults

Anything that cannot behave the same way (DT1, how the key scale bites, stages longer than 10 seconds and so on) is moved to the nearest value, and **a dialog lists what was approximated at the end.**

The full table of what goes where is under [Passing sounds between the FM chips and OPZX7S](/2686V_2686VFX/en/chips/common/#passing-sounds-between-the-fm-chips-and-opzx7s).

## Major categories: [[AMP ENV]] / [[PITCH ENV]]

Of the sections that act on the whole channel, the ones that move level are now gathered under **[[AMP ENV]]** and the ones that move pitch under **[[PITCH ENV]]**. The list of sections had grown long enough to be hard to follow.

<figure class="shot">
	<img src="/2686V_2686VFX/ui/parts/major_category.png" alt="Major categories" style="width:212px;" />
	<figcaption>SSG with [[AMP ENV]] open</figcaption>
</figure>

| Major category | Sections inside |
| --- | --- |
| **[[AMP ENV]]** | AMP ENV / SSG HW AMP ENV / SSG SW AMP ENV / SSG SW AMP ENV[11] / WT AMP MOD |
| **[[PITCH ENV]]** | PITCH ENV / SSG HW PITCH ENV / SSG SW PITCH ENV[11] / WT PITCH MOD |

- They appear on the OPNA / OPN / OPL / OPL3 / OPM / OPZX7S / SSG / WT / WT2 / WT+ / ADPCM / ADPCM+ / BEEP tabs. The order of the sections is unchanged
- The heading has an off-white background; the lamp colours are unchanged
- **They start closed.** OP / CL open and close them along with the sections inside
- Closing one **does not change the sound**

See [Major categories](/2686V_2686VFX/en/guide/screen/#major-categories--amp-env--pitch-env) for the detail.

## Fixes

Faults present up to 3.3.0.

- **Setting values on an operator or pad that TARGET was not pointing at lost the slider and selector values.** Only the on/off switches got through. This hit loading a whole-channel CH Params file and pasting operators and pads. It dates from 3.3.0, when the knobs started being rebound by TARGET
- **[EX]CH Params on OPN / OPL / OPL3 / OPM / OPZX7S wrote the TARGET operator's values into every operator.** Loading the pre-3.0.0 format, and OPL3's [IM]OPL CH Params, piled what was read onto the TARGET operator. Also from 3.3.0
- **The OPL3 tab did not show its left panel (LEVEL through UTILITY).** This dates from the tab rework in 3.3.0
- **Opening the save browser after loading a file left out the name field and the save button**
