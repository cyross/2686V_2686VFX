---
title: ADV (curve editing)
description: Deciding for yourself how a value changes over time
sidebar:
  order: 14
---

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/tab_curve.png" alt="Curve editing mode" style="width:600px;" />
	<figcaption>Curve editing mode</figcaption>
</figure>

The **ADV** tab is where you decide **how** a value travels over time.

On the hardware, setting an envelope's rate settled the shape of the change as well: it either ran in a straight line or followed the chip's own quirks. Curve editing **hands you the shape in between**.

:::note[Which plugins have it]
Curve editing is in **2686V**, **OPZX7S**, and the six narrower instruments
(**OPNV** / **OPLV** / **OPMV** / **WTV** / **PCMV** / **PULSEV**). It is left out of
2686VLight, 86V and 26V to keep the load down.

**In the OPZX7S plugin curve editing is standard**, and there is no way back to the old linear mode.
:::

## What it can be applied to

You settle three things: *where*, *what* and *how*.

### Position — where

| Value | What it covers |
| --- | --- |
| **Common** | The whole channel |
| **Op1 – Op8** | One operator at a time |

### Target — what

| Target | Values it can change |
| --- | --- |
| **RegValue** | AR / DR(D1R) / SL(D1L) / RR / SR(D2R) / TL |
| [**AmpEnv**](/2686V_2686VFX/en/chips/common/#amp-env) | AR / DR / RR / SR |
| [**PitchEnv**](/2686V_2686VFX/en/chips/common/#pitch-env) | AR / DR / RR |
| [**SsgSwEnv**](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env) | R1–R6 and LoopTo |
| [**SsgSwEnv11**](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env11) | R1–R11 and LoopTo |
| [**SsgSwPEnv11**](/2686V_2686VFX/en/chips/common/#ssg-sw-pitch-env11) | R1–R11 and LoopTo |

It reaches both the register values themselves and the rates of each envelope. For what is inside those envelopes, see [Shared sections](/2686V_2686VFX/en/chips/common/).

## Logic — how

There are **31** to pick from. Broadly:

### The single ones

| Logic | How it moves |
| --- | --- |
| **Linear** | A straight line. The old behaviour |
| **Arc (exponential)** | Slow at first, then suddenly |
| **Arc (logarithmic)** | Sudden at first, then slowly |
| **Exponential** | Speeding up |
| **Logarithmic** | Slowing down |
| **1-point spline** | Bent through one waypoint |
| **2-point spline** | Bent through two waypoints |

### The combined ones

**Split at waypoints, each stretch can move differently.**

| Shape | Example |
| --- | --- |
| Two stretches | `linear + exponential` / `1-point spline + linear` |
| Three stretches | `linear + logarithmic + linear` / `linear + linear + linear` |
| The same one repeated | `2-point spline + 2-point spline + 2-point spline` |

`linear + arc(logarithmic) + linear`, for instance, gives you **a straight fall, then a gentle curve, then a straight fall again**.

## Waypoints and control points

Where the stretches divide, and how hard they bend, are set by values on screen.

| Symbol | Meaning |
| --- | --- |
| **PX** / **PY** (`P1X`–`P2Y`) | Position of a waypoint |
| **CX** / **CY** (`C1X`–`C6Y`) | Control points that decide the bend |

X is the time direction, Y the value direction. Picking a spline adds control points.

## Where to use it

:::tip[Making a note feel like it rings on]
Put `logarithmic` on the AMP ENV's DR and you get **a quick initial fall with a slow remainder** — closer to how an acoustic instrument rings than the hardware's straight-line decay.
:::

:::tip[When you want to stay in period]
The other way round: when you are after the hardware's sound, **leave it on `linear`.** A curve gives you movement the hardware could never have produced.
:::

## About the load

Curve editing calculates per sample, so it costs more. **If you are not going to use it, 2686VLight runs lighter.**

For how the plugins differ, see [Choosing a plugin](/2686V_2686VFX/en/plugins/compare/).

## Files

Curve parameters read and write like any other parameters. For the formats, see [File formats and locations](/2686V_2686VFX/en/files/format/).
