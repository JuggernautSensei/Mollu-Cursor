#pragma once

#define NOMINMAX

#include <array>
#include <cassert>
#include <chrono>
#include <cstdint>
#include <mutex>
#include <thread>
#include <windows.h>

#include <tlhelp32.h>

#include <d3d11.h>
#include <wrl.h>
using Microsoft::WRL::ComPtr;

#pragma comment(lib, "d3d11.lib")
#pragma comment(lib, "d3dcompiler.lib")

// 3rd party includes
#include "nlohmann/json.hpp"
using Json = nlohmann::json;

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_stdlib.h"

// my includes
#include "Base.h"