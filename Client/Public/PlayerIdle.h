#pragma once
#include "State.h"

class CPlayerIdle : public CState
{
public:
    explicit CPlayerIdle() {}
    ~CPlayerIdle() {}

public:
    virtual CState* Input_Handler(CPlayer* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection) override;
    virtual void Enter(CPlayer* pObject) override;
    virtual void Update(CPlayer* pObject, _double TimeDelta = 0.0) override;
};

