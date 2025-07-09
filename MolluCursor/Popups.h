#pragma once

class GameDetector;

// popups
class EditStringPopup
{
public:
    explicit EditStringPopup(std::string& _rString);

    void operator()();

private:
    std::string* m_pEditString = nullptr;
    std::string  m_tmpString;
};

// popups
class EditGameDetectorTargetProgramNamePopup
{
public:
    explicit EditGameDetectorTargetProgramNamePopup(GameDetector& _rGameDetector);

    void operator()();

private:
    GameDetector* m_pGameDetector;
    std::string   m_tmpString;
};

class EditHotKeyPopup
{
public:
    explicit EditHotKeyPopup(eKey& _rKey);

    void operator()();

private:
    NODISCARD bool IsAllowedKey_(eKey _key) const;

    eKey*       m_pEditKey;
    eKey        m_tmpKey = eKey::None;
    FString<16> m_buf;
};

class MessageBoxPopup
{
public:
    explicit MessageBoxPopup(std::string _message);

    void operator()() const;

private:
    std::string m_message;
};

class EditMacroPositionPopup
{
public:
    EditMacroPositionPopup(Vec2& _rPosition, GameDetectorData& _rGameDetectorData);

    void operator()() const;

private:
    Vec2*             m_pEditPosition     = nullptr;
    GameDetectorData* m_pGameDetectorData = nullptr;
};

class ClearApplicationConfigPopup
{
public:
    void operator()() const;
};

class ClearMacroPopup
{
public:
    explicit ClearMacroPopup(std::vector<Macro>& _rMacros);

    void operator()() const;

private:
    std::vector<Macro>* m_pMacros = nullptr;
};

class InformationPopup
{
public:
    void operator()() const;
};