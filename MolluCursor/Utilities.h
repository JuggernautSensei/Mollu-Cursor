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

// enum utils
// ToString 함수에 의존함. ToString은 유효하지 않은 enum 값에 대해 빈 문자열을 반환한다고 가정
template<typename T>
NODISCARD bool IsValidEnum(const T _value)
{
    static_assert(std::is_enum_v<T>, "T must be an enum type");
    static_assert(std::is_same_v<typename decltype(ToString(std::declval<T>()))::value_type, char>, "ToString must return a string_view or string type");
    std::string_view enumName = ToString(_value);
    return !enumName.empty();
}
template<typename T>
NODISCARD T GetValidEnum(const T _value, const T _valueIfInvalid)
{
    static_assert(std::is_enum_v<T>, "T must be an enum type");
    if (IsValidEnum(_value))
    {
        return _value;
    }
    return _valueIfInvalid;
}