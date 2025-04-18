#pragma once

#include <Windows.h>
#include <functional>
#include <map>
#include <string>

class InputManager {
public:
    InputManager();
    ~InputManager();

    static bool initialize();

    bool registerHotkey(int id, int modifiers, int key, const std::function<void()>& callback);

    bool unregisterHotkey(int id);

    static bool isKeyPressed(int key) ;

    static bool isMouseButtonPressed(int button) ;

    static POINT getMousePosition() ;

    static void moveMouseTo(int x, int y);

    static void moveMouseRelative(int x, int y);

    void processInput();

    void enableMouseHook(bool enable);

private:
    std::map<int, std::function<void()>> m_hotkeyCallbacks;
    HHOOK m_mouseHook;
    HHOOK m_keyboardHook;

    static LRESULT CALLBACK mouseProc(int nCode, WPARAM wParam, LPARAM lParam);

    static LRESULT CALLBACK keyboardProc(int nCode, WPARAM wParam, LPARAM lParam);
};