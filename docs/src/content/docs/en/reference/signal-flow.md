---
title: Signal flow
description: What happens, and in what order, between a key press and sound
sidebar:
  order: 1
---

This is what a value passes through, and in what order, between pressing a key
and hearing sound. **Knowing the order tells you where to look when the sound
isn't what you wanted.**

## The whole plugin

<svg viewBox="0 0 720 90" role="img" aria-label="The path from MIDI to the output" style="max-width:100%;height:auto;">
  <style>
    .fbox { fill: var(--sl-color-gray-6, #f6f6f6); stroke: var(--sl-color-gray-4, #888); stroke-width: 1.5; }
    .facc { fill: var(--sl-color-accent-low, #dde); stroke: var(--sl-color-accent, #55a); stroke-width: 1.5; }
    .ftxt { fill: var(--sl-color-text, #222); font-size: 13px; font-family: sans-serif; text-anchor: middle; }
    .farr { stroke: var(--sl-color-gray-3, #666); stroke-width: 1.5; fill: none; }
  </style>
  <defs>
    <marker id="ah" markerWidth="8" markerHeight="8" refX="7" refY="3" orient="auto">
      <path d="M0,0 L7,3 L0,6 z" fill="var(--sl-color-gray-3, #666)" />
    </marker>
  </defs>
  <rect class="fbox" x="4"   y="26" width="96"  height="38" rx="6" />
  <text class="ftxt" x="52"  y="50">MIDI</text>
  <line class="farr" x1="102" y1="45" x2="130" y2="45" marker-end="url(#ah)" />
  <rect class="facc" x="132" y="26" width="130" height="38" rx="6" />
  <text class="ftxt" x="197" y="50">Sounding voices</text>
  <line class="farr" x1="264" y1="45" x2="292" y2="45" marker-end="url(#ah)" />
  <rect class="fbox" x="294" y="26" width="96"  height="38" rx="6" />
  <text class="ftxt" x="342" y="50">Sum</text>
  <line class="farr" x1="392" y1="45" x2="420" y2="45" marker-end="url(#ah)" />
  <rect class="fbox" x="422" y="26" width="110" height="38" rx="6" />
  <text class="ftxt" x="477" y="50">Headroom</text>
  <line class="farr" x1="534" y1="45" x2="562" y2="45" marker-end="url(#ah)" />
  <rect class="facc" x="564" y="26" width="70"  height="38" rx="6" />
  <text class="ftxt" x="599" y="50">FX</text>
  <line class="farr" x1="636" y1="45" x2="664" y2="45" marker-end="url(#ah)" />
  <text class="ftxt" x="692" y="50">Out</text>
</svg>

1. **MIDI** — what comes from the DAW is mixed with what you press on the
   on-screen keyboard
2. **Sounding voices** — one voice runs per held note
3. **Sum** — every voice is added together
4. **Headroom** — brings the sum back down after it has grown. Adjustable in the
   settings
5. **FX** — the order can be rearranged
6. **Out**

## Inside one voice

Inside a voice, **pitch and level are assembled separately.** The pitch side
decides how fast the oscillator advances; the level side is applied to the
waveform that comes out of it.

<svg viewBox="0 0 720 170" role="img" aria-label="How pitch and level split inside a voice" style="max-width:100%;height:auto;">
  <style>
    .vbox { fill: var(--sl-color-gray-6, #f6f6f6); stroke: var(--sl-color-gray-4, #888); stroke-width: 1.5; }
    .vacc { fill: var(--sl-color-accent-low, #dde); stroke: var(--sl-color-accent, #55a); stroke-width: 1.5; }
    .vtxt { fill: var(--sl-color-text, #222); font-size: 13px; font-family: sans-serif; text-anchor: middle; }
    .vsub { fill: var(--sl-color-gray-2, #555); font-size: 11px; font-family: sans-serif; text-anchor: middle; }
    .varr { stroke: var(--sl-color-gray-3, #666); stroke-width: 1.5; fill: none; }
  </style>
  <defs>
    <marker id="ah2" markerWidth="8" markerHeight="8" refX="7" refY="3" orient="auto">
      <path d="M0,0 L7,3 L0,6 z" fill="var(--sl-color-gray-3, #666)" />
    </marker>
  </defs>

  <rect class="vbox" x="4" y="14" width="176" height="44" rx="6" />
  <text class="vtxt" x="92" y="34">Pitch-side modulation</text>
  <text class="vsub" x="92" y="50">sets the advance rate</text>

  <rect class="vbox" x="4" y="112" width="176" height="44" rx="6" />
  <text class="vtxt" x="92" y="132">Waveform / timbre</text>
  <text class="vsub" x="92" y="148">operators / wave memory</text>

  <path class="varr" d="M182,36 L214,36 L214,85 L238,85" marker-end="url(#ah2)" />
  <path class="varr" d="M182,134 L214,134 L214,85 L238,85" marker-end="url(#ah2)" />

  <rect class="vacc" x="240" y="63" width="104" height="44" rx="6" />
  <text class="vtxt" x="292" y="90">Oscillator</text>

  <line class="varr" x1="346" y1="85" x2="374" y2="85" marker-end="url(#ah2)" />

  <rect class="vbox" x="376" y="63" width="156" height="44" rx="6" />
  <text class="vtxt" x="454" y="83">Level-side modulation</text>
  <text class="vsub" x="454" y="99">applied to the waveform</text>

  <line class="varr" x1="534" y1="85" x2="560" y2="85" marker-end="url(#ah2)" />

  <rect class="vbox" x="562" y="63" width="76" height="44" rx="6" />
  <text class="vtxt" x="600" y="90">Pan</text>

  <line class="varr" x1="640" y1="85" x2="666" y2="85" marker-end="url(#ah2)" />
  <text class="vtxt" x="692" y="82">to the</text>
  <text class="vtxt" x="692" y="97">sum</text>
</svg>

### Pitch side — how fast the oscillator advances

Applied from the top down. These are all **multipliers**, so reordering them
wouldn't change the result. The **envelopes take the previous value and work
from it**, though, so those follow the order shown.

| # | What applies | What decides it |
| --- | --- | --- |
| 1 | The base pitch | the key you pressed |
| 2 | Pitch bend | the MIDI keyboard |
| 3 | **PITCH ENV** | 3-tap; runs on press and release |
| 4 | **SSG SW PITCH ENV[11]** | 11-tap; runs on press and release |
| 5 | **SSG HW PITCH ENV** | repeats a fixed shape; depth is given in cents |
| 6 | **The LFO's PM** | depth is given in cents |
| 7 | **WT PITCH MOD** | its rate is a ratio against the carrier |
| 8 | MUL / DET | multiple and detune |
| 9 | Unison detune | a different value per voice |

### Level side — applied to the waveform

These are all **multiplications** too. If any one of them is 0, everything
downstream is silent.

| # | What applies | What decides it |
| --- | --- | --- |
| 1 | The waveform | what the oscillator produced |
| 2 | The TONE / NOISE blend | channels that carry noise, such as SSG |
| 3 | **LEVEL** | the slider on screen |
| 4 | **AMP ENV** | the ADSR; SSG SW ENV / ENV11 stack in here too |
| 5 | The base level | a per-channel baseline |
| 6 | **The LFO's AM** | depth is given as dB of attenuation |
| 7 | **SSG HW AMP ENV** | the hardware envelope |
| 8 | **WT AMP MOD** | swings between MIN and MAX with a wave memory shape |
| 9 | Unison gain compensation | brings the level down as voices are added |

:::tip[When nothing sounds]
The level side is a chain of multiplications, so **one zero anywhere means
silence.** Check LEVEL, the AMP ENV levels, and the MIN / MAX on SSG HW AMP ENV
and WT AMP MOD, in that order.
:::

## In 2686VFX

Being an effect, it **takes in audio** instead of making it.

<svg viewBox="0 0 720 90" role="img" aria-label="The path from input to output in 2686VFX" style="max-width:100%;height:auto;">
  <style>
    .xbox { fill: var(--sl-color-gray-6, #f6f6f6); stroke: var(--sl-color-gray-4, #888); stroke-width: 1.5; }
    .xacc { fill: var(--sl-color-accent-low, #dde); stroke: var(--sl-color-accent, #55a); stroke-width: 1.5; }
    .xtxt { fill: var(--sl-color-text, #222); font-size: 13px; font-family: sans-serif; text-anchor: middle; }
    .xarr { stroke: var(--sl-color-gray-3, #666); stroke-width: 1.5; fill: none; }
  </style>
  <defs>
    <marker id="ah3" markerWidth="8" markerHeight="8" refX="7" refY="3" orient="auto">
      <path d="M0,0 L7,3 L0,6 z" fill="var(--sl-color-gray-3, #666)" />
    </marker>
  </defs>
  <rect class="xbox" x="4"   y="26" width="110" height="38" rx="6" />
  <text class="xtxt" x="59"  y="50">Audio in</text>
  <line class="xarr" x1="116" y1="45" x2="144" y2="45" marker-end="url(#ah3)" />
  <rect class="xbox" x="146" y="26" width="110" height="38" rx="6" />
  <text class="xtxt" x="201" y="50">Headroom</text>
  <line class="xarr" x1="258" y1="45" x2="286" y2="45" marker-end="url(#ah3)" />
  <rect class="xacc" x="288" y="26" width="170" height="38" rx="6" />
  <text class="xtxt" x="373" y="50">Output modulation</text>
  <line class="xarr" x1="460" y1="45" x2="488" y2="45" marker-end="url(#ah3)" />
  <rect class="xacc" x="490" y="26" width="70"  height="38" rx="6" />
  <text class="xtxt" x="525" y="50">FX</text>
  <line class="xarr" x1="562" y1="45" x2="590" y2="45" marker-end="url(#ah3)" />
  <text class="xtxt" x="616" y="50">Out</text>
</svg>

**Modulation runs before the FX**, so that movement in level goes through the
effects too. Cut the sound with an envelope, for example, and that cut is what
the delay and reverb pick up.

The modulation follows the same order as in the instruments, split into a pitch
side and a level side. The pitch side is built differently, though, because
there is no oscillator: it **changes how fast buffered audio is read back**. See
[2686VFX (the effect)](/2686V_2686VFX/en/plugins/fx-plugin/) for details.

## When no MIDI key is held

In the instruments, no key means no voice, which means no sound.

In 2686VFX, **no key means no modulation, and the audio passes straight
through.** If you only want the FX, simply leave the keyboard alone.
