#include "pch.h"

#include "Application.h"

int APIENTRY wWinMain(_In_ const HINSTANCE hInstance, _In_opt_ [[maybe_unused]] HINSTANCE hPrevInstance, [[maybe_unused]] _In_ LPWSTR lpCmdLine, _In_ const int nCmdShow)
{
    // create instance
    Application::s_instance = new Application;

    // initialize
    Application& ctx = Application::GetInstance();
    ctx.Initialize_(hInstance, nCmdShow);

    // main loop
    int result = ctx.Run_();

    // destroy instance
    delete Application::s_instance;
    return result;
}
