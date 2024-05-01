#include "stdafx.h"
#include "RotationCmd.h"
#include "PlayerInfo.h"

void CRotationCmd::Execute(CGameObject* pObject)
{
    CPlayerInfo* pPlayerInfo = CPlayerInfo::GetInstance();
    Safe_AddRef(pPlayerInfo);

    pPlayerInfo->Rotation_Skul();

    Safe_Release(pPlayerInfo);
}

