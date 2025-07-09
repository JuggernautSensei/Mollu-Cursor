#include "pch.h"

#include "Application.h"

#include "CommandQueue.h"
#include "GameDetector.h"
#include "Popups.h"
#include "Utilities.h"

#include <filesystem>

#include "Input.h"

namespace
{

void NewHandler()
{
    // 메모리 할당 실패시 크래시 -> 정상적인 어플리케이션이 아님
    CRASH("메모리 할당 실패");
}

}   // namespace

IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
Application*           Application::s_ctx = nullptr;

Application::Application()  = default;
Application::~Application() = default;

void Application::Initialize_(const HINSTANCE _hInst, [[maybe_unused]] int _nCmdShow)
{
    ASSERT(!m_bRunning && !m_bInitialized, "Application is already initialized or running.");

    // 할당 핸들러
    std::set_new_handler(NewHandler);

    // 윈도우 초기화
    {
        // dpi 설정
        ::SetProcessDpiAwarenessContext(DPI_AWARENESS_CONTEXT_PER_MONITOR_AWARE_V2);

        // init class
        m_hInst = _hInst;

        WNDCLASSEXW wClassEx;
        wClassEx.cbSize        = sizeof(WNDCLASSEX);
        wClassEx.style         = CS_HREDRAW | CS_VREDRAW;
        wClassEx.lpfnWndProc   = WndProc_;
        wClassEx.cbClsExtra    = 0;
        wClassEx.cbWndExtra    = 0;
        wClassEx.hInstance     = m_hInst;
        wClassEx.hIcon         = nullptr;
        wClassEx.hCursor       = LoadCursor(nullptr, IDC_ARROW);
        wClassEx.hbrBackground = GetSysColorBrush(COLOR_WINDOW + 1);
        wClassEx.lpszMenuName  = nullptr;
        wClassEx.lpszClassName = k_applicationName;
        wClassEx.hIconSm       = nullptr;

        ATOM atom = RegisterClassExW(&wClassEx);
        if (!atom)
        {
            CRASH("윈도우 클래스를 초기화하는데 실패했습니다: {}", GetSystemErrorString_(GetLastError()));
        }

        // init window
        m_hWnd = CreateWindowW(wClassEx.lpszClassName, k_windowTitle, WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, 0, CW_USEDEFAULT, 0, nullptr, nullptr, _hInst, nullptr);
        if (!m_hWnd)
        {
            CRASH("윈도우를 생성하는데 실패했습니다: {}", GetSystemErrorString_(GetLastError()));
        }
    }

    // 렌더러 초기화
    {
        // d3d11 device creation
        D3D_FEATURE_LEVEL pFeatureLevels[] = {
            D3D_FEATURE_LEVEL_11_1,
            D3D_FEATURE_LEVEL_11_0,
            D3D_FEATURE_LEVEL_10_1,
            D3D_FEATURE_LEVEL_10_0,
            D3D_FEATURE_LEVEL_9_3,
            D3D_FEATURE_LEVEL_9_2,
            D3D_FEATURE_LEVEL_9_1
        };

        HRESULT hr;
        hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, nullptr, NULL, pFeatureLevels, std::size(pFeatureLevels), D3D11_SDK_VERSION, m_pd3dDevice.GetAddressOf(), nullptr, m_pd3dDeviceContext.GetAddressOf());
        if (FAILED(hr))
        {
            CRASH("D3D11CreateDevice failed: {}", GetSystemErrorString_(hr));
        }

        UINT quality;
        hr = m_pd3dDevice->CheckMultisampleQualityLevels(k_backBufferFormat, k_sampleCount, &quality);
        if (FAILED(hr) || quality == 0)
        {
            CRASH("CheckMultiSampleQualityLevels failed: {}", GetSystemErrorString_(hr));
        }
        quality = quality - 1;

        // create device and swap chain
        ComPtr<IDXGIDevice> dxgiDevice;
        hr = m_pd3dDevice.As(&dxgiDevice);
        if (FAILED(hr) || !dxgiDevice)
        {
            CRASH("Failed to get DXGI device: {}", GetSystemErrorString_(hr));
        }

        ComPtr<IDXGIAdapter> dxgiAdapter;
        hr = dxgiDevice->GetAdapter(dxgiAdapter.GetAddressOf());
        if (FAILED(hr) || !dxgiAdapter)
        {
            CRASH("Failed to get DXGI adapter: {}", GetSystemErrorString_(hr));
        }

        ComPtr<IDXGIFactory> dxgiFactory;
        hr = dxgiAdapter->GetParent(IID_PPV_ARGS(dxgiFactory.GetAddressOf()));
        if (FAILED(hr) || !dxgiFactory)
        {
            CRASH("Failed to get DXGI factory: {}", GetSystemErrorString_(hr));
        }

        DXGI_SWAP_CHAIN_DESC swapChainDesc               = {};
        swapChainDesc.BufferCount                        = 1;
        swapChainDesc.BufferDesc.Width                   = 1;   // will be re-size
        swapChainDesc.BufferDesc.Height                  = 1;   // will be re-size
        swapChainDesc.BufferDesc.Format                  = k_backBufferFormat;
        swapChainDesc.BufferDesc.RefreshRate.Numerator   = 60;
        swapChainDesc.BufferDesc.RefreshRate.Denominator = 1;
        swapChainDesc.BufferUsage                        = DXGI_USAGE_RENDER_TARGET_OUTPUT;
        swapChainDesc.OutputWindow                       = m_hWnd;
        swapChainDesc.SampleDesc.Count                   = k_sampleCount;
        swapChainDesc.SampleDesc.Quality                 = quality;
        swapChainDesc.Windowed                           = TRUE;
        swapChainDesc.SwapEffect                         = DXGI_SWAP_EFFECT_DISCARD;
        swapChainDesc.Flags                              = 0;

        hr = dxgiFactory->CreateSwapChain(m_pd3dDevice.Get(), &swapChainDesc, m_pSwapChain.GetAddressOf());
        if (FAILED(hr) || !m_pSwapChain)
        {
            CRASH("Failed to create swap chain: {}", GetSystemErrorString_(hr));
        }
    }

    // 윈도우 리사이즈
    {
        LONG dwStyle   = GetWindowLongA(m_hWnd, GWL_STYLE);
        LONG dwExStyle = GetWindowLongA(m_hWnd, GWL_EXSTYLE);

        RECT rect   = {};
        rect.right  = k_windowWidth;
        rect.bottom = k_windowHeight;

        AdjustWindowRectEx(&rect, dwStyle, false, dwExStyle);

        int width  = rect.right - rect.left;
        int height = rect.bottom - rect.top;

        SetWindowProperties_(0, 0, width, height, false);
    }

    // imgui 초기화
    {
        IMGUI_CHECKVERSION();
        ImGui::CreateContext();
        ImGuiIO& io = ImGui::GetIO();
        io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
        io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
        io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;

        ID3D11Device*        pd3dDevice        = m_pd3dDevice.Get();
        ID3D11DeviceContext* pd3dDeviceContext = m_pd3dDeviceContext.Get();
        ImGui_ImplWin32_Init(m_hWnd);
        ImGui_ImplDX11_Init(pd3dDevice, pd3dDeviceContext);
    }

    // 폰트 로드
    {
        ImGuiIO& io = ImGui::GetIO();

        // default font
        std::string path = std::format("{}{}", k_fontDirectory, k_fontFileName);
        m_pDefaultFont   = io.Fonts->AddFontFromFileTTF(path.c_str(), k_defaultFontSize, nullptr, io.Fonts->GetGlyphRangesKorean());
        io.FontDefault   = m_pDefaultFont;

        // bold font
        path                  = std::format("{}{}", k_fontDirectory, k_boldFontFileName);
        m_pMacroVisualizeFont = io.Fonts->AddFontFromFileTTF(path.c_str(), k_macroVisualizeFontSize, nullptr, io.Fonts->GetGlyphRangesKorean());
    }

    // 게임 감지기 초기화
    {
        // 크리티컬 섹션 방지를 위해 지역 람다로 핸들러 정의
        auto Handler = [](const GameDetectorData& _data)
        {
            Application& ctx = GetInstance();
            ctx.m_cmdQueue.SubmitCommand(
                [data = GameDetectorData { _data }]() mutable
                {
                    Application& context       = GetInstance();
                    context.m_gameDetectorData = std::move(data);
                });
        };

        m_gameDetector.Initialize(k_targetProgramName, k_gameDetectorScanIntervalSec, Handler);

        // 게임 감지기 스레드 실행
        std::thread tmpThread {
            [this]
            {
                m_gameDetector.Run();
            }
        };
        m_gameDetectorThread = std::move(tmpThread);
    }

    // 설정파일 로딩
    {
        Json configJson;
        if (std::filesystem::exists(k_configFilePath) && LoadJson(k_configFilePath, configJson))
        {
            m_config.Deserialize(configJson);
        }
    }

    // 초기화 끝
    {
        m_bInitialized = true;
        m_bRunning     = true;
    }
}

void Application::Shutdown_()
{
    // 설정파일 저장
    {
        Json json = m_config.Serialize();
        SaveJson(k_configFilePath, json);
    }

    // imgui 종료
    {
        ImGui_ImplDX11_Shutdown();
        ImGui_ImplWin32_Shutdown();
        ImGui::DestroyContext();
    }

    // 윈도우 파괴
    if (m_hWnd)
    {
        DestroyWindow(m_hWnd);
        m_hWnd = nullptr;
    }

    // 게임 감지기 종료 및 스레드 정리
    {
        m_gameDetector.Shutdown();
        if (!m_gameDetectorThread.joinable())
        {
            ASSERT(0, "something wrong... mol?lu");
        }
        m_gameDetectorThread.join();
    }
}

int Application::Run_()
{
    ASSERT(m_bInitialized, "Application is not initialized yet.");
    ASSERT(m_bRunning, "Application is not running.");

    MSG msg = {};

    while (m_bRunning)
    {
        // 윈도우 메시지 처리
        if (PeekMessage(&msg, nullptr, NULL, NULL, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }
        else
        {
            // 로직 처리
            UpdateMacros_();
            Input::Update();

            // 렌더링
            BeginFrame_();
            Render_();
            EndFrame_();
        }

        // 지연 처리
        m_cmdQueue.Execute();
    }

    // 종료
    Shutdown_();
    return static_cast<int>(msg.wParam);
}

void Application::Quit()
{
    // 종료는 지연처리로 안전하게
    m_cmdQueue.SubmitCommand(
        [this]
        {
            this->m_bRunning = false;
        });
}

void Application::ClearConfig()
{
    m_config.Clear();
}

Application& Application::GetInstance()
{
    ASSERT(s_ctx, "Application is not created yet.");
    return *s_ctx;
}

void Application::SetWindowProperties_(const int _x, const int _y, const int _width, const int _height, const bool _bTopMost) const
{
    HWND topModeHandle = _bTopMost ? HWND_TOPMOST : HWND_NOTOPMOST;
    ::SetWindowPos(m_hWnd, topModeHandle, _x, _y, _width, _height, SWP_SHOWWINDOW);
    ::SetForegroundWindow(m_hWnd);   // 얘가 있어야 NO-TOPMOST가 즉시 적용됨
}

void Application::BeginFrame_() const
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();
}

void Application::Render_()
{
    // gui 렌더링
    ShowWorkSpaceWindow_();
    ShowMacroVisualizeWindow_();
    ShowModalWindow_();

    // 설정파일 변경사항 적용
    ApplyConfigChanges_();
}

void Application::EndFrame_() const
{
    // 이 부분은 imgui 예제의 플로우를 그대로 따름
    ImGui::Render();

    // 렌더타겟 클리어
    {
        constexpr float clearColor[4] = { 0.f, 0.f, 0.f, 1.0f };
        m_pd3dDeviceContext->OMSetRenderTargets(1, m_pRenderTargetView.GetAddressOf(), nullptr);
        m_pd3dDeviceContext->ClearRenderTargetView(m_pRenderTargetView.Get(), clearColor);
    }

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    // 멀티 뷰포트 처리
    ImGuiIO& io = ImGui::GetIO();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        ImGui::UpdatePlatformWindows();
        ImGui::RenderPlatformWindowsDefault();
    }

    // 더블 버퍼링
    {
        HRESULT hr;
        hr = m_pSwapChain->Present(0, 0);   // 수직동기화 없이 최대속도로 렌더링

        if (FAILED(hr))
        {
            CRASH("Failed to present swap chain: {}", GetSystemErrorString_(hr));
        }
    }
}

// 윈도우 이벤트에 의해 호출될 것
void Application::OnResize_(const int _width, const int _height)
{
    if (!(m_windowWidth == _width && m_windowHeight == _height))
    {
        // 윈도우 사이즈 (client size + title bar size)
        RECT windowRect;
        GetWindowRect(m_hWnd, &windowRect);

        m_windowWidth  = windowRect.right - windowRect.left;
        m_windowHeight = windowRect.bottom - windowRect.top;

        // 클라이언트 사이즈
        uint32_t backBufferWidth  = static_cast<uint32_t>(_width);
        uint32_t backBufferHeight = static_cast<uint32_t>(_height);

        // 스왑체인 리사이징
        {
            m_pRenderTargetView = nullptr;
            ASSERT(m_pSwapChain != nullptr, "Swap chain is not created yet.");

            HRESULT hr;
            hr = m_pSwapChain->ResizeBuffers(0, backBufferWidth, backBufferHeight, DXGI_FORMAT_UNKNOWN, 0);
            ASSERT(SUCCEEDED(hr), "Failed to resize swap chain buffers: {}", GetSystemErrorString_(hr));
        }

        // 렌더타겟 재생성
        {
            HRESULT                hr;
            ComPtr<ID3D11Resource> pResource;
            hr = m_pSwapChain->GetBuffer(0, IID_PPV_ARGS(pResource.GetAddressOf()));
            ASSERT(SUCCEEDED(hr), "Swap chain GetBuffer fail: {}", GetSystemErrorString_(hr));

            // main render target view
            ComPtr<ID3D11Texture2D> pTexture2D;
            pResource.As<ID3D11Texture2D>(&pTexture2D);

            D3D11_TEXTURE2D_DESC texture2DDesc;
            pTexture2D->GetDesc(&texture2DDesc);

            D3D11_RENDER_TARGET_VIEW_DESC pDesc;
            pDesc.Format             = texture2DDesc.Format;
            pDesc.ViewDimension      = D3D11_RTV_DIMENSION_TEXTURE2DMS;
            pDesc.Texture2D.MipSlice = 0;
            hr                       = m_pd3dDevice->CreateRenderTargetView(pResource.Get(), &pDesc, m_pRenderTargetView.GetAddressOf());
            ASSERT(SUCCEEDED(hr), "create render target view fail: {}", GetSystemErrorString_(hr));
        }
    }
}

// 윈도우 프록시저에 의해 호출될 것
void Application::OnMove_([[maybe_unused]] int _x, [[maybe_unused]] int _y)
{
    // _x, _y is client position (do not need)
    RECT windowRect;
    GetWindowRect(m_hWnd, &windowRect);

    m_windowPosX = windowRect.left;
    m_windowPosY = windowRect.top;
}

void Application::ShowModalWindow_()
{
    // 요청이 올 경우 모달 윈도우 오픈
    if (m_bModalWindowOpen)
    {
        ImGui::OpenPopup("information");
        m_bModalWindowOpen = false;
    }

    constexpr ImVec2 k_pivot = ImVec2 { 0.5f, 0.5f };
    ImVec2           center  = ImGui::GetMainViewport()->GetCenter();

    // 모든 메시지박스 형태의 gui 는 여기서 처리됨
    ImGui::SetNextWindowPos(center, ImGuiCond_Always, k_pivot);   // 가운데 정렬
    if (ImGui::BeginPopupModal("information", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoScrollbar))
    {
        // draw user custom modal GUI
        m_modalGuiRenderFn();
        ImGui::EndPopup();
    }
}

void Application::ShowWorkSpaceWindow_()
{
    // 도킹 윈도우
    ImGuiViewport* viewport    = ImGui::GetMainViewport();
    ImGuiID        dockSpaceID = ImGui::DockSpaceOverViewport(0, viewport, ImGuiDockNodeFlags_PassthruCentralNode | ImGuiDockNodeFlags_AutoHideTabBar);

    ImGui::PushStyleColor(ImGuiCol_Button, k_transparentColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonActive, k_transparentColor);
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, k_transparentColor);

    // 메인 gui 윈도우
    ImGui::SetNextWindowDockID(dockSpaceID, ImGuiCond_Always);
    if (!ImGui::Begin("Workspace", nullptr, ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoCollapse | ImGuiWindowFlags_NoResize))
    {
        ImGui::PopStyleColor(3);
        ImGui::End();
        return;
    }
    ImGui::PopStyleColor(3);

    // 정보
    {
        ImGui::Text("Mollu Cursor (v %s)", k_version);

        ImGui::SameLine();
        ImGui::TextDisabled("more..");
        if (ImGui::IsItemClicked())
        {
            InformationPopup popup;
            OpenModalWindow_(popup);
        }

        ImGui::Separator();
    }

    // 게임 감지기 섹션
    {
        ImGui::BulletText("게임 감지");
        ImGui::TreePush("GameDetector");

        if (m_gameDetectorData.bDetected)
        {
            ImGui::TextColored(k_greenColor, "게임 감지됨");

            ImGui::Text("윈도우 좌상단 위치: (%d, %d)", m_gameDetectorData.windowPosX, m_gameDetectorData.windowPosY);
            ImGui::Text("윈도우 해상도: (%d, %d)", m_gameDetectorData.windowWidth, m_gameDetectorData.windowHeight);
            ImGui::Text("종횡비: %s", m_gameDetectorData.aspectRatio == eAspectRatio::Ratio_16_9 ? "16:9" : "4:3");
        }
        else
        {
            ImGui::TextUnformatted("다음 프로그램을 감지합니다.");

            ImGuiStyle& style          = ImGui::GetStyle();
            float       inputTextWidth = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("편집").x - style.FramePadding.x * 2.f - style.ItemSpacing.x;
            std::string targetName     = ConvertToString(m_gameDetectorData.targetProgramName);

            ImGui::SetNextItemWidth(inputTextWidth);
            ImGui::InputText("##FindingProgram", &targetName, ImGuiInputTextFlags_ReadOnly);

            ImGui::SameLine();
            if (ImGui::Button("편집"))
            {
                EditGameDetectorTargetProgramNamePopup popup { m_gameDetector };
                OpenModalWindow_(popup);
            }

            ImGui::TextColored(k_blueColor, "게임을 찾고있습니다.");
        }

        ImGui::TreePop();
    }
    ImGui::Separator();

    // 메뉴
    {
        ImGui::BulletText("메뉴");
        ImGui::TreePush("Menu");

        ImVec2 prettyButtonSize = CalcPrettyButtonSize(3);

        // 매크로 저장하기
        {
            ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_S);
            if (ImGui::Button("매크로 저장", prettyButtonSize))
            {
                SaveMacroStackByFileDialog_();
            }

            // hot key tooltip
            if (ImGui::BeginItemTooltip())
            {
                ImGui::TextUnformatted("단축키 (ctrl + s)");
                ImGui::EndTooltip();
            }
        }

        // 매크로 불러오기
        {
            ImGui::SameLine();
            ImGui::SetNextItemShortcut(ImGuiMod_Ctrl | ImGuiKey_O);
            if (ImGui::Button("매크로 불러오기", prettyButtonSize))
            {
                LoadMacroStackByFileDialog_();
            }

            // hot key tooltip
            if (ImGui::BeginItemTooltip())
            {
                ImGui::TextUnformatted("단축키 (ctrl + o)");
                ImGui::EndTooltip();
            }
        }

        // 설정 초기화
        {
            ImGui::SameLine();
            if (ImGui::Button("설정 초기화", prettyButtonSize))
            {
                ClearApplicationConfigPopup popup {};
                OpenModalWindow_(popup);
            }
        }

        // 옵션 테이블
        if (ImGui::BeginTable("OptionTable", 2, ImGuiTableFlags_SizingStretchProp))
        {
            enum class eOption
            {
                GlobalMacro = 0,
                TopMost,
                MacroVisualize,
                Count,
            };
            constexpr int k_eOptionCount = ToUnderlying(eOption::Count);

            constexpr const char* const k_label[k_eOptionCount][2] = {
                { "모든 매크로가 활성화 됨", "모든 매크로가 비활성화 됨" },
                { "가장 위에 띄우기 활성화 됨", "가장 위에 띄우기 비활성화 됨" },
                { "매크로 시각화 활성화 됨", "매크로 시각화 비활성화 됨" }
            };

            bool* pEnables[k_eOptionCount] = {
                &m_config.bGlobalMacro,
                &m_config.bTopMost,
                &m_config.bMacroVisualize
            };

            eKey* hotKeys[k_eOptionCount] = {
                &m_config.globalMacroHotkey,
                &m_config.topMostHotkey,
                &m_config.macroVisualizeHotkey,
            };

            for (int i = 0; i < k_eOptionCount; i++)
            {
                ImGui::TableNextRow();
                ImGui::PushID(i);

                eOption            option  = ToEnum<eOption>(i);
                eKey*              pHotkey = hotKeys[i];
                bool*              pEnable = pEnables[i];
                const char* const* labels  = k_label[i];

                ImGui::TableSetColumnIndex(0);
                {
                    const char* const label = pEnable ? labels[0] : labels[1];
                    ImGui::Checkbox(label, pEnable);

                    // macro visualize 의 경우 설명 추가
                    if (option == eOption::MacroVisualize)
                    {
                        if (ImGui::BeginItemTooltip())
                        {
                            ImGui::TextUnformatted("매크로 시각화는 게임이 감지되어야만 실행됩니다.");
                            ImGui::EndTooltip();
                        }
                    }
                }

                // 핫키
                ImGui::TableSetColumnIndex(1);
                {
                    static FString<16> s_buf;
                    s_buf.Set(ToString(*pHotkey));

                    ImGuiStyle& style          = ImGui::GetStyle();
                    float       inputTextWidth = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("편집").x - style.FramePadding.x * 2.f - style.ItemSpacing.x;
                    ImGui::SetNextItemWidth(inputTextWidth);
                    ImGui::InputText("##OptionText", s_buf.data(), s_buf.capacity(), ImGuiInputTextFlags_ReadOnly);

                    // 편집 버튼
                    ImGui::SameLine();
                    if (ImGui::Button("편집##OptionHotkeyEditButton"))
                    {
                        EditHotKeyPopup popup { *pHotkey };
                        OpenModalWindow_(popup);
                    }
                }

                ImGui::PopID();
            }

            ImGui::EndTable();
        }

        // 옵션 단축키 처리
        if (Input::IsKeyPressed(m_config.globalMacroHotkey)) { m_config.bGlobalMacro = !m_config.bGlobalMacro; }
        if (Input::IsKeyPressed(m_config.topMostHotkey)) { m_config.bTopMost = !m_config.bTopMost; }
        if (Input::IsKeyPressed(m_config.macroVisualizeHotkey)) { m_config.bMacroVisualize = !m_config.bMacroVisualize; }

        // 성능 디버깅
        if (ImGui::TreeNodeEx("성능 디버깅", ImGuiTreeNodeFlags_SpanAvailWidth))
        {
            ImGuiIO& io = ImGui::GetIO();
            ImGui::Text("루프 간격: %.4f ms", 1'000.f / io.Framerate);
            ImGui::Text("FPS: %.2f", io.Framerate);

            ImGui::TreePop();
        }

        // 폰트 크기조절 옵션
        {
            ImGui::SliderFloat("폰트 스케일", &m_config.fontScale, 0.5f, 2.f, "%.2f");
        }

        // 매크로 시각화 옵션
        {
            ImGui::SliderFloat("매크로 시각화 스케일", &m_config.macroVisualizeScale, 0.1f, 4.f, "%.2f", ImGuiSliderFlags_AlwaysClamp);

            float* pColor = reinterpret_cast<float*>(&m_config.macroVisualizeColor);
            ImGui::ColorEdit3("매크로 시각화 색상", pColor);

            float* pAlpha = pColor + 3;
            ImGui::SliderFloat("매크로 시각화 투명도", pAlpha, 0.f, 1.f);
        }

        ImGui::TreePop();
    }
    ImGui::Separator();

    // 매크로 스택
    {
        ImGui::BulletText("매크로");
        ImGui::TreePush("Macro");

        // 유틸리티 버튼
        {
            ImVec2 prettyButtonWidth = CalcPrettyButtonSize(2);

            // 추가
            if (ImGui::Button("매크로 추가", prettyButtonWidth))
            {
                std::string tmpName = std::format("매크로 {}", m_macroStack.size());
                m_macroStack.emplace_back(tmpName, eKey::None, Vec2::s_zero, true);
            }

            // 초기화
            ImGui::SameLine();
            if (ImGui::Button("모든 매크로 삭제", prettyButtonWidth))
            {
                ClearMacroPopup popup { m_macroStack };
                OpenModalWindow_(popup);
            }

            ImGui::Separator();
        }

        // 매크로 노드들
        for (size_t i = 0; i < m_macroStack.size(); ++i)
        {
            if (ImGui::BeginTable("MacroTable", 2, ImGuiTableFlags_SizingStretchProp))
            {
                ImGui::PushID(static_cast<int>(i));
                Macro& macro = m_macroStack[i];

                // Row 1: macro name
                ImGui::TableNextRow();
                {
                    // column 0: label
                    ImGui::TableSetColumnIndex(0);
                    {
                        DrawHelpTooltip("매크로 이름",
                                        "이름이 특별한 기능을 제공하진 않습니다.\n"
                                        "그냥 단순한 이름표입니다.\n"
                                        "이름은 유일할 필요없으며 가독성을 올리는 역할만 합니다.");
                        ImGui::SameLine();
                        ImGui::TextUnformatted("매크로 이름:");
                    }

                    // column 1: edit
                    ImGui::TableSetColumnIndex(1);
                    {
                        ImGuiStyle& style                   = ImGui::GetStyle();
                        float       macroNameInputItemWidth = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("편집").x - style.FramePadding.x * 2.f - style.ItemSpacing.x;

                        // name view
                        ImGui::SetNextItemWidth(macroNameInputItemWidth);
                        ImGui::InputText("##MacroName", &macro.name, ImGuiInputTextFlags_ReadOnly);

                        // edit button
                        ImGui::SameLine();
                        if (ImGui::Button(("편집##MacroName" + std::to_string(i)).c_str()))
                        {
                            EditStringPopup popup { macro.name };
                            OpenModalWindow_(popup);
                        }
                    }
                }

                // Row 2: hot key
                ImGui::TableNextRow();
                {
                    // column 0: label
                    ImGui::TableSetColumnIndex(0);
                    {
                        DrawHelpTooltip("단축키",
                                        "매크로에 단축키를 설정할 수 있습니다.\n"
                                        "편집 버튼을 눌러 원하는 단축키를 누르세요.\n"
                                        "단축키는 유일성을 보장하지 않으며 단축키 중복 설정에 주의해주세요.");

                        ImGui::SameLine();
                        ImGui::TextUnformatted("단축키:");
                    }

                    // column 1: edit
                    ImGui::TableSetColumnIndex(1);
                    {
                        static FString<16> s_buf;

                        ImGuiStyle& style                    = ImGui::GetStyle();
                        float       hotkeyNameInputItemWidth = ImGui::GetContentRegionAvail().x - ImGui::CalcTextSize("편집").x - style.FramePadding.x * 2.f - style.ItemSpacing.x;

                        // hot key view
                        std::string_view enumName = ToString(macro.hotkey);
                        s_buf.Set(enumName);

                        ImGui::SetNextItemWidth(hotkeyNameInputItemWidth);
                        ImGui::InputText("##Hotkey", s_buf.data(), s_buf.capacity(), ImGuiInputTextFlags_ReadOnly);

                        // edit button
                        ImGui::SameLine();
                        if (ImGui::Button("편집##Hotkey"))
                        {
                            EditHotKeyPopup popup { macro.hotkey };
                            OpenModalWindow_(popup);
                        }
                    }
                }

                // Row 3: coordinate (x, y)
                ImGui::TableNextRow();
                {
                    // column 0: label
                    ImGui::TableSetColumnIndex(0);
                    {
                        DrawHelpTooltip("커서 위치 (x, y)",
                                        "매크로 실행시 커서 위치를 설정합니다.\n"
                                        "위치는 게임 윈도우에 대한 상대적인 위치입니다.\n"
                                        "만약 게임이 감지 되지 않았다면, 전역 좌표계에서 동작합니다.\n"
                                        "편집 버튼을 누르면, 간편하게 좌표를 설정할 수 있습니다.\n"
                                        "편집은 게임이 감지된 경우에만 동작합니다.");

                        ImGui::SameLine();
                        ImGui::TextUnformatted("커서 위치 (x, y):");
                    }

                    // column 1: edit
                    ImGui::TableSetColumnIndex(1);
                    {
                        ImGuiStyle& style              = ImGui::GetStyle();
                        float       dragFloatItemWidth = ImGui::GetContentRegionAvail().x - style.ItemSpacing.x - style.FramePadding.x * 2.f - ImGui::CalcTextSize("편집").x;

                        // position view
                        ImGui::SetNextItemWidth(dragFloatItemWidth);
                        ImGui::DragFloat2("##EditPos", reinterpret_cast<float*>(&macro.position), 0.001f, 0.001f, 1.f, "%.3f");

                        // edit button
                        ImGui::SameLine();
                        {
                            ImGui::BeginDisabled(!m_gameDetectorData.bDetected);
                            if (ImGui::Button("편집"))
                            {
                                EditMacroPositionPopup popup { macro.position, m_gameDetectorData };
                                OpenModalWindow_(popup);
                            }
                            ImGui::EndDisabled();

                            if (!m_gameDetectorData.bDetected)
                            {
                                if (ImGui::BeginItemTooltip())
                                {
                                    ImGui::TextUnformatted("게임이 감지되지 않았습니다.\n"
                                                           "편집 모드는 게임이 감지된 경우에만 사용가능합니다.");
                                    ImGui::EndTooltip();
                                }
                            }
                        }
                    }
                }

                // Row 4: button
                ImGui::TableNextRow();
                {
                    // column 0: label
                    ImGui::TableSetColumnIndex(0);
                    {
                        DrawHelpTooltip("매크로 버튼", "매크로 실행 시 클릭할 버튼을 선택합니다.");

                        ImGui::SameLine();
                        ImGui::TextUnformatted("매크로 버튼:");
                    }

                    // column 1: edit
                    ImGui::TableSetColumnIndex(1);
                    {
                        constexpr const char* k_macroButtonLabels[k_eMacroButtonCount] = {
                            "선택안함",
                            "마우스 좌 버튼",
                            "마우스 휠 버튼",
                            "마우스 우 버튼"
                        };

                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                        ImGui::Combo("##SelectButton", reinterpret_cast<int*>(&macro.button), k_macroButtonLabels, std::size(k_macroButtonLabels));
                    }
                }

                // Row 5: action
                ImGui::TableNextRow();
                {
                    // column 0: label
                    ImGui::TableSetColumnIndex(0);
                    {
                        DrawHelpTooltip("매크로 액션", "매크로 실행 시 수행할 액션을 선택합니다.\n"
                                                       "선택안함: 아무런 동작도 하지 않습니다.\n"
                                                       "클릭: 마우스를 한 번 클릭합니다.\n"
                                                       "연타: 마우스를 연타합니다.\n"
                                                       "홀드: 마우스를 누른 상태로 유지합니다.\n\n"
                                                       "매크로 버튼이 '선택안함' 상태라면 어떤 액션도 취하지 않습니다.");

                        ImGui::SameLine();
                        ImGui::TextUnformatted("매크로 액션:");
                    }

                    // column 1: edit
                    ImGui::TableSetColumnIndex(1);
                    {
                        constexpr const char* k_macroActionLabels[k_eMacroActionCount] = {
                            "선택안함",
                            "클릭",
                            "연타",
                            "홀드"
                        };

                        ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                        ImGui::Combo("##SelectAction", reinterpret_cast<int*>(&macro.action), k_macroActionLabels, std::size(k_macroActionLabels));
                    }
                }

                // additional row : for repeat
                if (macro.action == eMacroAction::ClickRepeat)
                {
                    ImGui::TableNextRow();
                    {
                        // column 0: label
                        ImGui::TableSetColumnIndex(0);
                        {
                            DrawHelpTooltip("연타 간격", "매크로 연타 간격을 설정하며, 단위는 초(sec) 입니다.\n"
                                                         "Tab 버튼을 누를 시 키보드 입력도 가능합니다.");

                            ImGui::SameLine();
                            ImGui::TextUnformatted("연타 간격:");
                        }

                        // column 1: checkbox
                        ImGui::TableSetColumnIndex(1);
                        {
                            ImGui::SetNextItemWidth(ImGui::GetContentRegionAvail().x);
                            if (ImGui::DragFloat("##RepeatInterval", &macro.repeatIntervalSec, 0.001f, 0.f, 10.f, "%.3f 초", ImGuiSliderFlags_AlwaysClamp))
                            {
                                macro.timeCounterSec = 0.f;
                            }
                        }
                    }
                }

                // Row 6: move
                ImGui::TableNextRow();
                {
                    // column 0: label
                    ImGui::TableSetColumnIndex(0);
                    {
                        DrawHelpTooltip("매크로 이동", "매크로 실행 시 마우스 커서가 이동할지 말지를 결정합니다.\n"
                                                       "기본값은 true 입니다.");

                        ImGui::SameLine();
                        ImGui::TextUnformatted("매크로 이동:");
                    }

                    // column 1: checkbox
                    ImGui::TableSetColumnIndex(1);
                    {
                        constexpr const char* k_macroMoveLabels[2] = {
                            "이동이 활성화 됨",
                            "이동이 비활성화 됨"
                        };

                        int labelIndex = macro.bMove ? 0 : 1;
                        ImGui::Checkbox(k_macroMoveLabels[labelIndex], &macro.bMove);
                    }
                }

                // Row 7: checkbox
                ImGui::TableNextRow();
                {
                    // column 0: label
                    ImGui::TableSetColumnIndex(0);
                    {
                        DrawHelpTooltip("매크로 활성화", "매크로를 활성화 혹은 비활성화합니다.");

                        ImGui::SameLine();
                        ImGui::TextUnformatted("매크로 활성화:");
                    }

                    // column 1: checkbox
                    ImGui::TableSetColumnIndex(1);
                    {
                        constexpr const char* k_checkBoxLabel[2] = {
                            "매크로가 활성화 됨",
                            "매크로가 비활성화 됨"
                        };

                        int labelIndex = macro.bEnable ? 0 : 1;
                        ImGui::Checkbox(k_checkBoxLabel[labelIndex], &macro.bEnable);
                    }
                }

                ImGui::PopID();
                ImGui::EndTable();   // end table

                // util button
                {
                    ImGui::PushID(static_cast<int>(i));

                    // clone button
                    ImVec2 buttonWidth = CalcPrettyButtonSize(2);
                    if (ImGui::Button("복제", buttonWidth))
                    {
                        m_cmdQueue.SubmitCommand(
                            [&macro, this]()
                            {
                                Macro newNode = macro;
                                newNode.name += "_복제";
                                m_macroStack.emplace_back(newNode);
                            });
                    }

                    // edit button
                    ImGui::SameLine();
                    if (ImGui::Button("삭제", buttonWidth))
                    {
                        m_cmdQueue.SubmitCommand(
                            [i, this]()
                            {
                                m_macroStack.erase(m_macroStack.begin() + i);
                            });
                    }

                    ImGui::PopID();
                }
                ImGui::Separator();
            }
        }
        ImGui::TreePop();
    }

    ImGui::End();
}

void Application::ShowMacroVisualizeWindow_() const
{
    // target program이 감지된 경우에만 시각화 패널 활성화
    bool bEnable = m_gameDetectorData.bDetected && m_config.bMacroVisualize;
    if (!bEnable)
    {
        return;
    }

    // 감지된 게임 윈도우 만큼의 뷰포트를 생성
    ImVec2 windowSize = { static_cast<float>(m_gameDetectorData.windowWidth), static_cast<float>(m_gameDetectorData.windowHeight) };
    ImVec2 windowPos  = { static_cast<float>(m_gameDetectorData.windowPosX), static_cast<float>(m_gameDetectorData.windowPosY) };

    ImGui::SetNextWindowSize(windowSize, ImGuiCond_Always);
    ImGui::SetNextWindowPos(windowPos, ImGuiCond_Always);
    if (!ImGui::Begin("macro visualize panel", nullptr, ImGuiWindowFlags_NoBackground | ImGuiWindowFlags_NoInputs | ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoDocking))
    {
        ImGui::End();
        return;
    }

    // 오버레이 옵션 (imgui 소스 코드를 고쳐서 사용하고 있습니다. 혹시 오류가 발생한다면 아래 UserExtension 부분을 참고해주세요.)
    ImGuiViewport* viewport = ImGui::GetWindowViewport();
    viewport->Flags |= ImGuiViewportFlags_NoBlockInputs_UserExtension;
    viewport->Flags |= ImGuiViewportFlags_TopMost;

    // 시각화를 위해 굵은 폰트 적용
    ImGui::PushFont(m_pMacroVisualizeFont);

    // 매크로 시각화 시작
    ImDrawList* drawList = ImGui::GetWindowDrawList();
    for (const Macro& macro: m_macroStack)
    {
        // 만약 이동이 활성화되어 있지 않은 매크로는 시각화를 스킵
        if (!macro.bMove)
            continue;

        // 로컬 좌표 -> 전역 좌표
        float cx = static_cast<float>(m_gameDetectorData.windowPosX) + static_cast<float>(m_gameDetectorData.windowWidth) * macro.position.x;
        float cy = static_cast<float>(m_gameDetectorData.windowPosY) + static_cast<float>(m_gameDetectorData.windowHeight) * macro.position.y;

        // 반투명 원
        constexpr float k_circleRadius     = 50.f;   // 기본 사이즈
        float           scaledCircleRadius = k_circleRadius * m_config.macroVisualizeScale;
        drawList->AddCircleFilled(ImVec2 { cx, cy }, scaledCircleRadius, ImGui::GetColorU32(m_config.macroVisualizeColor), 32);

        // pretty center position
        const char* enumName  = ToString(macro.hotkey).data();
        const char* macroName = macro.name.c_str();

        // calc size
        ImVec2 nameSize   = ImGui::CalcTextSize(macroName);
        ImVec2 hotKeySize = ImGui::CalcTextSize(enumName);

        ImVec2 namePos;
        namePos.x = cx - nameSize.x * 0.5f;
        namePos.y = cy - nameSize.y;

        ImVec2 hotkeyPos;
        hotkeyPos.x = cx - hotKeySize.x * 0.5f;
        hotkeyPos.y = cy;

        // 텍스트 렌더링
        ImU32 textColor = ImGui::GetColorU32(ImGuiCol_Text);
        drawList->AddText(namePos, textColor, macroName);
        drawList->AddText(hotkeyPos, textColor, enumName);
    }

    ImGui::PopFont();

    ImGui::End();
}

void Application::UpdateMacros_()
{
    // 전역 매크로 비활성화인 경우 스킵
    if (!m_config.bGlobalMacro)
        return;

    for (Macro& macro: m_macroStack)
    {
        // 비활성화된 매크로 스킵
        if (!macro.bEnable)
            continue;

        // 매크로 카운터 업데이트 (연타 액션에서 사용)
        ImGuiIO& io = ImGui::GetIO();
        macro.timeCounterSec += 1.f / io.Framerate;

        // 매크로 트리거 조건 평가
        bool bInput   = (macro.action == eMacroAction::ClickRepeat) ? Input::IsKeyDown(macro.hotkey) : Input::IsKeyPressed(macro.hotkey);
        bool bRelease = (macro.action == eMacroAction::Hold) && Input::IsKeyReleased(macro.hotkey);
        bool bTime    = (macro.action != eMacroAction::ClickRepeat) || (macro.timeCounterSec >= macro.repeatIntervalSec); // Repeat 모드가 아니면 항상 true

        // 만약 시간 조건을 만족했을시 카운터 초기화
        if (bTime)
            macro.timeCounterSec = 0.f;

        // 눌렀는데 게임이 감지되지 않은 경우 스킵
        if (bInput && !m_gameDetectorData.bDetected)
        {
            MessageBoxPopup popup {
                "게임이 감지된 경우에만\n"
                "매크로를 실행할 수 있습니다."
            };
            OpenModalWindow_(popup);
            continue;
        }

        // 실행조건 -> 단축키를 누르고 시간 조건을 만족 or 릴리즈 상태
        if (!((bInput && bTime) || bRelease))
            continue;

        // 이하 매크로 실행
        // 커서 이동
        if (macro.bMove && !bRelease)
        {
            // 상대 위치 -> 절대 위치 변환
            int x = m_gameDetectorData.windowPosX + static_cast<int>(static_cast<float>(m_gameDetectorData.windowWidth) * macro.position.x);
            int y = m_gameDetectorData.windowPosY + static_cast<int>(static_cast<float>(m_gameDetectorData.windowHeight) * macro.position.y);
            SetCursorPos(x, y);
        }

        // 이벤트 전송
        if (macro.action != eMacroAction::None)
        {
            constexpr DWORD k_downEventTable[] = { MOUSEEVENTF_LEFTDOWN, MOUSEEVENTF_MIDDLEDOWN, MOUSEEVENTF_RIGHTDOWN };
            constexpr DWORD k_upEventTable[]   = { MOUSEEVENTF_LEFTUP, MOUSEEVENTF_MIDDLEUP, MOUSEEVENTF_RIGHTUP };

            DWORD buttonIndex = static_cast<DWORD>(macro.button) - 1;
            ASSERT(buttonIndex < std::size(k_downEventTable), "Invalid button index");

            INPUT inputs[2] = {};

            // 다운
            inputs[0].type       = INPUT_MOUSE;
            inputs[0].mi.dwFlags = k_downEventTable[buttonIndex];

            // 업
            inputs[1].type       = INPUT_MOUSE;
            inputs[1].mi.dwFlags = k_upEventTable[buttonIndex];

            int eventCount = (macro.action == eMacroAction::Hold) ? 1 : 2;   // 홀드는 다운만, 나머지는 다운+업
            int startIndex = bRelease ? 1 : 0;                               // 릴리즈 상태면 업 이벤트만 전송

            // 이벤트 전송
            SendInput(eventCount, inputs + startIndex, sizeof(INPUT));
        }

    }
}

void Application::ApplyConfigChanges_()
{
    // config 변경 감지

    // 폰트 스케일
    if (!IsAlmostSame(m_bPrevFontScale, m_config.fontScale))
    {
        ImGuiIO& io        = ImGui::GetIO();
        io.FontGlobalScale = m_config.fontScale;
        m_bPrevFontScale   = m_config.fontScale;
    }

    // 오버레이 윈도우
    if (m_bPrevTopMost != m_config.bTopMost)
    {
        SetWindowProperties_(m_windowPosX, m_windowPosY, m_windowWidth, m_windowHeight, m_config.bTopMost);
        m_bPrevTopMost = m_config.bTopMost;
    }
}

std::string Application::GetSystemErrorString_(const HRESULT _hr) const
{
    char* errorMsg = nullptr;
    FormatMessageA(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, nullptr, _hr, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), reinterpret_cast<LPSTR>(&errorMsg), 0, nullptr);
    std::string result = errorMsg ? errorMsg : "Unknown error.";
    LocalFree(errorMsg);
    return result;
}

void Application::OpenModalWindow_(const std::function<void()>& _guiRenderFn)
{
    m_modalGuiRenderFn = _guiRenderFn;
    m_bModalWindowOpen = true;
}

Json Application::SerializeMacroStack_(const std::vector<Macro>& _macroStack) const
{
    Json json;

    for (const Macro& macro: _macroStack)
    {
        json.emplace_back(macro.Serialize());
    }

    return json;
}

bool Application::DeserializeMacroStack_(std::vector<Macro>& _out_macroStack, const Json& _json) const
{
    if (!_json.is_array())
    {
        return false;
    }

    std::vector<Macro> tmp;
    tmp.reserve(_json.size());

    for (const Json& item: _json)
    {
        Macro macro;
        if (!macro.Deserialize(item))
        {
            return false;
        }

        tmp.emplace_back(macro);
    }

    _out_macroStack = std::move(tmp);
    return true;
}

void Application::SaveMacroStackByFileDialog_()
{
    std::filesystem::path path;
    if (ShowFileDialog_(path, k_fileDialogFilter, GetSaveFileName))
    {
        path.replace_extension(k_macroFileExtension);

        Json json = SerializeMacroStack_(m_macroStack);
        if (SaveJson(path, json))
        {
            MessageBoxPopup popup { "매크로 저장 성공" };
            OpenModalWindow_(popup);
            return;
        }
    }

    // 실패 시 여기로
    MessageBoxPopup popup { "매크로 저장 실패" };
    OpenModalWindow_(popup);
}

void Application::LoadMacroStackByFileDialog_()
{
    std::filesystem::path path;
    if (ShowFileDialog_(path, k_fileDialogFilter, GetOpenFileName))
    {
        if (path.extension() != k_macroFileExtension)
        {
            MessageBoxPopup popup { "불러올 수 없는 파일" };
            OpenModalWindow_(popup);
            return;
        }

        Json json;
        if (LoadJson(path, json))
        {
            if (DeserializeMacroStack_(m_macroStack, json))
            {
                MessageBoxPopup popup { "매크로 불러오기 성공" };
                OpenModalWindow_(popup);
                return;
            }
        }
    }

    MessageBoxPopup popup { "매크로 불러오기 실패" };
    OpenModalWindow_(popup);
}

bool Application::ShowFileDialog_(std::filesystem::path& _out_selectedPath, const wchar_t* const _pFilter, BOOL (*_showDialogCallback)(LPOPENFILENAMEW)) const
{
    OPENFILENAME ofn;
    wchar_t      szFile[260] = {};

    ZeroMemory(&ofn, sizeof(ofn));
    ofn.lStructSize  = sizeof(ofn);
    ofn.hwndOwner    = m_hWnd;
    ofn.lpstrFile    = szFile;
    ofn.nMaxFile     = sizeof(szFile);
    ofn.lpstrFilter  = _pFilter;
    ofn.nFilterIndex = 1;
    ofn.Flags        = OFN_PATHMUSTEXIST | OFN_FILEMUSTEXIST;

    if (_showDialogCallback(&ofn))
    {
        _out_selectedPath = szFile;
        return true;
    }
    else
    {
        _out_selectedPath.clear();
        return false;
    }
}

LRESULT Application::WndProc_(const HWND hWnd, const UINT message, const WPARAM wParam, const LPARAM lParam)
{
    if (ImGui_ImplWin32_WndProcHandler(hWnd, message, wParam, lParam))
        return true;

    switch (message)
    {
        case WM_DESTROY:
        {
            Application& ctx = GetInstance();
            ctx.Quit();
        }
        break;

        case WM_SIZE:
        {
            UINT         width  = LOWORD(lParam);
            UINT         height = HIWORD(lParam);
            Application& ctx    = GetInstance();
            ctx.OnResize_(static_cast<int>(width), static_cast<int>(height));
        }
        break;

        case WM_MOVE:
        {
            UINT         x   = LOWORD(lParam);
            UINT         y   = HIWORD(lParam);
            Application& ctx = GetInstance();
            ctx.OnMove_(static_cast<int>(x), static_cast<int>(y));
        }
        break;

        default:
            return ::DefWindowProcW(hWnd, message, wParam, lParam);
    }

    return true;
}

// 이하 Config 구조체 코드
bool Application::Config::Deserialize(const Json& _json)
{
    if (!_json.is_object())
    {
        ASSERT(0, "Config::Deserialize: Invalid JSON format.");
        return false;
    }

    // for use default value
    Config defaultConfig;

    // deserialize config
    bGlobalMacro        = _json.value("global_macro_enabled", defaultConfig.bGlobalMacro);
    bTopMost            = _json.value("top_most", defaultConfig.bTopMost);
    fontScale           = _json.value("font_scale", defaultConfig.fontScale);
    bMacroVisualize     = _json.value("macro_visualize_enabled", defaultConfig.bMacroVisualize);
    macroVisualizeScale = _json.value("macro_visualize_scale", defaultConfig.macroVisualizeScale);

    // hotkey
    globalMacroHotkey    = _json.value("global_macro_option_hotkey", defaultConfig.globalMacroHotkey);
    topMostHotkey        = _json.value("top_most_option_hotkey", defaultConfig.topMostHotkey);
    macroVisualizeHotkey = _json.value("macro_visualize_option_hotkey", defaultConfig.macroVisualizeHotkey);

    // 색상
    const Json& colorJson = _json["macro_visualize_color"];
    if (colorJson.is_array() && colorJson.size() == 4)
    {
        macroVisualizeColor.x = colorJson[0].get<float>();
        macroVisualizeColor.y = colorJson[1].get<float>();
        macroVisualizeColor.z = colorJson[2].get<float>();
        macroVisualizeColor.w = colorJson[3].get<float>();
    }
    else
    {
        // default value
        macroVisualizeColor = defaultConfig.macroVisualizeColor;
    }

    return true;
}

Json Application::Config::Serialize() const
{
    ImGuiIO& io = ImGui::GetIO();
    Json     json;

    // serialize config
    json["global_macro_enabled"]    = bGlobalMacro;
    json["top_most"]                = bTopMost;
    json["font_scale"]              = io.FontGlobalScale;
    json["macro_visualize_enabled"] = bMacroVisualize;
    json["macro_visualize_scale"]   = macroVisualizeScale;
    json["macro_visualize_color"]   = { macroVisualizeColor.x, macroVisualizeColor.y, macroVisualizeColor.z, macroVisualizeColor.w };

    // hotkey
    json["global_macro_option_hotkey"]    = globalMacroHotkey;
    json["top_most_option_hotkey"]        = topMostHotkey;
    json["macro_visualize_option_hotkey"] = macroVisualizeHotkey;

    return json;
}

void Application::Config::Clear()
{
    *this = Config {};
}
