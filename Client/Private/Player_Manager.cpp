#include "stdafx.h"
#include "Player_Manager.h"

IMPLEMENT_SINGLETON(CPlayer_Manager)

CPlayer_Manager::CPlayer_Manager()
    : m_pEquippedSkul(nullptr), m_pHoldingSkul(nullptr)
{
}

CPlayer_Manager::~CPlayer_Manager()
{
}

void CPlayer_Manager::Free()
{
    __super::Free();
    return;
}
