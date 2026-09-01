---
title: About these plugins
description: What the 2686V series is
sidebar:
  order: 1
---

2686V is a software instrument that recreates how the sound chips in 1980s and
90s computers and game consoles behaved. It runs both as VST3 and standalone.

## Five plugins

**Four are instruments** that make sound; the remaining one, **2686VFX, is an
effect** that processes audio you feed it. The instruments differ only in how
many sound chips they carry — the shared screens and the way you work are the
same — so most of this manual applies to all of them. The differences are
collected in [Choosing a plugin](/2686V_2686VFX/en/plugins/compare/).

| Plugin | What it is |
| --- | --- |
| **2686V** | the full one, with every sound chip |
| **2686VLight** | 2686V with curve editing removed, so it runs lighter |
| **86V** | matched to the PC-9801-86 layout |
| **OPZX7S** | focused on the OPZX7 chip. Curve editing is standard |
| **2686VFX** | makes no sound; processes audio you feed it |

## Requirements

- Windows 11 (x64 / ARM64)
- A DAW that hosts VST3, or the standalone build

Build settings for Linux and macOS are provided, but no binaries are
distributed.

## How the sound is approached

Where the real hardware had a feature, it follows the hardware's specification.
Where an extension has been added that the hardware never had, the manual marks
it as **original**. If you are after the real thing, leave the original entries
alone.

## One plugin plays one chip

There are many tabs for switching sound chips, but **only one of them sounds at
a time**. To layer several chips, use separate DAW tracks and put a plugin on
each.

That way level, panning and effects stay per-track, which gives you more room to
move — that is the thinking behind it.
