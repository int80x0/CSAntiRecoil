#include "PatternLoader.h"
#include "utils/Logger.h"
#include <filesystem>
#include <fstream>
#include <regex>
#include <sstream>
#include <iostream>

namespace fs = std::filesystem;

PatternLoader::PatternLoader(const std::string& patternDirectory)
        : m_patternDirectory(patternDirectory) {
    if (!fs::exists(patternDirectory)) {
        fs::create_directories(patternDirectory);
        Logger::log("Created pattern directory: " + patternDirectory);
    }
}

PatternLoader::~PatternLoader() = default;

bool PatternLoader::loadAllPatterns() {
    if (!fs::exists(m_patternDirectory)) {
        Logger::error("Pattern directory does not exist: " + m_patternDirectory);
        return false;
    }

    m_patterns.clear();

    try {
        for (const auto& entry : fs::directory_iterator(m_patternDirectory)) {
            if (entry.path().extension() == ".json") {
                PatternInfo pattern;
                if (parsePatternFile(entry.path().string(), pattern)) {
                    m_patterns[pattern.name] = pattern;
                    Logger::log("Loaded pattern: " + pattern.name + " (" + entry.path().filename().string() + ")");
                }
            }
        }
    } catch (const fs::filesystem_error& e) {
        Logger::error("Filesystem error when loading patterns: " + std::string(e.what()));
        return false;
    }

    Logger::log("Loaded " + std::to_string(m_patterns.size()) + " patterns");
    return !m_patterns.empty();
}

bool PatternLoader::loadPattern(const std::string& patternName) {
    std::string filename = m_patternDirectory + "/" + patternName + ".json";
    if (!fs::exists(filename)) {
        Logger::error("Pattern file does not exist: " + filename);
        return false;
    }

    PatternInfo pattern;
    if (!parsePatternFile(filename, pattern)) {
        return false;
    }

    m_patterns[pattern.name] = pattern;
    Logger::log("Loaded pattern: " + pattern.name);
    return true;
}

std::vector<PatternPoint> PatternLoader::getPattern(const std::string& patternName) const {
    auto it = m_patterns.find(patternName);
    if (it == m_patterns.end()) {
        Logger::error("Pattern not found: " + patternName);
        return {};
    }
    return it->second.pattern;
}

std::vector<std::string> PatternLoader::getAvailablePatterns() const {
    std::vector<std::string> patternNames;
    for (const auto& [name, _] : m_patterns) {
        patternNames.push_back(name);
    }
    return patternNames;
}

PatternInfo PatternLoader::getPatternInfo(const std::string& patternName) const {
    auto it = m_patterns.find(patternName);
    if (it == m_patterns.end()) {
        Logger::error("Pattern not found: " + patternName);
        return {};
    }
    return it->second;
}

bool PatternLoader::parsePatternFile(const std::string& filePath, PatternInfo& pattern) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        Logger::error("Failed to open pattern file: " + filePath);
        return false;
    }

    try {
        nlohmann::json data = nlohmann::json::parse(file);

        pattern.name = data["name"];
        pattern.description = data["description"];
        pattern.defaultHotkey = data["defaultHotkey"];

        pattern.baseSettings.sensitivity = data["baseSettings"]["sensitivity"];
        pattern.baseSettings.resolution.width = data["baseSettings"]["resolution"]["width"];
        pattern.baseSettings.resolution.height = data["baseSettings"]["resolution"]["height"];
        pattern.baseSettings.aspectRatio = data["baseSettings"]["aspectRatio"];

        for (const auto& pointData : data["pattern"]) {
            PatternPoint point{};
            point.x = pointData["x"];
            point.y = pointData["y"];
            point.d = pointData["d"];
            pattern.pattern.push_back(point);
        }

        return true;
    } catch (const nlohmann::json::exception& e) {
        Logger::error("JSON parsing error in " + filePath + ": " + std::string(e.what()));
        return false;
    }
}

bool PatternLoader::savePatternToFile(const PatternInfo& pattern, const std::string& filePath) {
    try {
        nlohmann::json data;
        data["name"] = pattern.name;
        data["description"] = pattern.description;
        data["defaultHotkey"] = pattern.defaultHotkey;

        data["baseSettings"]["sensitivity"] = pattern.baseSettings.sensitivity;
        data["baseSettings"]["resolution"]["width"] = pattern.baseSettings.resolution.width;
        data["baseSettings"]["resolution"]["height"] = pattern.baseSettings.resolution.height;
        data["baseSettings"]["aspectRatio"] = pattern.baseSettings.aspectRatio;

        nlohmann::json patternArray = nlohmann::json::array();
        for (const auto& point : pattern.pattern) {
            nlohmann::json pointData;
            pointData["x"] = point.x;
            pointData["y"] = point.y;
            pointData["d"] = point.d;
            patternArray.push_back(pointData);
        }
        data["pattern"] = patternArray;

        std::ofstream file(filePath);
        if (!file.is_open()) {
            Logger::error("Failed to open file for writing: " + filePath);
            return false;
        }

        file << data.dump(4);
        return true;
    } catch (const std::exception& e) {
        Logger::error("Error saving pattern to file: " + std::string(e.what()));
        return false;
    }
}

bool PatternLoader::convertLuaPatternsToJson(const std::string& luaScriptPath) {
    std::ifstream luaFile(luaScriptPath);
    if (!luaFile.is_open()) {
        Logger::error("Failed to open Lua script: " + luaScriptPath);
        return false;
    }

    std::string luaContent((std::istreambuf_iterator<char>(luaFile)), std::istreambuf_iterator<char>());
    luaFile.close();

    std::vector<std::string> weaponNames = {
            "ak47", "galil", "sg553", "m4a4", "m4a1", "famas", "aug", "mp9",
            "mac10", "ump45", "mp7", "mp5sd", "bizon", "p90", "cz75", "m249"
    };

    for (const auto& weaponName : weaponNames) {
        PatternInfo pattern;
        pattern.name = weaponName;
        pattern.description = "Recoil pattern for " + weaponName;

        pattern.defaultHotkey = 0;

        pattern.baseSettings.sensitivity = 2.0f;
        pattern.baseSettings.resolution.width = 1920;
        pattern.baseSettings.resolution.height = 1080;
        pattern.baseSettings.aspectRatio = "16:9";

        std::string patternRegex = weaponName + R"(\s*=\s*\{(.*?)\})";
        std::regex pattern_re("(?s)" + patternRegex, std::regex::ECMAScript);
        std::smatch match;

        if (std::regex_search(luaContent, match, pattern_re) && match.size() > 1) {
            std::string patternData = match[1].str();

            std::regex point_re(R"(\{x=(-?\d+),\s*y=(-?\d+),\s*d=(\d+)\})");
            auto points_begin = std::sregex_iterator(patternData.begin(), patternData.end(), point_re);
            auto points_end = std::sregex_iterator();

            for (std::sregex_iterator i = points_begin; i != points_end; ++i) {
                const std::smatch& pointMatch = *i;
                if (pointMatch.size() > 3) {
                    PatternPoint point{};
                    point.x = std::stoi(pointMatch[1].str());
                    point.y = std::stoi(pointMatch[2].str());
                    point.d = std::stoi(pointMatch[3].str());
                    pattern.pattern.push_back(point);
                }
            }

            std::string outputPath = m_patternDirectory + "/" + weaponName + ".json";
            if (savePatternToFile(pattern, outputPath)) {
                Logger::log("Converted pattern for " + weaponName + " (" +
                            std::to_string(pattern.pattern.size()) + " points)");
            } else {
                Logger::error("Failed to save converted pattern for " + weaponName);
            }
        } else {
            Logger::error("Failed to extract pattern for " + weaponName);
        }
    }

    return true;
}