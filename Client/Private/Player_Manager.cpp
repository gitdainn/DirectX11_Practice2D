#include "stdafx.h"
#include "Player_Manager.h"
#include "UI_Handler.h"

IMPLEMENT_SINGLETON(CPlayer_Manager)

CPlayer_Manager::CPlayer_Manager()
    : m_pMainSkul(nullptr), m_pSubSkul(nullptr)
{
}

CPlayer_Manager::~CPlayer_Manager()
{
}

// @refactoring - UI 교체 지정을.. 플레이어 매니저에서 하는 것이 맞는 것인가 ㅜ
void CPlayer_Manager::Acquire_Skul(CPlayer* pPlayer)
{
    if (nullptr == pPlayer)
        return;

    CUI_Handler* pUIHandler = CUI_Handler::GetInstance();
    if (nullptr == pUIHandler)
        return;
    Safe_AddRef(pUIHandler);

    // 이미 스컬이 가득 찬 상태면 기존 스컬 부셔서 뼈 획득하고 교체
    if (m_bIsMaxSkulEquipped)
    {
        // 기존 스컬 부시기
        m_pMainSkul->Set_IsEquipped(false);
        // 새 스컬 장착
        m_pMainSkul = pPlayer;
        m_pMainSkul->Set_IsEquipped(true);
        pUIHandler->ChangeSkul(pPlayer, true);
    }
    // 메인 스컬이 없는 경우
    else if (nullptr == m_pMainSkul)
    {
        m_pMainSkul = pPlayer;
        m_pMainSkul->Set_IsEquipped(true);

        m_bIsMaxSkulEquipped = false;
        pUIHandler->ChangeSkul(pPlayer, true);
        pUIHandler->OnOffRender(LAYER::UI_MAINSKUL_STATE, true);
        pUIHandler->OnOffRender(LAYER::UI_SUBSKUL_STATE, false);

    }
    else if (nullptr == m_pSubSkul)
    {
        m_pSubSkul = pPlayer;

        m_bIsMaxSkulEquipped = true;
        pUIHandler->ChangeSkul(pPlayer, false);
        // 서브 렌더 켜기!
        pUIHandler->OnOffRender(LAYER::UI_SUBSKUL_STATE, true);
    }
    else
        MSG_BOX("CPlayer_Manager - Acquire_Skul - ERROR");

    Safe_Release(pUIHandler);
}

void CPlayer_Manager::Drop_Skul()
{
    if (!m_bIsMaxSkulEquipped)
        return;

    m_pMainSkul = m_pSubSkul;
    m_pSubSkul = nullptr;
    m_bIsMaxSkulEquipped = false;

    CUI_Handler* pUIHandler = CUI_Handler::GetInstance();
    if (nullptr == pUIHandler)
        return;
    Safe_AddRef(pUIHandler);

    m_pMainSkul->Set_IsEquipped(false);
    pUIHandler->ChangeSkul(m_pSubSkul, true);

    // 서브 스컬 렌더 아예 끄기!
    pUIHandler->OnOffRender(LAYER::UI_SUBSKUL_STATE, false);

    Safe_Release(pUIHandler);

}

void CPlayer_Manager::Swap_Skul()
{
    if (!m_bIsMaxSkulEquipped)
        return;

    CPlayer* pTemp = m_pMainSkul;
    m_pMainSkul = m_pSubSkul;
    m_pSubSkul = pTemp;

    CUI_Handler* pUIHandler = CUI_Handler::GetInstance();
    if (nullptr == pUIHandler)
        return;
    Safe_AddRef(pUIHandler);

    pUIHandler->ChangeSkul(m_pMainSkul, true);
    pUIHandler->ChangeSkul(m_pSubSkul, false);
    m_pMainSkul->Set_IsEquipped(true);
    m_pSubSkul->Set_IsEquipped(false);

    Safe_Release(pUIHandler);
}

void CPlayer_Manager::Free()
{
    __super::Free();
    return;
}
