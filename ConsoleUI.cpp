#include "ConsoleUI.h"
#include "AntiRecoilEngine.h"
#include "ConfigManager.h"
#include "InputManager.h"
#include "PatternLoader.h"
#include <iostream>
#include <iomanip>
#include <string>
#include <utility>
#include <vector>
#include <Windows.h>

std::string vkToString(int vk) {
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
        default:
            char keyName[256];
            if (GetKeyNameTextA(MapVirtualKey(vk, MAPVK_VK_TO_VSC) << 16, keyName, sizeof(keyName))) {
                return keyName;
            }
            return "Key(" + std::to_string(vk) + ")";
    }
}

ConsoleUI::ConsoleUI(std::shared_ptr<AntiRecoilEngine> engine,
                     std::shared_ptr<ConfigManager> configManager,
                     std::shared_ptr<InputManager> inputManager,
                     std::shared_ptr<PatternLoader> patternLoader)
        : m_engine(std::move(engine))
        , m_configManager(std::move(configManager))
        , m_inputManager(std::move(inputManager))
        , m_patternLoader(std::move(patternLoader))
        , m_running(false)
{
}

ConsoleUI::~ConsoleUI() = default;

bool ConsoleUI::initialize() {
    SetConsoleTitleA(reinterpret_cast<LPCSTR>(L"CS Anti-Recoil"));

    HANDLE consoleHandle = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(consoleHandle, FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY);

    return true;
}

void ConsoleUI::run() {
    m_running = true;

    while (m_running) {
        showMainMenu();
        std::string selection = getUserInput();
        processMenuSelection(selection);

        m_inputManager->processInput();
    }
}

void ConsoleUI::showMainMenu() {
    clearConsole();

    std::cout << "=========================================" << std::endl;
    std::cout << "           CS2 ANTI-RECOIL              " << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << std::endl;

    showStatus();

    std::cout << std::endl;
    std::cout << "MAIN MENU:" << std::endl;
    std::cout << "1. Pattern Selection" << std::endl;
    std::cout << "2. Settings" << std::endl;
    std::cout << "3. Configure Hotkeys" << std::endl;
    std::cout << "4. Start Anti-Recoil" << std::endl;
    std::cout << "5. Stop Anti-Recoil" << std::endl;
    std::cout << "6. Load Patterns from Lua Script" << std::endl;
    std::cout << "7. Exit" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter selection: ";
}

void ConsoleUI::showPatternMenu() {
    clearConsole();

    std::cout << "=========================================" << std::endl;
    std::cout << "         PATTERN SELECTION              " << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << std::endl;

    std::vector<std::string> patterns = m_patternLoader->getAvailablePatterns();

    if (patterns.empty()) {
        std::cout << "No patterns available. Load patterns first." << std::endl;
        std::cout << std::endl;
        std::cout << "Press Enter to return to main menu...";
        getUserInput();
        return;
    }

    std::cout << "Available patterns:" << std::endl;
    for (size_t i = 0; i < patterns.size(); ++i) {
        std::cout << i + 1 << ". " << patterns[i] << std::endl;
    }

    std::cout << std::endl;
    std::cout << "0. Back to Main Menu" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter selection: ";

    std::string selection = getUserInput();
    processPatternSelection(selection);
}

void ConsoleUI::showSettingsMenu() {
    clearConsole();

    std::cout << "=========================================" << std::endl;
    std::cout << "              SETTINGS                  " << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << std::endl;

    std::cout << "Current Settings:" << std::endl;
    std::cout << "1. Sensitivity: " << m_configManager->getSensitivity() << std::endl;
    std::cout << "2. Resolution: " << m_configManager->getResolutionWidth() << "x"
              << m_configManager->getResolutionHeight() << std::endl;
    std::cout << "3. Aspect Ratio: " << m_configManager->getAspectRatio() << std::endl;
    std::cout << "4. Return to Original Position: "
              << (m_configManager->getReturnToOriginal() ? "Yes" : "No") << std::endl;

    std::cout << std::endl;
    std::cout << "0. Back to Main Menu" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter setting to change: ";

    std::string selection = getUserInput();
    processSettingsSelection(selection);
}

void ConsoleUI::showHotkeyMenu() {
    clearConsole();

    std::cout << "=========================================" << std::endl;
    std::cout << "           HOTKEY CONFIGURATION         " << std::endl;
    std::cout << "=========================================" << std::endl;
    std::cout << std::endl;

    std::vector<std::string> patterns = m_patternLoader->getAvailablePatterns();

    if (patterns.empty()) {
        std::cout << "No patterns available. Load patterns first." << std::endl;
        std::cout << std::endl;
        std::cout << "Press Enter to return to main menu...";
        getUserInput();
        return;
    }

    std::cout << "Current Hotkeys:" << std::endl;
    for (size_t i = 0; i < patterns.size(); ++i) {
        int hotkey = m_configManager->getHotkey(patterns[i]);
        std::cout << i + 1 << ". " << std::left << std::setw(10) << patterns[i] << ": ";

        if (hotkey == 0) {
            std::cout << "Not set" << std::endl;
        } else {
            std::cout << vkToString(hotkey) << std::endl;
        }
    }

    std::cout << std::endl;
    std::cout << "0. Back to Main Menu" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter pattern to configure hotkey: ";

    std::string selection = getUserInput();
    processHotkeySelection(selection);
}

void ConsoleUI::showStatus() {
    std::cout << "STATUS:" << std::endl;

    std::vector<std::string> patterns = m_patternLoader->getAvailablePatterns();
    if (patterns.empty()) {
        std::cout << "- Pattern: None loaded" << std::endl;
    } else {
        std::cout << "- Patterns: " << patterns.size() << " loaded" << std::endl;
    }

    std::cout << "- Anti-Recoil: " << (m_engine->isActive() ? "ACTIVE" : "INACTIVE") << std::endl;

    std::cout << "- Sensitivity: " << m_configManager->getSensitivity() << std::endl;
    std::cout << "- Resolution: " << m_configManager->getResolutionWidth() << "x"
              << m_configManager->getResolutionHeight() << std::endl;
}

void ConsoleUI::clearConsole() {
    system("cls");
}

std::string ConsoleUI::getUserInput() {
    std::string input;
    std::getline(std::cin, input);
    return input;
}

void ConsoleUI::showMessage(const std::string& message) {
    std::cout << std::endl << message << std::endl;
    std::cout << "Press Enter to continue...";
    getUserInput();
}

void ConsoleUI::processMenuSelection(const std::string& selection) {
    if (selection == "1") {
        showPatternMenu();
    } else if (selection == "2") {
        showSettingsMenu();
    } else if (selection == "3") {
        showHotkeyMenu();
    } else if (selection == "4") {
        if (m_engine->isActive()) {
            showMessage("Anti-recoil is already active.");
        } else {
            m_engine->startAntiRecoil();
            showMessage("Anti-recoil started.");
        }
    } else if (selection == "5") {
        if (!m_engine->isActive()) {
            showMessage("Anti-recoil is already inactive.");
        } else {
            m_engine->stopAntiRecoil();
            showMessage("Anti-recoil stopped.");
        }
    } else if (selection == "6") {
        clearConsole();
        std::cout << "Enter path to Lua script: ";
        std::string luaScriptPath = getUserInput();

        if (m_patternLoader->convertLuaPatternsToJson(luaScriptPath)) {
            m_patternLoader->loadAllPatterns();
            showMessage("Patterns loaded from Lua script successfully.");
        } else {
            showMessage("Failed to load patterns from Lua script.");
        }
    } else if (selection == "7") {
        m_running = false;
    } else {
        showMessage("Invalid selection. Please try again.");
    }
}

void ConsoleUI::processPatternSelection(const std::string& selection) {
    if (selection == "0") {
        return;
    }

    std::vector<std::string> patterns = m_patternLoader->getAvailablePatterns();

    try {
        int index = std::stoi(selection) - 1;
        if (index >= 0 && index < static_cast<int>(patterns.size())) {
            std::string selectedPattern = patterns[index];

            if (m_engine->loadPattern(selectedPattern)) {
                showMessage("Pattern '" + selectedPattern + "' loaded successfully.");
            } else {
                showMessage("Failed to load pattern '" + selectedPattern + "'.");
            }
        } else {
            showMessage("Invalid selection. Please try again.");
        }
    } catch (const std::exception& e) {
        showMessage("Invalid input: " + std::string(e.what()));
    }
}

void ConsoleUI::processSettingsSelection(const std::string& selection) {
    if (selection == "0") {
        return;
    } else if (selection == "1") {
        configureSensitivity();
    } else if (selection == "2") {
        configureResolution();
    } else if (selection == "3") {
        configureAspectRatio();
    } else if (selection == "4") {
        configureReturnToOriginal();
    } else {
        showMessage("Invalid selection. Please try again.");
    }
}

void ConsoleUI::processHotkeySelection(const std::string& selection) {
    if (selection == "0") {
        return;
    }

    std::vector<std::string> patterns = m_patternLoader->getAvailablePatterns();

    try {
        int index = std::stoi(selection) - 1;
        if (index >= 0 && index < static_cast<int>(patterns.size())) {
            std::string selectedPattern = patterns[index];
            configureHotkey(selectedPattern);
        } else {
            showMessage("Invalid selection. Please try again.");
        }
    } catch (const std::exception& e) {
        showMessage("Invalid input: " + std::string(e.what()));
    }
}

void ConsoleUI::configureHotkey(const std::string& patternName) {
    clearConsole();

    std::cout << "Configuring hotkey for pattern: " << patternName << std::endl;
    std::cout << std::endl;

    int currentHotkey = m_configManager->getHotkey(patternName);
    std::cout << "Current hotkey: " << (currentHotkey == 0 ? "Not set" : vkToString(currentHotkey)) << std::endl;
    std::cout << std::endl;

    std::cout << "Press the key you want to use as hotkey (ESC to cancel)..." << std::endl;

    int key = 0;
    while (key == 0) {
        for (int i = 1; i < 255; ++i) {
            if (m_inputManager->isKeyPressed(i)) {
                key = i;
                break;
            }
        }

        // Also check mouse buttons
        for (int i = 0; i < 5; ++i) {
            if (m_inputManager->isMouseButtonPressed(i)) {
                switch (i) {
                    case 0: key = VK_LBUTTON; break;
                    case 1: key = VK_RBUTTON; break;
                    case 2: key = VK_MBUTTON; break;
                    case 3: key = VK_XBUTTON1; break;
                    case 4: key = VK_XBUTTON2; break;
                }
                break;
            }
        }

        Sleep(100); // Avoid high CPU usage
    }

    if (key == VK_ESCAPE) {
        showMessage("Hotkey configuration cancelled.");
        return;
    }

    // Update hotkey
    m_configManager->setHotkey(patternName, key);
    m_configManager->saveConfig();

    showMessage("Hotkey for pattern '" + patternName + "' set to " + vkToString(key) + ".");
}

void ConsoleUI::configureSensitivity() {
    clearConsole();

    std::cout << "Configuring sensitivity" << std::endl;
    std::cout << std::endl;

    float currentSensitivity = m_configManager->getSensitivity();
    std::cout << "Current sensitivity: " << currentSensitivity << std::endl;
    std::cout << std::endl;

    std::cout << "Enter new sensitivity (default in CS2 is 2.0): ";
    std::string input = getUserInput();

    try {
        float newSensitivity = std::stof(input);
        if (newSensitivity <= 0) {
            showMessage("Sensitivity must be greater than 0.");
            return;
        }

        m_configManager->setSensitivity(newSensitivity);
        m_engine->setSensitivity(newSensitivity);
        m_configManager->saveConfig();

        showMessage("Sensitivity updated to " + std::to_string(newSensitivity) + ".");
    } catch (const std::exception& e) {
        showMessage("Invalid input: " + std::string(e.what()));
    }
}

void ConsoleUI::configureResolution() {
    clearConsole();

    std::cout << "Configuring resolution" << std::endl;
    std::cout << std::endl;

    int currentWidth = m_configManager->getResolutionWidth();
    int currentHeight = m_configManager->getResolutionHeight();
    std::cout << "Current resolution: " << currentWidth << "x" << currentHeight << std::endl;
    std::cout << std::endl;

    std::cout << "Enter new width: ";
    std::string widthInput = getUserInput();

    std::cout << "Enter new height: ";
    std::string heightInput = getUserInput();

    try {
        int newWidth = std::stoi(widthInput);
        int newHeight = std::stoi(heightInput);

        if (newWidth <= 0 || newHeight <= 0) {
            showMessage("Width and height must be greater than 0.");
            return;
        }

        m_configManager->setResolution(newWidth, newHeight);
        m_engine->setResolution(newWidth, newHeight);
        m_configManager->saveConfig();

        showMessage("Resolution updated to " + std::to_string(newWidth) + "x" + std::to_string(newHeight) + ".");
    } catch (const std::exception& e) {
        showMessage("Invalid input: " + std::string(e.what()));
    }
}

void ConsoleUI::configureAspectRatio() {
    clearConsole();

    std::cout << "Configuring aspect ratio" << std::endl;
    std::cout << std::endl;

    float currentAspectRatio = m_configManager->getAspectRatio();
    std::cout << "Current aspect ratio: " << currentAspectRatio << std::endl;
    std::cout << std::endl;

    std::cout << "Choose aspect ratio:" << std::endl;
    std::cout << "1. 16:9 (" << (16.0f / 9.0f) << ")" << std::endl;
    std::cout << "2. 16:10 (" << (16.0f / 10.0f) << ")" << std::endl;
    std::cout << "3. 4:3 (" << (4.0f / 3.0f) << ")" << std::endl;
    std::cout << "4. 5:4 (" << (5.0f / 4.0f) << ")" << std::endl;
    std::cout << "5. Custom..." << std::endl;
    std::cout << std::endl;
    std::cout << "Enter selection: ";

    std::string selection = getUserInput();
    float newAspectRatio;

    if (selection == "1") {
        newAspectRatio = 16.0f / 9.0f;
    } else if (selection == "2") {
        newAspectRatio = 16.0f / 10.0f;
    } else if (selection == "3") {
        newAspectRatio = 4.0f / 3.0f;
    } else if (selection == "4") {
        newAspectRatio = 5.0f / 4.0f;
    } else if (selection == "5") {
        std::cout << "Enter custom aspect ratio (e.g., 1.777): ";
        std::string customInput = getUserInput();

        try {
            newAspectRatio = std::stof(customInput);
        } catch (const std::exception& e) {
            showMessage("Invalid input: " + std::string(e.what()));
            return;
        }
    } else {
        showMessage("Invalid selection.");
        return;
    }

    if (newAspectRatio <= 0) {
        showMessage("Aspect ratio must be greater than 0.");
        return;
    }

    m_configManager->setAspectRatio(newAspectRatio);
    m_engine->setAspectRatio(newAspectRatio);
    m_configManager->saveConfig();

    showMessage("Aspect ratio updated to " + std::to_string(newAspectRatio) + ".");
}

void ConsoleUI::configureReturnToOriginal() {
    clearConsole();

    std::cout << "Configuring return to original position" << std::endl;
    std::cout << std::endl;

    bool currentSetting = m_configManager->getReturnToOriginal();
    std::cout << "Current setting: " << (currentSetting ? "Yes" : "No") << std::endl;
    std::cout << std::endl;

    std::cout << "Return cursor to original position after anti-recoil?" << std::endl;
    std::cout << "1. Yes" << std::endl;
    std::cout << "2. No" << std::endl;
    std::cout << std::endl;
    std::cout << "Enter selection: ";

    std::string selection = getUserInput();

    if (selection == "1") {
        m_configManager->setReturnToOriginal(true);
        m_engine->setReturnToOriginal(true);
    } else if (selection == "2") {
        m_configManager->setReturnToOriginal(false);
        m_engine->setReturnToOriginal(false);
    } else {
        showMessage("Invalid selection.");
        return;
    }

    m_configManager->saveConfig();
    showMessage("Setting updated.");
}