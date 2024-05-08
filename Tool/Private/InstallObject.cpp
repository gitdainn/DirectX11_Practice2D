#include "stdafx.h"
#include "InstallObject.h"

/* @note - 생성자에 부모 생성자도 초기화해야 하는 이유
* */
CInstallObject::CInstallObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CSpriteObject(pDevice, pContext)
{
}

HRESULT CInstallObject::Initialize_Prototype()
{
    return __super::Initialize_Prototype();
}

HRESULT CInstallObject::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
    if (FAILED(__super::Initialize(InSpriteInfo)))
    {
        return E_FAIL;
    }

    CTransform::TRANSFORM_DESC tTransDesc;
    tTransDesc.SpeedPerSec = 20.f;
    m_pTransformCom->Set_TransformDesc(tTransDesc);

    m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
    m_iCurrentAnim = (_uint)STATE_TYPE::IDLE;

    m_eRenderGroup = CRenderer::RENDER_PRIORITY;
    m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;

    _float2 fOriginalSize = m_pTextureCom->Get_OriginalTextureSize(m_tSpriteInfo.iTextureIndex);
    m_pTransformCom->Set_Scaled(_float3(fOriginalSize.x, fOriginalSize.y, 1.f));

    return S_OK;
}

_uint CInstallObject::Tick(_double TimeDelta)
{
    //Play_Animation(m_tSpriteInfo.iTextureIndex, TimeDelta);

    return __super::Tick(TimeDelta);
}

_uint CInstallObject::LateTick(_double TimeDelta)
{
    return __super::LateTick(TimeDelta);
}

HRESULT CInstallObject::Render()
{
    return __super::Render();
}


HRESULT CInstallObject::Add_Components(void* pArg)
{
    m_pTextureTag = TEXT("Prototype_Component_Sprite_Tile");

    if (FAILED(__super::Add_Components(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CInstallObject::SetUp_ShaderResources()
{
    if (FAILED(__super::SetUp_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

CInstallObject* CInstallObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInstallObject* pInstance = new CInstallObject(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created CInstallObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CSpriteObject* CInstallObject::Clone(const tSpriteInfo& InSpriteInfo, void* pArg) const
{
    CInstallObject* pInstance = new CInstallObject(*this);

    if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
    {
        MSG_BOX("Failed to Cloned CInstallObject");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CInstallObject::Free()
{
    __super::Free();
}
