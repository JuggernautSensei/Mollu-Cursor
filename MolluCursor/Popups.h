﻿#pragma once

class ProgramDetector;

// popups
class EditStringPopup
{
public:
    explicit EditStringPopup(std::string& _stringRef);

    void operator()();

private:
    std::string* m_pEditString = nullptr;
    std::string  m_tmpString;
};

// popups
class EditProgramDetectorTargetProgramPopup
{
public:
    explicit EditProgramDetectorTargetProgramPopup(ProgramDetector& _programDetectorRef);

    void operator()();

private:
    ProgramDetector* m_pProgramDetector;
    std::string      m_tmpString;
};

class EditHotKeyPopup
{
public:
    explicit EditHotKeyPopup(eKey& _keyRef);

    void operator()();

private:
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
    EditMacroPositionPopup(Vec2& _positionRef, ProgramData _programDetector);

    void operator()() const;

private:
    Vec2*       m_pEditPosition = nullptr;
    ProgramData m_programData;
};

class ClearApplicationConfigPopup
{
public:
    explicit ClearApplicationConfigPopup(const std::function<void()>& _configClearCallback);

    void operator()() const;

private:
    std::function<void()> m_configClearCallback;
};

class ClearMacroPopup
{
public:
    explicit ClearMacroPopup(const std::function<void()>& _macroClearCallback);

    void operator()() const;

private:
    std::function<void()> m_macroClearCallback;
};

class ReadMePopup
{
public:
    void operator()() const;
};