#include "InstallObject.h"

CInstallObject::CInstallObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    :CSpriteObject(pDevice, pContext)
{
}

HRESULT CInstallObject::Initialize_Prototype()
{
    return S_OK;
}

HRESULT CInstallObject::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
    return S_OK;
}

_uint CInstallObject::Tick(_double TimeDelta)
{
    return _uint();
}

_uint CInstallObject::LateTick(_double TimeDelta)
{
    return _uint();
}

HRESULT CInstallObject::Render()
{
    return S_OK;
}

HRESULT CInstallObject::Add_Components(void* pArg)
{
    return S_OK;
}

HRESULT CInstallObject::SetUp_ShaderResources()
{
    return S_OK;
}

CInstallObject* CInstallObject::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CInstallObject* pInstance = new CInstallObject(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("CInstallObject - Create - FAILED");
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
}
