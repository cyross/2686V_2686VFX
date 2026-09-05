---
title: 音の流れ
description: 鍵盤を押してから音が出るまで、何がどの順で掛かるか
sidebar:
  order: 1
---

鍵盤を押してから音が出るまでに、値が何にどの順で掛かるかをまとめます。
**掛かる順番が分かると、狙った音にならないときにどこを疑えばよいかが
はっきりします。**

## プラグイン全体

<svg viewBox="0 0 720 90" role="img" aria-label="MIDI から出力までの流れ" style="max-width:100%;height:auto;">
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
  <text class="ftxt" x="197" y="50">発音中のボイス</text>
  <line class="farr" x1="264" y1="45" x2="292" y2="45" marker-end="url(#ah)" />
  <rect class="fbox" x="294" y="26" width="96"  height="38" rx="6" />
  <text class="ftxt" x="342" y="50">合成</text>
  <line class="farr" x1="392" y1="45" x2="420" y2="45" marker-end="url(#ah)" />
  <rect class="fbox" x="422" y="26" width="110" height="38" rx="6" />
  <text class="ftxt" x="477" y="50">ヘッドルーム</text>
  <line class="farr" x1="534" y1="45" x2="562" y2="45" marker-end="url(#ah)" />
  <rect class="facc" x="564" y="26" width="70"  height="38" rx="6" />
  <text class="ftxt" x="599" y="50">FX</text>
  <line class="farr" x1="636" y1="45" x2="664" y2="45" marker-end="url(#ah)" />
  <text class="ftxt" x="692" y="50">出力</text>
</svg>

1. **MIDI** — DAW から来たものと、画面の鍵盤で押したものを混ぜます
2. **発音中のボイス** — 押されている音の数だけボイスが動きます
3. **合成** — すべてのボイスを足し合わせます
4. **ヘッドルーム** — 足し合わせて大きくなりすぎたぶんを下げます。設定で
   変えられます
5. **FX** — かける順番は入れ替えられます
6. **出力**

## ボイス 1 つの中

ボイスの中では、**音程側と音量側が別々に組み立てられます。** 音程側は発振器が
進む速さを決め、音量側は出てきた波形に掛かります。

<svg viewBox="0 0 720 170" role="img" aria-label="ボイスの中で音程側と音量側が分かれる流れ" style="max-width:100%;height:auto;">
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

  <rect class="vbox" x="4" y="14" width="170" height="44" rx="6" />
  <text class="vtxt" x="89" y="34">音程側の変調</text>
  <text class="vsub" x="89" y="50">進む速さを決める</text>

  <rect class="vbox" x="4" y="112" width="170" height="44" rx="6" />
  <text class="vtxt" x="89" y="132">波形・音色</text>
  <text class="vsub" x="89" y="148">オペレータ / 波形メモリ</text>

  <path class="varr" d="M176,36 L212,36 L212,85 L238,85" marker-end="url(#ah2)" />
  <path class="varr" d="M176,134 L212,134 L212,85 L238,85" marker-end="url(#ah2)" />

  <rect class="vacc" x="240" y="63" width="104" height="44" rx="6" />
  <text class="vtxt" x="292" y="90">発振器</text>

  <line class="varr" x1="346" y1="85" x2="374" y2="85" marker-end="url(#ah2)" />

  <rect class="vbox" x="376" y="63" width="150" height="44" rx="6" />
  <text class="vtxt" x="451" y="83">音量側の変調</text>
  <text class="vsub" x="451" y="99">波形に掛ける</text>

  <line class="varr" x1="528" y1="85" x2="556" y2="85" marker-end="url(#ah2)" />

  <rect class="vbox" x="558" y="63" width="80" height="44" rx="6" />
  <text class="vtxt" x="598" y="90">パン</text>

  <line class="varr" x1="640" y1="85" x2="668" y2="85" marker-end="url(#ah2)" />
  <text class="vtxt" x="694" y="82">合成</text>
  <text class="vtxt" x="694" y="97">へ</text>
</svg>

### 音程側 — 発振器が進む速さ

上から順に掛かります。どれも**倍率**なので、掛ける順番が変わっても結果は
変わりません。ただし**エンベロープは前の値を受け取って動く**ので、そちらは
並びどおりです。

| # | 掛かるもの | 何で決まるか |
| --- | --- | --- |
| 1 | 基準の音程 | 押した鍵盤 |
| 2 | ピッチベンド | MIDI キーボード |
| 3 | **PITCH ENV** | 3 タップ。押し離しで動く |
| 4 | **SSG SW PITCH ENV[11]** | 11 タップ。押し離しで動く |
| 5 | **SSG HW PITCH ENV** | 決まった形の波を繰り返す。セントで指定 |
| 6 | **LFO の PM** | 掛かり具合はセントで指定 |
| 7 | **WT PITCH MOD** | 搬送波に対する比で速さが決まる |
| 8 | MUL / DET | 逓倍とデチューン |
| 9 | ユニゾンのデチューン | ボイスごとに違う値 |

### 音量側 — 波形に掛かる

こちらもすべて**掛け算**です。ひとつでも 0 になれば、そこから先は無音です。

| # | 掛かるもの | 何で決まるか |
| --- | --- | --- |
| 1 | 波形 | 発振器が出したもの |
| 2 | TONE / NOISE の混ぜ具合 | SSG など、ノイズを持つチャンネル |
| 3 | **LEVEL** | 画面のスライダー |
| 4 | **AMP ENV** | ADSR。SSG SW ENV / ENV11 もここに重なる |
| 5 | 基準レベル | チャンネルごとの基準 |
| 6 | **LFO の AM** | 掛かり具合は dB の減衰で指定 |
| 7 | **SSG HW AMP ENV** | 実機のハードウェアエンベロープ |
| 8 | **WT AMP MOD** | 波形メモリの形で MIN〜MAX の間を動く |
| 9 | ユニゾンの音量補正 | ボイス数が増えたぶんを下げる |

:::tip[音が出ないときは]
音量側は掛け算なので、**どれか 1 つが 0 なら音は出ません。** LEVEL、AMP ENV の
レベル、SSG HW AMP ENV と WT AMP MOD の MIN / MAX あたりを順に見てください。
:::

## 2686VFX の場合

エフェクトなので、音を作る代わりに**入ってきた音を受け取ります。**

<svg viewBox="0 0 720 90" role="img" aria-label="2686VFX の入力から出力までの流れ" style="max-width:100%;height:auto;">
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
  <text class="xtxt" x="59"  y="50">音声入力</text>
  <line class="xarr" x1="116" y1="45" x2="144" y2="45" marker-end="url(#ah3)" />
  <rect class="xbox" x="146" y="26" width="110" height="38" rx="6" />
  <text class="xtxt" x="201" y="50">ヘッドルーム</text>
  <line class="xarr" x1="258" y1="45" x2="286" y2="45" marker-end="url(#ah3)" />
  <rect class="xacc" x="288" y="26" width="160" height="38" rx="6" />
  <text class="xtxt" x="368" y="50">出力への変調</text>
  <line class="xarr" x1="450" y1="45" x2="478" y2="45" marker-end="url(#ah3)" />
  <rect class="xacc" x="480" y="26" width="70"  height="38" rx="6" />
  <text class="xtxt" x="515" y="50">FX</text>
  <line class="xarr" x1="552" y1="45" x2="580" y2="45" marker-end="url(#ah3)" />
  <text class="xtxt" x="606" y="50">出力</text>
</svg>

**変調は FX より前に掛かります。** 音量の動きもエフェクトに通したいためです。
たとえばエンベロープで音を切ったあと、その切れ目がディレイやリバーブに乗り
ます。

変調の中身は音源側と同じ順で、音程側と音量側に分かれています。ただし音程側は
発振器が無いため、**溜めた音を読み出す速さを変える**という別の作りになって
います。詳しくは
[2686VFX (エフェクト)](/2686V_2686VFX/plugins/fx-plugin/) を参照してください。

## MIDI を押していないとき

音源では、鍵盤を押さなければボイスが動かないので何も鳴りません。

2686VFX では、**鍵盤を押さなければ変調が掛からず、音はそのまま素通しします。**
FX だけを使いたいときは、鍵盤を触らずにおけば済みます。
