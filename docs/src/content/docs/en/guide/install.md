---
title: Installation
description: Where to put the plugin, and what happens on first launch
sidebar:
  order: 4
---

## Where to put it

Place the VST3 in the folder your DAW scans. On Windows this is usually:

```text
C:\Program Files\Common Files\VST3
```

The standalone build can live anywhere.

## On first launch

A working folder is created under your Documents folder:

```text
(Documents)\2686V\
```

**All twelve plugins share this folder.** Presets, settings and any parameter
files you export are kept underneath it. See
[Formats and locations](/2686V_2686VFX/en/files/format/) for what goes where.

### The initial settings question

The first time the window opens, a dialog titled **「初期設定」** (initial
settings) asks:

> パラメータを簡易表示モード(必要最低限のパラメータのみ表示)で表示しますか？
> (Show parameters in simple view, with only the essentials on screen?)

| Answer | What happens |
| --- | --- |
| **はい** (Yes) | Simple view is turned on and the default settings file is created |
| **いいえ** (No), or closing it with ESC | Simple view is turned off and the default settings file is created |

Either way, it can be switched later with **"Simple view"** on the SETTINGS
tab. Simple view only hides some sections; the sound does not change.

The default settings file (`init.settings.json`) is shared by all twelve
plugins, so answering in one means the others will not ask. If the window is
closed before answering, no file is written and the question comes back the
next time it opens.

:::note[To be asked again]
Close every plugin, then rename or delete `(Documents)\2686V\init.settings.json`
(`init.settings.yaml` when YAML is selected). This resets all default settings,
so if you want to keep them, rename the file instead, answer the question, and
load it back with "Load from settings file" on the SETTINGS tab.
:::

## Saving your settings

Changes made on screen are not persisted automatically. Press
**"Save as default settings"** on the SETTINGS tab to have them applied from the
next launch onwards.
