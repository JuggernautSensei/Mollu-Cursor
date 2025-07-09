#include "pch.h"

#include "Utilities.h"

#include <fstream>

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