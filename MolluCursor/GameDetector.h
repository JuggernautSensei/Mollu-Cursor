#pragma once
#include "CommandQueue.h"

using GameDetectorHandler = std::function<void(const GameDetectorData&)>;

// 게임 감지기
// 메인 스레드가 아닌, 추가적인 스레드를 이용해야합니다.
// 메인 스레드와 통신하기위해 GameDetectorHandler를 사용합니다.
// 감지기가 게임을 감지하면 핸들러를 호출합니다.
class GameDetector
{
public:
    void Initialize(std::wstring_view _targetProgramName, float _scanIntervalSec, const GameDetectorHandler& _handler);
    void Shutdown();   // thread-safe

    void ChangeTargetProgramName(std::wstring_view _name);   // thread-safe
    void ChangeScanInterval(float _scanIntervalSec);         // thread-safe

private:
    void                 Run_();   // 자체적인 스레드에서 동작
    static BOOL CALLBACK EnumWindowsProc_(HWND _hWnd, LPARAM _lParam);

    GameDetectorHandler m_handler = {};   // 핸들러
    CommandQueue        m_cmdQueue; 
    std::thread         m_thread;         // game detector 쓰레드

    float        m_timeCounterSec    = 0.f;
    float        m_scanIntervalSec   = 1.f;   // 1초 간격으로 스캔
    std::wstring m_targetProgramName = {};    // 감시할 프로세스 이름

    bool m_bRunning    = false;
    bool m_bInitialize = false;
};