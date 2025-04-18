#include "ConfigManager.h"
#include "utils/Logger.h"
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

ConfigManager::ConfigManager(const std::string& configPath)
        : m_configPath(configPath)
        , m_sensitivity(2.0f)
        , m_resolutionWidth(1920)
        , m_resolutionHeight(1080)
        , m_aspectRatio(16.0f / 9.0f)
        , m_returnToOriginal(true)
{
    fs::path parentPath = fs::path(configPath).parent_path();
    if (!fs::exists(parentPath)) {
        fs::create_directories(parentPath);
    }
}

ConfigManager::~ConfigManager() {
    saveConfig();
}

bool ConfigManager::loadConfig() {
    if (!fs::exists(m_configPath)) {
        Logger::log("Config file does not exist, using default settings");
        return saveConfig();
    }

    try {
        std::ifstream file(m_configPath);
        if (!file.is_open()) {
            Logger::error("Failed to open config file: " + m_configPath);
            return false;
        }

        m_config = nlohmann::json::parse(file);

        m_sensitivity = m_config["settings"]["sensitivity"];
        m_resolutionWidth = m_config["settings"]["resolution"]["width"];
        m_resolutionHeight = m_config["settings"]["resolution"]["height"];
        m_aspectRatio = m_config["settings"]["aspectRatio"];
        m_returnToOriginal = m_config["settings"]["returnToOriginal"];

        m_hotkeyMappings.clear();
        for (const auto& [pattern, hotkey] : m_config["hotkeys"].items()) {
            m_hotkeyMappings[pattern] = hotkey;
        }

        Logger::log("Config loaded from: " + m_configPath);
        return true;
    } catch (const std::exception& e) {
        Logger::error("Error loading config: " + std::string(e.what()));
        return false;
    }
}

bool ConfigManager::saveConfig() const {
    try {
        nlohmann::json config;

        config["settings"]["sensitivity"] = m_sensitivity;
        config["settings"]["resolution"]["width"] = m_resolutionWidth;
        config["settings"]["resolution"]["height"] = m_resolutionHeight;
        config["settings"]["aspectRatio"] = m_aspectRatio;
        config["settings"]["returnToOriginal"] = m_returnToOriginal;

        for (const auto& [pattern, hotkey] : m_hotkeyMappings) {
            config["hotkeys"][pattern] = hotkey;
        }

        std::ofstream file(m_configPath);
        if (!file.is_open()) {
            Logger::error("Failed to open config file for writing: " + m_configPath);
            return false;
        }

        file << config.dump(4);
        Logger::log("Config saved to: " + m_configPath);
        return true;
    } catch (const std::exception& e) {
        Logger::error("Error saving config: " + std::string(e.what()));
        return false;
    }
}

float ConfigManager::getSensitivity() const {
    return m_sensitivity;
}

void ConfigManager::setSensitivity(float sensitivity) {
    m_sensitivity = sensitivity;
}

int ConfigManager::getResolutionWidth() const {
    return m_resolutionWidth;
}

int ConfigManager::getResolutionHeight() const {
    return m_resolutionHeight;
}

void ConfigManager::setResolution(int width, int height) {
    m_resolutionWidth = width;
    m_resolutionHeight = height;
}

float ConfigManager::getAspectRatio() const {
    return m_aspectRatio;
}

void ConfigManager::setAspectRatio(float aspectRatio) {
    m_aspectRatio = aspectRatio;
}

int ConfigManager::getHotkey(const std::string& patternName) const {
    auto it = m_hotkeyMappings.find(patternName);
    if (it != m_hotkeyMappings.end()) {
        return it->second;
    }
    return 0;
}

void ConfigManager::setHotkey(const std::string& patternName, int hotkey) {
    m_hotkeyMappings[patternName] = hotkey;
}

bool ConfigManager::getReturnToOriginal() const {
    return m_returnToOriginal;
}

void ConfigManager::setReturnToOriginal(bool returnToOriginal) {
    m_returnToOriginal = returnToOriginal;
}

std::map<std::string, int> ConfigManager::getAllHotkeyMappings() const {
    return m_hotkeyMappings;
}