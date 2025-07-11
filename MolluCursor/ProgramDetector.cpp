#include "pch.h"

#include "ProgramDetector.h"

#include "Utilities.h"

namespace
{

struct EnumWindowsProcUserData
{
    DWORD targetPID;
    HWND  hWndOrNull;   // if found. hWnd is target process
};

}   // namespace

void ProgramDetector::Initialize(const std::wstring_view _targetProgramName, const float _scanIntervalSec, const ProgramDetectorHandler& _handler)
{
    ASSERT(!_targetProgramName.empty(), "Target program name cannot be empty!");
    ASSERT(_scanIntervalSec > 0.0f, "Scan interval must be greater than 0!");
    ASSERT(_handler, "handler is null function!");
    ASSERT(!m_bInitialize && !m_bRunning, "already program detector initialized. need shutdown");

    m_handler           = _handler;
    m_scanIntervalSec   = _scanIntervalSec;
    m_timeCounterSec    = m_scanIntervalSec;
    m_targetProgramName = _targetProgramName;
    m_bRunning          = true;
    m_bInitialize       = true;

    m_thread = std::thread {
        [this]
        {
            Run_();
        }
    };
}

void ProgramDetector::Shutdown()
{
    ASSERT(m_bInitialize && m_bRunning, "program detector is not initialized");
    m_cmdQueue.SubmitCommand(
        [this]
        {
            m_bRunning = false;
        });

    // 완전히 종료할 때 까지 대기
    if (m_thread.joinable())
    {
        m_thread.join();
    }
}

void ProgramDetector::Run_()
{
    ASSERT(m_bInitialize && m_bRunning, "program detector is not initialized");
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
            DEBUG_SCOPED_STOP_WATCH("스냅샷 성능 프로파일링");

            ProgramData data;
            data.programName = m_targetProgramName;

            // 프로세스 스냅샷에서 타겟 프로그램 찾기
            DWORD          foundPID = 0;
            PROCESSENTRY32 pe32;
            pe32.dwSize = sizeof(pe32);
            HANDLE snap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            if (snap != INVALID_HANDLE_VALUE)
            {
                if (Process32First(snap, &pe32))
                {
                    do
                    {
                        if (_wcsicmp(pe32.szExeFile, m_targetProgramName.c_str()) == 0)
                        {
                            foundPID = pe32.th32ProcessID;
                            break;
                        }
                    } while (Process32Next(snap, &pe32));
                }
                CloseHandle(snap);
            }

            // 타겟 프로그램을 찾았다면 데이터 삽입
            if (foundPID != 0)
            {
                data.bValid = true;

                // 핸들 가져오기
                EnumWindowsProcUserData userdata;
                userdata.hWndOrNull = nullptr;
                userdata.targetPID  = foundPID;
                EnumWindows(EnumWindowsProc_, reinterpret_cast<LPARAM>(&userdata));

                if (userdata.hWndOrNull)
                {
                    RECT rect;
                    GetWindowRect(userdata.hWndOrNull, &rect);
                    data.windowPosX   = rect.left;
                    data.windowPosY   = rect.top;
                    data.windowWidth  = rect.right - rect.left;
                    data.windowHeight = rect.bottom - rect.top;

                    constexpr float k_16_9 = 16.f / 9.f;
                    constexpr float k_4_3  = 4.f / 3.f;

                    float aspectRatio     = static_cast<float>(data.windowWidth) / static_cast<float>(data.windowHeight);
                    data.bWideAspectRatio = std::abs(aspectRatio - k_16_9) < std::abs(aspectRatio - k_4_3);
                }
            }

            // 핸들러 호출
            m_handler(data);
        }

        // 커맨드 큐 실행
        m_cmdQueue.Execute();
    }

    m_bInitialize = false;   // 완전한 종료
}

void ProgramDetector::ChangeTargetProgramName(const std::wstring_view _name)
{
    ASSERT(m_bInitialize && m_bRunning, "program detector is not initialized");

    // 감지기 스레드에 요청 제출
    m_cmdQueue.SubmitCommand(
        [this, name = std::wstring { _name }]() mutable
        {
            m_targetProgramName = std::move(name);
            m_timeCounterSec    = m_scanIntervalSec;   // 즉시 스캔
        });
}

void ProgramDetector::ChangeScanInterval(float _scanIntervalSec)
{
    ASSERT(m_bInitialize && m_bRunning, "program detector is not initialized");

    // 감지기 스레드에 요청 제출
    m_cmdQueue.SubmitCommand(
        [this, _scanIntervalSec]
        {
            m_scanIntervalSec = _scanIntervalSec;
            m_timeCounterSec  = m_scanIntervalSec;   // 즉시 스캔
        });
}

BOOL ProgramDetector::EnumWindowsProc_(const HWND _hWnd, const LPARAM _lParam)
{
    EnumWindowsProcUserData* userData = reinterpret_cast<EnumWindowsProcUserData*>(_lParam);

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