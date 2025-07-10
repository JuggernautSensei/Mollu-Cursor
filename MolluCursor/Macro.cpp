#include "pch.h"

#include "Macro.h"
#include "Utilities.h"

MacroSerializer::MacroSerializer(Macro& _macroRef)
    : m_pMacro(&_macroRef)
{
    ASSERT(m_pMacro != nullptr, "MacroSerializer: Config reference is null.");
}

Json MacroSerializer::Serialize() const
{
    Json json;

    json["name"]         = m_pMacro->name;
    json["hotkey"]       = m_pMacro->hotkey;
    json["position"]     = m_pMacro->position;
    json["enabled"]      = m_pMacro->bEnable;
    json["button"]       = m_pMacro->button;
    json["action"]       = m_pMacro->action;
    json["move_enabled"] = m_pMacro->bMove;

    return json;
}

bool MacroSerializer::Deserialize(const Json& _json) const
{
    if (!_json.is_object())
    {
        return false;
    }

    *m_pMacro = Macro {};   // reset

    m_pMacro->name     = _json.value("name", m_pMacro->name);
    m_pMacro->hotkey   = _json.value("hotkey", m_pMacro->hotkey);
    m_pMacro->position = _json.value("position", m_pMacro->position);
    m_pMacro->bEnable  = _json.value("enabled", m_pMacro->bEnable);
    m_pMacro->button   = _json.value("button", m_pMacro->button);
    m_pMacro->action   = _json.value("action", m_pMacro->action);
    m_pMacro->bMove    = _json.value("move_enabled", m_pMacro->bMove);

    // 열거형 예외처리
    {
        std::string_view enumName = ToString(m_pMacro->hotkey);
        if (enumName.empty())
            m_pMacro->hotkey = eKey::None;
    }

    {
        std::string_view enumName = ToString(m_pMacro->button);
        if (enumName.empty())
            m_pMacro->button = eMacroButton::None;
    }

    {
        std::string_view enumName = ToString(m_pMacro->action);
        if (enumName.empty())
            m_pMacro->action = eMacroAction::None;
    }

    return true;
}