#pragma once
#include "State.h"

class CPlayerDash : public CState
{
public:
    explicit CPlayerDash(const SPRITE_DIRECTION eDirection);
    ~CPlayerDash();

public:
    CState* Input_Handler(CSpriteObject* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection) override;
    virtual void Enter(CSpriteObject* pObject) override;
    virtual void Update(CSpriteObject* pObject, _double TimeDelta = 0.0) override;

private:
    SPRITE_DIRECTION m_eDirection;
};

