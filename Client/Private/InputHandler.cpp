#include "stdafx.h"
#include "InputHandler.h"
#include "GameInstance.h"
#include "RotationCmd.h"
#include "MoveCmd.h"
#include "PlayerInfo.h"

IMPLEMENT_SINGLETON(CInputHandler)

CInputHandler::CInputHandler()
{
    m_CommandKeyMap.emplace(CONTROL_KEY::ROTATION, new CRotationCmd());
    m_CommandKeyMap.emplace(CONTROL_KEY::LEFT, new CMoveCmd(CONTROL_KEY::LEFT));
    m_CommandKeyMap.emplace(CONTROL_KEY::RIGHT, new CMoveCmd(CONTROL_KEY::RIGHT));
}

CInputHandler::~CInputHandler()
{
}

// 이중 포인터가 아니면 이 함수에 들어온 OutObject 포인터는 사본이라 원본 포인터엔 적용이 안됨
CCommand* CInputHandler::Key_Input() const
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CCommand* pCommand = { nullptr };
    /** @qurious - 이거 안되는 이유 진짜 모르겠음.. 하 . */
    //unordered_map<CONTROL_KEY, CCommand*>::iterator iter = m_CommandKeyMap.begin();
    auto iter = m_CommandKeyMap.end();
    CSpriteObject* pPlayer = CPlayerInfo::GetInstance()->Get_EquippedSkul();

    if (pGameInstance->Get_KeyDown(DIK_LEFT))
    {
        iter = m_CommandKeyMap.find(CONTROL_KEY::LEFT);
        pPlayer->Input_Handler(STATE_TYPE::WALK);
    }

    if (pGameInstance->Get_KeyDown(DIK_RIGHT))
    {
        iter = m_CommandKeyMap.find(CONTROL_KEY::RIGHT);
        pPlayer->Input_Handler(STATE_TYPE::WALK);
    }

    if (pGameInstance->Get_KeyUp(DIK_LEFT)
        || pGameInstance->Get_KeyUp(DIK_RIGHT))
    {
        pPlayer->Input_Handler(STATE_TYPE::IDLE);
    }

    if (pGameInstance->Get_KeyDown(DIK_X))
    {
        pPlayer->Input_Handler(STATE_TYPE::ATK);
    }

    if (pGameInstance->Get_KeyDown(DIK_C))
    {
        iter = m_CommandKeyMap.find(CONTROL_KEY::JUMP);
    }

    if (pGameInstance->Get_KeyDown(DIK_R))
    {
        iter = m_CommandKeyMap.find(CONTROL_KEY::ROTATION);
    }

    if (iter != m_CommandKeyMap.end())
        pCommand = iter->second;

    /** @note - CGameObject*에 CPlayer*는 담을 수 있지만, CGameObject**에 CPlayer*의 주소(&)를 담을 수 없음 */

    Safe_Release(pGameInstance);
    return pCommand;
}

void CInputHandler::Free()
{
    __super::Free();

    for (pair<const CONTROL_KEY, CCommand*> Pair : m_CommandKeyMap)
    {
        Safe_Delete(Pair.second);
    }
    m_CommandKeyMap.clear();

    return;
}
