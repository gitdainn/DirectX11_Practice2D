#include "stdafx.h"
#include "InputHandler.h"
#include "GameInstance.h"
#include "RotationCmd.h"

IMPLEMENT_SINGLETON(CInputHandler)

CInputHandler::CInputHandler()
{
    m_CommandKeyMap.emplace(CONTROL_KEY::ROTATION, new CRotationCmd());
}

CInputHandler::~CInputHandler()
{
}

CCommand* CInputHandler::Key_Input() const
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    CCommand* pCommand = {nullptr};
    /** @qurious - 이거 안되는 이유 진짜 모르겠음.. 하 . */
    //unordered_map<CONTROL_KEY, CCommand*>::iterator iter = m_CommandKeyMap.begin();
    auto iter = m_CommandKeyMap.end();

    if (pGameInstance->Get_KeyDown(DIK_R))
    {
        iter = m_CommandKeyMap.find(CONTROL_KEY::ROTATION);
    }

    if (iter != m_CommandKeyMap.end())
        pCommand = iter->second;

    Safe_Release(pGameInstance);
    return pCommand;
}

void CInputHandler::Free()
{
    __super::Free();
    return;
}
