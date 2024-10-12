#include "stdafx.h"
#include "UI_Handler.h"

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
    Ready_UI();

    return S_OK;
}

void CUI_Handler::Change_SkillSprite(const _tchar* pUITag, CPlayer* pPlayer)
{
    unordered_map<const _tchar*, list<CUI*>>::iterator SkillIter = m_UIMap.find(pUITag);

    if (m_UIMap.end() == SkillIter)
    {
        MSG_BOX("UI_Handler - SwapSkull() - END");
        return;
    }

    list<CUI*> SkillList = SkillIter->second;
    if (SkillList.empty())
        return;

    int iIndex = { 0 };
    for (CUI* pUI : SkillList)
    {
        ++iIndex;
        //if (iIndex <= pChangeTarget->GetSkillNum())
        //    pUI->Set_Render(false);

        auto ProtoTagIter = m_UIPrototypeMap.find(pPlayer->Get_NameTag());
        if (m_UIPrototypeMap.end() == ProtoTagIter)
        {
            MSG_BOX("UI_Handler - SwapSkul() - 해당 스컬에 대한 UI 정보가 없습니다.");
            return;
        }

        PROTOTAG_TYPE tProtoTag = ProtoTagIter->second;
        if (iIndex >= tProtoTag.SkillTagVec.size() - 1)
        {
            MSG_BOX("UI_Handler - SwapSkul() - Vector OverStackFlow");
            return;
        }
        //pUI->Change_TextureComponent(tProtoTag.SkillTagVec[iIndex]);
        pUI->Set_TextureTag(tProtoTag.SkillTagVec[iIndex]);
    }
}

void CUI_Handler::Change_ProfileSprite(const _tchar* pUITag, CPlayer* pPlayer)
{
    unordered_map<const _tchar*, list<CUI*>>::iterator SkillIter = m_UIMap.find(pUITag);

    if (m_UIMap.end() == SkillIter)
    {
        MSG_BOX("UI_Handler - SwapSkull() - END");
        return;
    }

    // Add_GameObject는 미리 MaxSkillNum인 3개까지 만들어두기.!
    list<CUI*> SkillList = SkillIter->second;
    if (SkillList.empty())
        return;

    int iIndex = { 0 };
    for (CUI* pUI : SkillList)
    {
        ++iIndex;
        //if (iIndex <= pChangeTarget->GetSkillNum())
        //    pUI->Set_Render(false);

        auto ProtoTagIter = m_UIPrototypeMap.find(pPlayer->Get_NameTag());
        if (m_UIPrototypeMap.end() == ProtoTagIter)
        {
            MSG_BOX("UI_Handler - SwapSkul() - 해당 스컬에 대한 UI 정보가 없습니다.");
            return;
        }

        PROTOTAG_TYPE tProtoTag = ProtoTagIter->second;
        pUI->Set_TextureTag(tProtoTag.pProfileTag);
        //pUI->Change_TextureComponent(tProtoTag.pProfileTag);
    }
}

void CUI_Handler::Ready_PrototypeTag()
{
    PROTOTAG_TYPE   tPrototype;
    ZeroMemory(&tPrototype, sizeof(PROTOTAG_TYPE));

    /* 리틀본 */
    tPrototype.SkillTagVec.emplace_back(TEXT("../../ㅇㄹㄴㄹㅇ"));
    tPrototype.SkillTagVec.emplace_back(TEXT("../../ㅇㄹㄴㄹㅇ"));
    tPrototype.pProfileTag = TEXT("djfsl");
    m_UIPrototypeMap.emplace(TEXT("리틀본"), tPrototype);

    /* 그림리퍼 */
    tPrototype.SkillTagVec.emplace_back(TEXT("GrimReaper_Harvest"));
    tPrototype.SkillTagVec.emplace_back(TEXT("GrimReaper_Passive"));
    tPrototype.pProfileTag = TEXT("GrimReaper");
    m_UIPrototypeMap.emplace(TEXT("그림리퍼"), tPrototype);
    ZeroMemory(&tPrototype, sizeof(PROTOTAG_TYPE));

    /* 워터스컬 */
    tPrototype.SkillTagVec.emplace_back(TEXT("AquaSkull_3_Passive"));
    tPrototype.SkillTagVec.emplace_back(TEXT("AquaSkull_3_SpiritsLake"));
    tPrototype.pProfileTag = TEXT("AquaSkull");
    m_UIPrototypeMap.emplace(TEXT("워터스컬"), tPrototype);

    return;
}

void CUI_Handler::Ready_UI()
{
    //// 스킬 UI 생성되면 얘네는 addChild로 자식을 생성하고, 자식들에게 명령 내리는 구조
    //CUI* pUI = new CSkillUI();
    //list<CUI*>  UIList;
    //UIList.emplace_back(pUI);

    //m_UIMap.emplace(UI_MAINSKILL, UIList);
    //
    //UIList.clear();
    //UIList.emplace_back(pUI);
    //pUI = new CSkillUI();
    //m_UIMap.emplace(UI_SUBSKILL, pUI);

    //pUI = new CBasicUI();
    //m_UIMap.emplace(UI_MAINPROFILE, pUI);

    //pUI = new CBasicUI();
    //m_UIMap.emplace(UI_SUBPROFILE, pUI);
}

void CUI_Handler::Free()
{
    __super::Free();

    m_UIMap.clear();
    m_UIPrototypeMap.clear();

    return;
}
