#pragma once
#include "ApplicationConfig.h"
#include "CommandQueue.h"
#include "Macro.h"
#include "ProgramDetector.h"
#include "pch.h"

class Application
{
    struct MacroEx
    {
        Macro macro;
        float timeCounterSec = 0.f;   // for repeat action
    };

public:
    ~Application();

    Application(const Application&)            = delete;
    Application(Application&&)                 = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&)      = delete;

    void Quit();
    void SubmitCommand(const std::function<void()>& _command);   // thread-safe

    // singleton instance
    static NODISCARD Application& GetInstance();

    // public config
    constexpr static const char* const k_version = "0.1.1";

private:
    Application();

    // interface
    void Initialize_(HINSTANCE _hInst, int _nCmdShow);
    void Shutdown_();
    int  Run_();

    // window
    static LRESULT CALLBACK WndProc_(HWND, UINT, WPARAM, LPARAM);

    void SetWindowProperties_(int _x, int _y, int _width, int _height, bool _bTopMost) const;

    // macro
    void UpdateMacros_();

    // rendering
    void BeginFrame_() const;
    void Render_();
    void EndFrame_() const;

    // on event
    void OnResize_(int _width, int _height);
    void OnMove_(int _x, int _y);

    // window
    void ShowModalWindow_();
    void ShowWorkSpaceWindow_();
    void ShowMacroVisualizeWindow_() const;

    // serialize
    Json           SerializeMacroStack_();
    NODISCARD bool DeserializeMacroStack_(const Json& _json);

    void SaveMacroStackByFileDialog_();   // using file dialog
    void LoadMacroStackByFileDialog_();   // using file dialog

    // utilities
    NODISCARD std::string GetSystemErrorString_(HRESULT _hr) const;
    NODISCARD std::optional<std::filesystem::path> ShowFileDialog_(const wchar_t* _pFilter, BOOL(WINAPI* _showDialogCallback)(LPOPENFILENAMEW)) const;

    // etc.
    void OpenModalWindow_(const std::function<void()>& _guiRenderFn);
    void ApplyConfigChanges_();

    // application
    bool m_bRunning     = false;
    bool m_bInitialized = false;

    // config
    ApplicationConfig m_config     = {};
    ApplicationConfig m_prevConfig = {};   // for dirty check

    // window
    HINSTANCE m_hInst = nullptr;
    HWND      m_hWnd  = nullptr;

    int m_windowPosX   = 0;
    int m_windowPosY   = 0;
    int m_windowWidth  = 0;
    int m_windowHeight = 0;

    // graphics
    ComPtr<ID3D11Device>           m_pd3dDevice;
    ComPtr<ID3D11DeviceContext>    m_pd3dDeviceContext;
    ComPtr<IDXGISwapChain>         m_pSwapChain;
    ComPtr<ID3D11RenderTargetView> m_pRenderTargetView;

    // program context
    struct ProgramDataEx
    {
        ProgramData data;
        std::string programName;
    };
    ProgramDetector m_programDetector = {};
    ProgramDataEx   m_programData     = {};

    std::vector<MacroEx> m_macroStack = {};   // macro stack

    // modal
    std::function<void()> m_modalGuiRenderFn = {};
    bool                  m_bModalWindowOpen = false;

    // command queue
    CommandQueue m_cmdQueue = {};

    // font
    ImFont* m_pDefaultFont        = nullptr;
    ImFont* m_pMacroVisualizeFont = nullptr;

    static Application* s_instance;
    friend int APIENTRY wWinMain(HINSTANCE, HINSTANCE, LPWSTR, int);

    // configuration
    constexpr static DXGI_FORMAT k_backBufferFormat = DXGI_FORMAT_R8G8B8A8_UNORM;
    constexpr static uint32_t    k_sampleCount      = 4;

    constexpr static const wchar_t* const k_macroFileExtension = L".json";
    constexpr static const char* const    k_configFilePath     = "config.json";

    // windows
    constexpr static const wchar_t* const k_windowTitle     = L"mollu cursor";
    constexpr static const wchar_t* const k_applicationName = L"mollu cursor";
    constexpr static int                  k_windowWidth     = 500;
    constexpr static int                  k_windowHeight    = 800;

    // game detector
    constexpr static const wchar_t* const k_targetProgramName              = L"BlueArchive.exe";
    constexpr static float                k_programDetectorScanIntervalSec = 1.f;

    // color
    constexpr static ImVec4 k_transparentColor = ImVec4(0.f, 0.f, 0.f, 0.f);
    constexpr static ImVec4 k_redColor         = ImVec4(0.902f, 0.306f, 0.267f, 1.f);
    constexpr static ImVec4 k_greenColor       = ImVec4(0.514f, 0.835f, 0.424f, 1.f);
    constexpr static ImVec4 k_blueColor        = ImVec4(0.000f, 0.702f, 1.000f, 1.f);

    // font
    constexpr static const char* const k_fontDirectory          = "fonts\\";
    constexpr static const char* const k_fontFileName           = "NanumSquareRoundR.ttf";
    constexpr static const char* const k_boldFontFileName       = "NanumSquareRoundEB.ttf";
    constexpr static float             k_defaultFontSize        = 18.f;
    constexpr static float             k_macroVisualizeFontSize = 20.f;

    // file dialog
    constexpr static wchar_t k_fileDialogFilter[] = L"macro file (*.json)\0*.json\0all files (*.*)\0*.*\0";
};
