#pragma once
#include "CommandQueue.h"

using ProgramDetectorHandler = std::function<void(const ProgramData&)>;

// 프로그램 감지기
// 메인 스레드가 아닌, 추가적인 스레드를 이용해야합니다.
// 메인 스레드와 통신하기위해 ProgramDetectorHandler를 사용합니다.
// 감지기가 프로그램을 감지하면 핸들러를 호출합니다.
class ProgramDetector
{
public:
    void Initialize(std::wstring_view _targetProgramName, float _scanIntervalSec, const ProgramDetectorHandler& _handler);
    void Shutdown();   // thread-safe

    void ChangeTargetProgramName(std::wstring_view _name);   // thread-safe
    void ChangeScanInterval(float _scanIntervalSec);         // thread-safe

private:
    void                 Run_();   // 자체적인 스레드에서 동작
    static BOOL CALLBACK EnumWindowsProc_(HWND _hWnd, LPARAM _lParam);

    ProgramDetectorHandler m_handler = {};   // 핸들러
    CommandQueue           m_cmdQueue;
    std::thread            m_thread;   // program detector 쓰레드

    float        m_timeCounterSec    = 0.f;
    float        m_scanIntervalSec   = 1.f;   // 1초 간격으로 스캔
    std::wstring m_targetProgramName = {};    // 감시할 프로세스 이름

    bool m_bRunning    = false;
    bool m_bInitialize = false;
};