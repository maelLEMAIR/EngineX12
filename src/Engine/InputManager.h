#ifndef INPUT_MANAGER_H_INCLUDED
#define INPUT_MANAGER_H_INCLUDED

#include "define.h"

enum InputKeyboard
{
    F1, F2, F3, F4, F5, F6, F7, F8, F9, F10, F11, F12,

    A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z,

    _0, _1, _2, _3, _4, _5, _6, _7, _8, _9,
    NUMPAD0, NUMPAD1, NUMPAD2, NUMPAD3, NUMPAD4, NUMPAD5, NUMPAD6, NUMPAD7, NUMPAD8, NUMPAD9,
    NUMPAD_DIVIDE, NUMPAD_MULTIPLY, NUMPAD_SUBTRACT, NUMPAD_ADD, NUMPAD_RETURN, NUMPAD_DECIMAL,

    NUMLOCK, CAPSLOCK, SCROLL_LOCK,
    PAUSE,

    ²,

    LCTRL, RCTRL,
    LSHIFT, RSHIFT,
    LALT, RALT,
    LWINDOW, RWINDOW,

    UP, LEFT, DOWN, RIGHT,

    ESCAPE, TAB, SPACE, BACKSPACE, RETURN,
    INSERT, DELETE_,
    HOME, END,
    PAGE_UP, PAGE_DOWN,

    AMOUNT_KEY,

    ESC = ESCAPE,
    SPACEBAR = SPACE,

    ENTER = RETURN,
    NUMPAD_ENTER = NUMPAD_RETURN,

    LCONTROL = LCTRL,
    RCONTROL = RCTRL,
    ALTGR = RALT,

    UP_ARROW = UP,
    LEFT_ARROW = LEFT,
    DOWN_ARROW = DOWN,
    RIGHT_ARROW = RIGHT,
};

enum InputMouse
{
    LEFT_MOUSE,
    RIGHT_MOUSE,
    MIDDLE_MOUSE,
    AMOUNT_MOUSE
};

enum InputState : UINT8
{
    NONE = 0,
    DOWN_STATE = 1 << 0,
    PRESSED_STATE = 1 << 1,
    UP_STATE = 1 << 2
};

class InputManager
{
public:
    static void Initialize(HWND pHWND) { s_pHWND = pHWND; };
    static void Update(float _dt);
    static void HandleInput();

    static bool IsKeyPressed(InputKeyboard key);
    static bool IsKeyUp(InputKeyboard key);
    static bool IsKeyDown(InputKeyboard key);
    static bool IsMouseButtonPressed(InputMouse key);
    static bool IsMouseButtonUp(InputMouse key);
    static bool IsMouseButtonDown(InputMouse key);
    static XMINT2 GetMousePosition();
    static XMINT2 GetMousePositionCenter();
    static void SetMousePosition(XMINT2 const& coordinates);
    static void LockMouseCursor();
    static void UnlockMouseCursor();
    static bool IsMouseCursorLocked() { return s_cursorLocked; }
    static void ShowMouseCursor();
    static void HideMouseCursor();
    static bool IsMouseCursorVisible() { return s_cursorVisible; }

    static const String GetTypedChar();
    static void ClearTypedChars();

private:
    
    static UnorderedMap<UINT8, INT32> s_keyboardMap;
    inline static InputState s_keyboardStates[AMOUNT_KEY];

    inline static bool s_cursorLocked = false;
    inline static bool s_cursorVisible = true;
    inline static int s_cursorVisibilityCount = 0;

    static UnorderedMap<UINT8, INT32> s_mouseMap;
    inline static InputState s_mouseStates[AMOUNT_MOUSE];

    inline static HWND s_pHWND = nullptr;

    static String s_typedChars;

};

#endif