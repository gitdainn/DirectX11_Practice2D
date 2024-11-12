#pragma once
#include "State.h"

class CPlayerDamaged : public CState
{
public:
    explicit CPlayerDamaged() {}
    ~CPlayerDamaged() {}

public:
    virtual CState* Input_Handler(CSpriteObject* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection) override;
    virtual void Enter(CSpriteObject* pObject) override;
    virtual void Update(CSpriteObject* pObject, _double TimeDelta = 0.0) override;

public:
    void        Set_TargetDirection(const SPRITE_DIRECTION& eTargetDirection) 
    { 
        m_fKnockBackDistance *= eTargetDirection == SPRITE_DIRECTION::LEFT ? -1.f : 1.f;
    }

private:
    _double     m_DamagedTimeAcc = { 0.0 };
    _bool       m_bHasOnce = { false };
  
    _float    m_fKnockBackDistance = { 20.f };
    _vector     m_vStartPosition = { 0.f, 0.f, 0.f, 0.f };
    _vector     m_vEndPosition = { 0.f, 0.f, 0.f, 0.f };
};

