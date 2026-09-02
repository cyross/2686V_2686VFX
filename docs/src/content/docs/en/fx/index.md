---
title: About FX
description: The chain of effects, and what every one of them shares
sidebar:
  order: 0
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/parts/fx_pane.png" alt="The FX pane" style="width:200px;" />
	<figcaption>The FX pane</figcaption>
</figure>

**FX** is where the sound gets processed after the chips have made it. On the
instruments it is not a tab — it sits **in a pane down the right-hand side**,
always visible. In 2686VFX it is the whole plugin, applied to whatever audio
you feed it.

## The effects

| Effect | What it does |
| --- | --- |
| [Filter](/2686V_2686VFX/en/fx/filter/) | Cuts above or below a frequency you choose |
| [3-band EQ](/2686V_2686VFX/en/fx/eq3band/) | Raises or lowers the low, middle and high |
| [Tremolo](/2686V_2686VFX/en/fx/tremolo/) | Wobbles the level |
| [Vibrato](/2686V_2686VFX/en/fx/vibrato/) | Wobbles the pitch |
| [Bit crusher](/2686V_2686VFX/en/fx/bitcrusher/) | Coarsens with bit depth and decimation |
| [Delay](/2686V_2686VFX/en/fx/delay/) | Holds the sound back and repeats it |
| [Reverb](/2686V_2686VFX/en/fx/reverb/) | Adds the sound of a space |
| [SFC echo](/2686V_2686VFX/en/fx/sfc-echo/) | The hardware echo, shaped by coefficients |
| [PCM bit crusher](/2686V_2686VFX/en/fx/pcm-bitcrusher/) | Coarsens on the instruments' QUALITY steps (**2686VFX only**) |

2686VFX has one more: [modulating the output](/2686V_2686VFX/en/fx/mod/). It takes the
envelopes and LFO the instruments carry per note, and applies them to the audio
coming in.

The plugin itself is introduced on
[2686VFX (the effect)](/2686V_2686VFX/en/plugins/fx-plugin/).

## What every effect has

| Knob | What it does | Default |
| --- | --- | --- |
| **Bypass** | Switched on, the effect is taken out of the chain | on (not applied) |
| **MIX** | Blend of dry and effect. 0.0 is dry, 1.0 is effect only | 0.0 |

**Every effect starts bypassed with MIX at 0.0.** Switch bypass off, raise MIX,
and then reach for the other knobs. Switching bypass off on its own changes
nothing.

There is also a **bypass** for the whole chain
([`FX_BYPASS`](/2686V_2686VFX/en/reference/automation/fx/#fx-bypass), default off). Use it to
pull every effect out at once and hear the dry signal for comparison.

**Parameter reset** returns every knob in this section to its default.

## The order they run in

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_order.png" alt="FX order" style="width:400px;" />
	<figcaption>FX order</figcaption>
</figure>

Effects run from the top down. **Distorting and then adding space** sounds
nothing like **adding space and then distorting**.

You can reorder them under **Settings**. Exporting the order gives you a
`.fxo.json` file.

:::note[Stored by name]
The order file stores effects **by name**, not by position. Move a file between
plugins that carry different numbers of effects and nothing shifts into the
wrong slot — names the plugin does not know are simply skipped.
:::

## Files

| What | Extension | Spec |
| --- | --- | --- |
| Effect settings | `.2fx.json` | [Effect settings](/2686V_2686VFX/en/reference/file-spec/2fx/) |
| Processing order | `.fxo.json` | [Effect order](/2686V_2686VFX/en/reference/file-spec/fxo/) |

## Automation

Every knob can be driven from the DAW. The lists are under
[FX](/2686V_2686VFX/en/reference/automation/fx/) (on the instruments) and
[2686VFX](/2686V_2686VFX/en/reference/automation/fx-plugin/).
