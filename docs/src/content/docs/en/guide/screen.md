---
title: Reading the screen
description: Where things are, and how the shared controls behave
sidebar:
  order: 3
---

Every plugin is laid out the same way. This page covers the parts they share.

## The main division

In the instruments, the screen splits into **tabs on the left and the FX pane on
the right**.

| Where | What's there |
| --- | --- |
| Left (tabs) | the sound channels, plus ADV / PRESET / SETTINGS / COLORS / ABOUT |
| Right (pane) | FX. **It stays put as you switch tabs** |

FX is always visible so that you can adjust the effects no matter which chip you
happen to be working on.

:::note
**In 2686VFX, FX is a tab.** There are no sound-chip tabs, so there is nothing
for it to sit beside.
:::

## The tabs

They run left to right. Which ones you get depends on the plugin (see
[Choosing a plugin](/2686V_2686VFX/en/plugins/compare/)).

### Sound chip tabs

OPNA / OPN / OPL / OPL3 / OPM / OPZX7S / SSG / WT / WT2 / WT+ / RHYTHM /
ADPCM / ADPCM+ / BEEP

**Only the chip on the open tab sounds.** To layer them, use separate DAW
tracks.

Tab colours vary by family, so the FM, SSG, wave-memory and PCM groups are
distinguishable at a glance.

### The other tabs

| Tab | What's there |
| --- | --- |
| **ADV** | curve editing — how a value changes over time (2686V / OPNV / OPLV / OPM / OPZX7S / PULSEV / WTV / PCMV) |
| **PRESET** | saving and loading presets, favourites, history, filtering by format and channel |
| **SETTINGS** | behaviour, folder locations, file format |
| **COLORS** | changing the interface colours |
| **ABOUT** | version and licence |

## The FM and RHYTHM tabs — envelope displays and TARGET

The OPNA / OPN / OPL / OPL3 / OPM / OPZX7S / RHYTHM tabs have **envelope displays at
the top and a single set of settings below**. Rather than repeating the
settings for every operator (or pad), there is one set, and **TARGET** chooses
which one it points at.

<figure class="shot">
	<img src="/2686V_2686VFX/ui/channel/ch_opzx7s.png" alt="OPZX7S channel" style="width:600px;" />
	<figcaption>OPZX7S: each frame shows the WAVE SHAPE wave and an envelope</figcaption>
</figure>

| Part | What it does |
| --- | --- |
| Frames at the top | one envelope display per operator (or pad), four to a row. **Clicking one moves TARGET there** |
| Outline | only the frame TARGET points at is drawn with a heavy border |
| TARGET | the operator (or pad) the settings below point at. It is remembered when the window closes |
| Name | to the right of TARGET, the name of what it points at ("オペレーター1", "Pad 1 (BD)" and so on) |
| AMP / PIT / SSG / S11 / P11 | **which envelope the envelope displays show** — switched for all frames at once, not per frame |
| Setting columns | one section per column, side by side; the scrollbar underneath reaches the rest |

What a frame shows differs a little by tab.

| Tab | In each frame |
| --- | --- |
| OPNA / OPN / OPL / OPL3 / OPM | the envelope |
| OPZX7S | the WAVE SHAPE wave and the envelope |
| RHYTHM | the loaded sample's waveform and the envelope |

The frames do not each get their own switch because the envelope displays are there
**to be compared** — showing a different thing in each would leave nothing to
compare.

The columns start open. A few short sections share a column (OPN's
"KEY SCALE / MUL/DET / FIX / N88 LFO", for example).

**Ctrl + ← / →** steps TARGET one at a time and **Ctrl + a digit** jumps to that
number ([Keyboard shortcuts](/2686V_2686VFX/en/reference/shortcuts/)).

## Shared controls

### Category headings

Related controls sit under a heading. **The lamp at its left shows whether it is
open.** Press the heading to open or close it.

While it is open, a backdrop sits behind its contents, so you can see where the
category ends.

**The heading's colour tells you what kind of feature it is.**

| Colour | Meaning |
| --- | --- |
| Yellow | a feature the real chip had |
| Blue | something that moves level (AMP ENV, SSG SW AMP ENV, WT AMP MOD, …) |
| Turquoise | something that moves pitch (PITCH ENV, SSG SW PITCH ENV, WT PITCH MOD, …) |
| Purple | the LFO |
| Cyan | anything else (OPTIONAL, UNISON・HARMONY, FIX, MUL・DET) |
| Red | sound quality (QUALITY) |
| Lime | other things this plugin adds |
| Off-white | a major category (below) |

Blue, turquoise, purple and cyan are all either **things the drivers of the
day did in software** or additions of ours. There were too many of them for a
single cyan to stay readable, so 3.1.0 splits them by role.

The order follows the same grouping: **hardware-derived sections come first**,
then level, then pitch, then the LFO.

### Major categories — [[AMP ENV]] / [[PITCH ENV]]

Of the sections that act on the whole channel, the ones that move level are
gathered under a **[[AMP ENV]]** heading and the ones that move pitch under
**[[PITCH ENV]]**. There are enough of them that a plain vertical list was hard
to follow. Added in 3.4.0.

<figure class="shot">
	<img src="/2686V_2686VFX/ui/parts/major_category.png" alt="Major categories" style="width:212px;" />
	<figcaption>SSG with [[AMP ENV]] open and [[PITCH ENV]] closed</figcaption>
</figure>

| Major category | Sections inside |
| --- | --- |
| **[[AMP ENV]]** | AMP ENV / SSG HW AMP ENV / SSG SW AMP ENV / SSG SW AMP ENV[11] / WT AMP MOD |
| **[[PITCH ENV]]** | PITCH ENV / SSG HW PITCH ENV / SSG SW PITCH ENV[11] / WT PITCH MOD |

- They appear on the OPNA / OPN / OPL / OPL3 / OPM / OPZX7S / SSG / WT / WT2 /
  WT+ / ADPCM / ADPCM+ / BEEP tabs. The order of the sections is unchanged
- Only the sections the tab actually has go inside. The FM tabs have no
  channel-wide SSG SW AMP ENV or PITCH ENV
- The heading has an off-white background. The lamp colours, and pressing it to
  open or close, work like any other heading
- **They start closed.** Opening one lays its sections out on a backdrop
- If simple view hides every section inside, the major category heading is
  hidden too
- Closing a major category **does not change the sound**

### Language

You can choose the display language between Japanese and English from the dropdown at the top of the SETTINGS tab.

The options are always shown in their respective native scripts — "日本語" for Japanese and "English" for English. This ensures that even if you accidentally switch to a language you cannot read, you can easily find your way back.

By default, the plugin automatically detects your OS language (falling back to English for any language other than Japanese). Switching the language rebuilds the screen on the fly.

### Simple view

There are enough sections that a tab runs long, so SETTINGS carries a switch
that **keeps the ones you use and folds the rest away**. It only hides what is
on screen — **the sound does not change**, and the values of the hidden
sections stay as they were.

The first time the window opens, you are asked whether to turn it on
([the initial settings question](/2686V_2686VFX/en/guide/install/#the-initial-settings-question)).

What gets hidden is decided per tab, and **a section the channel has as a
feature of the real chip stays**. That is why SSG keeps SSG HW AMP ENV, and
WT / WT2 / WT+ keep WT PITCH MOD. In 2686VFX the modulation panels are hidden
and the effect panels stay.

Turning it on reveals **Simple view customisation** below it. Anything ticked
there is kept on screen even while simple view is on.

| Sections |
| --- |
| AMP ENV / SSG HW AMP ENV / SSG SW AMP ENV / SSG SW AMP ENV[11] / WT AMP MOD |
| PITCH ENV / SSG HW PITCH ENV / SSG SW PITCH ENV[11] / WT PITCH MOD |
| LFO / MUL・DET / FIX / UNISON・HARMONY |

The setting is saved to the settings file along with everything else.

Where sections are laid out side by side, as on the FM and RHYTHM tabs, **a
hidden section takes its whole column with it and the columns to its right
move left.**

#### Bypassing what is hidden

Hiding is display only, so **the sound is the same as before you hid
anything**. To switch the hidden sections off in one go, press
**[Bypass hidden sections]**, which sits to the right of the simple view
switch. Only the sections currently hidden are switched off; anything you kept
on screen through customisation is left alone. The button is disabled while
simple view is off.

**MUL・DET** and **UNISON・HARMONY** carry no on/off switch — only values — so
they are left alone. On the **LFO** both PM and AM are switched off.

#### Opening and closing sections in bulk — OP / CL

**[OP]** and **[CL]** at the top right tidy up which sections are expanded.
They cover the same sections as simple view, and **apply to every channel,
every operator and every pad at once.**

| Button | What it does |
| --- | --- |
| **OP** | expands the sections that are not switched off |
| **CL** | collapses the sections that are switched off |

Use them to get to "only what I am actually using is open" in one press. They
work whether or not simple view is on. Sections with no on/off switch are left
alone by both.

Major categories follow the sections inside them. **OP** opens one if any
section inside is not switched off, and **CL** closes one if every section
inside is switched off.

### Sliders

Drag the control, or type a number in directly.

- **Hover over the button** to see the value's range (can be turned off in
  SETTINGS)
- Double-click to return to the default

### Nudge

The small buttons beside a slider move the value a step at a time. **The "N"
button shows and hides them** — fold them away when you don't need them and the
screen gets roomier.

LEVEL also takes stepped input through **Steps**. Its N button brings up
buttons that move the value by ±1.0 / ±0.1 / ±0.01 and ones that set it straight
to 1.0, 0.5, 0.25 and so on. The RHYTHM pad's LEVEL has the same.

### Waveform previews

Loaded audio files, wave memory and envelope movement are all drawn out, so you
are never assembling something from numbers alone.

### The generated waveform

The current channel's settings are played and drawn as **three lanes,
L / M / R**. It is built only when the **generate** button is pressed, never
while you play.

It runs to 10 seconds at most and stops 0.5 seconds after the sound dies away
— ten seconds of a short sound is a moment of movement followed by a flat
line.

Pick 1, 2, 5 or 10 cycles. The view starts on a cycle boundary, so the
waveform does not drift sideways.

What is built is kept in a file and not rebuilt next time. **Clear previews**
in SETTINGS removes them all.

## The file picker

Reading or writing a preset, a parameter file, a waveform or an audio sample
all open the same list, covering most of the window.

| Control | What it does |
| --- | --- |
| Keyword | While something is typed, the search reaches into the folders below |
| Kind | Narrows to OPNA / OPM / wave (WT) and so on |
| File format | Narrows to JSON / YAML / plain |
| Heading | Each click cycles unsorted, ascending, descending |
| Folder… | Jump straight to a distant folder |
| New folder / Delete folder | Create or remove a folder here |
| Generate / Regenerate | Build the waveform preview for that row |
| Generate all / Delete all | Do the whole folder at once |

Rows are chosen with a **double click** — folders included: one click selects,
two enters. The first row goes up to the parent folder.

Parameters, presets and wave files **cannot go above the plugin's own
folder**, since there is nothing selectable outside it. Audio samples (wav)
are not restricted.

Clicking outside closes the list without choosing.

## Top right of the screen

| Button | What it does |
| --- | --- |
| Undo / redo | walks the edit history |
| Initialise | resets the open tab's parameters to their defaults |
| Panic | stops any note that got stuck on |
| Preview | shows and hides the waveform preview strip |
| OP / CL | opens and closes sections in bulk (see below) |
| View mode | cycles the three views |

## View modes

| Mode | When to use it |
| --- | --- |
| **Full** | normal editing |
| **Mini player** | checking the sound while keeping the window small |
| **Minimum** | the smallest view, for when tracks are stacked up |

## Wallpaper

You can set an image file in the "Wallpaper" section of the SETTINGS tab to change the plugin background.

You can also change the display mode of the image using the "Wallpaper display scale" combo box.

For example, I will use the following image created with Adobe Firefly.

*From v3.4.0, this image can also be used as wallpaper.*

<p align="center">
  <img alt="Wallpaper source image" src="/2686V_2686VFX/figures/wp/wp_sample0.png">
</p>

## Wallpaper display modes

### Stretch

<p align="center">
  <img alt="Stretch setting" src="/2686V_2686VFX/figures/wp/wp_sample1.png">
</p>

- Stretches or shrinks the image both vertically and horizontally to fit the screen.

### Fill

<p align="center">
  <img alt="Fill setting" src="/2686V_2686VFX/figures/wp/wp_sample2.png">
</p>

- Scales the image while maintaining its aspect ratio.
  Depending on the aspect ratio of the image, the background may be covered.

### Fit

<p align="center">
  <img alt="Fit setting" src="/2686V_2686VFX/figures/wp/wp_sample3.png">
</p>

- "Fit": Similar to "Fill", but the background might be clipped conversely.
  Where it is clipped, a blurred version of the background is displayed behind it.

### Original

<p align="center">
  <img alt="Original setting" src="/2686V_2686VFX/figures/wp/wp_sample4.png">
</p>

- Displays the image in its original resolution, pasted in the center.

## The keyboard

A MIDI keyboard can be shown along the bottom, so you can try a sound without
sending MIDI from the DAW. Show or hide it in SETTINGS.

For what the computer keyboard does, see
[Keyboard shortcuts](/2686V_2686VFX/en/reference/shortcuts/).
