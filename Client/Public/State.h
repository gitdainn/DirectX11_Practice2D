#pragma once
#include "Client_Defines.h"

#include "Player.h"

class CState abstract
{
public:
    explicit CState() {}
    virtual ~CState() {}

public:
    /** @note - 순수 가상 함수는 virtual 가상 함수에만 사용 가능 */
    virtual CState* Input_Handler(CPlayer* pObject, const STATE_TYPE Input
        , const SPRITE_DIRECTION eDirection = SPRITE_DIRECTION::DIRECTION_END) = 0;
    virtual void Enter(CPlayer* pObject) = 0;
    virtual void Update(CPlayer* pObject, double TimeDelta = 0.0) = 0;
};

