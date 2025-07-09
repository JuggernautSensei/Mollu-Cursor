#pragma once
#include "CommandQueue.h"

using GameDetectorHandler = std::function<void(const GameDetectorData&)>;

// 게임 감지기
// 메인 스레드가 아닌, 추가적인 스레드를 이용해야합니다.
// 메인 스레드와 통신하기위해 GameDetectorHandler를 사용합니다.
// 감지기가 게임을 감지하면 핸들러를 호출합니다. 크리티컬 섹션이 되지 않도록 주의해주세요
class GameDetector
{
public:
    void Initialize(std::wstring_view _targetProgramName, float _scanIntervalSec, const GameDetectorHandler& _handler);
    void Shutdown();

    void Run();   // must run additional thread

    // 설정변경을 '요청'하는 함수.
    void ChangeTargetProgramName(std::wstring_view _name);
    void ChangeScanInterval(float _scanIntervalSec);

private:
    void                 Scan_();
    static BOOL CALLBACK EnumWindowsProc_(HWND _hWnd, LPARAM _lParam);

    GameDetectorHandler m_handler         = {};    // 핸들러
    float               m_scanIntervalSec = 1.f;   // 1초 간격으로 스캔
    float               m_timeCounterSec  = 0.f;   // 시간 누적기

    std::unordered_map<std::wstring, DWORD> m_detectedPrograms  = {};   // name | pid, 감지한 프로세스
    std::wstring                            m_targetProgramName = {};   // 감시할 프로세스 이름

    CommandQueue m_cmdQueue = {};

    bool m_bRunning    = false;
    bool m_bInitialize = false;
};