---
title: Modulating the output
description: 2686VFX only — the envelopes and LFO applied to the audio itself
sidebar:
  order: 10
---

This section exists **only in 2686VFX**. It takes the envelopes and LFO the
instruments carry per note and applies them **to the incoming audio itself**.
Chop a drum loop with an amplitude envelope, drop a guitar with a pitch
envelope, and so on.

:::note[What the keyboard drives, and what it doesn't]
The envelopes run on **MIDI note-on and note-off**. 2686VFX has no keyboard of
its own, so send MIDI to the track it sits on. Until you do, the envelope
panels pass the sound through untouched.

The LFO and the pitch shift run whether or not a key is down. That is why they
have their own switches.
:::

## Switching modulation on and off

Four switches sit in the Effector panel. **All four start bypassed** (that is,
doing nothing). Switch off only the ones you want.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Bypass envelopes** | Takes AMP ENV, SSG HW AMP ENV, SSG SW AMP ENV[11] and WT AMP MOD out together | on / off | on | [`MOD_ENV_BYPASS`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-env-bypass) |
| **Bypass LFO** | Takes the LFO out | on / off | on | [`MOD_LFO_BYPASS`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-lfo-bypass) |
| **Bypass pitch modulation** | Takes PITCH ENV, SSG HW PITCH ENV, SSG SW PITCH ENV[11] and WT PITCH MOD out together | on / off | on | [`MOD_PITCH_BYPASS`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-pitch-bypass) |
| **Bypass pitch shift** | Takes MUL・DET and UNISON・HARMONY out together | on / off | on | [`MOD_SHIFT_BYPASS`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-shift-bypass) |

## Level — AMP ENV

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_amp_env.png" alt="AMP ENV" style="width:400px;" />
	<figcaption>AMP ENV</figcaption>
</figure>

Moves the level from the moment the key goes down to after it comes up.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Bypass** | Takes this panel out | on / off | off | [`MOD_ADSR_BYPASS`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-adsr-bypass) |
| **STL** | Level the note starts at | 0 – 1 | 0 | [`MOD_STL`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-stl) |
| **AR** | Time to reach full level (seconds) | 0.001 – 10 | 0.001 | [`MOD_AR`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-ar) |
| **DR** | Time to fall to SL (seconds) | 0.001 – 10 | 0.001 | [`MOD_DR`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-dr) |
| **SL** | Level held while the key is down | 0 – 1 | 1 | [`MOD_SL`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-sl) |
| **RR** | Time to fade out after release (seconds) | 0.001 – 10 | 0.001 | [`MOD_RR`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-rr) |
| **KOR** | Play the whole envelope out even after release | on / off | off | [`MOD_KOR`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-kor) |

Under the same "bypass envelopes" switch there are also
[SSG HW AMP ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-amp-env),
[SSG SW AMP ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env11) and
[WT AMP MOD](/2686V_2686VFX/en/chips/common/#wt-amp-mod). They work exactly as they do on
the instruments.

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_ssg_hw_amp_env.png" alt="SSG HW AMP ENV" style="width:400px;" />
	<figcaption>SSG HW AMP ENV</figcaption>
</figure>

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_ssg_sw_amp_env_11.png" alt="SSG SW AMP ENV[11]" style="width:400px;" />
	<figcaption>SSG SW AMP ENV[11]</figcaption>
</figure>

## LFO

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_lfo.png" alt="LFO" style="width:400px;" />
	<figcaption>LFO</figcaption>
</figure>

Wobbles level and pitch at a steady rate. It is the instruments'
[LFO](/2686V_2686VFX/en/chips/common/#lfo): the AM side works on level, the PM side on pitch.

It keeps running whether or not a key is down, which is why **bypass LFO** is a
separate switch.

## Pitch — PITCH ENV

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_pitch_env.png" alt="PITCH ENV" style="width:400px;" />
	<figcaption>PITCH ENV</figcaption>
</figure>

Moves the pitch from the moment the key goes down to after it comes up.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Bypass** | Takes this panel out | on / off | off | [`MOD_PITCHENV_BYPASS`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-pitchenv-bypass) |
| **STL** | Pitch the note starts at (cents) | -4800 – 4800 | 0 | [`MOD_PITCH_STL`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-pitch-stl) |
| **AR** | Time to reach ATL (seconds) | 0.001 – 10 | 0.001 | [`MOD_PITCH_AR`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-pitch-ar) |
| **ATL** | Pitch at the peak (cents) | -4800 – 4800 | 0 | [`MOD_PITCH_ATL`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-pitch-atl) |
| **DR** | Time to reach SSL (seconds) | 0.001 – 10 | 0.001 | [`MOD_PITCH_DR`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-pitch-dr) |
| **SSL** | Pitch held while the key is down (cents) | -4800 – 4800 | 0 | [`MOD_PITCH_SSL`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-pitch-ssl) |
| **RR** | Time to reach RLL (seconds) | 0.001 – 10 | 0.001 | [`MOD_PITCH_RR`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-pitch-rr) |
| **RLL** | Pitch it settles at after release (cents) | -4800 – 4800 | 0 | [`MOD_PITCH_RLL`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-pitch-rll) |

Under the same "bypass pitch modulation" switch there are also
[SSG HW PITCH ENV](/2686V_2686VFX/en/chips/common/#ssg-hw-pitch-env),
[SSG SW PITCH ENV\[11\]](/2686V_2686VFX/en/chips/common/#ssg-sw-pitch-env11) and
[WT PITCH MOD](/2686V_2686VFX/en/chips/common/#wt-pitch-mod).

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_ssg_sw_pitch_env_11.png" alt="SSG SW PITCH ENV[11]" style="width:400px;" />
	<figcaption>SSG SW PITCH ENV[11]</figcaption>
</figure>

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_wt_pitch_mod.png" alt="WT PITCH MOD" style="width:400px;" />
	<figcaption>WT PITCH MOD</figcaption>
</figure>

### The WT PITCH MOD / WT AMP MOD base frequency

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Base frequency** | The pitch the wobble is measured against (Hz) | 1 – 2000 | 440 | [`MOD_WTMOD_BASEFREQ`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-wtmod-basefreq) |

On the instruments the pitch being played is the reference. 2686VFX has no
pitch of its own, so you set one with **base frequency**. There is no hardware
equivalent — this one is ours. WT PITCH MOD and WT AMP MOD share it.

The **panel colour** also tells the sections apart: effects are blue,
modulation is red, the LFO is green, and MUL・DET and UNISON・HARMONY are cyan.

## Pitch shift — MUL・DET

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_mul_det.png" alt="MUL/DET" style="width:400px;" />
	<figcaption>MUL/DET</figcaption>
</figure>

Shifts the pitch by a fixed amount.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **MUL** | Multiple of the original pitch | 0 – 21 | 2 | [`MOD_MUL`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-mul) |
| **MURT** | Set that multiple as a number, finely | 0.01 – 100 | 1 | [`MOD_MUL_RATIO`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-mul-ratio) |
| **DT1** | Shift it slightly | 0 – 15 | 0 | [`MOD_DT`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-dt) |
| **DT2** | Shift it coarsely | 0 – 3 | 0 | [`MOD_DT2`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-dt2) |
| **DT3** | Shift it by a number of cents | -4800 – 4800 | 0 | [`MOD_DT3`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-dt3) |

Under the same "bypass pitch shift" switch there is also
[UNISON・HARMONY](/2686V_2686VFX/en/chips/common/#unisonharmony), which splits the incoming sound into
several voices and stacks them at different pitches.

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_unison_harmony.png" alt="UNISON/HARMONY" style="width:400px;" />
	<figcaption>UNISON/HARMONY</figcaption>
</figure>

:::caution[What moving the pitch means here]
An effect has no oscillator. Everything on the pitch side works by buffering
the incoming audio and reading it back at a different speed. **It is a simple
arrangement**, so the further you shift, the rougher it gets. How it works is
described under
[how the pitch is moved](/2686V_2686VFX/en/plugins/fx-plugin/#how-the-pitch-is-moved).
:::

## Automation

The full list is under
[2686VFX automation](/2686V_2686VFX/en/reference/automation/fx-plugin/). Everything starting
with `MOD_` belongs to this section.
