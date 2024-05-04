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
        pSpriteObject->Change_Sprite(STATE_TYPE::WALK);
    }
}
