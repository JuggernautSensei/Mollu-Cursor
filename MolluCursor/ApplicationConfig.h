#pragma once

struct ApplicationConfig
{
    // options
    bool  bGlobalMacro    = true;
    bool  bTopMost        = false;
    bool  bInputIntercept = true;
    float fontScale       = 1.f;

    // macro visualize
    bool   bMacroVisualize     = true;
    float  macroVisualizeScale = 1.f;
    ImVec4 macroVisualizeColor = ImVec4 { 0.f, 0.f, 0.f, 0.7f };

    bool   bMacroAimVisualize     = true;
    ImVec4 macroAimVisualizeColor = ImVec4 { 0.f, 0.f, 0.f, 1.0f };
    
    // hotkey
    eKey globalMacroHotkey       = eKey::F5;
    eKey topMostHotkey           = eKey::F6;
    eKey inputInterceptHotkey    = eKey::F7;
    eKey macroVisualizeHotkey    = eKey::F8;
    eKey macroAimVisualizeHotkey = eKey::F9;
};

 // for serialization
class ConfigSerializer
{
public:
    explicit ConfigSerializer(ApplicationConfig& _configRef);

    NODISCARD Json Serialize() const;
    bool           Deserialize(const Json& _json) const;

private:
    ApplicationConfig* m_pConfig = nullptr;
};
