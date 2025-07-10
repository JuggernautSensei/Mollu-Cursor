#pragma once

#ifdef _DEBUG
#    define ASSERT(x, ...)                                                                                                         \
        do                                                                                                                         \
        {                                                                                                                          \
            if (!(x))                                                                                                              \
            {                                                                                                                      \
                fprintf(stderr, "Assertion failed: %s, file %s, line %d\n", std::format(__VA_ARGS__).c_str(), __FILE__, __LINE__); \
                __debugbreak();                                                                                                    \
            }                                                                                                                      \
        } while (0)
#else
#    define ASSERT(x, ...) ((void)0)
#endif

namespace detail
{

void CrashImpl(std::string_view _msg);

}

#ifdef _DEBUG

// 디버그 모드에서는 __debugbreak()를 호출. 매크로 정의 -> 콜 스택 때문
#    define CRASH(...)                                   \
        do                                               \
        {                                                \
            detail::CrashImpl(std::format(__VA_ARGS__)); \
            __debugbreak();                              \
            std::terminate();                            \
        } while (0)

#else
#    define CRASH(...)                                   \
        do                                               \
        {                                                \
            detail::CrashImpl(std::format(__VA_ARGS__)); \
            std::terminate();                            \
        } while (0)

#endif

#define NODISCARD [[nodiscard]]

enum class eKey : unsigned int
{
    None              = 0x00,
    Cancel            = 0x03,
    Backspace         = 0x08,
    Tab               = 0x09,
    Clear             = 0x0C,
    Enter             = 0x0D,
    Shift             = 0x10,
    Control           = 0x11,
    Alt               = 0x12,
    Pause             = 0x13,
    CapsLock          = 0x14,
    Kana              = 0x15,
    Junja             = 0x17,
    Final             = 0x18,
    Kanji             = 0x19,
    Escape            = 0x1B,
    Convert           = 0x1C,
    NonConvert        = 0x1D,
    Accept            = 0x1E,
    ModeChange        = 0x1F,
    Space             = 0x20,
    PageUp            = 0x21,
    PageDown          = 0x22,
    End               = 0x23,
    Home              = 0x24,
    Left              = 0x25,
    Up                = 0x26,
    Right             = 0x27,
    Down              = 0x28,
    Select            = 0x29,
    Print             = 0x2A,
    Execute           = 0x2B,
    PrintScreen       = 0x2C,
    Insert            = 0x2D,
    Delete            = 0x2E,
    Help              = 0x2F,
    Key0              = 0x30,
    Key1              = 0x31,
    Key2              = 0x32,
    Key3              = 0x33,
    Key4              = 0x34,
    Key5              = 0x35,
    Key6              = 0x36,
    Key7              = 0x37,
    Key8              = 0x38,
    Key9              = 0x39,
    A                 = 0x41,
    B                 = 0x42,
    C                 = 0x43,
    D                 = 0x44,
    E                 = 0x45,
    F                 = 0x46,
    G                 = 0x47,
    H                 = 0x48,
    I                 = 0x49,
    J                 = 0x4A,
    K                 = 0x4B,
    L                 = 0x4C,
    M                 = 0x4D,
    N                 = 0x4E,
    O                 = 0x4F,
    P                 = 0x50,
    Q                 = 0x51,
    R                 = 0x52,
    S                 = 0x53,
    T                 = 0x54,
    U                 = 0x55,
    V                 = 0x56,
    W                 = 0x57,
    X                 = 0x58,
    Y                 = 0x59,
    Z                 = 0x5A,
    LeftWindows       = 0x5B,
    RightWindows      = 0x5C,
    Apps              = 0x5D,
    Sleep             = 0x5F,
    Numpad0           = 0x60,
    Numpad1           = 0x61,
    Numpad2           = 0x62,
    Numpad3           = 0x63,
    Numpad4           = 0x64,
    Numpad5           = 0x65,
    Numpad6           = 0x66,
    Numpad7           = 0x67,
    Numpad8           = 0x68,
    Numpad9           = 0x69,
    Multiply          = 0x6A,
    Add               = 0x6B,
    Separator         = 0x6C,
    Subtract          = 0x6D,
    Decimal           = 0x6E,
    Divide            = 0x6F,
    F1                = 0x70,
    F2                = 0x71,
    F3                = 0x72,
    F4                = 0x73,
    F5                = 0x74,
    F6                = 0x75,
    F7                = 0x76,
    F8                = 0x77,
    F9                = 0x78,
    F10               = 0x79,
    F11               = 0x7A,
    F12               = 0x7B,
    F13               = 0x7C,
    F14               = 0x7D,
    F15               = 0x7E,
    F16               = 0x7F,
    F17               = 0x80,
    F18               = 0x81,
    F19               = 0x82,
    F20               = 0x83,
    F21               = 0x84,
    F22               = 0x85,
    F23               = 0x86,
    F24               = 0x87,
    NumLock           = 0x90,
    ScrollLock        = 0x91,
    LeftShift         = 0xA0,
    RightShift        = 0xA1,
    LeftControl       = 0xA2,
    RightControl      = 0xA3,
    LeftMenu          = 0xA4,
    RightMenu         = 0xA5,
    BrowserBack       = 0xA6,
    BrowserForward    = 0xA7,
    BrowserRefresh    = 0xA8,
    BrowserStop       = 0xA9,
    BrowserSearch     = 0xAA,
    BrowserFavorites  = 0xAB,
    BrowserHome       = 0xAC,
    VolumeMute        = 0xAD,
    VolumeDown        = 0xAE,
    VolumeUp          = 0xAF,
    MediaNext         = 0xB0,
    MediaPrev         = 0xB1,
    MediaStop         = 0xB2,
    MediaPlayPause    = 0xB3,
    LaunchMail        = 0xB4,
    LaunchMediaSelect = 0xB5,
    LaunchApp1        = 0xB6,
    LaunchApp2        = 0xB7,

    Count
};
constexpr unsigned int k_eKeyCount = static_cast<int>(eKey::Count);
std::string_view       ToString(eKey e);

enum class eMouse : int
{
    LeftButton,
    MiddleButton,
    RightButton,

    Count
};
constexpr int k_eMouseCount = static_cast<int>(eMouse::Count);

enum class eMacroButton : int
{
    None,
    LeftButton,
    MiddleButton,
    RightButton,

    Count
};
constexpr int    k_eMacroButtonCount = static_cast<int>(eMacroButton::Count);
std::string_view ToString(eMacroButton e);

enum class eMacroAction : int
{
    None,
    ClickOnce,
    ClickRepeat,
    Hold,

    Count
};
constexpr int    k_eMacroActionCount = static_cast<int>(eMacroAction::Count);
std::string_view ToString(eMacroAction e);

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

struct ProgramData
{
    bool bValid = false;

    std::wstring programName      = {};
    int          windowPosX       = 0;
    int          windowPosY       = 0;
    int          windowWidth      = 0;
    int          windowHeight     = 0;
    bool         bWideAspectRatio = false;   // true: 16:9, false: 4:3
};
