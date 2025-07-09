#include "pch.h"

#include "GameDetector.h"

namespace detail
{

struct EnumWindowsProcUserData
{
    DWORD targetPID;
    HWND  hWndOrNull;   // if found. hWnd is target process
};

}   // namespace detail

void GameDetector::Scan_()
{
    DEBUG_SCOPED_STOP_WATCH("start game detector scan");
    ASSERT(m_bInitialize && m_bRunning, "game detector is not initialized");

    m_detectedPrograms.clear();

    HANDLE         hProcessSnap;
    PROCESSENTRY32 pe32;
    hProcessSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

    if (hProcessSnap == INVALID_HANDLE_VALUE)
    {
        ASSERT(0, "CreateToolhelp32Snapshot failed!");
        return;
    }

    pe32.dwSize = sizeof(PROCESSENTRY32);
    if (Process32First(hProcessSnap, &pe32))
    {
        do
        {
            m_detectedPrograms.emplace(pe32.szExeFile, pe32.th32ProcessID);
        } while (Process32Next(hProcessSnap, &pe32));
    }

    CloseHandle(hProcessSnap);
}

void GameDetector::Initialize(const std::wstring_view _targetProgramName, const float _scanIntervalSec, const GameDetectorHandler& _handler)
{
    ASSERT(!_targetProgramName.empty(), "Target program name cannot be empty!");
    ASSERT(_scanIntervalSec > 0.0f, "Scan interval must be greater than 0!");
    ASSERT(_handler, "handler is null function!");
    ASSERT(!m_bInitialize && !m_bRunning, "already game detector initialized. need shutdown");

    m_detectedPrograms.clear();

    m_targetProgramName = _targetProgramName;
    m_scanIntervalSec     = _scanIntervalSec;
    m_handler           = _handler;
    m_timeCounterSec   = m_scanIntervalSec;   // 바로 스캔하기 위해서 큰 값으로 초기화
    m_bRunning          = true;
    m_bInitialize       = true;
}

void GameDetector::Shutdown()
{
    ASSERT(m_bInitialize && m_bRunning, "game detector is not initialized");
    m_bRunning = false;
}

void GameDetector::Run()
{
    ASSERT(m_bInitialize && m_bRunning, "game detector is not initialized");
    auto lastCount = std::chrono::high_resolution_clock::now();

    while (m_bRunning)
    {
        // 틱 계산
        auto  currentCount = std::chrono::high_resolution_clock::now();
        float deltaTime    = static_cast<float>(std::chrono::duration_cast<std::chrono::nanoseconds>(currentCount - lastCount).count()) * 1e-9f;
        lastCount          = currentCount;

        m_timeCounterSec += deltaTime;
        if (m_timeCounterSec >= m_scanIntervalSec)
        {
            // 스캔
            Scan_();

            // 타켓 프로그램을 발견했는가?
            auto it = m_detectedPrograms.find(m_targetProgramName);

            // 데이터 초기화
            GameDetectorData detectData;
            detectData.targetProgramName = m_targetProgramName;
            detectData.bDetected         = false;

            if (it != m_detectedPrograms.end())
            {
                // 타겟을 발견 함
                DEBUG_SCOPED_STOP_WATCH("start game detector extract target program infos");

                // 프로세트의 HWND 얻기
                detail::EnumWindowsProcUserData userdata;
                userdata.targetPID = it->second;
                EnumWindows(EnumWindowsProc_, reinterpret_cast<LPARAM>(&userdata));

                if (userdata.hWndOrNull)
                {
                    // 발견 성공
                    detectData.bDetected = true;

                    RECT windowRect;
                    GetWindowRect(userdata.hWndOrNull, &windowRect);

                    // 위치, 크기 계산
                    detectData.windowPosX   = windowRect.left;
                    detectData.windowPosY   = windowRect.top;
                    detectData.windowWidth  = windowRect.right - windowRect.left;
                    detectData.windowHeight = windowRect.bottom - windowRect.top;

                    // 종횡비 계산
                    constexpr float k_aspectRatio_16_9 = 16.f / 9.f;
                    constexpr float k_aspectRatio_4_3  = 4.f / 3.f;

                    float aspectRatio      = static_cast<float>(detectData.windowWidth) / static_cast<float>(detectData.windowHeight);
                    bool  b16_9            = std::abs(aspectRatio - k_aspectRatio_16_9) < std::abs(aspectRatio - k_aspectRatio_4_3);
                    detectData.aspectRatio = b16_9 ? eAspectRatio::Ratio_16_9 : eAspectRatio::Ratio_4_3;
                }
            }

            // 핸들러 호출
            ASSERT(m_handler, "handler is nullptr");
            m_handler(detectData);

            // 누적 시간 초기화
            m_timeCounterSec = 0;
        }

        // 커맨드 큐 실행
        m_cmdQueue.Execute();
    }

    m_bInitialize = false;   // 완전한 종료
}

void GameDetector::ChangeTargetProgramName(const std::wstring_view _name)
{
    ASSERT(m_bInitialize && m_bRunning, "game detector is not initialized");

    // 변경을 요청
    m_cmdQueue.SubmitCommand(
        [this, name = std::wstring { _name }]() mutable
        {
            m_targetProgramName = std::move(name);
            m_scanIntervalSec     = m_timeCounterSec;   // 즉시 스캔
        });
}

void GameDetector::ChangeScanInterval(float _scanIntervalSec)
{
    ASSERT(m_bInitialize && m_bRunning, "game detector is not initialized");

    // 변경을 요청
    m_cmdQueue.SubmitCommand(
        [this, _scanIntervalSec]
        {
            m_scanIntervalSec = _scanIntervalSec;
            m_scanIntervalSec = m_timeCounterSec;   // 즉시 스캔
        });
}

BOOL GameDetector::EnumWindowsProc_(const HWND _hWnd, const LPARAM _lParam)
{
    detail::EnumWindowsProcUserData* userData = reinterpret_cast<detail::EnumWindowsProcUserData*>(_lParam);

    DWORD pid;
    GetWindowThreadProcessId(_hWnd, &pid);

    if (pid == userData->targetPID && IsWindowVisible(_hWnd))
    {
        userData->hWndOrNull = _hWnd;
        return FALSE;
    }

    userData->hWndOrNull = nullptr;
    return TRUE;
}