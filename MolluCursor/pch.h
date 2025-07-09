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

#include "nlohmann/json.hpp"
using Json = nlohmann::json;

#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"
#include "imgui/imgui_impl_win32.h"
#include "imgui/imgui_internal.h"
#include "imgui/imgui_stdlib.h"

#include "Macros.h"

#include "Enums.h"

struct Vec2
{
    constexpr Vec2() = default;
    constexpr Vec2(const float _x, const float _y)
        : x(_x)
        , y(_y)
    {
    }

    constexpr ~Vec2() = default;

    constexpr Vec2(const Vec2& _other)                = default;
    constexpr Vec2& operator=(const Vec2& _other)     = default;
    constexpr Vec2(Vec2&& _other) noexcept            = default;
    constexpr Vec2& operator=(Vec2&& _other) noexcept = default;

    float x = 0.f;
    float y = 0.f;

    static Vec2 s_one;
    static Vec2 s_zero;
};

template<size_t N>
class FString
{
    static_assert(N > 0, "FString buffer size must be greater than 0");

public:
    constexpr FString() noexcept  = default;
    constexpr ~FString() noexcept = default;

    explicit FString(const char* _str)
    {
        ASSERT(_str != nullptr, "String cannot be null");
        Set(_str);
    }

    explicit FString(const std::string_view _view)
    {
        Set(_view);
    }

    constexpr FString(const FString&) noexcept            = default;
    constexpr FString(FString&&) noexcept                 = default;
    constexpr FString& operator=(const FString&) noexcept = default;
    constexpr FString& operator=(FString&&) noexcept      = default;

    constexpr FString& operator=(const char* _str)
    {
        ASSERT(_str != nullptr, "String cannot be null");
        Set(_str);
        return *this;
    }

    constexpr FString& operator=(const std::string_view _sv)
    {
        Set(_sv);
        return *this;
    }

    void Set(const char* _str)
    {
        ASSERT(_str != nullptr, "String cannot be null");
        size_t len = std::strlen(_str);
        Set(_str, len);
    }

    constexpr void Set(const std::string_view _view)
    {
        Set(_view.data(), _view.size());
    }

    constexpr void Set(const char* _str, const size_t len)
    {
        ASSERT(_str != nullptr, "String cannot be null");
        ASSERT(len < N, "String length exceeds buffer size");
        size_t copyLen = (len < N - 1 ? len : N - 1);
        std::memcpy(m_str, _str, copyLen);
        m_str[copyLen] = '\0';
        m_length       = copyLen;
    }

    constexpr void Append(const char* _str, const size_t len)
    {
        ASSERT(_str != nullptr, "String cannot be null");
        ASSERT(m_length + len < N, "String length exceeds buffer size");
        size_t copyLen = (len < N - m_length - 1 ? len : N - m_length - 1);
        std::memcpy(m_str + m_length, _str, copyLen);
        m_length += copyLen;
        m_str[m_length] = '\0';
    }

    constexpr void Append(const std::string_view _view)
    {
        Append(_view.data(), _view.size());
    }

    void Append(const char* _str)
    {
        ASSERT(_str != nullptr, "String cannot be null");
        size_t len = std::strlen(_str);
        Append(_str, len);
    }

    NODISCARD constexpr char*       data() noexcept { return m_str; }
    NODISCARD constexpr const char* c_str() const noexcept { return m_str; }
    NODISCARD constexpr size_t      size() const noexcept { return m_length; }
    NODISCARD constexpr size_t      capacity() const noexcept { return N; }
    NODISCARD constexpr bool        empty() const noexcept { return m_length == 0; }

    NODISCARD constexpr char& operator[](size_t idx)
    {
        ASSERT(idx < m_length, "Index out of range");
        return m_str[idx];
    }
    NODISCARD constexpr const char& operator[](size_t idx) const
    {
        ASSERT(idx < m_length, "Index out of range");
        return m_str[idx];
    }

    constexpr void clear() noexcept
    {
        m_str[0] = '\0';
        m_length = 0;
    }

    NODISCARD constexpr bool operator==(const FString& other) const noexcept
    {
        if (m_length != other.m_length) return false;
        for (size_t i = 0; i < m_length; ++i)
        {
            if (m_str[i] != other.m_str[i]) return false;
        }
        return true;
    }

    NODISCARD constexpr bool operator!=(const FString& other) const noexcept
    {
        return !(*this == other);
    }

    NODISCARD constexpr bool operator==(std::string_view _view) const noexcept
    {
        if (m_length != _view.size()) return false;
        for (size_t i = 0; i < m_length; ++i)
        {
            if (m_str[i] != _view[i]) return false;
        }
        return true;
    }

    NODISCARD constexpr bool operator!=(std::string_view _view) const noexcept
    {
        return !(*this == _view);
    }

private:
    char   m_str[N] = {};
    size_t m_length = 0;
};

class ScopedStopWatch
{
public:
    explicit ScopedStopWatch(const std::string_view _msg = "")
        : m_msg(_msg)
        , m_start(std::chrono::high_resolution_clock::now())
    {
        std::string message = std::format("\n********** {} - stop watch start **********\n", _msg);
        OutputDebugStringA(message.c_str());
    }

    ~ScopedStopWatch()
    {
        auto        end      = std::chrono::high_resolution_clock::now();
        auto        duration = std::chrono::duration_cast<std::chrono::nanoseconds>(end - m_start).count();
        std::string message  = std::format("\n********** duration: {} ms - stop watch end **********\n", static_cast<float>(duration) * 1e-6f);
        OutputDebugStringA(message.c_str());
    }

    ScopedStopWatch(const ScopedStopWatch&)                = delete;
    ScopedStopWatch(ScopedStopWatch&&) noexcept            = delete;
    ScopedStopWatch& operator=(const ScopedStopWatch&)     = delete;
    ScopedStopWatch& operator=(ScopedStopWatch&&) noexcept = delete;

private:
    std::string_view                               m_msg;
    std::chrono::high_resolution_clock::time_point m_start;
};

struct GameDetectorData
{
    bool bDetected = false;

    std::wstring targetProgramName = {};
    int          windowPosX        = 0;
    int          windowPosY        = 0;
    int          windowWidth       = 0;
    int          windowHeight      = 0;
    eAspectRatio aspectRatio       = eAspectRatio::None;
};

struct Macro
{
    NODISCARD Json Serialize() const;
    bool           Deserialize(const Json& _json);

    std::string name     = "macro";
    eKey        hotkey   = eKey::None;
    Vec2        position = Vec2::s_zero;
    bool        bEnable  = true;

    // action
    eMacroButton button = eMacroButton::LeftButton;
    eMacroAction action = eMacroAction::ClickOnce;
    bool         bMove  = true;

    // [internal - for repeat]
    float timeCounterSec    = 0.f;
    float repeatIntervalSec = 0.2f;
};
