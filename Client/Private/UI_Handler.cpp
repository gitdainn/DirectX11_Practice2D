#include "stdafx.h"
#include "UI_Handler.h"
#include "HealthBarUI.h"
#include "Widget.h"

IMPLEMENT_SINGLETON(CUI_Handler)

CUI_Handler::CUI_Handler()
{
}

CUI_Handler::~CUI_Handler()
{
}

HRESULT CUI_Handler::Initialize()
{
    Ready_PrototypeTag();

    return S_OK;
}

void CUI_Handler::Ready_PrototypeTag()
{
    PROTOTAG_TYPE   tPrototype;
    //ZeroMemory(&tPrototype, sizeof(PROTOTAG_TYPE)); // 구조체 안에 컨테이너(vector)가 있기에 ZeroMemory 금물!

    /* 리틀본 */
    //tPrototype.SkillTagVec.emplace_back(TEXT("../../ㅇㄹㄴㄹㅇ"));
    //tPrototype.SkillTagVec.emplace_back(TEXT("../../ㅇㄹㄴㄹㅇ"));
    //tPrototype.pProfileTag = TEXT("djfsl");
    //m_UIPrototypeMap.emplace(TEXT("리틀본"), tPrototype);

    /* 그림리퍼 */
    tPrototype.SkillTagVec.emplace_back(TEXT("GrimReaper_Harvest"));
    tPrototype.SkillTagVec.emplace_back(TEXT("GrimReaper_Passive"));
    tPrototype.pProfileTag = TEXT("GrimReaper");
    m_UIPrototypeMap.emplace(TEXT("그림리퍼"), tPrototype);
    tPrototype.SkillTagVec.clear();

    /* 워터스컬 */
    tPrototype.SkillTagVec.emplace_back(TEXT("AquaSkull_3_Passive"));
    tPrototype.SkillTagVec.emplace_back(TEXT("AquaSkull_3_SpiritsLake"));
    tPrototype.pProfileTag = TEXT("AquaSkull");
    m_UIPrototypeMap.emplace(TEXT("워터스컬"), tPrototype);
    tPrototype.SkillTagVec.clear();

    return;
}

void CUI_Handler::ChangeSkul(CPlayer* pPlayer, _bool bChangeMainSkul)
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

void CUI_Handler::OnOffRender(const _tchar* pLayer, const _bool bIsRender)
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

void CUI_Handler::Set_Hp(const _int iHp)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    if (nullptr == pGameInstance)
    {
        Safe_Release(pGameInstance);
        return;
    }
    Safe_AddRef(pGameInstance);

    CGameObject* pObject = pGameInstance->Get_GameObjectByName((_uint)LEVEL_LOGO, LAYER_UI, TEXT("PlayerHealthBar"));
    if (nullptr == pObject)
    {
        Safe_Release(pGameInstance);
        return;
    }

    CHealthBarUI* pHealthBar = dynamic_cast<CHealthBarUI*>(pObject);
    if (nullptr == pHealthBar)
    {
        Safe_Release(pGameInstance);
        return;
    }

    pHealthBar->Set_Hp(iHp);
    Safe_Release(pGameInstance);
}

void CUI_Handler::Set_Hp(CWidget* pWidgetCom, const _int iHp)
{
    if (nullptr == pWidgetCom)
        return;

    CGameObject* pObject = pWidgetCom->Get_WidgetByName(TEXT("EnemyHealthBar"));
    if (nullptr == pObject)
        return;

    CHealthBarUI* pHealthBar = dynamic_cast<CHealthBarUI*>(pObject);
    if (nullptr == pHealthBar)
        return;

    pHealthBar->Set_Hp(iHp);

    return;
}

void CUI_Handler::Free()
{
    __super::Free();
    
    /* @note - Pair 원본 참조(&)시 오류 뜨는 이유
    *  for(pair<const _tchar*, PROTOTAG_TYPE>& Pair : m_UIPrototypeMap) 오류!
    1. &원본 참조를 하지 않으면 복사본에 대한 clear()가 수행되므로 실제 원본에는 clear가 적용되지 않음.
    2. 아래 pair을 & 참조 시 오류가 뜸 
    -> key값인 const _tchar*은 상수라 수정X가 원칙인데 Pair에 대해 비참조 연산자 & 사용 시 전체 pair 객체의 모든 멤버에 대한 참조가 생김
    -> auto&를 이용하면 타입 추론을 통해 정확한 타입을 가져와 수정이 필요한 PROTOTAG_TYPE에만 참조를 적용시킴 
    */
    for(auto& Pair : m_UIPrototypeMap)
    {
       Pair.second.SkillTagVec.clear();
    }
    m_UIPrototypeMap.clear();

    return;
}
