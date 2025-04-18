#pragma once

#include <string>
#include <memory>

class AntiRecoilEngine;
class ConfigManager;
class InputManager;
class PatternLoader;

class ConsoleUI {
public:
    ConsoleUI(std::shared_ptr<AntiRecoilEngine> engine,
              std::shared_ptr<ConfigManager> configManager,
              std::shared_ptr<InputManager> inputManager,
              std::shared_ptr<PatternLoader> patternLoader);
    ~ConsoleUI();

    static bool initialize();

    void run();

    void showMainMenu();

    void showPatternMenu();

    void showSettingsMenu();

    void showHotkeyMenu();

    void showStatus();

    static void clearConsole();

    static std::string getUserInput();

    static void showMessage(const std::string& message);

private:
    std::shared_ptr<AntiRecoilEngine> m_engine;
    std::shared_ptr<ConfigManager> m_configManager;
    std::shared_ptr<InputManager> m_inputManager;
    std::shared_ptr<PatternLoader> m_patternLoader;

    bool m_running;

    void processMenuSelection(const std::string& selection);

    void processPatternSelection(const std::string& selection);

    void processSettingsSelection(const std::string& selection);

    void processHotkeySelection(const std::string& selection);

    void configureHotkey(const std::string& patternName);

    void configureSensitivity();

    void configureResolution();

    void configureAspectRatio();

    void configureReturnToOriginal();
};