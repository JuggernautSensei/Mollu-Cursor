#pragma once

// input 은 WM 가 아닌 인풋 후킹으로 갱신합니다.
class Input
{
public:
    static void Update();

    static NODISCARD bool IsKeyUp(eKey _key);
    static NODISCARD bool IsKeyDown(eKey _key);
    static NODISCARD bool IsKeyPressed(eKey _key);
    static NODISCARD bool IsKeyReleased(eKey _key);

    static NODISCARD bool IsMouseUp(eMouse _mouse);
    static NODISCARD bool IsMouseDown(eMouse _mouse);
    static NODISCARD bool IsMousePressed(eMouse _mouse);
    static NODISCARD bool IsMouseReleased(eMouse _mouse);
};
