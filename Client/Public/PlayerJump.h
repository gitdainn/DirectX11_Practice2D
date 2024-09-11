#pragma once
#include "State.h"

class CPlayerJump : public CState
{
public:
    /** @note - 생성자가 하나라도 명시적으로 선언되어 있으면 자동으로 멤버들을 1대1 대입해주는 디폴트 생성자는 없기 떄문에 무조건 모든 변수 직접 초기화 또는 대입해야함 */
    explicit CPlayerJump();
    explicit CPlayerJump(const _uint& iJumpCount);
    explicit CPlayerJump(const CPlayerJump& rhs);
    ~CPlayerJump();

public:
    CState* Input_Handler(CSpriteObject* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection) override;
    virtual void Enter(CSpriteObject* pObject) override;
    virtual void Update(CSpriteObject* pObject, const _double TimeDelta = 0.0) override;

private:
    void    Parabola(CSpriteObject* pObject, const _double TimeDelta);
    const bool     IsOnGround(CSpriteObject* pObject);

private:
    _bool   m_bCanJump;
    _bool   m_bIsFalling;
    _bool   m_bIsDead;
    _bool   m_bIsPaused;
    _uint   m_iMaxJumpCount;
    _uint   m_iJumpCount;
    _double m_UpTime;
    _double m_DownTime;
    _double m_JumpTimeAcc;
};

