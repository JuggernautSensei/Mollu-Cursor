#include "pch.h"

#include "Macros.h"

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
