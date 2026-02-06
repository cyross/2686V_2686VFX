# 「2686V」「2686VFX」

## ビルド手順

### 対象OS

- Windows11(x64)

### 必要な環境

- Microsoft Visual Studio Code
- Microsoft Visual Studio 2026 Community以上
  - CMake などのコマンドラインツールインストール済み
- Developer PowerShell や Developer Command Prompt が実行できる環境
- Git
  - パスを通しておく

### 2686V_2686VFX をチェックアウト

```bash
git clone git@github.com:cyross/2686V_2686VFX.git 2686V_2686VFX
```

### サブリポジトリのチェックアウト

```bash
cd /path/of/2686V_2686VFX
git submodule update --init --recursive
cd /path/of/2686V_2686VFX/libs/vst3sdk
git submodule update --init --recursive
```

### JUCE のバージョンを最新タグにそろえる

```bash
cd /path/of/2686V_2686VFX/libs/JUCE
git switch -d [JUCEの最新バージョン番号 例: "8.0.12"]
```

### JUCE を Visual Studio 2026 向けに CMake ビルド

```bash
cd /path/of/2686V_2686VFX/libs/JUCE
cmake -B cmake-build-install -DCMAKE_INSTALL_PREFIX=/path/of/2686V_2686VFX/libs/JUCE
cmake --build cmake-build-install --target install
```

### VST3SDK のバージョンを最新タグにそろえる

```bash
cd /path/of/2686V_2686VFX/libs/vst3sdk
git switch -d v3.8.0_build_66
```

### VST3SDK を Visual Studio 2026 向けに CMake ビルド

```bash
cd /path/of/2686V_2686VFX/libs/vst3sdk
md build
cd build
cmake -G "Visual Studio 18 2026" -A x64 ..\ -DSMTG_CREATE_PLUGIN_LINK=0 -DMSTG_PLUGIN_TARGET_USER_PROGRAM_FILES_COMMON=1
```
