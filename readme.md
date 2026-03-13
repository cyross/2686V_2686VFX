# レトロ音源風VST「2686V」＋レトロ音源風エフェクトVST「2686VFX」

(C)2026 CYROSS

**Please refer to `readme.en.md` for the English version.**

## はじめに

「2686V」「2686VFX」(以降、合わせて「本ソフト」と表記します)は、80年代～90年代にプレイしまくったゲームの音楽の雰囲気を`できるだけ`近づけた`(はず)`レトロゲーム音源(FM/SSG/ADPCM等)`風`の音を出すプラグイン「2686V」と、レトロゲーム音源`風`の音に変換する「2686VFX」の2つを提供します。

## 目的

DAW上で、「PC-9801-26」と「PC-9801-86」を再現したような曲を作りたかったけど眼鏡にかなうものがなかったので、プラグインを作ろう…としたはずだったが…

## 概要

- 2686V:  ぼくのかんがえたさいきょうのレトロゲーム音源VSTプラグイン
  - 当方の技術スタックでは100%再現するのは不可能だったり、音作りの自由度を上げることも鑑み、実在の音源に近づけるよりも、当時の雰囲気を醸し出すのに必要十分なプラグインを作ることにした
    - そのため、実機の音を確認しながらクオリティを追求する気はナッシングです(ごめん！)
  - また、目的は「曲を作ること」だったため、コードはほぼ `Google Gemini3` による実装で執り行った。当方は動作確認やレビューがメイン
- 2686VFX: 「2686V」のエフェクト機能をDAW上のどのトラックでも使えるようしたプラグイン
  - 面白そうだと思って

## 特徴

- 多数のFM音源オペレーター(風音源)に対応
  - OPL/OPL3/OPM/OPN/OPNA
  - 幅広いパラメータ値対応
    - 浮動小数点対応
  - MMLによるパラメータ値反映対応
  - オペレータマスク対応
  - 効果音モードを全オペレータに適応可能
  - (OPL/OPM)LFOのパラメータを全OP対象・各OP対象で切り分け
  - オリジナルのFM音源オペレーター・OPZX3対応
    - OPZのオペレーター、OPXのアルゴリズム、MA-3(SD-1)の原波形を盛り込んだオペレーター
      - 謎な部分も多く(特にOPXのアルゴリズム)、近似的な実装となっている
    - さらに、OPXが持つPCM音源(音声ファイル)をオペレーターの波形として利用するモードも搭載
    - さらに、Geminiがお勧めしてきたオリジナル波形も多数用意したことで、**60種以上**の波形を利用可能
  - 幅広いパラメータ値に対応
  - MML風表記によるパラメータ値反映対応
  - オペレータマスク対応
  - 効果音モードを全オペレータに適応可能
  - (OPNA/OPM/OPZX3)ハードウェアLFOの仕様整理と、ソフトウェアLFOの追加
    - チャネル単位(2686Vでは音源全体)とオペレーター単位でのLFO設定が入り混じったものを、音源全体(グローバル)とチャネル単位のLFOを分離して実装
- SSG(風音源)対応
  - 完全再現ではなく、PSGの機能も盛り込んだ強化版
  - パルス波は、SSGだけではなくファミコンやKONAMI VRC VIのデューティ比に対応
  - 位相逆転可能
- 波形メモリ(WAVETABLE)(風音源)対応
  - 複数ビットレートと32/64サンプルに対応
  - プリセット波形メモリ
- リズム音源(風音源)サポート
  - ビットクラッシャー付きのドラムマシン
    - ビットレートとサンプリングレートを変更可能
    - 4bitのときはリニアPCMとADPCMを選択可能
  - 8個のパッドが付属(2608Bでは6個だったが、8個に拡張)
    - 各パッドに音声ファイルと割り当て対応
    - MIDI CCの割り当て対応
- ADPCM(風音源)サポート
  - 1つの音声ファイルをサンプラーのように再生可能
    - ビットレートとサンプリングレートを変更可能
    - 4bitのときはリニアPCMとADPCMを選択可能
- BEEP音サポート
  - 音階を付けて鳴らせるモードと、指定周波数で鳴らせるモードの2種類準備
  - ADSRエンベロープ対応
- 独立エフェクト対応
  - 搭載エフェクトは以下
    - フィルター
    - ソフトウェアLFO
    - ビブラート
    - トレモロ
    - リバーブ
    - ディレイ
    - モダンビットクラッシャー
    - レトロビットクラッシャー
    - ソフトクリッパー
  - 各FXと全体のバイパス対応
- マスターボリューム対応
  - 標準: -3dB
- 値の範囲を確認可能
  - スライドバーのボタンをホバーすると範囲を表示
  - 設定画面にON/OFF切り替え可能
- 和音演奏可能
  - 最大同時32ボイス対応
  - ヘッドルーム確保ゲインの変更可能
  - モノフォニックモードとの切り替え可能(1.2.0-)
- もちろん、オートメーション可能
- MIDIキーボードのピッチベント・モジュレーション機能対応
- プリセットの保存・読み込み可能
  - ADPCM・リズム音源の音声ファイルを相対ディレクトリで保存
- 壁紙の設定が可能
- 波形のプレビュー機能搭載
  - 固定音色プレビュー
  - リアルタイムプレビュー
- 仮想MIDIキーボード対応

## 対象OS

- Windows10(x64) 以上
- Mac は…ごめんなさい!
  - 先日ご報告があり、少量の修正で MacOS 上の clang++ でビルドできたというご報告いただきました。感謝!
    - `atsushieno` さん、ありがとうございます！
  - 1.2.0 のソースには、ご指摘いただいた修正を盛り込んでおります。

## プラグイン規格・バージョン

- VST3(3.8)

## 操作マニュアル

- `GitHub Wiki` にて提供いたします
  - [2686V オペレーションマニュアル](https://github.com/cyross/2686V_2686VFX/wiki/2686V-2686VFX-%E3%82%AA%E3%83%9A%E3%83%AC%E3%83%BC%E3%82%B7%E3%83%A7%E3%83%B3%E3%83%9E%E3%83%8B%E3%83%A5%E3%82%A2%E3%83%AB)
  - **但し、ほとんど出来上がっておりませんので、気長にお待ちください…(ごめんなさい!)**
  - とはいえ、UIをご覧いただければ直感でお判りいただけるかと…

## ビルドしたVST3ファイルの配布

- `GitHub` のリリースノートか `Booth` にて配布予定

## ビルド手順

### 必要な環境

- `Microsoft Visual Studio Code`
- `Microsoft Visual Studio 2026 Community` 以上
- `Developer PowerShell` か `Developer Command Prompt` を起動できる環境
  - `CMake` などのコマンドラインツールインストール済み
- `Git`
  - パスを通しておく
  - GitHubへのアクセスが可能なことを確認しておく

### 手順1: ターミナルを開く

- `Developer PowerShell` か `Developer Command Prompt` を起動する

### 2686V_2686VFX をチェックアウト

- 開いているターミナルで、適当なディレクトリ(プロジェクトを管理しているディレクトリなど)に移動する
- 移動したら、以下のコマンドを入力する

```bash
cd /path/of/root
git clone git@github.com:cyross/2686V_2686VFX.git 2686V_2686VFX
```

- ※以降、`適当なディレクトリ/2686V_2686VFX`ディレクトリパスを **プロジェクトルート** と呼ぶ
- 同じターミナルで、プロジェクトルートに移動する

```bash
cd 2686V_2686VFX
```

### プロジェクトをバージョンなどを確認・更新

- クローンが終わったところで、プロジェクトのブランチを確認する
- 使いたいバージョンに変えるのもここで行う

```bash
git branch # ブランチが "main" になっていることを確認
git switch -d vx.y.z # 指定のバージョンのソースに置き換える
```

### サブモジュールの更新

- 続けて、サブモジュールのバージョンを最新版にしておく
- 前項と同じターミナルで、カレントディレクトリがプロジェクトルートにあることを確認する
- ターミナルで以下のコマンドを入力する

```bash
cd ./external
mkdir JUCE
cd JUCE
git submodule update --init --recursive
git switch -d [JUCEの最新バージョン番号 例: "8.0.12"]
cd ..
mkdir vst3sdk
cd vst3sdk
git submodule update --init --recursive
git switch -d [vst3sdkの最新バージョン番号 例: "v3.8.0_build_66"]
cd base
git submodule update --init --recursive
cd ../cmake
git submodule update --init --recursive
cd ../doc
git submodule update --init --recursive
cd ../pluginterfaces
git submodule update --init --recursive
cd ../public.sdk
git submodule update --init --recursive
cd ../tutrials
git submodule update --init --recursive
cd ../vstgui4
git submodule update --init --recursive

```

## 2686V/2686VFX を Visual Studio 2026 向けに CMake ビルド

- Visual Studio 2026 で、 「folderを開く」を選択
- フォルダ選択ダイアログが開いたら、プロジェクトルートを選択して「OK」ボタンをクリックする
- Visual Studio 2026 の画面が開いたら、「プロジェクト」メニューの「2686V の構成」を選択する
- エラー無く無事に構成ビルドが終われば完了

## ソリューション下で編集・ビルド

- 「ソリューションエクスプローラー」のリストに `2686V_Bundle.sln` があるのでそれを選択する
- あとは、ソースを編集したりビルドすることでVST3ファイルやスタンドアロンexeファイルを作成できる

## 今後の課題

- リファクタリングの徹底
- ドキュメントの整備

## ライセンス / License

本ソフトウェアのソースコード自体は、MITライセンスの下で公開されています（詳細はリポジトリ内の LICENSE ファイルをご参照ください）。

ただし、ビルドされたバイナリファイル（VST3プラグイン）には GPLv3 (GNU General Public License v3) が適用されます。 これは、本ソフトウェアが「JUCE」フレームワークおよび「VST3 SDK」とリンクしており、それらのライセンス（GPLv3/AGPLv3）を継承するためです。

Source code in this repository is available under the MIT License. However, the compiled binary links against the JUCE framework and VST3 SDK, which are subject to the GPLv3. Therefore, any distributed binaries must comply with the GPLv3 terms.

禁止事項: 本ソフトウェア、または本ソフトウェアに含まれる素材ファイル等を、著作者を偽って再配布すること、および著作者の許諾なく商用製品として販売することを禁じます（GPLv3の規定に基づく著作権表示の改変・削除は認められません）。

## 利用許諾・免責事項

【楽曲への利用について】 本ソフトウェアを使用して制作された楽曲・効果音等の音声作品については、商用・非商用を問わず、ロイヤリティフリー（無料）で自由にご利用いただけます。 本ソフトウェアを利用して生成・創作された作品の著作権は、その作品の制作者（利用者ご本人）に帰属します。

【免責事項】 本ソフトウェアは現状のまま提供されます。 本ソフトウェアを利用したことによって生じた、いかなる損害、不利益、トラブル等について、著者は一切の責任を負わないものとします。

【商標について】 VST is a trademark of Steinberg Media Technologies GmbH.

### vst3sdk の再配布ライセンスファイル

vst3sdkのライセンスは、`external/vst3sdk/LICENSE.txt` にございます。

### JUCE の再配布ライセンスファイル

JUCEのライセンスは、`external/JUCE/LICENSE.md` にございます。

## 連絡先

当方への連絡先は以下にお願いいたします。

- `cyross4github@gmail.com`

## SNS他

- [X](https://x.com/cyross4fortnite)
- [Youtube](https://www.youtube.com/@%E3%81%95%E3%81%84%E3%82%8D%E3%81%99)
- [ニコニコ動画](https://www.nicovideo.jp/user/3223933)
- [Piapro](https://piapro.jp/cyross_p)
- [Booth](https://cyross.booth.pm/)

## 参考文献

- 「マシン語サウンドプログラミング」
  - 青山学／日高徹 著
  - アスキー出版局
  - 1994年

- 「PC-9801-86サウンドボード ユーザーズマニュアル」
  - 日本電気 編
  - 日本電気
  - 1993年

- 「Inside X68000」
  - 桒野雅彦 著
  - ソフトバンク
  - 1992年

- 「MA-5 オーサリングツール ユーザーマニュアル(ATS-SMAFPhraseL2版)」
  - ヤマハ株式会社 編
  - ヤマハ株式会社
  - 2006年

- YMF262のデータシート

- [Wikipedia - YAMAHA YM2414](https://en.wikipedia.org/wiki/Yamaha_YM2414)
- [DTM STATION - YAMAHAのFM音源チップ、YMF825がMAKE市場向けに登場。Maker Faireで先行発売が決定だ！](https://www.dtmstation.com/archives/52000321.html)
- [MAMEのFM271部分](https://git.libretro.com/libretro/mame/-/blob/f3218cc336813f24adf720e7c48b544e7bfc2d0e/src/devices/sound/ymf271.cpp)
- [OPL3解析ドキュメント](https://moddingwiki.shikadi.net/wiki/OPL_chip)
- [YAMAHA V50の取扱説明書](https://jp.yamaha.com/support/manuals/index.html?l=ja&k=V50)
- [YAMAHA TX81Zの取扱説明書](https://jp.yamaha.com/products/music_production/synthesizers/tx81z/downloads.html#product-tabs)
- [Programmer's Guide to OPL3 FM Music Synthesizer](https://www.gamedev.net/tutorials/programming/general-and-gameplay-programming/programmers-guide-to-opl3-fm-music-synthesizer-r447/)

## 更新履歴

[「2686V オペレーションマニュアル」の更新履歴](https://github.com/cyross/2686V_2686VFX/wiki/%E6%9B%B4%E6%96%B0%E5%B1%A5%E6%AD%B4)をご参照ください
