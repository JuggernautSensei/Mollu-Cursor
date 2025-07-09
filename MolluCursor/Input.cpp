#include "pch.h"

#include "Input.h"

#include "Utilities.h"

namespace detail
{

struct InputState
{
    // key input
    bool keyDownStates[k_eKeyCount]     = {};
    bool prevKeyDownStates[k_eKeyCount] = {};
    bool bInitialized                   = false;
};

static InputState g_inputState;

}   // namespace detail

void Input::Update()
{
    using namespace detail;

    if (g_inputState.bInitialized == false)
    {
        std::ranges::fill(g_inputState.keyDownStates, 0);
        std::ranges::fill(g_inputState.prevKeyDownStates, 0);
        g_inputState.bInitialized = true;
    }

    // update input
    for (size_t i = 0; i < std::size(g_inputState.keyDownStates); ++i)
    {
        g_inputState.prevKeyDownStates[i] = g_inputState.keyDownStates[i];
        g_inputState.keyDownStates[i]     = GetAsyncKeyState(i) & 0x8000;
    }
}

bool Input::IsKeyDown(const eKey _key)
{
    return detail::g_inputState.keyDownStates[ToUnderlying(_key)];
}

bool Input::IsKeyUp(const eKey _key)
{
    return !detail::g_inputState.keyDownStates[ToUnderlying(_key)];
}

bool Input::IsKeyPressed(const eKey _key)
{
    detail::InputState& state = detail::g_inputState;
    size_t              idx   = ToUnderlying(_key);
    return !state.prevKeyDownStates[idx] && state.keyDownStates[idx];
}

bool Input::IsKeyReleased(const eKey _key)
{
    detail::InputState& state = detail::g_inputState;
    size_t              idx   = ToUnderlying(_key);
    return state.prevKeyDownStates[idx] && !state.keyDownStates[idx];
}