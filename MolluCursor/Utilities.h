#pragma once

// for performance measurement
#ifdef _DEBUG
#    define DEBUG_SCOPED_STOP_WATCH(...) ScopedStopWatch(std::format(__VA_ARGS__))
#else
#    define DEBUG_SCOPED_STOP_WATCH(...) (void)(0)
#endif

// imgui utils
constexpr ImVec2 operator+(const ImVec2& a, const ImVec2& b) { return ImVec2 { a.x + b.x, a.y + b.y }; }
constexpr ImVec2 operator-(const ImVec2& a, const ImVec2& b) { return ImVec2 { a.x - b.x, a.y - b.y }; }
constexpr ImVec2 operator*(const ImVec2& a, const ImVec2& b) { return ImVec2 { a.x * b.x, a.y * b.y }; }
constexpr ImVec2 operator*(const ImVec2& a, const float b) { return ImVec2 { a.x * b, a.y * b }; }
constexpr ImVec2 operator*(const float a, const ImVec2& b) { return ImVec2 { a * b.x, a * b.y }; }

// string utils
inline std::string ConvertToString(const std::wstring_view _wStr)
{
    std::string str;
    str.resize(_wStr.size());
    errno_t errorCode = ::wcstombs_s(nullptr, str.data(), str.size(), _wStr.data(), _wStr.size());
    ASSERT(errorCode == 0, "Failed to convert wide string to string, error code: %d", errorCode);
    return str;
}

inline std::wstring ConvertToWideString(const std::string_view _str)
{
    std::wstring wStr;
    wStr.resize(_str.size());
    errno_t errorCode = ::mbstowcs_s(nullptr, wStr.data(), wStr.size(), _str.data(), _str.size());
    ASSERT(errorCode == 0, "Failed to convert string to wide string, error code: %d", errorCode);
    return wStr;
}

// imgui util
inline ImVec2 CalcPrettyButtonSize(const int _buttonCount)
{
    ASSERT(_buttonCount > 0, "_buffer count is bigger than 0");

    ImGuiStyle& style       = ImGui::GetStyle();
    float       availWidth  = ImGui::GetContentRegionAvail().x;
    float       buttonWidth = (availWidth - static_cast<float>(_buttonCount - 1) * style.ItemSpacing.x) / static_cast<float>(_buttonCount);
    return ImVec2 { buttonWidth, 0.f };
}

// imgui widget
inline void DrawHelpTooltip(const char* _title, const char* _description)
{
    ImGui::TextDisabled("(?)");
    if (ImGui::BeginItemTooltip())
    {
        // title
        ImGui::TextUnformatted(_title);

        // desc
        ImGui::Separator();
        ImGui::TextUnformatted(_description);

        ImGui::EndTooltip();
    }
}

// json
std::optional<Json> LoadJson(const std::filesystem::path& _path);
bool                SaveJson(const std::filesystem::path& _path, const Json& _json);

// math utils
NODISCARD constexpr float AbsoluteFloat(const float _f)
{
    return _f >= 0.f ? _f : -_f;
}

NODISCARD constexpr bool IsAlmostSame(const float _left, const float _right)
{
    return AbsoluteFloat(_left - _right) < 1e-6f;
}