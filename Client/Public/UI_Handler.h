#pragma once
#include "Base.h"
#include "UI.h"
#include "Player_Manager.h"



/** @qurious - 싱글톤에 final 붙이면 안된다? */
class CUI_Handler : public CBase
{
    DECLARE_SINGLETON(CUI_Handler)

public:
    explicit CUI_Handler();
    ~CUI_Handler();

private:
    struct PROTOTAG_TYPE
    {
        vector<const _tchar*> SkillTagVec;
        const _tchar*         pProfileTag;
    };

public:
    HRESULT     Initialize();

private:
    void        Ready_PrototypeTag();

public:
    /* 실제 Player_Manager에서 교체 후 호출되어야 함 (현재 장착 상태로 UI가 반영되기 때문) */
    void    ChangeSkul(CPlayer* pPlayer, _bool bChangeMainSkul)
    {
        if (nullptr == pPlayer)
        {
            MSG_BOX("CUI_Handler - SwapSkul() - NULL");
            return;
        }

        CGameInstance* pGameInstance = CGameInstance::GetInstance();
        if (nullptr == pGameInstance)
            return;
        Safe_AddRef(pGameInstance);

        // true이면 main 스컬을 교체
        if (bChangeMainSkul)
        {
            CGameObject* pObject = pGameInstance->Get_GameObjectByName((_uint)LEVEL_LOGO, LAYER_MAINSKUL_UI, TEXT("MainProfile"));
            if (nullptr == pObject)
            {
                Safe_Release(pGameInstance);
                return;
            }
            CUI* pUI = dynamic_cast<CUI*>(pObject);
            if (nullptr == pUI)
            {
                Safe_Release(pGameInstance);
                return;
            }


            auto CompareStr = [=](const pair<const _tchar*, PROTOTAG_TYPE>& Pair)->bool
                {
                    return !lstrcmp(Pair.first, pPlayer->Get_NameTag());
                };
            /* @note - find_if는 단일 인자를 받는 함수/함수객체만 가능함. - 요소를 조건자로 자동 전달하기 때문 */
            auto ChangePlayerIter = find_if(m_UIPrototypeMap.begin(), m_UIPrototypeMap.end(), CompareStr);
            if (m_UIPrototypeMap.end() == ChangePlayerIter)
            {
                Safe_Release(pGameInstance);
                return;
            }
            pUI->Set_SpriteFileName(ChangePlayerIter->second.pProfileTag);

            pObject = pGameInstance->Get_GameObjectByName((_uint)LEVEL_LOGO, LAYER_MAINSKUL_UI, TEXT("MainSkill1"));
            if (nullptr == pObject)
            {
                Safe_Release(pGameInstance);
                return;
            }
            pUI = dynamic_cast<CUI*>(pObject);
            if (nullptr == pUI)
            {
                Safe_Release(pGameInstance);
                return;
            }
            pUI->Set_SpriteFileName(ChangePlayerIter->second.SkillTagVec[0]);

            pObject = pGameInstance->Get_GameObjectByName((_uint)LEVEL_LOGO, LAYER_MAINSKUL_UI, TEXT("MainSkill2"));
            if (nullptr == pObject)
            {
                Safe_Release(pGameInstance);
                return;
            }
            pUI = dynamic_cast<CUI*>(pObject);
            if (nullptr == pUI)
            {
                Safe_Release(pGameInstance);
                return;
            }
            pUI->Set_SpriteFileName(ChangePlayerIter->second.SkillTagVec[1]);
            
        }
        else
        {
            CGameObject* pObject = pGameInstance->Get_GameObjectByName((_uint)LEVEL_LOGO, LAYER_SUBSKUL_UI, TEXT("SubProfile"));
            if (nullptr == pObject)
            {
                Safe_Release(pGameInstance);
                return;
            }
            CUI* pUI = dynamic_cast<CUI*>(pObject);
            if (nullptr == pUI)
            {
                Safe_Release(pGameInstance);
                return;
            }

            auto CompareStr = [=](const pair<const _tchar*, PROTOTAG_TYPE>& Pair)->bool
                {
                    return !lstrcmp(Pair.first, pPlayer->Get_NameTag());
                };
            /* @note - find_if는 단일 인자를 받는 함수/함수객체만 가능함. - 요소를 조건자로 자동 전달하기 때문 */
            auto ChangePlayerIter = find_if(m_UIPrototypeMap.begin(), m_UIPrototypeMap.end(), CompareStr);
            if (m_UIPrototypeMap.end() == ChangePlayerIter)
            {
                Safe_Release(pGameInstance);
                return;
            }
            pUI->Set_SpriteFileName(ChangePlayerIter->second.pProfileTag);

            pObject = pGameInstance->Get_GameObjectByName((_uint)LEVEL_LOGO, LAYER_SUBSKUL_UI, TEXT("SubSkill1"));
            if (nullptr == pObject)
            {
                Safe_Release(pGameInstance);
                return;
            }
            pUI = dynamic_cast<CUI*>(pObject);
            if (nullptr == pUI)
            {
                Safe_Release(pGameInstance);
                return;
            }
            pUI->Set_SpriteFileName(ChangePlayerIter->second.SkillTagVec[0]);

            pObject = pGameInstance->Get_GameObjectByName((_uint)LEVEL_LOGO, LAYER_SUBSKUL_UI, TEXT("SubSkill2"));
            if (nullptr == pObject)
            {
                Safe_Release(pGameInstance);
                return;
            }

            pUI = dynamic_cast<CUI*>(pObject);
            if (nullptr == pUI)
            {
                Safe_Release(pGameInstance);
                return;
            }
            pUI->Set_SpriteFileName(ChangePlayerIter->second.SkillTagVec[1]);
        }

        Safe_Release(pGameInstance);
        return;
    }

    void    OnOffRender(const _tchar* pLayer, const _bool bIsRender)
    {
        CGameInstance* pGameInstance = CGameInstance::GetInstance();
        if (nullptr == pGameInstance)
        {
            Safe_Release(pGameInstance);
            return;
        }
        Safe_AddRef(pGameInstance);

        list<CGameObject*>* pObjList = pGameInstance->Get_ObjectList((_uint)LEVEL_LOGO, pLayer);
        if (nullptr == pObjList)
        {
            Safe_Release(pGameInstance);
            return;
        }

        for (CGameObject* pObj : *pObjList)
        {
            if (nullptr == pObj)
                continue;
            pObj->OnOffRender(bIsRender);
        }

        Safe_Release(pGameInstance);
        return;
    }

private:
    void        Change_SkillSprite(const _tchar* pUITag, CPlayer* pPlayer);
    void        Change_ProfileSprite(const _tchar* pUITag, CPlayer* pPlayer);

private:
    unordered_map<const _tchar*, PROTOTAG_TYPE>     m_UIPrototypeMap;
public:
    void Free() override;
};

