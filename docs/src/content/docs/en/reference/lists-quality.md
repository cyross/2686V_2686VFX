---
title: QUALITY lists
description: The bit depths, rates and interpolations you can pick when reducing quality
sidebar:
  order: 8
---

**QUALITY** is where the bit depth and sample rate are dropped to get the texture of period gear. This page tells you **what each number gets you**.

| Kind | Channels that use it |
| --- | --- |
| **BIT / RATE** | FM, SSG, wavetable |
| **BIT / RATE / INTP** | ADPCM, RHYTHM (the PCM family) |

## BIT — bit depth (FM, SSG, wavetable)

Simply reduces the number of steps. `Raw` reduces nothing.

There are **12**.

| Value | Name | Notes |
| ---: | --- | --- |
| 1 | 4-bit (16 steps) | |
| 2 | 5-bit (32 steps) | |
| 3 | 6-bit (64 steps) | |
| 4 | 7-bit (128 steps) | |
| 5 | 8-bit (256 steps) | |
| 6 | 9-bit (512 steps) | |
| 7 | 10-bit (1024 steps) | |
| 8 | 12-bit (4096 steps) | |
| 9 | 16-bit (32768 steps) | |
| 10 | 20-bit (1048576 steps) | |
| 11 | 24-bit (16777216 steps) | |
| 12 | Raw | |

## BIT — bit depth (the PCM family)

Alongside plain step reduction, **the compression schemes the hardware used** are listed here. A compression scheme distorts differently from simply dropping bits.

There are **21**.

| Value | Name | Notes |
| ---: | --- | --- |
| 1 | Raw (32bit) | |
| 2 | 24-bit PCM | |
| 3 | 20-bit PCM | |
| 4 | 16-bit PCM | |
| 5 | 12-bit PCM | |
| 6 | 10-bit PCM | |
| 7 | 9-bit PCM | |
| 8 | 8-bit PCM | |
| 9 | 7-bit PCM | |
| 10 | 6-bit PCM | |
| 11 | 5-bit PCM | |
| 12 | 4-bit PCM | |
| 13 | 4-bit ADPCM | Playback quality of the YM-2608B's ADPCM |
| 14 | 1-bit DPCM | Playback quality of the DPCM used on the Famicom |
| 15 | SNES BRR | Playback quality of the Bit Rate Reduction compression used by the Super Famicom's SPC-700 |
| 16 | PS1 VAG | Playback quality of the VAG compression used on the PS1 / PS2 / PSP |
| 17 | IMA ADPCM | Playback quality of IMA ADPCM compression |
| 18 | CD-ROM XA | Playback quality of the CD-XA used on CD-ROM |
| 19 | YMZ280B | Playback quality of the PCM used by the YMZ280B |
| 20 | K053260 | Playback quality of the KONAMI K053260 PCM/ADPCM chip |
| 21 | K054539 | Playback quality of the KONAMI K054539 PCM/ADPCM chip |

## RATE — sample rate

The lower it goes, the coarser it gets. The list is the same for the FM family and the PCM family.

There are **15**.

| Value | Name | Notes |
| ---: | --- | --- |
| 1 | 96kHz | |
| 2 | 55.5kHz | |
| 3 | 49.7kHz | |
| 4 | 48kHz | |
| 5 | 44.1kHz | |
| 6 | 33.08kHz | |
| 7 | 32kHz | |
| 8 | 22.05kHz | |
| 9 | 16kHz | |
| 10 | 12kHz | |
| 11 | 11kHz | |
| 12 | 8kHz | |
| 12 | 5.5kHz | |
| 13 | 4kHz | |
| 15 | 2kHz | |

## INTP — how the gaps are filled (the PCM family)

Once the rate has been dropped, this decides how the gaps are filled on the way back.

There are **7**.

| Value | Name | Notes |
| ---: | --- | --- |
| 1 | No interpolation (Nearest) | |
| 2 | Linear interpolation (Linear) | |
| 3 | Gaussian interpolation (Gaussian) | The interpolation algorithm used by the Super Famicom's SPC-700 |
| 4 | ZOH (Zero-Order Hold) | |
| 5 | Cosine interpolation (Cosine) | |
| 6 | B-spline interpolation (B-Spline) | |
| 7 | Lagrange interpolation (Lagrange) | |
