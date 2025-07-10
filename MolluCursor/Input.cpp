#include "pch.h"

#include "Input.h"

#include "Application.h"

namespace
{

struct InputState
{
    // 키보드 상태
    bool keyDownStates[k_eKeyCount]     = {};
    bool prevKeyDownStates[k_eKeyCount] = {};

    // 마우스 상태
    bool mouseDownStates[k_eMouseCount]     = {};
    bool prevMouseDownStates[k_eMouseCount] = {};
};

InputState g_inputState;

}   // namespace

void Input::Update()
{
    // 키보드 상태 업데이트
    for (int i = 0; i < k_eKeyCount; i++)
    {
        g_inputState.prevKeyDownStates[i] = g_inputState.keyDownStates[i];
        g_inputState.keyDownStates[i]     = GetAsyncKeyState(i) & 0x8000;
    }

    // 마우스 상태 업데이트
    for (int i = 0; i < k_eMouseCount; i++)
    {
        constexpr int k_vKeyTable[k_eMouseCount] = { VK_LBUTTON, VK_RBUTTON, VK_MBUTTON };
        g_inputState.prevMouseDownStates[i]      = g_inputState.mouseDownStates[i];
        g_inputState.mouseDownStates[i]          = GetAsyncKeyState(k_vKeyTable[i]) & 0x8000;
    }
}

bool Input::IsKeyDown(const eKey _key)
{
    return g_inputState.keyDownStates[static_cast<int>(_key)];
}

bool Input::IsKeyUp(const eKey _key)
{
    return !g_inputState.keyDownStates[static_cast<int>(_key)];
}

bool Input::IsKeyPressed(const eKey _key)
{
    int idx = static_cast<int>(_key);
    return !g_inputState.prevKeyDownStates[idx] && g_inputState.keyDownStates[idx];
}

bool Input::IsKeyReleased(const eKey _key)
{
    int idx = static_cast<int>(_key);
    return g_inputState.prevKeyDownStates[idx] && !g_inputState.keyDownStates[idx];
}

bool Input::IsMouseUp(const eMouse _mouse)
{
    int idx = static_cast<int>(_mouse);
    return !g_inputState.mouseDownStates[idx];
}

bool Input::IsMouseDown(const eMouse _mouse)
{
    int idx = static_cast<int>(_mouse);
    return g_inputState.mouseDownStates[idx];
}

bool Input::IsMousePressed(const eMouse _mouse)
{
    int idx = static_cast<int>(_mouse);
    return !g_inputState.prevMouseDownStates[idx] && g_inputState.mouseDownStates[idx];
}

bool Input::IsMouseReleased(const eMouse _mouse)
{
    int idx = static_cast<int>(_mouse);
    return g_inputState.prevMouseDownStates[idx] && !g_inputState.mouseDownStates[idx];
}
