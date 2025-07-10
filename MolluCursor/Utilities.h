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
std::string  ConvertToString(std::wstring_view _wStr);
std::wstring ConvertToWideString(std::string_view _str);

// imgui util
ImVec2 CalcPrettyButtonSize(int _buttonCount);
void   DrawHelpTooltip(const char* _title, const char* _description);

// serialization utils
std::optional<Json> LoadJson(const std::filesystem::path& _path);
bool                SaveJson(const std::filesystem::path& _path, const Json& _json);

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(ImVec4, x, y, z, w);
NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE_WITH_DEFAULT(Vec2, x, y);

// math utils
NODISCARD constexpr float AbsoluteFloat(const float _f)
{
    return _f >= 0.f ? _f : -_f;
}

NODISCARD constexpr bool IsAlmostSame(const float _left, const float _right)
{
    return AbsoluteFloat(_left - _right) < 1e-6f;
}