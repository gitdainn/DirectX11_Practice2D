#include "stdafx.h"
#include "PlayerInfo.h"

IMPLEMENT_SINGLETON(CPlayerInfo)

CPlayerInfo::CPlayerInfo()
    : m_pEquippedSkul(nullptr), m_pHoldingSkul(nullptr)
{
}

CPlayerInfo::~CPlayerInfo()
{
}

void CPlayerInfo::Free()
{
    __super::Free();
    return;
}
