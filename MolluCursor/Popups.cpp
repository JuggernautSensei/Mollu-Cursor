#include "pch.h"

#include "Popups.h"

#include "Application.h"
#include "GameDetector.h"
#include "Input.h"
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

EditGameDetectorTargetProgramNamePopup::EditGameDetectorTargetProgramNamePopup(GameDetector& _gameDetectorRef)
    : m_pGameDetector(&_gameDetectorRef)
{
    ASSERT(m_pGameDetector, "GameDetector reference is null");
}

void EditGameDetectorTargetProgramNamePopup::operator()()
{
    ImGui::Text("타겟 프로그램 이름을 입력해주세요.");
    ImGui::InputText("##input name", &m_tmpString, ImGuiInputTextFlags_EnterReturnsTrue | ImGuiInputTextFlags_EscapeClearsAll);

    // ok button
    ImVec2 buttonSize = CalcPrettyButtonSize(2);
    if (ImGui::Button("확인", buttonSize))
    {
        std::wstring newTargetName = ConvertToWideString(m_tmpString);
        m_pGameDetector->ChangeTargetProgramName(newTargetName);

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

EditMacroPositionPopup::EditMacroPositionPopup(Vec2& _positionRef, GameDetectorData& _gameDetectorRef)
    : m_pEditPosition(&_positionRef)
    , m_pGameDetectorData(&_gameDetectorRef)
{
    ASSERT(m_pEditPosition, "m_pEditPosition is nullptr");
    ASSERT(m_pGameDetectorData, "m_pGameDetectorData is nullptr");
}

void EditMacroPositionPopup::operator()() const
{
    ImGui::BulletText("현재 매크로 상대적인 좌표 (x, y)");
    ImGui::TreePush("macro position");

    // get global cursor pos
    POINT point;
    ::GetCursorPos(&point);

    Vec2 nativePos;
    nativePos.x = static_cast<float>(point.x - static_cast<LONG>(m_pGameDetectorData->windowPosX));
    nativePos.y = static_cast<float>(point.y - static_cast<LONG>(m_pGameDetectorData->windowPosY));

    Vec2 relativePos;
    ASSERT(m_pGameDetectorData->windowWidth > 0, "width is zero");
    ASSERT(m_pGameDetectorData->windowHeight > 0, "height is zero");
    relativePos.x = nativePos.x / static_cast<float>(m_pGameDetectorData->windowWidth);
    relativePos.y = nativePos.y / static_cast<float>(m_pGameDetectorData->windowHeight);

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

void ClearApplicationConfigPopup::operator()() const
{
    ImGui::TextUnformatted("정말로 설정을 초기화하겠습니까?");

    ImVec2 buttonWidth = CalcPrettyButtonSize(2);
    if (ImGui::Button("확인", buttonWidth))
    {
        Application& app = Application::GetInstance();
        app.ClearConfig();

        ImGui::CloseCurrentPopup();
    }

    ImGui::SameLine();
    if (ImGui::Button("취소", buttonWidth))
    {
        ImGui::CloseCurrentPopup();
    }
}

ClearMacroPopup::ClearMacroPopup(std::function<void()> _macroClearCallback)
    :m_macroClearCallback(std::move(_macroClearCallback))
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

void InformationPopup::operator()() const
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

    ImGui::Spacing();
    ImVec2 buttonSize = CalcPrettyButtonSize(1);
    if (ImGui::Button("확인", buttonSize))
    {
        ImGui::CloseCurrentPopup();
    }
}
