#include "pch.h"

#include "Base.h"

#include <fstream>

namespace detail
{

void CrashImpl(const std::string_view _msg)
{
    std::string filePath = std::format("mollu_crash_dump_{}.txt", std::time(nullptr));

    std::fstream fs;
    fs.open(filePath, std::ios::out);
    if (fs.is_open())
    {
        fs << _msg;
        fs.close();
    }

    fs.close();
    ::MessageBoxA(nullptr, _msg.data(), "Fatal Error", MB_OK | MB_ICONERROR);
}

}   // namespace detail

Vec2 Vec2::s_one  = Vec2(1.0f, 1.0f);
Vec2 Vec2::s_zero = Vec2(0.0f, 0.0f);

std::string_view ToString(const eKey e)
{
    switch (e)
    {
        case eKey::None: return "None";
        case eKey::Cancel: return "Cancel";
        case eKey::Backspace: return "Backspace";
        case eKey::Tab: return "Tab";
        case eKey::Clear: return "Clear";
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
        case eKey::Key0: return "0";
        case eKey::Key1: return "1";
        case eKey::Key2: return "2";
        case eKey::Key3: return "3";
        case eKey::Key4: return "4";
        case eKey::Key5: return "5";
        case eKey::Key6: return "6";
        case eKey::Key7: return "7";
        case eKey::Key8: return "8";
        case eKey::Key9: return "9";
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
        default: return "";
    }
}

std::string_view ToString(const eMacroButton e)
{
    switch (e)
    {
        case eMacroButton::None: return "None";
        case eMacroButton::LeftButton: return "LeftButton";
        case eMacroButton::MiddleButton: return "MiddleButton";
        case eMacroButton::RightButton: return "RightButton";
        default: return "";
    }
}

std::string_view ToString(const eMacroAction e)
{
    switch (e)
    {
        case eMacroAction::None: return "None";
        case eMacroAction::ClickOnce: return "ClickOnce";
        case eMacroAction::ClickRepeat: return "ClickRepeat";
        case eMacroAction::Hold: return "Hold";
        default: return "";
    }
}
