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

    [[nodiscard]] bool saveConfig() const;

    [[nodiscard]] float getSensitivity() const;

    void setSensitivity(float sensitivity);

    [[nodiscard]] int getResolutionWidth() const;

    [[nodiscard]] int getResolutionHeight() const;

    void setResolution(int width, int height);

    [[nodiscard]] float getAspectRatio() const;

    void setAspectRatio(float aspectRatio);

    [[nodiscard]] int getHotkey(const std::string& patternName) const;

    void setHotkey(const std::string& patternName, int hotkey);

    [[nodiscard]] bool getReturnToOriginal() const;

    /**
     * Set return to original position setting
     * @param returnToOriginal New setting value
     */
    void setReturnToOriginal(bool returnToOriginal);

    [[nodiscard]] std::map<std::string, int> getAllHotkeyMappings() const;

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