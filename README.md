# RandPlay - Random File Player

RandPlay is a Windows application that allows you to randomly open files of specific types from a selected directory.

## Features

- Select a directory to scan for files
- Choose from predefined file types (videos, images) or specify custom file extensions
- Build an index of matching files
- Randomly open files using the system's default application
- View the current file path and open its containing folder

## How to Use

1. **Build the application**:
   - Run `build.bat` to compile the application
   - Requires Visual Studio with C++ development tools installed

2. **Using the application**:
   - Click "Browse..." to select a directory
   - Choose a file type from the dropdown (Video Files, Image Files, or Custom Extension)
   - If selecting "Custom Extension", enter the desired file extension (e.g., ".txt")
   - Click "Build Index" to scan the directory and create an index of matching files
   - Click "Open Random File" to randomly select and open a file from the index
   - Use "Open Folder" to open the containing folder of the currently opened file

## Technical Details

- Built with C++ and Win32 API
- Supports recursive directory scanning
- Stores the index in a file named `index.dat` in the application directory
- Saves user settings in a file named `settings.dat` in the application directory
- Uses the system's default application to open files

## Requirements

- Windows operating system
- Visual Studio (2017 or newer) with C++ development tools

---

# RandPlay - 随机文件播放器

RandPlay 是一个 Windows 应用程序，允许您从选定目录中随机打开特定类型的文件。

## 功能特点

- 选择要扫描文件的目录
- 从预定义的文件类型（视频、图像）中选择或指定自定义文件扩展名
- 建立匹配文件的索引
- 使用系统默认应用程序随机打开文件
- 查看当前文件路径并打开其所在文件夹

## 使用方法

1. **构建应用程序**：
   - 运行 `build.bat` 编译应用程序
   - 需要安装有 C++ 开发工具的 Visual Studio

2. **使用应用程序**：
   - 点击"浏览..."选择一个目录
   - 从下拉菜单中选择文件类型（视频文件、图像文件或自定义扩展名）
   - 如果选择"自定义扩展名"，请输入所需的文件扩展名（例如，".txt"）
   - 点击"构建索引"扫描目录并创建匹配文件的索引
   - 点击"打开随机文件"随机选择并打开索引中的文件
   - 使用"打开文件夹"打开当前打开文件的所在文件夹

## 技术细节

- 使用 C++ 和 Win32 API 构建
- 支持递归目录扫描
- 在应用程序目录中将索引存储在名为 `index.dat` 的文件中
- 在应用程序目录中将设置存储在名为 `settings.dat` 的文件中
- 使用系统默认应用程序打开文件

## 系统要求

- Windows 操作系统
- Visual Studio（2017 或更新版本）与 C++ 开发工具