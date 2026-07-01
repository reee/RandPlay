# RandPlay - Random File Player

RandPlay is a Windows desktop application that randomly opens files from a user-selected directory using the system's default application. It indexes files matching video, image, or custom extensions and exposes a simple GUI: build an index, open a random file, view recent files.

- Target: Windows 10+, CMake 3.16+, Visual Studio 2019+ (or Build Tools), Windows 10 SDK
- Language: C++17 / Win32 API
- Localization: English and Simplified Chinese, loaded at runtime from resource-only DLLs

## Features

- Select a directory and scan it recursively for files
- Choose from predefined file types (videos, images) or specify custom extensions
- Build an index of matching files
- Randomly open files using the system's default application
- View the current file path and open its containing folder
- Global hotkey support for opening a random file from anywhere
- Live-switchable UI language (English / Simplified Chinese)

## Build

The canonical build is CMake. From the repo root:

```
cmake -B build -A x64
cmake --build build --config Release
```

Use `--config Debug` for a debug build. Outputs land in `build/bin/{Debug,Release}/`.

The build produces **three targets that must ship together** in the same directory:

- `RandPlay.exe` — the GUI application
- `RandPlay_en.dll` — English language resources
- `RandPlay_zh_cn.dll` — Simplified Chinese language resources

The EXE loads language resources from the DLLs at runtime; if a DLL is missing it falls back to English, then to resources embedded in the EXE.

### Build scripts

Convenience batch scripts live in `scripts/` (flat layout) and auto-locate CMake via `PATH` or `vswhere`:

| Script | Description |
|--------|-------------|
| `scripts\build.bat` | Dev build. Optional args: `debug`, `run` (combinable, e.g. `build.bat debug run`) |
| `scripts\build_release.bat` | Clean → Release build → zip distribution package under `dist\RandPlay_<version>\` |
| `scripts\dev.bat` | Interactive dev menu (build / release / clean / run / open output dir) |
| `scripts\functions.bat` | Shared helper library sourced by the other scripts |

See [`scripts/README.md`](scripts/README.md) for full details.

## How to Use

1. Click **Browse...** to select a directory
2. Choose a file type from the dropdown (Video Files, Image Files, or Custom Extension)
3. If selecting **Custom Extension**, enter the desired extension (e.g. `.txt`)
4. Click **Build Index** to scan the directory and create an index of matching files
5. Click **Open Random File** (or press the configured global hotkey) to randomly open a file
6. Right-click a recent file in the list to **Open File** or **Open Folder**
7. Use **Settings** to switch language, configure the global hotkey, or change file extensions

## Data Files

- `index.dat` — binary file index, located in the **executable's directory**
- `settings.dat` — binary settings dump, located in the **current working directory**

> Note: these two files live in different directories and diverge if the app is launched from a different working directory.

## Project Structure

```
RandPlay/
├── src/                          # Source code
│   ├── main.cpp                  # WinMain, WindowProc, message loop, global state
│   ├── constants/                # AppConstants.h, ResourceIds.h
│   ├── core/                     # Business logic (Utils, Settings, FileIndexer,
│   │                             #   LanguageManager, HotkeyManager, ResourceDLL)
│   └── ui/                       # UIHelpers
├── resources/                    # .rc files, icons, string tables
│   ├── RandPlay.rc               # EXE resources (English fallback embedded)
│   ├── RandPlay_en.rc            # English resource DLL
│   ├── RandPlay_zh_cn.rc         # Chinese resource DLL
│   ├── icons/RandPlay.ico
│   └── strings/                  # lang_en.rc, lang_zh_cn.rc (string tables)
├── include/pch.h                 # Precompiled header
├── scripts/                      # Build/dev batch scripts
│   ├── build.bat
│   ├── build_release.bat
│   ├── dev.bat
│   ├── functions.bat
│   └── README.md
├── CMakeLists.txt
├── AGENTS.md                     # Guidance for coding agents
└── CMAKE_CONFIG.md               # CMake/compiler options (partially stale)
```

## Requirements

- Windows 10 or later
- CMake 3.16+ (or the version shipped with Visual Studio)
- Visual Studio 2019+ or Visual Studio Build Tools, with the Windows 10 SDK

---

# RandPlay - 随机文件播放器

RandPlay 是一个 Windows 桌面应用程序，使用系统默认程序从用户选择的目录中随机打开文件。它索引匹配视频、图片或自定义扩展名的文件，并提供简单的图形界面：构建索引、随机打开文件、查看最近文件。

- 目标平台：Windows 10+，CMake 3.16+，Visual Studio 2019+（或 Build Tools），Windows 10 SDK
- 语言：C++17 / Win32 API
- 本地化：英文与简体中文，运行时从资源 DLL 加载

## 功能特点

- 选择目录并递归扫描文件
- 从预定义文件类型（视频、图片）中选择，或指定自定义扩展名
- 构建匹配文件的索引
- 使用系统默认程序随机打开文件
- 查看当前文件路径并打开其所在文件夹
- 支持全局快捷键，可在任意位置触发随机打开
- UI 语言可实时切换（英文 / 简体中文）

## 构建

标准构建方式为 CMake，在仓库根目录执行：

```
cmake -B build -A x64
cmake --build build --config Release
```

使用 `--config Debug` 进行调试构建。产物输出到 `build/bin/{Debug,Release}/`。

构建会产生**三个必须一同发布到同一目录的目标**：

- `RandPlay.exe` — 图形界面应用程序
- `RandPlay_en.dll` — 英文语言资源
- `RandPlay_zh_cn.dll` — 简体中文语言资源

EXE 在运行时从 DLL 加载语言资源；若 DLL 缺失，则依次回退到英文、EXE 内嵌资源。

### 构建脚本

便捷批处理脚本位于 `scripts/`（扁平结构），会通过 `PATH` 或 `vswhere` 自动定位 CMake：

| 脚本 | 说明 |
|------|------|
| `scripts\build.bat` | 开发构建。可选参数：`debug`、`run`（可组合，如 `build.bat debug run`） |
| `scripts\build_release.bat` | 清理 → Release 构建 → 在 `dist\RandPlay_<版本>\` 下打包 |
| `scripts\dev.bat` | 交互式开发菜单（构建 / 发布 / 清理 / 运行 / 打开输出目录） |
| `scripts\functions.bat` | 其他脚本共享的公共函数库 |

详见 [`scripts/README.md`](scripts/README.md)。

## 使用方法

1. 点击**浏览...**选择一个目录
2. 从下拉菜单中选择文件类型（视频文件、图片文件或自定义扩展名）
3. 若选择**自定义扩展名**，请输入所需扩展名（例如 `.txt`）
4. 点击**构建索引**扫描目录并创建匹配文件索引
5. 点击**打开随机文件**（或按下配置的全局快捷键）随机打开一个文件
6. 在最近文件列表中右键单击，可选择**打开文件**或**打开文件夹**
7. 通过**设置**切换语言、配置全局快捷键或更改文件扩展名

## 数据文件

- `index.dat` — 二进制文件索引，位于**可执行文件所在目录**
- `settings.dat` — 二进制设置转储，位于**当前工作目录**

> 注意：这两个文件位于不同目录，若从其他工作目录启动应用，二者会不一致。

## 系统要求

- Windows 10 或更高版本
- CMake 3.16+（或随 Visual Studio 安装的版本）
- Visual Studio 2019+ 或 Visual Studio Build Tools，含 Windows 10 SDK
