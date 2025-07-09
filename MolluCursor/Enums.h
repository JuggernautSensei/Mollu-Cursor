#pragma once

// enum 유틸리티

template<typename T>
constexpr decltype(auto) ToUnderlying(T _e)
{
    static_assert(std::is_enum_v<T>, "T must be an enum type");
    return static_cast<std::underlying_type_t<T>>(_e);
}

template<typename Enum>
constexpr Enum ToEnum(std::underlying_type_t<Enum> _value)
{
    static_assert(std::is_enum_v<Enum>, "Enum must be an enum type");
    return static_cast<Enum>(_value);
}

enum class eAspectRatio
{
    None,
    Ratio_16_9,
    Ratio_4_3,

    Count
};
constexpr auto k_eAspectRatioCount = ToUnderlying(eAspectRatio::Count);

enum class eKey
{
    None              = 0x00,
    LeftButton        = 0x01,
    RightButton       = 0x02,
    Cancel            = 0x03,
    MiddleButton      = 0x04,
    XButton1          = 0x05,
    XButton2          = 0x06,
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
constexpr auto k_eKeyCount = ToUnderlying(eKey::Count);

enum class eMacroButton
{
    None,
    LeftButton,
    MiddleButton,
    RightButton,

    Count,
};
constexpr auto k_eMacroButtonCount = ToUnderlying(eMacroButton::Count);

enum class eMacroAction
{
    None,
    ClickOnce,
    ClickRepeat,
    Hold,

    Count
};
constexpr auto k_eMacroActionCount = ToUnderlying(eMacroAction::Count);

constexpr std::string_view ToString(const eMacroButton _e)
{
    switch (_e)
    {
        case eMacroButton::None: return "None";
        case eMacroButton::LeftButton: return "LeftButton";
        case eMacroButton::MiddleButton: return "MiddleButton";
        case eMacroButton::RightButton: return "RightButton";

        default:
            ASSERT(0, "unknown enum");
            return "unknown";
    }
}

constexpr std::string_view ToString(const eMacroAction _e)
{
    switch (_e)
    {
        case eMacroAction::None: return "None";
        case eMacroAction::ClickOnce: return "ClickOnce";
        case eMacroAction::ClickRepeat: return "ClickRepeat";
        case eMacroAction::Hold: return "Hold";

        default:
            ASSERT(0, "unknown enum");
            return "unknown";
    }
}

constexpr std::string_view ToString(const eKey _key)
{
    switch (_key)
    {
        case eKey::None: return "None";
        case eKey::LeftButton: return "LeftButton";
        case eKey::RightButton: return "RightButton";
        case eKey::Cancel: return "Cancel";
        case eKey::MiddleButton: return "MiddleButton";
        case eKey::XButton1: return "XButton1";
        case eKey::XButton2: return "XButton2";
        case eKey::Backspace: return "Backspace";
        case eKey::Tab: return "Tab";
        case eKey::Clear: return "ClearField";
        case eKey::Enter: return "Enter";
        case eKey::Shift: return "Shift";
        case eKey::Control: return "Control";
        case eKey::Alt: return "Alt";
        case eKey::Pause: return "Pause";
        case eKey::CapsLock: return "CapsLock";
        case eKey::Kana: return "Kana";
        case eKey::Junja: return "Junja";
        case eKey::Final: return "Final";
        case eKey::Kanji: return "Kanji";
        case eKey::Escape: return "Escape";
        case eKey::Convert: return "Convert";
        case eKey::NonConvert: return "NonConvert";
        case eKey::Accept: return "Accept";
        case eKey::ModeChange: return "ModeChange";
        case eKey::Space: return "Space";
        case eKey::PageUp: return "PageUp";
        case eKey::PageDown: return "PageDown";
        case eKey::End: return "End";
        case eKey::Home: return "Home";
        case eKey::Left: return "Left";
        case eKey::Up: return "Up";
        case eKey::Right: return "Right";
        case eKey::Down: return "Down";
        case eKey::Select: return "Select";
        case eKey::Print: return "Print";
        case eKey::Execute: return "Execute";
        case eKey::PrintScreen: return "PrintScreen";
        case eKey::Insert: return "Insert";
        case eKey::Delete: return "Delete";
        case eKey::Help: return "Help";
        case eKey::Key0: return "Key0";
        case eKey::Key1: return "Key1";
        case eKey::Key2: return "Key2";
        case eKey::Key3: return "Key3";
        case eKey::Key4: return "Key4";
        case eKey::Key5: return "Key5";
        case eKey::Key6: return "Key6";
        case eKey::Key7: return "Key7";
        case eKey::Key8: return "Key8";
        case eKey::Key9: return "Key9";
        case eKey::A: return "A";
        case eKey::B: return "B";
        case eKey::C: return "C";
        case eKey::D: return "D";
        case eKey::E: return "E";
        case eKey::F: return "F";
        case eKey::G: return "G";
        case eKey::H: return "H";
        case eKey::I: return "I";
        case eKey::J: return "J";
        case eKey::K: return "K";
        case eKey::L: return "L";
        case eKey::M: return "M";
        case eKey::N: return "N";
        case eKey::O: return "O";
        case eKey::P: return "P";
        case eKey::Q: return "Q";
        case eKey::R: return "R";
        case eKey::S: return "S";
        case eKey::T: return "T";
        case eKey::U: return "U";
        case eKey::V: return "V";
        case eKey::W: return "W";
        case eKey::X: return "X";
        case eKey::Y: return "Y";
        case eKey::Z: return "Z";
        case eKey::LeftWindows: return "LeftWindows";
        case eKey::RightWindows: return "RightWindows";
        case eKey::Apps: return "Apps";
        case eKey::Sleep: return "Sleep";
        case eKey::Numpad0: return "Numpad0";
        case eKey::Numpad1: return "Numpad1";
        case eKey::Numpad2: return "Numpad2";
        case eKey::Numpad3: return "Numpad3";
        case eKey::Numpad4: return "Numpad4";
        case eKey::Numpad5: return "Numpad5";
        case eKey::Numpad6: return "Numpad6";
        case eKey::Numpad7: return "Numpad7";
        case eKey::Numpad8: return "Numpad8";
        case eKey::Numpad9: return "Numpad9";
        case eKey::Multiply: return "Multiply";
        case eKey::Add: return "Add";
        case eKey::Separator: return "Separator";
        case eKey::Subtract: return "Subtract";
        case eKey::Decimal: return "Decimal";
        case eKey::Divide: return "Divide";
        case eKey::F1: return "F1";
        case eKey::F2: return "F2";
        case eKey::F3: return "F3";
        case eKey::F4: return "F4";
        case eKey::F5: return "F5";
        case eKey::F6: return "F6";
        case eKey::F7: return "F7";
        case eKey::F8: return "F8";
        case eKey::F9: return "F9";
        case eKey::F10: return "F10";
        case eKey::F11: return "F11";
        case eKey::F12: return "F12";
        case eKey::F13: return "F13";
        case eKey::F14: return "F14";
        case eKey::F15: return "F15";
        case eKey::F16: return "F16";
        case eKey::F17: return "F17";
        case eKey::F18: return "F18";
        case eKey::F19: return "F19";
        case eKey::F20: return "F20";
        case eKey::F21: return "F21";
        case eKey::F22: return "F22";
        case eKey::F23: return "F23";
        case eKey::F24: return "F24";
        case eKey::NumLock: return "NumLock";
        case eKey::ScrollLock: return "ScrollLock";
        case eKey::LeftShift: return "LeftShift";
        case eKey::RightShift: return "RightShift";
        case eKey::LeftControl: return "LeftControl";
        case eKey::RightControl: return "RightControl";
        case eKey::LeftMenu: return "LeftMenu";
        case eKey::RightMenu: return "RightMenu";
        case eKey::BrowserBack: return "BrowserBack";
        case eKey::BrowserForward: return "BrowserForward";
        case eKey::BrowserRefresh: return "BrowserRefresh";
        case eKey::BrowserStop: return "BrowserStop";
        case eKey::BrowserSearch: return "BrowserSearch";
        case eKey::BrowserFavorites: return "BrowserFavorites";
        case eKey::BrowserHome: return "BrowserHome";
        case eKey::VolumeMute: return "VolumeMute";
        case eKey::VolumeDown: return "VolumeDown";
        case eKey::VolumeUp: return "VolumeUp";
        case eKey::MediaNext: return "MediaNext";
        case eKey::MediaPrev: return "MediaPrev";
        case eKey::MediaStop: return "MediaStop";
        case eKey::MediaPlayPause: return "MediaPlayPause";
        case eKey::LaunchMail: return "LaunchMail";
        case eKey::LaunchMediaSelect: return "LaunchMediaSelect";
        case eKey::LaunchApp1: return "LaunchApp1";
        case eKey::LaunchApp2: return "LaunchApp2";
        default:
            ASSERT(0, "unknown enum");
            return "unknown";
    }
}
