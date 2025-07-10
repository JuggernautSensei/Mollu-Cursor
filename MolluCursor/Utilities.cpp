#include "pch.h"

#include "Utilities.h"

#include <fstream>

std::string ConvertToString(const std::wstring_view _wStr)
{
    if (_wStr.empty()) return {};

    int byteCount = ::WideCharToMultiByte(CP_UTF8, 0, _wStr.data(), static_cast<int>(_wStr.size()), nullptr, 0, nullptr, nullptr);
    if (byteCount <= 0) return {};

    std::string result;
    result.resize(byteCount);

    ::WideCharToMultiByte(CP_UTF8, 0, _wStr.data(), static_cast<int>(_wStr.size()), result.data(), byteCount, nullptr, nullptr);
    return result;
}

std::wstring ConvertToWideString(const std::string_view _str)
{
    if (_str.empty()) return {};

    int charCount = ::MultiByteToWideChar(CP_UTF8, 0, _str.data(), static_cast<int>(_str.size()), nullptr, 0);
    if (charCount <= 0) return {};

    std::wstring wResult;
    wResult.resize(charCount);

    ::MultiByteToWideChar(CP_UTF8, 0, _str.data(), static_cast<int>(_str.size()), wResult.data(), charCount);
    return wResult;
}

ImVec2 CalcPrettyButtonSize(const int _buttonCount)
{
    ASSERT(_buttonCount > 0, "_buffer count is bigger than 0");

    ImGuiStyle& style       = ImGui::GetStyle();
    float       availWidth  = ImGui::GetContentRegionAvail().x;
    float       buttonWidth = (availWidth - static_cast<float>(_buttonCount - 1) * style.ItemSpacing.x) / static_cast<float>(_buttonCount);
    return ImVec2 { buttonWidth, 0.f };
}

void DrawHelpTooltip(const char* _title, const char* _description)
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

std::optional<Json> LoadJson(const std::filesystem::path& _path)
{
    std::fstream fs(_path, std::ios::in);
    if (!fs.is_open())
    {
        return std::nullopt;
    }

    return Json::parse(fs, nullptr, false);
}

bool SaveJson(const std::filesystem::path& _path, const Json& _json)
{
    std::fstream fs(_path, std::ios::out);
    if (!fs.is_open())
    {
        return false;
    }

    fs << _json.dump(4);
    return true;
}