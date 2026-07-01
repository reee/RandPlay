# RandPlay 构建脚本

本目录包含 RandPlay 项目的构建与开发工具脚本。

## 脚本概览

| 脚本 | 说明 |
|------|------|
| `functions.bat` | 公共函数库：路径配置、彩色输出、CMake 自动定位等 |
| `build.bat` | 开发构建脚本，日常开发使用 |
| `build_release.bat` | 发布构建脚本，生成可分发的发布包 |
| `dev.bat` | 交互式开发菜单 |

> 所有脚本会自动定位 CMake：优先使用 PATH 中的 `cmake`，若未找到则通过 `vswhere` 使用 Visual Studio 自带的 CMake。无需手动配置环境变量。

## 使用方法

### 1. 开发构建 (`build.bat`)

```batch
scripts\build.bat              REM 默认 Release 构建
scripts\build.bat debug        REM Debug 构建
scripts\build.bat run          REM 构建后立即运行
scripts\build.bat debug run    REM 组合使用
```

构建产物（位于 `build\bin\<Config>\`）：
- `RandPlay.exe` — 应用程序主程序
- `RandPlay_en.dll` — 英文资源
- `RandPlay_zh_cn.dll` — 简体中文资源

> 三个文件必须放在同一目录下，应用启动时按语言加载对应资源 DLL。

### 2. 发布构建 (`build_release.bat`)

```batch
scripts\build_release.bat
```

执行完整清理 → Release 构建 → 打包。发布包生成在 `dist\RandPlay_<版本>\`，包含：

- `RandPlay.exe` + `RandPlay_en.dll` + `RandPlay_zh_cn.dll` + `RandPlay.ico`
- `README.md`、`LICENSE`
- `VERSION.txt`（构建信息）、`HOW_TO_USE.txt`（使用说明）

### 3. 开发菜单 (`dev.bat`)

```batch
scripts\dev.bat
```

交互式菜单：
1. 快速构建（Debug）
2. 构建（Release）
3. 发布打包
4. 清理构建目录
5. 运行 RandPlay
6. 打开构建输出目录

## 项目结构

```
RandPlay/
├── build/                  构建输出目录
│   └── bin/
│       ├── Debug/          Debug 版本产物
│       └── Release/        Release 版本产物
├── dist/                   发布包输出目录
├── scripts/                构建脚本（本目录）
├── src/                    源代码
└── ...
```

## 环境要求

- Windows 10 或更高版本
- CMake 3.16+（或随 Visual Studio 安装的版本，脚本会自动定位）
- Visual Studio 2019+ 或 Build Tools（脚本通过 `vswhere` 自动发现）

## 故障排除

1. **"cmake not found"** — 既不在 PATH 中，也未检测到 Visual Studio 安装。安装 CMake 或 VS Build Tools 后重试。
2. **构建失败** — 确认已安装 Visual Studio 2019+ 及 Windows 10 SDK；清理 `build\` 目录后重新构建。
3. **资源 DLL 缺失** — 三个产物必须同目录，否则应用会回退到 EXE 内嵌的英文资源。

## 添加新脚本

新脚本开头加载公共函数库即可复用所有工具与配置：

```batch
@echo off
setlocal EnableDelayedExpansion
call "%~dp0functions.bat"
```

可用变量：`PROJECT_ROOT`、`BUILD_DIR`、`BIN_DIR`、`PROJECT_VERSION`、`CMAKE_CMD`（`call :find_cmake` 之后）等；可用函数：`:print_header/success/error/warning/info/step`、`:find_cmake`、`:ensure_dir`、`:safe_rmdir`、`:format_size`、`:get_timestamp`。
