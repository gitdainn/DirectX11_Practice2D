#pragma once
#include "State.h"

class CPlayerJump : public CState
{
public:
    /** @note - 생성자가 하나라도 명시적으로 선언되어 있으면 자동으로 멤버들을 1대1 대입해주는 디폴트 생성자는 없기 떄문에 무조건 모든 변수 직접 초기화 또는 대입해야함 */
    explicit CPlayerJump();
    explicit CPlayerJump(const CPlayerJump& rhs);
    ~CPlayerJump();

public:
    CState* Input_Handler(CPlayer* pObject, const STATE_TYPE Input, const SPRITE_DIRECTION eDirection) override;
    virtual void Enter(CPlayer* pObject) override;
    virtual void Update(CPlayer* pObject, const _double TimeDelta = 0.0) override;

public:
    void    Set_IsFalling(const _bool bIsFalling)
    {
        m_bIsFalling = bIsFalling;
        m_fPower = 0.f;
    }

    void  Set_IsDead(const _bool bIsDead) { m_bIsDead = bIsDead; }

public:
    const _bool Get_IsFalling() const { return m_bIsFalling; }

private:
    void    Parabola(CPlayer* pObject, const _double TimeDelta);
    _bool           AttachToLineIfBelow(CPlayer* pObject, const _float& fLandingY);

private:
    _bool   m_bCanJump;
    _bool   m_bIsFalling;
    _bool   m_bIsDead;
    _bool   m_bIsPaused;
    _uint   iMaxJumpCount;
    _uint   m_iJumpCount;
    _double m_UpTime;
    _double m_DownTime;
    _double m_JumpTimeAcc;
    _float m_fPower;
};

