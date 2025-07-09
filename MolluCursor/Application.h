#pragma once
#include "GameDetector.h"

class Application
{
    struct Config
    {
        NODISCARD Json Serialize() const;
        bool           Deserialize(const Json& _json);

        void Clear();

        // options
        bool  bGlobalMacro = true;
        bool  bTopMost     = false;
        float fontScale    = 1.f;

        // macro visualize
        bool   bMacroVisualize     = true;
        float  macroVisualizeScale = 1.f;
        ImVec4 macroVisualizeColor = ImVec4 { 0.f, 0.f, 0.f, 0.7f };

        // hotkey
        eKey globalMacroHotkey    = eKey::F5;
        eKey topMostHotkey        = eKey::F6;
        eKey macroVisualizeHotkey = eKey::F7;
    };

public:
    ~Application();

    Application(const Application&)            = delete;
    Application(Application&&)                 = delete;
    Application& operator=(const Application&) = delete;
    Application& operator=(Application&&)      = delete;

    void Quit();
    void ClearConfig();

    // singleton
    static NODISCARD Application& GetInstance();

    // public config
    constexpr static const char* const k_version = "0.0.1";

private:
    Application();

    // interface
    void Initialize_(HINSTANCE _hInst, int _nCmdShow);
    void Shutdown_();
    int  Run_();

    // window
    static LRESULT CALLBACK WndProc_(HWND, UINT, WPARAM, LPARAM);
    void                    SetWindowProperties_(int _x, int _y, int _width, int _height, bool _bTopMost) const;

    // update
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

    // serialize macros
    NODISCARD Json SerializeMacroStack_(const std::vector<Macro>& _macroStack) const;
    NODISCARD std::optional<std::vector<Macro>> DeserializeMacroStack_(const Json& _json) const;

    void SaveMacroStackByFileDialog_();   // using file dialog
    void LoadMacroStackByFileDialog_();   // using file dialog

    // utilities
    NODISCARD std::string GetSystemErrorString_(HRESULT _hr) const;
    NODISCARD std::optional<std::filesystem::path> ShowFileDialog_(const wchar_t* _pFilter, BOOL (*_showDialogCallback)(LPOPENFILENAMEW)) const;

    // etc.
    void OpenModalWindow_(const std::function<void()>& _guiRenderFn);
    void ApplyConfigChanges_();

    // application
    bool m_bRunning     = false;
    bool m_bInitialized = false;

    // config
    Config m_config         = {};
    float  m_bPrevFontScale = 0.f;     // dirty checking
    bool   m_bPrevTopMost   = false;   // dirty checking

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
    GameDetector     m_gameDetector     = {};
    GameDetectorData m_gameDetectorData = {};
    std::thread      m_gameDetectorThread;

    // macro
    std::vector<Macro> m_macroStack = {};

    // modal
    std::function<void()> m_modalGuiRenderFn = {};
    bool                  m_bModalWindowOpen = false;

    // command queue
    CommandQueue m_cmdQueue = {};

    // font
    ImFont* m_pDefaultFont        = nullptr;
    ImFont* m_pMacroVisualizeFont = nullptr;

    static Application* s_ctx;
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
    constexpr static const wchar_t* const k_targetProgramName           = L"BlueArchive.exe";
    constexpr static float                k_gameDetectorScanIntervalSec = 1.f;

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
