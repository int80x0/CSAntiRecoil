#include "StringUtils.h"
#include <algorithm>
#include <sstream>
#include <cctype>
#include <regex>
#include <Windows.h>

std::string StringUtils::wideToUtf8(const std::wstring& wstr) {
    if (wstr.empty()) {
        return std::string();
    }

    int size_needed = WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()),
                                          NULL, 0, NULL, NULL);

    std::string strUtf8(size_needed, 0);
    WideCharToMultiByte(CP_UTF8, 0, wstr.data(), static_cast<int>(wstr.size()),
                        &strUtf8[0], size_needed, NULL, NULL);

    return strUtf8;
}

std::wstring StringUtils::utf8ToWide(const std::string& str) {
    if (str.empty()) {
        return std::wstring();
    }

    int size_needed = MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()),
                                          NULL, 0);

    std::wstring wstrTo(size_needed, 0);
    MultiByteToWideChar(CP_UTF8, 0, str.data(), static_cast<int>(str.size()),
                        &wstrTo[0], size_needed);

    return wstrTo;
}

std::string StringUtils::trim(const std::string& str) {
    const auto strBegin = str.find_first_not_of(" \t\r\n");
    if (strBegin == std::string::npos) {
        return "";
    }

    const auto strEnd = str.find_last_not_of(" \t\r\n");
    const auto strRange = strEnd - strBegin + 1;

    return str.substr(strBegin, strRange);
}

std::vector<std::string> StringUtils::split(const std::string& str, const std::string& delimiter) {
    std::vector<std::string> tokens;
    size_t prev = 0, pos = 0;

    do {
        pos = str.find(delimiter, prev);
        if (pos == std::string::npos) {
            pos = str.length();
        }

        std::string token = str.substr(prev, pos - prev);
        if (!token.empty()) {
            tokens.push_back(token);
        }

        prev = pos + delimiter.length();
    } while (pos < str.length() && prev < str.length());

    return tokens;
}

std::string StringUtils::toLower(const std::string& str) {
    std::string lower = str;
    std::transform(lower.begin(), lower.end(), lower.begin(),
                   [](unsigned char c) { return std::tolower(c); });
    return lower;
}

std::string StringUtils::toUpper(const std::string& str) {
    std::string upper = str;
    std::transform(upper.begin(), upper.end(), upper.begin(),
                   [](unsigned char c) { return std::toupper(c); });
    return upper;
}

bool StringUtils::startsWith(const std::string& str, const std::string& prefix) {
    return str.size() >= prefix.size() && 0 == str.compare(0, prefix.size(), prefix);
}

bool StringUtils::endsWith(const std::string& str, const std::string& suffix) {
    return str.size() >= suffix.size() &&
           0 == str.compare(str.size() - suffix.size(), suffix.size(), suffix);
}

std::string StringUtils::replaceAll(const std::string& str, const std::string& from, const std::string& to) {
    if (from.empty()) {
        return str;
    }

    std::string result = str;
    size_t start_pos = 0;

    while ((start_pos = result.find(from, start_pos)) != std::string::npos) {
        result.replace(start_pos, from.length(), to);
        start_pos += to.length();
    }

    return result;
}

std::string StringUtils::vkToString(int vk) {
    switch (vk) {
        case VK_LBUTTON: return "Left Mouse Button";
        case VK_RBUTTON: return "Right Mouse Button";
        case VK_MBUTTON: return "Middle Mouse Button";
        case VK_XBUTTON1: return "X1 Mouse Button";
        case VK_XBUTTON2: return "X2 Mouse Button";
        case VK_BACK: return "Backspace";
        case VK_TAB: return "Tab";
        case VK_RETURN: return "Enter";
        case VK_SHIFT: return "Shift";
        case VK_CONTROL: return "Ctrl";
        case VK_MENU: return "Alt";
        case VK_PAUSE: return "Pause";
        case VK_CAPITAL: return "Caps Lock";
        case VK_ESCAPE: return "Esc";
        case VK_SPACE: return "Space";
        case VK_PRIOR: return "Page Up";
        case VK_NEXT: return "Page Down";
        case VK_END: return "End";
        case VK_HOME: return "Home";
        case VK_LEFT: return "Left Arrow";
        case VK_UP: return "Up Arrow";
        case VK_RIGHT: return "Right Arrow";
        case VK_DOWN: return "Down Arrow";
        case VK_INSERT: return "Insert";
        case VK_DELETE: return "Delete";
        case '0': return "0";
        case '1': return "1";
        case '2': return "2";
        case '3': return "3";
        case '4': return "4";
        case '5': return "5";
        case '6': return "6";
        case '7': return "7";
        case '8': return "8";
        case '9': return "9";
        case 'A': return "A";
        case 'B': return "B";
        case 'C': return "C";
        case 'D': return "D";
        case 'E': return "E";
        case 'F': return "F";
        case 'G': return "G";
        case 'H': return "H";
        case 'I': return "I";
        case 'J': return "J";
        case 'K': return "K";
        case 'L': return "L";
        case 'M': return "M";
        case 'N': return "N";
        case 'O': return "O";
        case 'P': return "P";
        case 'Q': return "Q";
        case 'R': return "R";
        case 'S': return "S";
        case 'T': return "T";
        case 'U': return "U";
        case 'V': return "V";
        case 'W': return "W";
        case 'X': return "X";
        case 'Y': return "Y";
        case 'Z': return "Z";
        case VK_NUMPAD0: return "Numpad 0";
        case VK_NUMPAD1: return "Numpad 1";
        case VK_NUMPAD2: return "Numpad 2";
        case VK_NUMPAD3: return "Numpad 3";
        case VK_NUMPAD4: return "Numpad 4";
        case VK_NUMPAD5: return "Numpad 5";
        case VK_NUMPAD6: return "Numpad 6";
        case VK_NUMPAD7: return "Numpad 7";
        case VK_NUMPAD8: return "Numpad 8";
        case VK_NUMPAD9: return "Numpad 9";
        case VK_MULTIPLY: return "Numpad *";
        case VK_ADD: return "Numpad +";
        case VK_SUBTRACT: return "Numpad -";
        case VK_DECIMAL: return "Numpad .";
        case VK_DIVIDE: return "Numpad /";
        case VK_F1: return "F1";
        case VK_F2: return "F2";
        case VK_F3: return "F3";
        case VK_F4: return "F4";
        case VK_F5: return "F5";
        case VK_F6: return "F6";
        case VK_F7: return "F7";
        case VK_F8: return "F8";
        case VK_F9: return "F9";
        case VK_F10: return "F10";
        case VK_F11: return "F11";
        case VK_F12: return "F12";
        default: {
            char keyName[256];
            if (GetKeyNameTextA(MapVirtualKey(vk, MAPVK_VK_TO_VSC) << 16, keyName, sizeof(keyName))) {
                return keyName;
            }
            return "Key(" + std::to_string(vk) + ")";
        }
    }
}