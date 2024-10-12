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
    void        Ready_UI();

public:
    /* 실제 Player_Manager에서 교체 후 호출되어야 함 (현재 장착 상태로 UI가 반영되기 때문) */
    void    ChangeSkul(CPlayer* pPlayer, _bool bChangeMainSkul)
    {
        if (nullptr == pPlayer)
        {
            MSG_BOX("CUI_Handler - SwapSkul() - NULL");
            return;
        }

        if (bChangeMainSkul)
        {
            // Main이면 ->GetObjectList(MAIN_LAYER); List->GetObj(pSkillTag); List->GetObj(pProfileTag);
            Change_SkillSprite(UI_MAINSKILL, pPlayer);
            Change_ProfileSprite(UI_MAINPROFILE, pPlayer);
        }
        else
        {
            Change_SkillSprite(UI_SUBSKILL, pPlayer);
            Change_ProfileSprite(UI_SUBPROFILE, pPlayer);
        }
    }

    void    OnOffSubRender(const _tchar* pLayer, const _bool bIsRender)
    {
        if (nullptr == pLayer)
            return;

        // -> Get_ObjectList(SUB_LAYER);
        // for(Obj) -> OnOffRender(false);
        auto Iter = m_UIMap.find(pLayer);
        if (m_UIMap.end() == Iter)
            return;

        list<CUI*> UIList = Iter->second;
        if (UIList.empty())
            return;

        for (CUI* pUI : UIList)
        {
            pUI->OnOffRender(bIsRender);
        }
    }

private:
    void        Change_SkillSprite(const _tchar* pUITag, CPlayer* pPlayer);
    void        Change_ProfileSprite(const _tchar* pUITag, CPlayer* pPlayer);

private:
    unordered_map<const _tchar*, list<CUI*>>        m_UIMap; // GameObjectList의 LAYER로 관리할 거라 필요없
    unordered_map<const _tchar*, PROTOTAG_TYPE>     m_UIPrototypeMap;
public:
    void Free() override;
};

