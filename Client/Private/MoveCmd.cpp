#include "stdafx.h"
#include "MoveCmd.h"
#include "SpriteObject.h"

CMoveCmd::CMoveCmd(const CONTROL_KEY eMove)
    : m_eMoveInfo(eMove)
{
}

CMoveCmd::~CMoveCmd()
{
}

void CMoveCmd::Execute(CGameObject* pObject)
{
    CSpriteObject* pSpriteObject = dynamic_cast<CSpriteObject*>(pObject);

    if (nullptr != pObject)
    {
        pSpriteObject->Set_CurrentState(STATE_TYPE::WALK);
        pSpriteObject->Change_AnimType((_uint)STATE_TYPE::WALK);
    }
}
