---
title: What changed in v3.6.2
description: What's fixed and changed in 3.6.2
sidebar:
  order: 2
---

This page covers changes from 3.6.1 to 3.6.2. **Your files remain compatible.**
Three of the fixes do change the sound, though; each section below says how.

## Fast short notes in polyphonic mode no longer pile up

**In polyphonic mode, notes could stack far beyond the 10-note polyphony — up
to 80.**

There are 80 voices: 10 notes times the maximum unison of 8. Polyphonic mode
handed out notes from all 80, so even with unison at 1, up to 80 notes could
sound at once. Playing short notes in quick succession kept taking fresh voices
while the previous notes were still in their release, stacking up to nearly 80
and overloading the CPU.

In 3.6.2, the number of voices sounding at once is capped at 10 times the
unison count. When the cap is reached, the oldest note gives way — notes
already released go first, and held notes are kept where possible.

| Example (OPNA, processing time per 10 ms) | 3.6.1 | 3.6.2 |
| --- | --- | --- |
| Unison 1, pressing key after key and holding for 6 s | 13.9 ms | 1.8 ms |

:::note[When the sound changes]
Where more than 10 notes used to overlap, the release of the oldest notes is
now cut short.
:::

Unison still takes one voice per unison part (unison 8 means 10 notes × 8 = 80
voices), so a heavy patch with a lot of unison is still heavy when you play
chords.

## RHYTHM's MUL/DET and FIX now work

**On RHYTHM pads, MUL/DET and FIX did not change the pitch.**

A pad plays its sample at its own pitch, and the frequency moved by MUL/DET and
FIX was never applied to the playback speed (it only reached the noise and the
modulation rate). In 3.6.2, the ratio between the moved frequency and the
original one is applied to the playback speed.

- MUL at ×2.0 plays an octave higher
- FIX plays at the set frequency, relative to the key the pad is assigned to

Every other non-FM channel (SSG / BEEP / WT / WT2 / WT+ / ADPCM / ADPCM+) was
checked by playing it too; all of them already worked.

:::note[When the sound changes]
RHYTHM pads with MUL/DET or FIX away from their defaults change pitch. Pads
left at the defaults do not change.
:::

## PCM channels keep their pitch when QUALITY changes mid-note

**On RHYTHM / ADPCM / ADPCM+, changing QUALITY while a note was sounding put
that note off pitch until it ended.**

When QUALITY is set to an encoding mode, the channel reads a copy of the
sample brought down to the chosen sample rate. Changing QUALITY swaps the
sample being read, but a sounding note kept the playback speed it started
with. Switching from Raw (the original 44.1 kHz) to YM2608 ADPCM at 16 kHz,
for example, read the new sample about 2.8 times too fast, so the note went
up in pitch. The next note played at the right pitch.

In 3.6.2, a sounding note recalculates its playback position and speed for
the new sample.

## MUL/DET and FIX can apply while notes play

On every channel (including FM operators), MUL/DET and FIX set the pitch from
**their values when the key is pressed**. Moving them while a note played only
took effect from the next note.

SETTINGS now has "Apply MUL/DET and FIX while notes play". Turned on, they act
on the sounding note straight away, the way writing the register on the real
chip does. The phase carries on, so the sound does not break when you move them.

It is off by default, so notes keep the values they started with, as before.
Projects that automate MUL/DET or FIX sound different with it turned on.

2686VFX's MUL/DET has always applied as soon as you move it, so it has no such setting.

## 2686VFX

### An output level (LEVEL)

**LEVEL** now sits below the global bypass in the Effector. It is the same
control as a channel's LEVEL: a multiplier from 0.0 to 10.0, default 1.0. It is
automated as `FX_LEVEL` and saved in the FX parameter file (`.2fx`) as
`output.level`.

### The output was too quiet

**The input was cut to a quarter (−12 dB), so the output was quiet even with
every effect off.**

It came from the same "headroom" stage the instruments use. The instruments
need it to bring down voices that add up; 2686VFX sums nothing, so it does not.
2686VFX no longer applies headroom, and the "Keep headroom" and "Headroom gain"
settings are gone from SETTINGS. Set the level with LEVEL instead.

:::note[When the sound changes]
Projects using 2686VFX become 12 dB (4×) louder. To keep the previous level,
set LEVEL to 0.25.
:::

### The global bypass did not bypass

**Switching on the global bypass still changed the sound.**

The bypass was only checked inside the effects, so the headroom and the output
modulation in front of them stayed in place. In 3.6.2 the global bypass skips
the modulation, the effects and LEVEL, and returns the input untouched.
