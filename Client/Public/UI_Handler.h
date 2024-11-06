#pragma once
#include "Base.h"
#include "UI.h"
#include "Player_Manager.h"

BEGIN(Engine)
class CWidget;
END

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
    void    ChangeSkul(CPlayer* pPlayer, _bool bChangeMainSkul);

    void    OnOffRender(const _tchar* pLayer, const _bool bIsRender);

    void    Set_Hp(const _int iHp);
    void    Set_Hp(CWidget* pWidgetCom, const _int iHp);

private:
    void        Change_SkillSprite(const _tchar* pUITag, CPlayer* pPlayer);
    void        Change_ProfileSprite(const _tchar* pUITag, CPlayer* pPlayer);

private:
    unordered_map<const _tchar*, PROTOTAG_TYPE>     m_UIPrototypeMap;
public:
    void Free() override;
};

