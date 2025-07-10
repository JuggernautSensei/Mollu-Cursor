#include "pch.h"

#include "Input.h"

#include "Application.h"

namespace
{

struct InputState
{
    // 키보드 후킹
    std::atomic<bool> inputInterceptTable[k_eKeyCount] = {};
    std::atomic<bool> bGlobalInputIntercept            = true;
    HHOOK             hKeyboardHook                    = nullptr;

    // 키보드 상태
    bool keyDownStates[k_eKeyCount]     = {};
    bool prevKeyDownStates[k_eKeyCount] = {};

    // 마우스 상태
    bool mouseDownStates[k_eMouseCount]     = {};
    bool prevMouseDownStates[k_eMouseCount] = {};

    // ect.
    bool bInitialized = false;
};

InputState g_inputState;

}   // namespace

void Input::Initialize()
{
    ASSERT(!g_inputState.bInitialized, "Input system already initialized!");

    // 키보드 훅 걸기
    g_inputState.hKeyboardHook = SetWindowsHookEx(WH_KEYBOARD_LL, LowLevelKeyboardProc_, nullptr, 0);
    if (!g_inputState.hKeyboardHook)
    {
        CRASH("Failed to set low-level keyboard hook: %d", GetLastError());
    }

    // 키 상태 초기화
    std::ranges::fill(g_inputState.keyDownStates, false);
    std::ranges::fill(g_inputState.prevKeyDownStates, false);

    // 마우스 상태 초기화
    std::ranges::fill(g_inputState.mouseDownStates, false);
    std::ranges::fill(g_inputState.prevMouseDownStates, false);

    g_inputState.bInitialized = true;
}

void Input::Update()
{
    for (int i = 0; i < k_eMouseCount; i++)
    {
        constexpr int vKey[k_eMouseCount] = { VK_LBUTTON, VK_RBUTTON, VK_MBUTTON };
        g_inputState.prevMouseDownStates[i] = g_inputState.mouseDownStates[i];
        g_inputState.mouseDownStates[i]     = GetAsyncKeyState(vKey[i]) & 0x8000;
    }
}

void Input::Shutdown()
{
    ASSERT(g_inputState.bInitialized, "Input system not initialized!");

    if (g_inputState.hKeyboardHook)
    {
        UnhookWindowsHookEx(g_inputState.hKeyboardHook);
        g_inputState.hKeyboardHook = nullptr;
    }

    g_inputState.bInitialized = false;
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

void Input::SetInputInterceptState(const eKey _key, const bool _block)   // thread-safe
{
    g_inputState.inputInterceptTable[static_cast<unsigned int>(_key)].store(_block, std::memory_order_relaxed);
}

void Input::SetGlobalInputInterceptEnabled(const bool _enable)   // thread-safe
{
    g_inputState.bGlobalInputIntercept.store(_enable, std::memory_order_relaxed);
}

LRESULT Input::LowLevelKeyboardProc_(const int nCode, const WPARAM wParam, const LPARAM lParam)   // thread-safe
{
    if (nCode == HC_ACTION)
    {
        KBDLLHOOKSTRUCT* pKeyBoard = reinterpret_cast<KBDLLHOOKSTRUCT*>(lParam);

        bool bKeyDown = (wParam == WM_KEYDOWN || wParam == WM_SYSKEYDOWN);
        bool bKeyUp   = (wParam == WM_KEYUP || wParam == WM_SYSKEYUP);

        if (bKeyDown || bKeyUp)
        {
            // 메인 스레드를 기준으로 키 상태를 업데이트
            {
                eKey key = static_cast<eKey>(pKeyBoard->vkCode);

                // thread-safe 함
                Application& app = Application::GetInstance();
                app.SubmitCommand(
                    [key, bKeyDown]
                    {
                        int idx                             = static_cast<int>(key);
                        g_inputState.prevKeyDownStates[idx] = g_inputState.keyDownStates[idx];
                        g_inputState.keyDownStates[idx]     = bKeyDown;
                    });
            }

            // 키가 차단되어 있으면 메시지를 무시
            if (g_inputState.bGlobalInputIntercept.load(std::memory_order_relaxed)
                && g_inputState.inputInterceptTable[pKeyBoard->vkCode].load(std::memory_order_relaxed))
            {
                return 1;
            }
        }
    }

    return CallNextHookEx(nullptr, nCode, wParam, lParam);
}
