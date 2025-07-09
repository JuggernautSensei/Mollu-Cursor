#include "pch.h"

#include "Utilities.h"

#include <fstream>

bool LoadJson(const std::filesystem::path& _path, Json& _out_json) 
{
    std::fstream fs(_path, std::ios::in);
    if (!fs.is_open())
    {
        ASSERT(0, "open fail: {}", _path.string());
        return false;
    }

    _out_json = Json::parse(fs, nullptr, false);
    return true;
}

bool SaveJson(const std::filesystem::path& _path, const Json& _json) 
{
    std::fstream fs(_path, std::ios::out);
    if (!fs.is_open())
    {
        ASSERT(0, "open fail: {}", _path.string());
        return false;
    }

    fs << _json.dump(4);
    return true;
}