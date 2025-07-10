#pragma once

struct InputEvent
{
    enum class eInputType
    {
        KeyDown,
        KeyUp,
        MouseDown,
        MouseUp,
        MouseMove,
        MouseWheel
    };

    eInputType                 type;
    std::variant<eKey, eMouse> input;   // eKey or eMouse
};

// input 은 WM 가 아닌 인풋 후킹으로 갱신합니다.
class Input
{
public:
    static void Initialize();
    static void Shutdown();

    static NODISCARD bool IsKeyUp(eKey _key);
    static NODISCARD bool IsKeyDown(eKey _key);
    static NODISCARD bool IsKeyPressed(eKey _key);
    static NODISCARD bool IsKeyReleased(eKey _key);

    static NODISCARD bool IsMouseUp(eMouse _mouse);
    static NODISCARD bool IsMouseDown(eMouse _mouse);
    static NODISCARD bool IsMousePressed(eMouse _mouse);
    static NODISCARD bool IsMouseReleased(eMouse _mouse);

    static void SetInputInterceptState(eKey _key, bool _block);
    static void SetGlobalInputInterceptEnabled(bool _enable);

    static void OnInputEvent(const InputEvent& _msg);

private:
    // 아래 함수는 다른 스레드에서 실행됨
    static LRESULT CALLBACK LowLevelKeyboardProc_(int nCode, WPARAM wParam, LPARAM lParam);
};
