#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <nlohmann/json.hpp>
#include "AntiRecoilEngine.h"

struct PatternInfo {
    std::string name;
    std::string description;
    int defaultHotkey;
    struct {
        float sensitivity;
        struct {
            int width;
            int height;
        } resolution;
        std::string aspectRatio;
    } baseSettings;
    std::vector<PatternPoint> pattern;
};

class PatternLoader {
public:
    PatternLoader(const std::string& patternDirectory);
    ~PatternLoader();

    bool loadAllPatterns();

    bool loadPattern(const std::string& patternName);

    std::vector<PatternPoint> getPattern(const std::string& patternName) const;

    std::vector<std::string> getAvailablePatterns() const;

    PatternInfo getPatternInfo(const std::string& patternName) const;

    bool convertLuaPatternsToJson(const std::string& luaScriptPath);

private:
    std::string m_patternDirectory;
    std::map<std::string, PatternInfo> m_patterns;

    bool parsePatternFile(const std::string& filePath, PatternInfo& pattern);

    static bool savePatternToFile(const PatternInfo& pattern, const std::string& filePath);
};