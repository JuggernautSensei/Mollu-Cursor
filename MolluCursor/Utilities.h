#pragma once

// imgui utils
constexpr ImVec2 operator+(const ImVec2& a, const ImVec2& b) { return ImVec2 { a.x + b.x, a.y + b.y }; }
constexpr ImVec2 operator-(const ImVec2& a, const ImVec2& b) { return ImVec2 { a.x - b.x, a.y - b.y }; }
constexpr ImVec2 operator*(const ImVec2& a, const ImVec2& b) { return ImVec2 { a.x * b.x, a.y * b.y }; }
constexpr ImVec2 operator*(const ImVec2& a, const float b) { return ImVec2 { a.x * b, a.y * b }; }
constexpr ImVec2 operator*(const float a, const ImVec2& b) { return ImVec2 { a * b.x, a * b.y }; }

// string converter
inline std::string ConvertToString(std::wstring_view _wStr)
{
    return { _wStr.begin(), _wStr.end() };
}

inline std::wstring ConvertToWideString(std::string_view _str)
{
    return { _str.begin(), _str.end() };
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
bool LoadJson(const std::filesystem::path& _path, Json& _out_json);
bool SaveJson(const std::filesystem::path& _path, const Json& _json);

NODISCARD constexpr float AbsoluteFloat(const float _f)
{
    return _f >= 0.f ? _f : -_f;
}

NODISCARD constexpr bool IsAlmostSame(const float _left, const float _right)
{
    return AbsoluteFloat(_left - _right) < 1e-6f;
}