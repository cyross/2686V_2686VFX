# レトロ音源風VST「2686V」＋レトロ音源風エフェクトVST「2686VFX」

(C)2026 CYROSS

**Please refer to `readme.en.md` for the English version.**

## はじめに

「2686V」「2686VFX」(以降、合わせて「本ソフト」と表記します)は、レトロ音源(FM/SSG/ADPCM等)風の音を出すプラグイン「2686V」と、レトロ音源風の音に変換する「2686VFX」の2つを提供します。

## 目的

DAW上で、「PC-9801-26」と「PC-9801-86」を再現したような曲を作りたかったけど眼鏡にかなうものがなかったので、プラグインを作ろう…としたはずだったが…

## 概要

- 2686V:  ぼくのかんがえたさいきょうのレトロ音源VSTプラグイン
  - 当方の技術スタックでは100%再現するのは不可能だったため、不可能なら不可能なりに色々な音源風に聞こえるプラグインを作成した
  - また、目的は「曲を作ること」だったため、コードはほぼ `Google Gemini3` による実装で執り行った。当方は動作確認やレビューがメイン
- 2686VFX: 「2686V」のエフェクト機能を他のトラックでも使えるようしたプラグイン
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
  - 幅広いパラメータ値対応
  - MML風表記によるパラメータ値反映対応
  - オペレータマスク対応
  - 効果音モードを全オペレータに適応可能
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
- 独立エフェクト対応
  - ビブラート・トレモロ・リバーブ・ディレイ・モダンビットクラッシャー・レトロビットクラッシャー
  - バイパス対応
- マスターボリューム対応
  - 標準: -6dB
- 値の範囲を確認可能
  - スライドバーのボタンをホバーすると範囲を表示
  - 設定画面にON/OFF切り替え可能
- 和音演奏可能
  - 最大同時32ボイス対応
  - ヘッドルーム確保ゲインの変更可能
- もちろん、オートメーション可能
- MIDIキーボードのピッチベント・モジュレーション機能対応
- プリセットの保存・読み込み可能
  - ADPCM・リズム音源の音声ファイルを相対ディレクトリで保存
- 壁紙の設定が可能

## 対象OS

- Windows10(x64) 以上
- Mac は…ごめんなさい!

## プラグイン規格・バージョン

- VST3(3.8)

## 操作マニュアル

- Google ドキュメントにて提供いたします
  - [2686V オペレーションマニュアル](https://docs.google.com/document/d/15j-xlIXjTfTPhNDFNUibMSHOStxEhvXMIdExFLiXkAw/edit?usp=sharing)
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
cd ./external/JUCE
git submodule update --init --recursive
git switch -d [JUCEの最新バージョン番号 例: "8.0.12"]
cd ../vst3sdk
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

- [Wikipedia: YAMAHA YM2414](https://en.wikipedia.org/wiki/Yamaha_YM2414)
- [YAMAHAのFM音源チップ、YMF825がMAKE市場向けに登場。Maker Faireで先行発売が決定だ！](https://www.dtmstation.com/archives/52000321.html)
- [MAMEのFM271部分](https://git.libretro.com/libretro/mame/-/blob/f3218cc336813f24adf720e7c48b544e7bfc2d0e/src/devices/sound/ymf271.cpp)
- [OPL3解析ドキュメント](https://moddingwiki.shikadi.net/wiki/OPL_chip)

## 更新履歴

### 2026.02.28

- v0.4.0
- <2686V>FM音源部分のバグを駆逐
  - 設定やキーを押す強さによってノイズが乗る問題を解消
- <2686V>(OPL/OPM/OPZX3)LFOのパラメータを全OP対象・各OP対象で切り分け
- <2686V>DT/DT1の表記を-3～3に変更
  - 内部では 0～7 のまま
- <2686V>MMLボタンを押した際にフォーカスがテキストボックスに移るように対応
- <2686V><2686VFX>FXタブに2つのエフェクトを追加
  - Filter
  - SoftClipper(tanH)
- <2686VFX>波形プレビューを追加
- <2686V>波形プレビューをどのタブでも有効化
- <2686V>波形プレビューを標準で表示するように変更
- オペレーションマニュアルのPDF化(プロジェクトリポジトリに添付)
- 軽微バグ修正

### 2026.02.24

- v0.3.0
- 音声出力のバグを多数修正
  - 特にリズム音源とADPCM
- <2686V>波形プレビューを2種類追加
- <2686V>仮想MIDIキーボード追加
- <2686V>プリセットタブに検索を追加
- ABOUTのUIを調整
- ロゴを追加
- マニュアル修正
- 軽微バグ修正

### 2026.02.23

- v0.2.0
- <2686V>OPM/OPZX3 のパラメータ設定から Feedback2 を削除
- <2686V>OPZX3 のアルゴリズム見直し
- <2686V>OPZX3 の各OPに任意のPCMファイルをロードする機能を追加(WSの拡張機能扱い)
- <2686V>カスタム波形メモリに以下の機能を追加
  - 波形メモリ編集画面をホバー・ドラッグすると、その値をツールチップ表示
  - 各スライダーに-1.0, -0.5,  0.0,  0.5,  1.0に変更するボタンを追加
  - 全スライダーに-1.0,  1.0  に変更するボタンを追加
- - Master Volumeの初期値を -3.0dB に変更
- リファクタリング第2弾
  - 大幅なモジュール化
  - 定数・構造体の分離
- 一部パラメータの設定バグを修正
- マニュアル修正
- 軽微バグ修正

### 2026.02.19

- v0.1.0
- <2686V>SSGにNoise On Noteモードを追加
- <2686V>SSGのNoise Freqの範囲を 0 ～ 30000 に変更
- 全体的なUIの変更・コンパクト化
- 一部パラメータの設定バグを修正
- リファクタリング第1弾
- マニュアル修正
- 軽微バグ修正

### 2026.02.16

- v0.0.3
- <2686V>プリセットにコメント欄を追加
- <2686V>プリセットのメタデータのサニタイズ対応
- <2686V>プリセットのメタデータに長さ制限対応
- <2686V>OPLにTL追加
- <2686V>SR/D2Rの値を全音源で0.0～10.0に統一
- <2686V>パラメータの値の範囲が間違っていたバグを修正
  - OPM/OPZX3のオートメーションパラメータを、もともとの音源の名称に変更しました
    - DR -> D1R, SL -> D1L, SR -> D2R
  - ※旧バージョンで作成したプリセットが正しく読み込まれなくなる可能性がございます
- <2686V>MMLモードのバグ修正
- <2686V>スタンドアロン版同梱
- <2686VFX>サイズの軽量化
- ビルド環境構築方式変更
- サンプルプリセット追加

- v0.0.2
- <2686V>ヘッドルーム設定追加

### 2026.02.15

- v0.0.1
- <2686V>初版
- <2686VFX>初版
