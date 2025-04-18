#include <iostream>
#include <memory>
#include <Windows.h>
#include <filesystem>

#include "AntiRecoilEngine.h"
#include "PatternLoader.h"
#include "ConfigManager.h"
#include "InputManager.h"
#include "ConsoleUI.h"
#include "utils/Logger.h"

namespace fs = std::filesystem;

// Global hotkey IDs
constexpr int HOTKEY_ID_BASE = 100;

int main() {
    SetConsoleTitleA(reinterpret_cast<LPCSTR>(L"CS Anti-Recoil"));

    fs::create_directories("logs");
    Logger::initialize("logs/anti_recoil.log");

    Logger::log("CS Anti-Recoil starting up...");

    fs::create_directories("data/patterns");

    // In main.cpp
    std::shared_ptr<PatternLoader> patternLoader = std::make_shared<PatternLoader>("data/patterns");
    if (!patternLoader->loadAllPatterns()) {
        Logger::warning("No patterns found. You'll need to import patterns from a Lua script.");
    }

    std::shared_ptr<ConfigManager> configManager = std::make_shared<ConfigManager>("data/config.json");
    if (!configManager->loadConfig()) {
        Logger::warning("Failed to load config, using defaults.");
    }

    std::shared_ptr<InputManager> inputManager = std::make_shared<InputManager>();
    if (!inputManager->initialize()) {
        Logger::error("Failed to initialize input manager.");
        return 1;
    }

    std::shared_ptr<AntiRecoilEngine> engine = std::make_shared<AntiRecoilEngine>(patternLoader);

    engine->setSensitivity(configManager->getSensitivity());
    engine->setResolution(configManager->getResolutionWidth(), configManager->getResolutionHeight());
    engine->setAspectRatio(configManager->getAspectRatio());
    engine->setReturnToOriginal(configManager->getReturnToOriginal());

    auto hotkeyMappings = configManager->getAllHotkeyMappings();
    int hotkeyId = HOTKEY_ID_BASE;

    for (const auto& [patternName, vk] : hotkeyMappings) {
        if (vk == 0) {
            continue;
        }

        inputManager->registerHotkey(hotkeyId, 0, vk, [patternName, engine]() {
            if (engine->isActive()) {
                engine->stopAntiRecoil();
                Logger::log("Stopped anti-recoil (hotkey)");
            } else {
                engine->loadPattern(patternName);
                engine->startAntiRecoil();
                Logger::log("Started anti-recoil with pattern: " + patternName + " (hotkey)");
            }
        });

        hotkeyId++;
    }

    ConsoleUI ui(engine, configManager, inputManager, patternLoader);
    ui.initialize();
    ui.run();

    Logger::log("CS Anti-Recoil shutting down...");
    Logger::cleanup();

    return 0;
}