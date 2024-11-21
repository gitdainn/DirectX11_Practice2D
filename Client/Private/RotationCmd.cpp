#include "stdafx.h"
#include "RotationCmd.h"
#include "Player_Manager.h"

void CRotationCmd::Execute(CGameObject* pObject)
{
    CPlayer_Manager* pPlayerInfo = CPlayer_Manager::GetInstance();
    Safe_AddRef(pPlayerInfo);

    pPlayerInfo->Swap_Skul();

    Safe_Release(pPlayerInfo);
}

