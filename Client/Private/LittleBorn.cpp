#include "stdafx.h"
#include "LittleBorn.h"

/* @note - 생성자에 부모 생성자도 초기화해야 하는 이유
* */
CLittleBorn::CLittleBorn(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPlayer(pDevice, pContext)
{
}

HRESULT CLittleBorn::Initialize_Prototype()
{
    return __super::Initialize_Prototype();
}

HRESULT CLittleBorn::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
    if (FAILED(__super::Initialize(InSpriteInfo)))
    {
        return E_FAIL;
    }

    Add_Animation();

    CTransform::TRANSFORM_DESC tTransDesc;
    tTransDesc.SpeedPerSec = 20.f;
    m_pTransformCom->Set_TransformDesc(tTransDesc);

    m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
    m_iCurrentAnim = (_uint)STATE_TYPE::IDLE;

    m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;

    return S_OK;
}

HRESULT CLittleBorn::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
    {
        return E_FAIL;
    }


    CTransform::TRANSFORM_DESC tTransDesc;
    tTransDesc.SpeedPerSec = 20.f;
    m_pTransformCom->Set_TransformDesc(tTransDesc);

    m_iShaderPassIndex = (_uint)VTXTEX_PASS::UV_Anim;
    m_iCurrentAnim = (_uint)STATE_TYPE::IDLE;
    m_iTextureIndex = 0;

    m_iUVTexNumX = 10;
    m_iUVTexNumY = 15;
    m_bIsAnimUV = true;

    Add_Animation();

    return S_OK;
}

_uint CLittleBorn::Tick(_double TimeDelta)
{
    Play_Animation(m_iTextureIndex, TimeDelta);

    return __super::Tick(TimeDelta);
}

_uint CLittleBorn::LateTick(_double TimeDelta)
{
    return __super::LateTick(TimeDelta);
}

HRESULT CLittleBorn::Render()
{
    return __super::Render();
}

void CLittleBorn::Add_Animation()
{
    m_pAnimInfo = new ANIM_INFO[(_uint)STATE_TYPE::MOTION_END];

    m_pAnimInfo[(_uint)STATE_TYPE::IDLE].iStartIndex = 0;
    m_pAnimInfo[(_uint)STATE_TYPE::IDLE].iEndIndex = 47;

    m_pAnimInfo[(_uint)STATE_TYPE::DASH].iStartIndex = 0;
    m_pAnimInfo[(_uint)STATE_TYPE::DASH].iEndIndex = 47;

    return;
}

HRESULT CLittleBorn::Add_Components(void* pArg)
{
    if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_LittleBorn"),
        TAG_TEXTURE, (CComponent**)&m_pTextureCom, nullptr)))
        return E_FAIL;

    return __super::Add_Components(pArg);
}

HRESULT CLittleBorn::SetUp_ShaderResources()
{
    if (FAILED(__super::SetUp_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

CLittleBorn* CLittleBorn::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CLittleBorn* pInstance = new CLittleBorn(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created CLittleBorn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CSpriteObject* CLittleBorn::Clone(const tSpriteInfo& InSpriteInfo, void* pArg) const
{
    CLittleBorn* pInstance = new CLittleBorn(*this);

    if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
    {
        MSG_BOX("Failed to Cloned CLittleBorn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CSpriteObject* CLittleBorn::Clone(void* pArg) const
{
    CLittleBorn* pInstance = new CLittleBorn(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned CLittleBorn");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CLittleBorn::Free()
{
    __super::Free();
}
