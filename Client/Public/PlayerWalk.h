#pragma once
#include "State.h"

class CPlayerWalk : public CState
{
public:
    explicit CPlayerWalk() {}
    ~CPlayerWalk() {}

public:
    CState* Input_Handler(CSpriteObject* pObject, const STATE_TYPE Input) override;
    virtual void Enter(CSpriteObject* pObject) override;
    virtual void Update(CSpriteObject* pObject, _double TimeDelta = 0.0) override;
};

