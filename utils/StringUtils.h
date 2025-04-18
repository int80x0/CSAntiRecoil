#pragma once

#include <string>
#include <vector>
#include <Windows.h>

namespace StringUtils {
    std::string wideToUtf8(const std::wstring& wstr);

    std::wstring utf8ToWide(const std::string& str);

    std::string trim(const std::string& str);

    std::vector<std::string> split(const std::string& str, const std::string& delimiter);

    std::string toLower(const std::string& str);

    std::string toUpper(const std::string& str);

    bool startsWith(const std::string& str, const std::string& prefix);

    bool endsWith(const std::string& str, const std::string& suffix);

    std::string replaceAll(const std::string& str, const std::string& from, const std::string& to);

    std::string vkToString(int vk);
}