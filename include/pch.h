#pragma once

// Windows headers
#ifndef UNICODE
#define UNICODE
#endif

#ifndef _UNICODE
#define _UNICODE
#endif

#include <windows.h>
#include <windowsx.h>
#include <commctrl.h>
#include <shlobj.h>
#include <shlwapi.h>

// DPI awareness headers (Windows 8.1+)
#include <shellscalingapi.h>

// C++ standard library
#include <string>
#include <vector>
#include <fstream>
#include <random>
#include <filesystem>
#include <algorithm>
#include <sstream>

// Link required libraries
#pragma comment(lib, "comctl32.lib")
#pragma comment(lib, "shlwapi.lib")
#pragma comment(lib, "shcore.lib")

// Manifest for visual styles
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' \"\
\"version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
