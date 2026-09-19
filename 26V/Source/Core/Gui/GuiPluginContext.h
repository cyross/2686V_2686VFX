#pragma once

// ============================================================================
// 画面の部品へ渡したもの (GuiContext) から、このプラグインのプロセッサと
// エディタへ戻す
// ============================================================================
// GuiContext が持つのは窓口 (GuiHost.h) だけ。タブのように、このプラグイン
// だけの持ち物を使うコードはここを通す。12 本で共有する部品では使わない。
struct GuiContext;
class AudioPlugin2686V;
class AudioPlugin2686VEditor;

AudioPlugin2686V& pluginOf(const GuiContext& ctx);
AudioPlugin2686VEditor& editorOf(const GuiContext& ctx);
