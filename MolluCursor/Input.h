#pragma once

class Input
{
public:
    static void Update();

    // key input
    static NODISCARD bool IsKeyDown(eKey _key);
    static NODISCARD bool IsKeyUp(eKey _key);
    static NODISCARD bool IsKeyPressed(eKey _key);
    static NODISCARD bool IsKeyReleased(eKey _key);
};
