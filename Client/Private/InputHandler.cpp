#include "stdafx.h"
#include "InputHandler.h"
#include "GameInstance.h"
#include "RotationCmd.h"
#include "Player_Manager.h"

IMPLEMENT_SINGLETON(CInputHandler)

CInputHandler::CInputHandler()
{
    m_CommandKeyMap.emplace(CONTROL_KEY::ROTATION, new CRotationCmd());
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
    CSpriteObject* pPlayer = CPlayer_Manager::GetInstance()->Get_MainSkul();
    if (nullptr == pPlayer)
    {
        Safe_Release(pGameInstance);
        return nullptr;
    }
#pragma region Move
    if (pGameInstance->Get_KeyStay(DIK_LEFT))
    {
        pPlayer->Input_Handler(STATE_TYPE::WALK, SPRITE_DIRECTION::LEFT);
    }

    if (pGameInstance->Get_KeyStay(DIK_RIGHT))
    {
        pPlayer->Input_Handler(STATE_TYPE::WALK, SPRITE_DIRECTION::RIGHT);
    }

    if (pGameInstance->Get_KeyUp(DIK_LEFT)
        && SPRITE_DIRECTION::LEFT == pPlayer->Get_SpriteDirection())
    {
        if (STATE_TYPE::WALK == pPlayer->Get_CurrentState())
            pPlayer->Input_Handler(STATE_TYPE::IDLE);
    }

    if (pGameInstance->Get_KeyUp(DIK_RIGHT)
        && SPRITE_DIRECTION::RIGHT == pPlayer->Get_SpriteDirection())
    {
        if (STATE_TYPE::WALK == pPlayer->Get_CurrentState())
            pPlayer->Input_Handler(STATE_TYPE::IDLE);
    }

    if (pGameInstance->Get_KeyDown(DIK_X))
    {
        pPlayer->Input_Handler(STATE_TYPE::DEFAULT_ATK);
    }

    if (pGameInstance->Get_KeyDown(DIK_Z))
    {
        pPlayer->Input_Handler(STATE_TYPE::DASH);
    }

    if (pGameInstance->Get_KeyDown(DIK_C))
    {
        pPlayer->Input_Handler(STATE_TYPE::JUMP);
    }
#pragma endregion

#pragma region InteractionKey
    if (pGameInstance->Get_KeyDown(DIK_R))
    {
        iter = m_CommandKeyMap.find(CONTROL_KEY::ROTATION);
    }

    if (iter != m_CommandKeyMap.end())
    {
        pCommand = iter->second;
        pCommand->Execute();
    }

    if (pGameInstance->Get_KeyDown(DIK_A))
    {
        CPlayer* pPPlayer = dynamic_cast<CPlayer*>(pPlayer);
        if (nullptr != pPPlayer)
        {
            pPPlayer->Execute_Skill(0);
        }
    }

    if (pGameInstance->Get_KeyDown(DIK_S))
    {
        CPlayer* pPPlayer = dynamic_cast<CPlayer*>(pPlayer);
        if (nullptr != pPPlayer)
        {
            pPPlayer->Execute_Skill(1);
        }
    }

    if (pGameInstance->Get_KeyDown(DIK_SPACE))
    {
        CPlayer_Manager::GetInstance()->Swap_Skul();
    }
#pragma endregion


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
