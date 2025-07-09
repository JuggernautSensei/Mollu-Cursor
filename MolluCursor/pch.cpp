#include "pch.h"

#include "Application.h"

#include <fstream>

Vec2 Vec2::s_one  = Vec2(1.0f, 1.0f);
Vec2 Vec2::s_zero = Vec2(0.0f, 0.0f);

Json Macro::Serialize() const
{
    Json json;

    json["name"]   = name;
    json["hotkey"] = hotkey;
    json["x"]      = position.x;
    json["y"]      = position.y;
    json["enable"] = bEnable;
    json["button"] = button;
    json["action"] = action;
    json["bMove"]  = bMove;

    return json;
}

bool Macro::Deserialize(const Json& _json)
{
    if (!_json.is_object())
    {
        return false;
    }

    Macro s_defaultMacro;

    name       = _json.value("name", s_defaultMacro.name);
    hotkey     = ToEnum<eKey>(_json.value("hotkey", ToUnderlying(s_defaultMacro.hotkey)));
    position.x = _json.value("x", s_defaultMacro.position.x);
    position.y = _json.value("y", s_defaultMacro.position.y);
    bEnable    = _json.value("enable", s_defaultMacro.bEnable);
    button     = _json.value("button", s_defaultMacro.button);
    action     = _json.value("action", s_defaultMacro.action);
    bMove      = _json.value("bMove", s_defaultMacro.bMove);

    return true;
}
