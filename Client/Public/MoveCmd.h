#pragma once
#include "Command.h"

/** @note - 템플릿 클래스를 상속 받으면 자식도 무조건 템플릿이어야 한다. */
class CMoveCmd : public CCommand
{
public:
    explicit CMoveCmd(const CONTROL_KEY eMove);
    ~CMoveCmd();

public:
    virtual void Execute(CGameObject* pObject) override;

private:
    const CONTROL_KEY m_eMoveInfo;
};
