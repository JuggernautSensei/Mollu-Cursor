#include "pch.h"

#include "ApplicationConfig.h"
#include "Utilities.h"

ConfigSerializer::ConfigSerializer(ApplicationConfig& _configRef)
    : m_pConfig(&_configRef)
{
    ASSERT(m_pConfig != nullptr, "ConfigSerializer: Config reference is null.");
}

Json ConfigSerializer::Serialize() const
{
    ImGuiIO& io = ImGui::GetIO();
    Json     json;

    // serialize config
    json["global_macro_enabled"] = m_pConfig->bGlobalMacro;
    json["top_most"]             = m_pConfig->bTopMost;
    json["font_scale"]           = io.FontGlobalScale;

    json["macro_visualize_enabled"] = m_pConfig->bMacroVisualize;
    json["macro_visualize_scale"]   = m_pConfig->macroVisualizeScale;
    json["macro_visualize_color"]   = m_pConfig->macroVisualizeColor;

    json["macro_aim_visualize_enabled"] = m_pConfig->bMacroAimVisualize;
    json["macro_aim_visualize_color"]   = m_pConfig->macroAimVisualizeColor;

    // hotkey
    json["global_macro_hotkey"]        = m_pConfig->globalMacroHotkey;
    json["top_most_hotkey"]            = m_pConfig->topMostHotkey;
    json["macro_visualize_hotkey"]     = m_pConfig->macroVisualizeHotkey;
    json["macro_aim_visualize_hotkey"] = m_pConfig->macroAimVisualizeHotkey;

    return json;
}

bool ConfigSerializer::Deserialize(const Json& _json) const
{
    if (!_json.is_object())
    {
        ASSERT(0, "Config::Deserialize: Invalid JSON format.");
        return false;
    }

    *m_pConfig = ApplicationConfig {};   // reset

    // deserialize config
    m_pConfig->bGlobalMacro    = _json.value("global_macro_enabled", m_pConfig->bGlobalMacro);
    m_pConfig->bTopMost        = _json.value("top_most", m_pConfig->bTopMost);
    m_pConfig->fontScale       = _json.value("font_scale", m_pConfig->fontScale);

    m_pConfig->bMacroVisualize     = _json.value("macro_visualize_enabled", m_pConfig->bMacroVisualize);
    m_pConfig->macroVisualizeScale = _json.value("macro_visualize_scale", m_pConfig->macroVisualizeScale);
    m_pConfig->macroVisualizeColor = _json.value("macro_visualize_color", m_pConfig->macroVisualizeColor);

    m_pConfig->bMacroAimVisualize     = _json.value("macro_aim_visualize_enabled", m_pConfig->bMacroAimVisualize);
    m_pConfig->macroAimVisualizeColor = _json.value("macro_aim_visualize_color", m_pConfig->macroAimVisualizeColor);

    // hotkey
    m_pConfig->globalMacroHotkey       = _json.value("global_macro_hotkey", m_pConfig->globalMacroHotkey);
    m_pConfig->topMostHotkey           = _json.value("top_most_hotkey", m_pConfig->topMostHotkey);
    m_pConfig->macroVisualizeHotkey    = _json.value("macro_visualize_hotkey", m_pConfig->macroVisualizeHotkey);
    m_pConfig->macroAimVisualizeHotkey = _json.value("macro_aim_visualize_hotkey", m_pConfig->macroAimVisualizeHotkey);

    // fix hotkey values
    m_pConfig->globalMacroHotkey       = GetValidEnum(m_pConfig->globalMacroHotkey, eKey::None);
    m_pConfig->topMostHotkey           = GetValidEnum(m_pConfig->topMostHotkey, eKey::None);
    m_pConfig->macroVisualizeHotkey    = GetValidEnum(m_pConfig->macroVisualizeHotkey, eKey::None);
    m_pConfig->macroAimVisualizeHotkey = GetValidEnum(m_pConfig->macroAimVisualizeHotkey, eKey::None);

    return true;
}
