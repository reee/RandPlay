# Win32 应用开发最佳实践：中文支持与 HiDPI 适配

## 一、中文支持最佳实践

### 1. 使用 Unicode API
- 始终使用宽字符版本的 Win32 API（W 后缀）
- 使用 `CreateWindowExW()` 而非 `CreateWindowExA()`
- 使用 `RegisterClassExW()` 而非 `RegisterClassExA()`
- 使用 `MessageBoxW()` 而非 `MessageBoxA()`

### 2. 字符类型和字符串字面量
- 使用 `WCHAR` 类型存储字符串
- 使用 `L""` 前缀定义宽字符串字面量
- 推荐使用 `TCHAR` 和 `TEXT()` 宏以保持兼容性
- 示例：`WCHAR szTitle[] = L"我的应用程序";`

### 3. 源文件编码设置
- 将 .cpp 文件保存为 **UTF-8 with BOM** 格式
- Visual Studio 中：文件 → 高级保存选项 → UTF-8 with signature
- 添加编译器选项：`/utf-8`
- CMake 配置：`add_compile_options(/utf-8)`

### 4. 字体选择
使用 `CreateFontW()` 创建支持中文的字体：
- 字符集参数设为 `DEFAULT_CHARSET`
- 推荐字体：`Microsoft YaHei UI`（微软雅黑）
- 质量参数使用 `CLEARTYPE_QUALITY` 以获得更好的渲染效果
- 确保字体名称使用 `L""` 前缀

## 二、HiDPI 支持最佳实践

### 1. 声明 DPI 感知

#### 方法 A：应用程序清单文件
创建 `app.manifest` 文件并添加 DPI 感知声明：
- 使用 `<dpiAwareness>` 标签声明 Per-Monitor V2（Windows 10 1703+）
- 使用 `<dpiAware>` 标签提供向后兼容（Windows 8.1+）
- 推荐设置：`PerMonitorV2` 和 `true/PM`

#### 方法 B：代码中启用
在 `WinMain` 开始处调用 DPI 感知 API（按优先级）：
- `SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2)` - Windows 10 1703+
- `SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE)` - Windows 8.1+
- `SetProcessDPIAware()` - 更旧系统的回退方案
- 需要链接 `Shcore.lib`

### 2. 动态获取和处理 DPI

#### 获取窗口 DPI
- 优先使用 `GetDpiForWindow(hwnd)` API（Windows 10 1607+）
- 需要动态加载该函数（通过 `GetProcAddress`）
- 回退方案：使用 `GetDeviceCaps(hdc, LOGPIXELSX)` 获取主显示器 DPI

#### DPI 缩放计算
- 标准 DPI 为 96
- 缩放公式：`scaledValue = MulDiv(baseValue, currentDpi, 96)`
- 避免硬编码像素值，所有尺寸都应根据 DPI 动态缩放

### 3. 处理 DPI 变化

#### WM_DPICHANGED 消息
- 当窗口移动到不同 DPI 的显示器时触发
- `HIWORD(wParam)` 包含新的 DPI 值
- `lParam` 包含系统建议的新窗口矩形
- 使用 `SetWindowPos()` 应用建议的窗口大小和位置

#### UI 更新响应
处理 DPI 变化时需要：
- 重新创建所有字体（根据新 DPI 计算字体大小）
- 调整所有控件的位置和大小
- 更新图标和位图资源
- 调用 `InvalidateRect()` 触发重绘

### 4. DPI 感知的资源管理

#### 字体管理
- 为当前 DPI 动态创建字体
- 在 DPI 变化时删除旧字体并创建新字体
- 将字体句柄发送给子控件（`WM_SETFONT` 消息）

#### 控件布局
- 为所有控件实现基于 DPI 的位置和大小计算
- 在创建时和 DPI 变化时都要应用缩放
- 使用 `SetWindowPos()` 更新控件位置

## 三、CMake 配置建议

### 编译选项
- 设置 C++ 标准（推荐 C++17 或更高）
- 添加 MSVC UTF-8 编译选项：`/utf-8`
- 使用生成器表达式确保仅在 MSVC 编译器下应用

### 链接库
必需的库：
- `Shcore.lib` - DPI 感知 API
- `User32.lib` - 窗口管理
- `Gdi32.lib` - 图形设备接口

## 四、测试建议

### 测试场景
1. **不同 DPI 缩放级别**
   - 100%（96 DPI）
   - 125%（120 DPI）
   - 150%（144 DPI）
   - 200%（192 DPI）

2. **多显示器环境**
   - 不同 DPI 的显示器组合
   - 跨显示器拖动窗口
   - 在不同显示器上启动应用

3. **中文显示**
   - 简体中文字符显示
   - 繁体中文字符显示
   - 中英文混排效果
   - 长文本自动换行

## 五、资源文件处理

### .rc 文件编码
- 资源脚本文件应保存为 **UTF-16 LE** 编码
- 包含中文字符串的资源必须使用宽字符
- 图标和位图资源应准备多个 DPI 版本


## 六、向后兼容性

### 系统版本支持
- Windows 10 1703+：完整的 Per-Monitor V2 支持
- Windows 10 1607+：`GetDpiForWindow` API
- 不需要支持更老的系统