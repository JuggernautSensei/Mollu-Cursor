#include "pch.h"

#include "Popups.h"

#include "Application.h"
#include "Input.h"
#include "ProgramDetector.h"
#include "Utilities.h"

EditStringPopup::EditStringPopup(std::string& _stringRef)
    : m_pEditString(&_stringRef)
    , m_tmpString(_stringRef)
{
    ASSERT(m_pEditString, "m_pEditString is null");
}

void EditStringPopup::operator()()
{
    ImGui::Text("텍스트를 입력해주세요");
    ImGui::InputText("##input name", &m_tmpString, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll);

    ImVec2 buttonSize = CalcPrettyButtonSize(2);

    // ok button
    if (ImGui::Button("확인", buttonSize))
    {
        *m_pEditString = m_tmpString;
        ImGui::CloseCurrentPopup();
    }

    // cancel button
    ImGui::SameLine();
    if (ImGui::Button("취소", buttonSize))
    {
        ImGui::CloseCurrentPopup();
    }
}

EditProgramDetectorTargetProgramPopup::EditProgramDetectorTargetProgramPopup(ProgramDetector& _programDetectorRef)
    : m_pProgramDetector(&_programDetectorRef)
{
    ASSERT(m_pProgramDetector, "ProgramDetector reference is null");
}

void EditProgramDetectorTargetProgramPopup::operator()()
{
    ImGui::Text("타겟 프로그램 이름을 입력해주세요.");
    ImGui::InputText("##input name", &m_tmpString, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll);

    // ok button
    ImVec2 buttonSize = CalcPrettyButtonSize(2);
    if (ImGui::Button("확인", buttonSize))
    {
        std::wstring newTargetName = ConvertToWideString(m_tmpString);
        m_pProgramDetector->ChangeTargetProgramName(newTargetName);

        ImGui::CloseCurrentPopup();
    }

    // cancel button
    ImGui::SameLine();
    if (ImGui::Button("취소", buttonSize))
    {
        ImGui::CloseCurrentPopup();
    }
}

EditHotKeyPopup::EditHotKeyPopup(eKey& _keyRef)
    : m_pEditKey(&_keyRef)
    , m_tmpKey(_keyRef)
{
    ASSERT(m_pEditKey, "m_pEditKey is null");

    std::string_view enumName = ToString(m_tmpKey);
    m_buf.Set(enumName);
}

void EditHotKeyPopup::operator()()
{
    // detect pressed button
    for (unsigned int i = 0; i < k_eKeyCount; ++i)
    {
        eKey key = static_cast<eKey>(i);

        if (!IsValidEnum(key))
            continue;

        if (Input::IsKeyPressed(key))
        {
            m_tmpKey = key;

            std::string_view enumName = ToString(m_tmpKey);
            m_buf.Set(enumName);
        }
    }

    // rendering
    ImGui::Text("단축키로 설정할 키를 누르세요.");
    ImGui::InputText("##EditHotKeyPopup", m_buf.data(), m_buf.capacity(), ImGuiInputTextFlags_ReadOnly);

    ImVec2 buttonSize = CalcPrettyButtonSize(3);

    if (ImGui::Button("확인", buttonSize))
    {
        *m_pEditKey = m_tmpKey;
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("초기화", buttonSize))
    {
        *m_pEditKey = eKey::None;
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("취소", buttonSize))
    {
        ImGui::CloseCurrentPopup();
    }
}

MessageBoxPopup::MessageBoxPopup(std::string _message)
    : m_message(std::move(_message))
{
}

void MessageBoxPopup::operator()() const
{
    ImGui::TextUnformatted(m_message.c_str());

    ImVec2 buttonSize = CalcPrettyButtonSize(1);
    if (ImGui::Button("확인", buttonSize))
    {
        ImGui::CloseCurrentPopup();
    }
}

EditMacroPositionPopup::EditMacroPositionPopup(Vec2& _positionRef, ProgramData _programDetector)
    : m_pEditPosition(&_positionRef)
    , m_programData(std::move(_programDetector))
{
    ASSERT(m_pEditPosition, "m_pEditPosition is nullptr");
}

void EditMacroPositionPopup::operator()() const
{
    ImGui::BulletText("현재 매크로 상대적인 좌표 (x, y)");
    ImGui::TreePush("macro position");

    // get global cursor pos
    POINT point;
    ::GetCursorPos(&point);

    Vec2 nativePos;
    nativePos.x = static_cast<float>(point.x - static_cast<LONG>(m_programData.windowPosX));
    nativePos.y = static_cast<float>(point.y - static_cast<LONG>(m_programData.windowPosY));

    Vec2 relativePos;
    ASSERT(m_programData.windowWidth > 0, "width is zero");
    ASSERT(m_programData.windowHeight > 0, "height is zero");
    relativePos.x = nativePos.x / static_cast<float>(m_programData.windowWidth);
    relativePos.y = nativePos.y / static_cast<float>(m_programData.windowHeight);

    ImGui::DragFloat2("##macro position", reinterpret_cast<float*>(&relativePos), 1.f, 0.f, 0.f, "%.3f", ImGuiSliderFlags_ReadOnly);
    ImGui::TreePop();

    ImGui::TextUnformatted(
        "마우스 왼쪽 버튼을 눌러 위치를 선택합니다.\n"
        "마우스 오른쪽 버튼을 눌러 편집을 취소합니다.");

    if (Input::IsMousePressed(eMouse::LeftButton))
    {
        *m_pEditPosition = relativePos;
        ImGui::CloseCurrentPopup();
    }
    else if (Input::IsMousePressed(eMouse::RightButton))
    {
        ImGui::CloseCurrentPopup();
    }
}

ClearApplicationConfigPopup::ClearApplicationConfigPopup(const std::function<void()>& _configClearCallback)
    : m_configClearCallback(_configClearCallback)
{
    ASSERT(m_configClearCallback, "callback is nullptr");
}

void ClearApplicationConfigPopup::operator()() const
{
    ImGui::TextUnformatted("정말로 설정을 초기화하겠습니까?");

    ImVec2 buttonWidth = CalcPrettyButtonSize(2);
    if (ImGui::Button("확인", buttonWidth))
    {
        m_configClearCallback();
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("취소", buttonWidth))
    {
        ImGui::CloseCurrentPopup();
    }
}

ClearMacroPopup::ClearMacroPopup(const std::function<void()>& _macroClearCallback)
    : m_macroClearCallback(_macroClearCallback)
{
    ASSERT(m_macroClearCallback, "callback is nullptr");
}

void ClearMacroPopup::operator()() const
{
    ImGui::TextUnformatted("정말로 매크로를 모두 삭제하시겠습니까?");

    ImVec2 buttonWidth = CalcPrettyButtonSize(2);
    if (ImGui::Button("확인", buttonWidth))
    {
        m_macroClearCallback();
        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("취소", buttonWidth))
    {
        ImGui::CloseCurrentPopup();
    }
}

void ReadMePopup::operator()() const
{
    if (ImGui::BeginTabBar("ReadMePopup"))
    {
        if (ImGui::BeginTabItem("필독"))
        {
            ImGui::TextUnformatted("UI를 굉장히 직관적이고 단순해서 이리저리 클릭하거나 (?) 도움말 툴팁을 누르면 금방 사용할 수 있을겁니다.\n"
                                   "해당 프로그램은 단순하게 마우스를 원하는 위치로 옮기고 클릭 이벤트를 발생시키는 것이 전부입니다.\n"
                                   "다른 응용프로그램에 대한 어떠한 해킹도 없습니다. 원리는 다른 좌표 수정 프로그램과 동일합니다.\n"
                                   "그 외 기능으로 다음과 같은 기능을 제공합니다.");

            ImGui::NewLine();
            ImGui::BulletText("클릭, 연타, 홀드, 이동 등 다양한 매크로 액션");
            ImGui::BulletText("프로그램 자동 감지, 더하여 블루아카이브 뿐만 아니라 다른 프로그램에서도 해당 프로그램 사용 가능");
            ImGui::BulletText("프로그램의 위치, 크기에 상관없이 일관적인 좌표 제공 (단, 종횡비가 달라질 시 좌표는 달라질 수 있음)");
            ImGui::BulletText("매크로 파일 저장/불러오기");
            ImGui::BulletText("매크로 시각화 및 오버레이");

            ImGui::NewLine();
            ImGui::SeparatorText("블루 아카이브에 사용할 이용자라면 반드시 읽어주세요.");
            ImGui::TextUnformatted("블루 아카이브는 ex스킬 조준 도중에 특정 키를 누르면 조준이 풀립니다.\n"
                                   "그래서 매크로로 사용할 수 있는 단축키가 굉장히 적습니다.\n"
                                   "이건 블루 아카이브 자체 문제이기 때문에, 여러 방면으로 노력해봤으나 해결하기 어려웠습니다.\n"
                                   "다른 매크로 프로그램들도 해당 문제를 해결하지 못한걸로 확인했습니다.\n"
                                   ""
                                   "몇 개의 키는 눌러도 조준이 풀리지 않는데, 지금까지 확인된 바로는\n"
                                   "F1~F12, Tab, Ctrl, Alt, Shift .. 등 키는 사용할 수 있는 것으로 발견되었습니다.\n"
                                   "최소 10개 이상의 키를 사용할 수 있으니 실전에서 사용하기에 충분한 숫자라고 생각합니다.\n"
                                   "모의전 및 임무 등에서 충분히 테스트 해보고 실전에 사용해주시길 바랍니다.\n"
                                   "숫자 및 알파벳 단축키는 사용하지 못합니다. 8ㅅ8\n"
                                   "\n"
                                   "키보드 마우스 훅 걸어서 블루 아카이브로 가는 OS메시지를 조작해보려고 했으나 불안정해서 포기했습니다.\n"
                                   "다른 좋은 방법 있으면 공유부탁드립니다.\n"
                                   "\n");

            ImGui::BulletText("3줄요약");
            ImGui::TextUnformatted("1. 블루아카이브에 매크로 쓸 때 알파벳 ~ 숫자 및 상당히 많은 단축키를 사용하지 못함.\n"
                                   "2. F1~F12, Tab, Shift, Ctrl, Cap lock 등 키는 사용가능. 직접 테스트해보고 실전 투입 권장.\n"
                                   "3. 김용하 대머리\n");

            ImGui::EndTabItem();
        }

        if (ImGui::BeginTabItem("기타"))
        {
            ImGui::Text("Mollu Cursor (v %s)", Application::k_version);
            ImGui::Separator();

            ImGui::TextUnformatted("개발: 몸짱헬창센세 (JuggernautSensei)\n\n"
                                   "단순한 마우스 매크로 프로그램입니다.\n"
                                   "저 포함 다 같이 유용하게 사용할 수 있는 프로그램이 되길 바랍니다.\n"
                                   "자세한 설명은 아래 Github 링크의 Readme를 읽어주세요.\n"
                                   "건의, 요청, 버그 리포트 등은 Github의 Issues를 이용해주세요.\n"
                                   "해당 프로젝트는 MIT 라이선스를 적용합니다.\n\n");

            ImGui::Text("Github:");
            ImGui::SameLine();
            ImGui::TextLinkOpenURL("https://github.com/JuggernautSensei/Mollu-Cursor", "https://github.com/JuggernautSensei/Mollu-Cursor");

            ImGui::EndTabItem();
        }

        ImGui::EndTabBar();

        ImVec2 buttonSize = CalcPrettyButtonSize(1);
        if (ImGui::Button("확인", buttonSize))
        {
            ImGui::CloseCurrentPopup();
        }
    }
}
