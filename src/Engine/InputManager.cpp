#ifndef INPUT_MANAGER_CPP_INCLUDED
#define INPUT_MANAGER_CPP_INCLUDED

#include "InputManager.h"
#include "EngineManager.h"
#include "Generic/Base/Window.h"


UnorderedMap<UINT8, INT32> InputManager::s_keyboardMap{
    { BACKSPACE,       VK_BACK     },
    { TAB,             VK_TAB      },
    { RETURN,          VK_RETURN   },
    { PAUSE,           VK_PAUSE    },
    { CAPSLOCK,        VK_CAPITAL  },
    { ESCAPE,          VK_ESCAPE   },
    { SPACE,           VK_SPACE    },
    { PAGE_UP,         VK_PRIOR    },
    { PAGE_DOWN,       VK_NEXT     },
    { END,             VK_END      },
    { HOME,            VK_HOME     },
    { LEFT,            VK_LEFT     },
    { UP,              VK_UP       },
    { RIGHT,           VK_RIGHT    },
    { DOWN,            VK_DOWN     },
    { INSERT,          VK_INSERT   },
    { DELETE_,         VK_DELETE   },
    { LWINDOW,         VK_LWIN     },
    { RWINDOW,         VK_RWIN     },
    { NUMPAD0,         VK_NUMPAD0  },
    { NUMPAD1,         VK_NUMPAD1  },
    { NUMPAD2,         VK_NUMPAD2  },
    { NUMPAD3,         VK_NUMPAD3  },
    { NUMPAD4,         VK_NUMPAD4  },
    { NUMPAD5,         VK_NUMPAD5  },
    { NUMPAD6,         VK_NUMPAD6  },
    { NUMPAD7,         VK_NUMPAD7  },
    { NUMPAD8,         VK_NUMPAD8  },
    { NUMPAD9,         VK_NUMPAD9  },
    { NUMPAD_MULTIPLY, VK_MULTIPLY },
    { NUMPAD_ADD,      VK_ADD      },
    { NUMPAD_SUBTRACT, VK_SUBTRACT },
    { NUMPAD_DECIMAL,  VK_DECIMAL  },
    { NUMPAD_DIVIDE,   VK_DIVIDE   },
    { F1,              VK_F1       },
    { F2,              VK_F2       },
    { F3,              VK_F3       },
    { F4,              VK_F4       },
    { F5,              VK_F5       },
    { F6,              VK_F6       },
    { F7,              VK_F7       },
    { F8,              VK_F8       },
    { F9,              VK_F9       },
    { F10,             VK_F10      },
    { F11,             VK_F11      },
    { F12,             VK_F12      },
    { NUMLOCK,         VK_NUMLOCK  },
    { SCROLL_LOCK,     VK_SCROLL   },
    { LSHIFT,          VK_LSHIFT   },
    { RSHIFT,          VK_RSHIFT   },
    { LCONTROL,        VK_LCONTROL },
    { RCONTROL,        VK_RCONTROL },
    { LALT,            VK_LMENU    },
    { RALT,            VK_RMENU    },
    { A,              'A'          },
    { B,              'B'          },
    { C,              'C'          },
    { D,              'D'          },
    { E,              'E'          },
    { F,              'F'          },
    { G,              'G'          },
    { H,              'H'          },
    { I,              'I'          },
    { J,              'J'          },
    { K,              'K'          },
    { L,              'L'          },
    { M,              'M'          },
    { N,              'N'          },
    { O,              'O'          },
    { P,              'P'          },
    { Q,              'Q'          },
    { R,              'R'          },
    { S,              'S'          },
    { T,              'T'          },
    { U,              'U'          },
    { V,              'V'          },
    { W,              'W'          },
    { X,              'X'          },
    { Y,              'Y'          },
    { Z,              'Z'          },
    { _0,             '0'          },
    { _1,             '1'          },
    { _2,             '2'          },
    { _3,             '3'          },
    { _4,             '4'          },
    { _5,             '5'          },
    { _6,             '6'          },
    { _7,             '7'          },
    { _8,             '8'          },
    { _9,             '9'          },
    { ²,              VK_OEM_7     },
};

UnorderedMap<UINT8, INT32> InputManager::s_mouseMap{
    { LEFT_MOUSE,   VK_LBUTTON  },
    { RIGHT_MOUSE,  VK_RBUTTON  },
    { MIDDLE_MOUSE, VK_MBUTTON  }
};

String InputManager::s_typedChars = "";

void InputManager::HandleInput()
{
    for (const Pair<unsigned char, int> input : s_keyboardMap)
    {
        unsigned char inputKey = input.first;
        int indexKey = input.second;

        bool isKeyDown = (GetAsyncKeyState(indexKey) & 0x8000) != 0;
        InputState currentState = s_keyboardStates[inputKey];

        if (isKeyDown)
        {
            if (currentState == DOWN_STATE || currentState == PRESSED_STATE)
                s_keyboardStates[inputKey] = PRESSED_STATE;
            else
                s_keyboardStates[inputKey] = DOWN_STATE;
        }
        else
        {
            if (currentState == DOWN_STATE || currentState == PRESSED_STATE)
                s_keyboardStates[inputKey] = UP_STATE;
            else
                s_keyboardStates[inputKey] = NONE;
        }
    }

    for (const Pair<unsigned char, int> input : s_mouseMap)
    {
        unsigned char inputButton = input.first;
        int indexButton = input.second;

        bool isButtonDown = (GetAsyncKeyState(indexButton) & 0x8000) != 0;
        InputState currentState = s_mouseStates[inputButton];

        if (isButtonDown)
        {
            if (currentState == DOWN_STATE || currentState == PRESSED_STATE)
                s_mouseStates[inputButton] = PRESSED_STATE;
            else
                s_mouseStates[inputButton] = DOWN_STATE;
        }
        else
        {
            if (currentState == DOWN_STATE || currentState == PRESSED_STATE)
                s_mouseStates[inputButton] = UP_STATE;
            else
                s_mouseStates[inputButton] = NONE;
        }
    }
}

void InputManager::Update(float _dt)
{
    HandleInput();

    if (s_cursorLocked)
    {
        XMINT2 center = {
            EngineManager::GetWindow()->GetWidth() / 2,
            EngineManager::GetWindow()->GetHeight() / 2
        };
        XMINT2 currentPos = GetMousePosition();
        s_mouseDelta = { currentPos.x - center.x, currentPos.y - center.y };
        SetMousePosition(center);
    }
    else
    {
        s_mouseDelta = { 0, 0 };
    }

    if (IsKeyDown(BACKSPACE))
        if (s_typedChars.empty() == false)
            s_typedChars.pop_back();

    if (IsKeyDown(SPACE))
        s_typedChars += ' ';

    if (IsKeyDown(NUMPAD_DECIMAL))
        s_typedChars += '.';

    // A - Z
    for (int i = A; i <= Z; ++i)
    {
        if (IsKeyDown(static_cast<InputKeyboard>(i)))
        {
            bool shiftDown = false;

            if (IsKeyDown(LSHIFT)) shiftDown = true;
            if (IsKeyPressed(LSHIFT)) shiftDown = true;
            if (IsKeyDown(RSHIFT)) shiftDown = true;
            if (IsKeyPressed(RSHIFT)) shiftDown = true;

            char c = 'a';
            if (shiftDown)
            {
                c = 'A' + (i - A);
            }
            else
            {
                c = 'a' + (i - A);
            }
            s_typedChars += c;
        }
    }

    // 0 - 9 
    for (int i = _0; i <= _9; ++i)
    {
        if (IsKeyDown(static_cast<InputKeyboard>(i)))
        {
            char c = '0' + (i - _0);
            s_typedChars += c;
        }
    }

    // 0 - 9 (NUMPAD)
    for (int i = NUMPAD0; i <= NUMPAD9; ++i)
    {
        if (IsKeyDown(static_cast<InputKeyboard>(i)))
        {
            char c = '0' + (i - NUMPAD0);
            s_typedChars += c;
        }
    }
}

bool InputManager::IsKeyPressed(InputKeyboard key)
{
    return s_keyboardStates[key] == PRESSED_STATE;
}

bool InputManager::IsKeyUp(InputKeyboard key)
{
    return s_keyboardStates[key] == UP_STATE;
}

bool InputManager::IsKeyDown(InputKeyboard key)
{
    return s_keyboardStates[key] == DOWN_STATE;
}

bool InputManager::IsMouseButtonPressed(InputMouse key)
{
    return s_mouseStates[key] == PRESSED_STATE;
}

bool InputManager::IsMouseButtonUp(InputMouse key)
{
    return s_mouseStates[key] == UP_STATE;
}

bool InputManager::IsMouseButtonDown(InputMouse key)
{
    return s_mouseStates[key] == DOWN_STATE;
}

XMINT2 InputManager::GetMousePosition()
{
    POINT p;
    GetCursorPos(&p);
    ScreenToClient(s_pHWND, &p);
    return { p.x, p.y };
}

XMINT2 InputManager::GetMousePositionCenter()
{
    XMINT2 pos = GetMousePosition();
    pos.x -= EngineManager::GetWindow()->GetWidth() / 2;
    pos.y -= EngineManager::GetWindow()->GetHeight() / 2;

    return pos;
}

void InputManager::SetMousePosition(XMINT2 const& coordinates)
{
    POINT p{ coordinates.x, coordinates.y };
    ClientToScreen(s_pHWND, &p);
    SetCursorPos(p.x, p.y);
}

void InputManager::LockMouseCursor()
{
    if (s_pHWND == nullptr) return;
    s_cursorLocked = true;

    if (s_cursorLocked == false || s_pHWND == nullptr) return;

    RECT clientRect;
    if (GetClientRect(s_pHWND, &clientRect) == false) return;

    POINT topLeft = { clientRect.left, clientRect.top };
    POINT bottomRight = { clientRect.right, clientRect.bottom };

    ClientToScreen(s_pHWND, &topLeft);
    ClientToScreen(s_pHWND, &bottomRight);

    RECT const clipRect = { topLeft.x, topLeft.y, bottomRight.x, bottomRight.y };
    ClipCursor(&clipRect);
}

void InputManager::UnlockMouseCursor()
{
    s_cursorLocked = false;
    ClipCursor(nullptr);
}

void InputManager::ShowMouseCursor()
{
    if (s_cursorVisible) return;
    s_cursorVisible = true;

    while (s_cursorVisibilityCount < 0)
        s_cursorVisibilityCount = ShowCursor(TRUE);
}

void InputManager::HideMouseCursor()
{
    if (s_cursorVisible == false) return;
    s_cursorVisible = false;

    while (s_cursorVisibilityCount >= 0)
        s_cursorVisibilityCount = ShowCursor(FALSE);
}

const String InputManager::GetTypedChar()
{
    return s_typedChars;
}

void InputManager::ClearTypedChars()
{
    s_typedChars.clear();
}

XMINT2 InputManager::GetMouseDelta()
{
    return s_mouseDelta;
}

#endif