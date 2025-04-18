#pragma once

#include <string>
#include <map>
#include <nlohmann/json.hpp>

/**
 * Manages application configuration
 */
class ConfigManager {
public:
    ConfigManager(const std::string& configPath);
    ~ConfigManager();

    bool loadConfig();

    bool saveConfig() const;

    float getSensitivity() const;

    void setSensitivity(float sensitivity);

    int getResolutionWidth() const;

    int getResolutionHeight() const;

    void setResolution(int width, int height);

    float getAspectRatio() const;

    void setAspectRatio(float aspectRatio);

    int getHotkey(const std::string& patternName) const;

    void setHotkey(const std::string& patternName, int hotkey);

    bool getReturnToOriginal() const;

    /**
     * Set return to original position setting
     * @param returnToOriginal New setting value
     */
    void setReturnToOriginal(bool returnToOriginal);

    std::map<std::string, int> getAllHotkeyMappings() const;

private:
    std::string m_configPath;
    nlohmann::json m_config;
    float m_sensitivity;
    int m_resolutionWidth;
    int m_resolutionHeight;
    float m_aspectRatio;
    bool m_returnToOriginal;
    std::map<std::string, int> m_hotkeyMappings;
};