#pragma once

struct Macro
{
    std::string name     = "macro";
    eKey        hotkey   = eKey::None;
    Vec2        position = Vec2::s_zero;
    bool        bEnable  = true;

    // action
    eMacroButton button            = eMacroButton::LeftButton;
    eMacroAction action            = eMacroAction::ClickOnce;
    bool         bMove             = true;
    float        repeatIntervalSec = 0.2f;
};

class MacroSerializer
{
public:
    explicit MacroSerializer(Macro& _macroRef);

    NODISCARD Json Serialize() const;
    bool           Deserialize(const Json& _json) const;

private:
    Macro* m_pMacro = nullptr;
};