#pragma once
#include "State.h"

class CPlayerWalk : public CState
{
public:
    explicit CPlayerWalk(const SPRITE_DIRECTION eDir);
    ~CPlayerWalk();

public:
    CState* Input_Handler(CPlayer* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection);
    virtual void Enter(CPlayer* pObject) override;
    virtual void Update(CPlayer* pObject, _double TimeDelta = 0.0) override;

private:
    SPRITE_DIRECTION m_eDirection;
};

