#pragma once
#include "Command.h"

/** @note - 템플릿 클래스를 상속 받으면 자식도 무조건 템플릿이어야 한다. 
@qurious - 일반 클래스 상속 받고 템플릿 클래스로 정의 불가능?*/
template <typename T>
class CTemplateCmd : public CCommand
{
public:
    explicit CTemplateCmd(const CONTROL_KEY eMove);
    ~CTemplateCmd();

public:
    virtual void Execute(T& pObject) override;

private:
    const CONTROL_KEY m_eMoveInfo;
};
