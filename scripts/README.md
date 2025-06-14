# RandPlay 构建脚本使用说明

本目录包含 RandPlay 项目的构建和开发工具脚本。

## 脚本概览

### 核心脚本

- **`functions.bat`** - 公共函数库，包含所有脚本使用的工具函数和配置
- **`build.bat`** - 开发构建脚本，用于日常开发
- **`build_release.bat`** - 发布构建脚本，生成完整的发布包
- **`dev.bat`** - 开发工具菜单，提供交互式操作界面

### 使用方法

#### 1. 开发构建 (`build.bat`)

```batch
# 基本构建 (Release)
scripts\build.bat

# 构建调试版本
scripts\build.bat debug

# 构建指定目标
scripts\build.bat en          # 只构建英文版
scripts\build.bat zh          # 只构建中文版

# 构建后立即运行
scripts\build.bat release run
```

#### 2. 发布构建 (`build_release.bat`)

```batch
# 生成完整发布包
scripts\build_release.bat
```

发布包将生成在 `dist\RandPlay_{version}\` 目录中，包含：
- 英文版和中文版可执行文件
- 应用程序图标
- 文档文件 (README.md, LICENSE)
- 版本信息文件 (VERSION.txt)
- 使用说明文件 (HOW_TO_USE.txt)

#### 3. 开发工具 (`dev.bat`)

```batch
# 启动交互式开发菜单
scripts\dev.bat
```

提供以下功能：
1. 快速构建 (Debug)
2. 发布构建 (Release)
3. 清理构建
4. 运行英文版
5. 运行中文版
6. 查看构建状态

## 项目结构

```
RandPlay/
├── build/                  # 构建输出目录
│   ├── bin/               # 可执行文件输出
│   │   ├── Debug/         # Debug 版本
│   │   └── Release/       # Release 版本
│   └── ...                # CMake 生成的文件
├── dist/                  # 发布包目录
├── scripts/               # 构建脚本目录 (本目录)
├── src/                   # 源代码
└── ...
```

## 环境要求

- Windows 10 或更高版本
- CMake 3.15 或更高版本
- Visual Studio 2019 或更高版本 (或 Build Tools)
- 管理员权限 (某些操作可能需要)

## 故障排除

### 常见问题

1. **"cmake 未找到"**
   - 确保 CMake 已安装并添加到系统 PATH

2. **构建失败**
   - 检查 Visual Studio 是否正确安装
   - 确保项目目录路径不包含中文或特殊字符
   - 尝试清理构建目录后重新构建

3. **权限错误**
   - 以管理员身份运行命令提示符
   - 检查防病毒软件是否阻止了文件操作

### 清理构建环境

如果遇到构建问题，可以清理构建环境：

```batch
# 使用开发工具清理
scripts\dev.bat  # 选择选项 3

# 或手动删除构建目录
rmdir /s /q build
rmdir /s /q dist
```

## 自定义配置

所有配置都在 `functions.bat` 中定义：

- `PROJECT_NAME` - 项目名称
- `PROJECT_VERSION` - 项目版本
- `DEFAULT_CONFIG` - 默认构建配置
- 各种路径配置

## 添加新脚本

如果需要添加新的构建脚本：

1. 在脚本开头添加：
   ```batch
   @echo off
   setlocal EnableDelayedExpansion
   
   REM 加载公共函数
   call "%~dp0functions.bat"
   ```

2. 使用预定义的函数和变量
3. 遵循现有的命名约定和错误处理模式

## 贡献

如果您发现构建脚本的问题或有改进建议，请：

1. 检查是否已有相关的 issue
2. 创建新的 issue 描述问题
3. 提交 Pull Request (如果有解决方案)

---

更新日期: 2025年6月14日
