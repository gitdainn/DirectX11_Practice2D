#pragma once
#include "Client_Defines.h"

#include "SpriteObject.h"

class CState
{
public:
    explicit CState() {}
    ~CState() {}

public:
    /** @note - 순수 가상 함수는 virtual 가상 함수에만 사용 가능 */
    virtual CState* Input_Handler(CSpriteObject* pObject, const STATE_TYPE Input) = 0;
    virtual void Enter(CSpriteObject* pObject) = 0;
    virtual void Update(CSpriteObject* pObject, double TimeDelta = 0.0) = 0;
};

