---
title: Modulating the output
description: 2686VFX only — the envelopes and LFO applied to the audio itself
sidebar:
  order: 11
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

Set [Key assign](#key-assign--which-key-drives-the-modulation) to Customize to
choose a key per target. The LFO and the pitch shift then apply only while their
assigned keys are held.
:::

## Switching modulation on and off

Four switches sit in the Effector panel. **All four start bypassed** (that is,
doing nothing). Switch off only the ones you want.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Bypass envelopes** | Takes AMP ENV, SSG HW AMP ENV, SSG SW AMP ENV[256], WT AMP MOD and AMP SEQUENCER out together | on / off | on | [`MOD_ENV_BYPASS`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-env-bypass) |
| **Bypass LFO** | Takes the LFO out | on / off | on | [`MOD_LFO_BYPASS`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-lfo-bypass) |
| **Bypass pitch modulation** | Takes PITCH ENV, SSG HW PITCH ENV, SSG SW PITCH ENV[256], WT PITCH MOD and PITCH SEQUENCER out together | on / off | on | [`MOD_PITCH_BYPASS`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-pitch-bypass) |
| **Bypass pitch shift** | Takes MUL・DET and UNISON・HARMONY out together | on / off | on | [`MOD_SHIFT_BYPASS`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-shift-bypass) |

## Key assign — which key drives the modulation

Found in the Effects frame, above the file import/export buttons. It decides
**which key drives which modulation**.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Key assign** | Single key / Customize | 0 to 1 | 0 (Single key) | [`MOD_KEYASSIGN_MODE`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-keyassign-mode) |

- **Single key** (default): same as before. Any key drives everything.
- **Customize**: each target only responds to its own key. Choosing it shows the
  list of keys below.

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_single_key_assign.png" alt="Single key assign" style="width:400px;" />
	<figcaption>Single key assign</figcaption>
</figure>

### A key per target

Shown only in Customize mode. Keys range from C-2 (0) to G8 (127); as with FIX's
note names, 60 is C3.

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_key_assign.png" alt="Customized key assign" style="width:400px;" />
	<figcaption>Customized key assign</figcaption>
</figure>

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **AMP ENV** | The key that drives AMP ENV | 0 to 127 | 60 (C3) | [`MOD_KEY_AMPENV`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-key-ampenv) |
| **SSG HW AMP ENV** | The key that drives SSG HW AMP ENV | 0 to 127 | 60 (C3) | [`MOD_KEY_SSGHWENV`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-key-ssghwenv) |
| **WT AMP MOD** | The key that drives WT AMP MOD | 0 to 127 | 60 (C3) | [`MOD_KEY_WTAMPMOD`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-key-wtampmod) |
| **SSG SW AMP ENV\[256\]** | The key that drives SSG SW AMP ENV\[256\] | 0 to 127 | 60 (C3) | [`MOD_KEY_SSGSWENV11`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-key-ssgswenv11) |
| **AMP SEQUENCER** | The key that drives AMP SEQUENCER (from 4.0.0) | 0 to 127 | 60 (C3) | [`MOD_KEY_TAPAMP`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-key-tapamp) |
| **PITCH ENV** | The key that drives PITCH ENV | 0 to 127 | 60 (C3) | [`MOD_KEY_PITCHENV`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-key-pitchenv) |
| **SSG HW PITCH ENV** | The key that drives SSG HW PITCH ENV | 0 to 127 | 60 (C3) | [`MOD_KEY_SSGHWPENV`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-key-ssghwpenv) |
| **SSG SW PITCH ENV\[256\]** | The key that drives SSG SW PITCH ENV\[256\] | 0 to 127 | 60 (C3) | [`MOD_KEY_SSGSWPENV11`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-key-ssgswpenv11) |
| **PITCH SEQUENCER** | The key that drives PITCH SEQUENCER (from 4.0.0) | 0 to 127 | 60 (C3) | [`MOD_KEY_TAPPITCH`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-key-tappitch) |
| **WT PITCH MOD** | The key that drives WT PITCH MOD | 0 to 127 | 60 (C3) | [`MOD_KEY_WTMOD`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-key-wtmod) |
| **LFO AM** | The key that drives LFO AM | 0 to 127 | 60 (C3) | [`MOD_KEY_LFOAM`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-key-lfoam) |
| **LFO PM** | The key that drives LFO PM | 0 to 127 | 60 (C3) | [`MOD_KEY_LFOPM`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-key-lfopm) |
| **MUL/DET** | The key that drives MUL/DET | 0 to 127 | 60 (C3) | [`MOD_KEY_MULDET`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-key-muldet) |
| **UNISON/HARMONY** | The key that drives UNISON/HARMONY | 0 to 127 | 60 (C3) | [`MOD_KEY_UNISON`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-key-unison) |
| **ARPEGGIO** | The key that drives ARPEGGIO | 0 to 127 | 60 (C3) | [`MOD_KEY_ARP`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-key-arp) |

What the assigned key does depends on the target:

| Target | Behavior |
| --- | --- |
| Envelopes (AMP ENV, PITCH ENV, SSG HW, SSG SW[256] and WT MOD panels) | Start on key down, release on key up |
| LFO AM / LFO PM | Applied **only while the key is held**. Restarts from the top on key down |
| MUL/DET / UNISON/HARMONY | Applied **only while the key is held** |
| ARPEGGIO | Arpeggiates **only while the key is held**. Starts from the first voice on key down |
| AMP SEQUENCER | Each press moves on one step; **only while the key is held** the sound plays at that step's ratio. Released, it is silent |
| PITCH SEQUENCER | Each press moves on one step; **only while the key is held** the pitch shifts by that step's cents. Released, it returns to the original pitch |

- The same key can be assigned to several targets; pressing it drives all of them.
- In Customize mode, the colour of each modulation panel's title shows whether it
  is active: grey while its key is not held, off-white while it is. The LFO panel
  lights up for either the AM or the PM key, and the UNISON/HARMONY panel for either
  the UNISON or the ARPEGGIO key. In Single key mode every title stays off-white,
  as before.
- When several keys are held, everything assigned to each of them runs.
- Targets are processed in the same order as in Single key mode.
- An LFO switched in and out by a key fades over a few milliseconds so the sound
  does not jump.
- Each section's own on/off switch still applies on top of the key assignment. A
  section switched off does not move even when its key is pressed.

Assignments are saved in the FX parameter file ([`.2fx`](/2686V_2686VFX/en/reference/file-spec/2fx/)).

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
[SSG SW AMP ENV\[256\]](/2686V_2686VFX/en/chips/common/#ssg-sw-amp-env256) and
[WT AMP MOD](/2686V_2686VFX/en/chips/common/#wt-amp-mod). They work exactly as they do on
the instruments.

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_ssg_hw_amp_env.png" alt="SSG HW AMP ENV" style="width:400px;" />
	<figcaption>SSG HW AMP ENV</figcaption>
</figure>

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_ssg_sw_amp_env_256.png" alt="SSG SW AMP ENV[256]" style="width:400px;" />
	<figcaption>SSG SW AMP ENV[256]</figcaption>
</figure>

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_wt_amp_mod.png" alt="WT AMP MOD" style="width:400px;" />
	<figcaption>WT AMP MOD</figcaption>
</figure>

### AMP SEQUENCER

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_tap_amp_sequencer.png" alt="AMP SEQUENCER" style="width:400px;" />
	<figcaption>AMP SEQUENCER</figcaption>
</figure>

Added in 4.0.0. It works like the instruments' [TAP AMP SEQUENCER](/2686V_2686VFX/en/chips/common/#tap-sequencer):
**each key press moves on one step**, and while the key is held, the incoming sound's level is
multiplied by that step's ratio. Use it, for example, to give each hit of a drum loop its own level.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Enable** | Turns it on | on / off | off | [`MOD_TAPAMP_ENABLE`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-tapamp-enable) |
| **STEPS** | Number of steps. At 1, every press uses the same ratio | 1 to 256 | 1 | [`MOD_TAPAMP_STEPS`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-tapamp-steps) |
| **LOOP** | Goes back along the loop sets (LP.TARGET / START / END / COUNT / ADD / DELETE). From 4.1.0 | on / off | off | [`MOD_TAPAMP_LOOP`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-tapamp-loop) |
| **S.TARGET** | The step the value knob edits | 1 to STEPS | 1 | — |
| **RATIO** | That step's ratio. The buttons below nudge it | 0.0 to 4.0 | 1.0 | — |
| **SKIP** | Makes the S.TARGET step a SKIP step (ON; the button turns red), or clears it (OFF) (from 4.1.0). A SKIP step applies no ratio | OFF / ON | OFF | — |
| **INCREMENT** | Moves the current step on by one (after the last comes 1; from not started, 1) | — | — | [`MOD_TAPAMP_INC`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-tapamp-inc) |
| **DECREMENT** | Moves the current step back by one (before 1 comes the last; from not started, the last) | — | — | [`MOD_TAPAMP_DEC`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-tapamp-dec) |
| **CLEAR STEP** | Goes back to not started (the next press uses step 1). RESET in 4.0.0 | — | — | [`MOD_TAPAMP_RESET`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-tapamp-reset) |

- The step values are drawn as horizontal bars, with an "S1: value / S2: value / …" list under them.
  The step with the changed background is the current step (the one the last press used). Before the
  first press, and after the DAW stops playing, it is not started and no row is highlighted
- A SKIP step applies no ratio when pressed (it plays at x1). Its bar is red (orange while S.TARGET
  points at it). Moving RATIO on a SKIP step clears it
- Releasing the key silences the output, so the keys also work as a gate that chops the sound
- It works only while "Bypass envelopes" is off. With that switch on, Enable has no effect
- INCREMENT / DECREMENT / CLEAR STEP can also be pressed by automation (once each time the value rises past 0.5)
- The step values and SKIP are saved as a table in the FX parameter file and the DAW state (automation cannot move them).
  The section's own parameter file is `.tapAmp`, read and written from the heading's right-click menu
- LOOP and the loop sets work as in the instruments' [TAP SEQUENCER](/2686V_2686VFX/en/chips/common/#tap-sequencer).
  INCREMENT follows the sets, and CLEAR STEP also restarts the loop counts

:::caution[Send MIDI once Enable is on]
With Enable on, **nothing comes out while no key is held**. Unlike AMP ENV and the others, it does
not pass the sound through while no MIDI is arriving.
:::

## LFO

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_lfo.png" alt="LFO" style="width:400px;" />
	<figcaption>LFO</figcaption>
</figure>

Wobbles level and pitch at a steady rate. It is the instruments'
[LFO](/2686V_2686VFX/en/chips/common/#lfo): the AM side works on level, the PM side on pitch.

It keeps running whether or not a key is down, which is why **bypass LFO** is a
separate switch. With [Key assign](#key-assign--which-key-drives-the-modulation)
set to Customize, AM and PM each apply only while their assigned key is held.

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
[SSG SW PITCH ENV\[256\]](/2686V_2686VFX/en/chips/common/#ssg-sw-pitch-env256) and
[WT PITCH MOD](/2686V_2686VFX/en/chips/common/#wt-pitch-mod).

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_ssg_sw_pitch_env_256.png" alt="SSG SW PITCH ENV[256]" style="width:400px;" />
	<figcaption>SSG SW PITCH ENV[256]</figcaption>
</figure>

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_wt_pitch_mod.png" alt="WT PITCH MOD" style="width:400px;" />
	<figcaption>WT PITCH MOD</figcaption>
</figure>

### PITCH SEQUENCER

<figure class="shot">
	<img src="/2686V_2686VFX/ui/fx/fx_tap_pitch_sequencer.png" alt="PITCH SEQUENCER" style="width:400px;" />
	<figcaption>PITCH SEQUENCER</figcaption>
</figure>

Added in 4.0.0. It works like the instruments' [TAP PITCH SEQUENCER](/2686V_2686VFX/en/chips/common/#tap-sequencer):
**each key press moves on one step**, and while the key is held, the incoming sound is shifted by
that step's cents.

| Knob | What it does | Range | Default | Automation |
| --- | --- | --- | ---: | --- |
| **Enable** | Turns it on | on / off | off | [`MOD_TAPPITCH_ENABLE`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-tappitch-enable) |
| **STEPS** | Number of steps. At 1, every press shifts by the same amount | 1 to 256 | 1 | [`MOD_TAPPITCH_STEPS`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-tappitch-steps) |
| **LOOP** | Goes back along the loop sets (from 4.1.0; same as AMP SEQUENCER) | on / off | off | [`MOD_TAPPITCH_LOOP`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-tappitch-loop) |
| **S.TARGET** | The step the value knob edits | 1 to STEPS | 1 | — |
| **CENT** | That step's shift in cents. The buttons below nudge it | −2400 to 2400 | 0 | — |
| **SKIP** | Makes the S.TARGET step a SKIP step (ON; the button turns red), or clears it (OFF) (from 4.1.0). A SKIP step does not shift | OFF / ON | OFF | — |
| **INCREMENT** | Moves the current step on by one (after the last comes 1; from not started, 1) | — | — | [`MOD_TAPPITCH_INC`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-tappitch-inc) |
| **DECREMENT** | Moves the current step back by one (before 1 comes the last; from not started, the last) | — | — | [`MOD_TAPPITCH_DEC`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-tappitch-dec) |
| **CLEAR STEP** | Goes back to not started (the next press uses step 1). RESET in 4.0.0 | — | — | [`MOD_TAPPITCH_RESET`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-tappitch-reset) |

- Releasing the key returns to the original pitch. While no key is held, the sound passes through
- It works only while "Bypass pitch modulation" is off
- The step display, SKIP, the INCREMENT buttons and how it is saved are the same as AMP SEQUENCER.
  The section's own parameter file is `.tapPitch`
- Like the other pitch modulation, it works by changing how fast the incoming sound is read back,
  so larger shifts sound grainier

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
| **MUL.RATE** | Set that multiple as a number, finely | 0.01 – 100 | 1 | [`MOD_MUL_RATIO`](/2686V_2686VFX/en/reference/automation/fx-plugin/#mod-mul-ratio) |
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
