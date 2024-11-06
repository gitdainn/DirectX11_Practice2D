#include "stdafx.h"
#include "HealthBarWidget.h"
#include "UI.h"

CHealthBarWidget::CHealthBarWidget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CWidget(pDevice, pContext)
{
}

CHealthBarWidget::CHealthBarWidget(const CHealthBarWidget& rhs)
    :CWidget(rhs)
{
}

HRESULT CHealthBarWidget::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CHealthBarWidget::Initialize(void* pArg)
{
    if (nullptr == pArg)
        return E_FAIL;

    CWidget::WIDGET_ARGUMENT tWidgetArgument;
    memcpy(&tWidgetArgument, pArg, sizeof(WIDGET_ARGUMENT));

    m_pOwner = tWidgetArgument.pOwner;
    if (nullptr == m_pOwner)
        return E_FAIL;

    if (!lstrcmp(tWidgetArgument.pFilePath, TEXT("HealthBarW")))
    {
        Attach_HealthBar(m_pOwner);
    }

    return S_OK;
}

HRESULT CHealthBarWidget::Attach_HealthBar(CGameObject* pObject)
{
    /** @error Initialize 단계에선 부모가 정의되지 않은 상태라 부모 위치/크기를 받아올 수 없으므로 LateInitialize를 만들어야할 듯함. */
    if (nullptr == pObject)
        return E_FAIL;

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    if (nullptr == pGameInstance)
        return E_FAIL;
    Safe_AddRef(pGameInstance);

    SPRITE_INFO tSpriteInfo;
    tSpriteInfo.pTextureComTag = TEXT("Prototype_Component_Sprite_UI_HealthBar");

    tSpriteInfo.fSize = _float2(80.f, 10.f);
    if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_DisplayUI"), (_uint)LEVEL_LOGO, LAYER_UI, tSpriteInfo)))
    {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }
    CUI* pUI = dynamic_cast<CUI*>(pGameInstance->Get_LastObject((_uint)LEVEL_LOGO, LAYER_UI));
    if (nullptr != pUI)
    {
        pUI->Set_SpriteFileName(TEXT("EnemyHealthBar_Back"));
        pUI->Set_ShaderPass((_uint)eVTXTEX_PASS::Wrap_X);
        pUI->Set_Owner(pObject);
        pUI->Set_Order(2);
        Add_Widget(pUI);
    }

    tSpriteInfo.fSize = _float2(80.f, 5.f);
    if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_HealthBarUI"), (_uint)LEVEL_LOGO, LAYER_UI, tSpriteInfo)))
    {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }
    pUI = dynamic_cast<CUI*>(pGameInstance->Get_LastObject((_uint)LEVEL_LOGO, LAYER_UI));
    if (nullptr != pUI)
    {
        pUI->Set_SpriteFileName(TEXT("EnemyHealthBar"));
        pUI->Set_NameTag(TEXT("EnemyHealthBar"));
        pUI->Set_Owner(pObject);
        pUI->Set_Order(2);
        Add_Widget(pUI);
    }

    Safe_Release(pGameInstance);
    return S_OK;
}

CHealthBarWidget* CHealthBarWidget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CHealthBarWidget* pInstance = new CHealthBarWidget(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created : CHealthBarWidget");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CComponent* CHealthBarWidget::Clone(void* pArg)
{
    CHealthBarWidget* pInstance = new CHealthBarWidget(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned : CHealthBarWidget");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CHealthBarWidget::Free()
{
    __super::Free();
}
