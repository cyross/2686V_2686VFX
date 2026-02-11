# レトロ音源風VST「2686V」＋レトロ音源風エフェクトVST「2686VFX」

(C)2026 CYROSS

## はじめに

「2686V」「2686VFX」(以降、合わせて「本ソフト」と表記します)

## 目的

- 2686V:  DAW上で、80~90年代のパソコンやゲーム機で使用されていたFM音源ボードを再現させる…はずだったが、結果的に「ぼくのかんがえたさいきょうのレトロ音源VST」に
- 2686VFX: 「2686V」のエフェクト機能を他のトラックでも使いたかった

## OS

- Windows10/11(x64)

## プラグイン規格・バージョン

- VST3(3.8)

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

- 開いているターミナルで以下のコマンドを入力する

```bash
cd /path/of/root
git clone git@github.com:cyross/2686V_2686VFX.git 2686V_2686VFX
```

### サブモジュールの更新

- 開いているターミナルで以下のコマンドを入力する
- 同時に、サブモジュールのバージョンを最新版にしておく

```bash
cd ./2686V_2686VFX/external/JUCE
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

- 開いているターミナルで以下のコマンドを入力する

```bash
cd /path/of/root/2686V_2686VFX/
md build
cd build
cmake .. -G "Visual Studio 18 2026"
```

### CMakeLists.txt の編集したい時の手順

- VS2026上で更新すると挙動が変になることがあったので、無用なトラブルを避けることもあって、以下のように行っている

- VS2026 を終了する
- VSCode でプロジェクトディレクトリ(もしくはワークスペース)を開く
- `CMakeFile.txt` を編集(このとき、CMakeが動くときがあるが気にしない)
- CMake のビルドエラーが無いことを確認してVSCodeを終了
- `Developer PowerShell` か `Developer Command Prompt` を開く
- 開いたターミナルで以下のコマンドを入力する

```bash
cd /path/of/root/2686V_2686VFX/
rmdir -Force build
md build
cd build
cmake .. -G "Visual Studio 18 2026"
```
- エラー無く終了したのを確認して、VS2026 を開き、ソリューションファイルを選択

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

- X: https://x.com/cyross4fortnite
- Youtube: https://www.youtube.com/@%E3%81%95%E3%81%84%E3%82%8D%E3%81%99
- ニコニコ動画: https://www.nicovideo.jp/user/3223933
- Piapro: https://piapro.jp/cyross_p
