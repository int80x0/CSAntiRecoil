#include "InputManager.h"
#include "utils/Logger.h"
#include <unordered_map>

static std::unordered_map<int, std::function<void()>> s_hotkeyCallbacks;
static HHOOK s_mouseHook = nullptr;
static HHOOK s_keyboardHook = nullptr;

InputManager::InputManager() : m_mouseHook(nullptr), m_keyboardHook(nullptr) {
}

InputManager::~InputManager() {
    for (const auto& [id, _] : m_hotkeyCallbacks) {
        unregisterHotkey(id);
    }

    enableMouseHook(false);
}

bool InputManager::initialize() {
    return true;
}

bool InputManager::registerHotkey(int id, int modifiers, int key, const std::function<void()>& callback) {
    if (RegisterHotKey(nullptr, id, modifiers, key)) {
        m_hotkeyCallbacks[id] = callback;
        s_hotkeyCallbacks[id] = callback;

        Logger::log("Registered hotkey with ID: " + std::to_string(id));
        return true;
    } else {
        DWORD error = GetLastError();
        Logger::error("Failed to register hotkey with ID: " + std::to_string(id) +
                      ", error code: " + std::to_string(error));
        return false;
    }
}

bool InputManager::unregisterHotkey(int id) {
    if (UnregisterHotKey(nullptr, id)) {
        m_hotkeyCallbacks.erase(id);
        s_hotkeyCallbacks.erase(id);

        Logger::log("Unregistered hotkey with ID: " + std::to_string(id));
        return true;
    } else {
        DWORD error = GetLastError();
        Logger::error("Failed to unregister hotkey with ID: " + std::to_string(id) +
                      ", error code: " + std::to_string(error));
        return false;
    }
}

bool InputManager::isKeyPressed(int key) {
    return (GetAsyncKeyState(key) & 0x8000) != 0;
}

bool InputManager::isMouseButtonPressed(int button) {
    int vk = 0;
    switch (button) {
        case 0: vk = VK_LBUTTON; break;  // Left button
        case 1: vk = VK_RBUTTON; break;  // Right button
        case 2: vk = VK_MBUTTON; break;  // Middle button
        case 3: vk = VK_XBUTTON1; break; // X1 (Back)
        case 4: vk = VK_XBUTTON2; break; // X2 (Forward)
        default: return false;
    }

    return (GetAsyncKeyState(vk) & 0x8000) != 0;
}

POINT InputManager::getMousePosition() {
    POINT p;
    GetCursorPos(&p);
    return p;
}

void InputManager::moveMouseTo(int x, int y) {
    SetCursorPos(x, y);
}

void InputManager::moveMouseRelative(int x, int y) {
    POINT p;
    GetCursorPos(&p);
    SetCursorPos(p.x + x, p.y + y);
}

void InputManager::processInput() {
    MSG msg;
    while (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE)) {
        if (msg.message == WM_HOTKEY) {
            int id = static_cast<int>(msg.wParam);
            auto it = m_hotkeyCallbacks.find(id);
            if (it != m_hotkeyCallbacks.end()) {
                it->second();
            }
        }

        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }
}

void InputManager::enableMouseHook(bool enable) {
    if (enable) {
        if (!m_mouseHook) {
            m_mouseHook = SetWindowsHookEx(WH_MOUSE_LL, mouseProc, NULL, 0);
            if (!m_mouseHook) {
                DWORD error = GetLastError();
                Logger::error("Failed to set mouse hook, error code: " + std::to_string(error));
            } else {
                Logger::log("Mouse hook enabled");
            }
        }
    } else {
        if (m_mouseHook) {
            UnhookWindowsHookEx(m_mouseHook);
            m_mouseHook = NULL;
            Logger::log("Mouse hook disabled");
        }
    }
}

// Static hook procedures
LRESULT CALLBACK InputManager::mouseProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        auto* pMouseStruct = reinterpret_cast<MSLLHOOKSTRUCT*>(lParam);

        switch (wParam) {
            case WM_LBUTTONDOWN:
                Logger::log("Left mouse button down");
                break;
            case WM_LBUTTONUP:
                Logger::log("Left mouse button up");
                break;
            case WM_RBUTTONDOWN:
                Logger::log("Right mouse button down");
                break;
            case WM_RBUTTONUP:
                Logger::log("Right mouse button up");
                break;
            case WM_MBUTTONDOWN:
                Logger::log("Middle mouse button down");
                break;
            case WM_MBUTTONUP:
                Logger::log("Middle mouse button up");
                break;
            case WM_MOUSEWHEEL:
                Logger::log("Mouse wheel");
                break;
            case WM_XBUTTONDOWN:
                Logger::log("X button down");
                break;
            case WM_XBUTTONUP:
                Logger::log("X button up");
                break;
        }
    }

    return CallNextHookEx(s_mouseHook, nCode, wParam, lParam);
}

LRESULT CALLBACK InputManager::keyboardProc(int nCode, WPARAM wParam, LPARAM lParam) {
    if (nCode >= 0) {
        KBDLLHOOKSTRUCT* pKeyboardStruct = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        switch (wParam) {
            case WM_KEYDOWN:
                Logger::log("Key down: " + std::to_string(pKeyboardStruct->vkCode));
                break;
            case WM_KEYUP:
                Logger::log("Key up: " + std::to_string(pKeyboardStruct->vkCode));
                break;
        }
    }

    return CallNextHookEx(s_keyboardHook, nCode, wParam, lParam);
}