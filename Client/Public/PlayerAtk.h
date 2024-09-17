#pragma once
#include "State.h"

class CPlayerAtk : public CState
{
public:
    explicit CPlayerAtk() : m_eAttackType(STATE_TYPE::DEFAULT_ATK) {}
    explicit CPlayerAtk(STATE_TYPE AttackType) : m_eAttackType(AttackType) {}
    ~CPlayerAtk() {}

public:
    CState* Input_Handler(CSpriteObject* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection) override;
    virtual void Enter(CSpriteObject* pObject) override;
    virtual void Update(CSpriteObject* pObject, _double TimeDelta = 0.0) override;

private:
    STATE_TYPE  m_eAttackType;
    const int   m_iMaxCombo = { 2 };
    _uint       m_iCombo = { 0 };
};

