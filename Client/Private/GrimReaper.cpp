#include "stdafx.h"
#include "GrimReaper.h"

/* @note - 생성자에 부모 생성자도 초기화해야 하는 이유
* */
CGrimReaper::CGrimReaper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPlayer(pDevice, pContext)
{
}

HRESULT CGrimReaper::Initialize_Prototype()
{
    return __super::Initialize_Prototype();
}

HRESULT CGrimReaper::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
    if (FAILED(__super::Initialize(InSpriteInfo)))
    {
        return E_FAIL;
    }

    Add_Animation();

    CTransform::TRANSFORM_DESC tTransDesc;
    tTransDesc.SpeedPerSec = 20.f;
    m_pTransformCom->Set_TransformDesc(tTransDesc);

    m_eRenderGroup = CRenderer::RENDER_PRIORITY;
    m_iShaderPassIndex = (_uint)VTXTEX_PASS::UV_Anim;
    m_iCurrentAnim = (_uint)STATE_TYPE::IDLE;
    m_tSpriteInfo.iTextureIndex = 0;

    m_iUVTexNumX = 10;
    m_iUVTexNumY = 15;
    m_bIsAnimUV = true;

    return S_OK;
}

_uint CGrimReaper::Tick(_double TimeDelta)
{
    Play_Animation(m_iUVTextureIndex, TimeDelta);
    return CPlayer::Tick(TimeDelta);
}

_uint CGrimReaper::LateTick(_double TimeDelta)
{
    return CPlayer::LateTick(TimeDelta);
}

HRESULT CGrimReaper::Render()
{
    return CPlayer::Render();
}

void CGrimReaper::Add_Animation()
{
    m_pAnimInfo = new ANIM_INFO[(_uint)STATE_TYPE::MOTION_END];

    m_pAnimInfo[(_uint)STATE_TYPE::IDLE].iStartIndex = 1;
    m_pAnimInfo[(_uint)STATE_TYPE::IDLE].iEndIndex = 10;

    m_pAnimInfo[(_uint)STATE_TYPE::WALK].iStartIndex = 11;
    m_pAnimInfo[(_uint)STATE_TYPE::WALK].iEndIndex =20;

    m_pAnimInfo[(_uint)STATE_TYPE::ATK].iStartIndex = 21;
    m_pAnimInfo[(_uint)STATE_TYPE::ATK].iEndIndex = 30;

    m_pAnimInfo[(_uint)STATE_TYPE::SKILL1].iStartIndex = 31;
    m_pAnimInfo[(_uint)STATE_TYPE::SKILL1].iEndIndex = 40;

    m_pAnimInfo[(_uint)STATE_TYPE::JUMP_ATK].iStartIndex = 41;
    m_pAnimInfo[(_uint)STATE_TYPE::JUMP_ATK].iEndIndex = 50;

    m_pAnimInfo[(_uint)STATE_TYPE::JUMP].iStartIndex = 51;
    m_pAnimInfo[(_uint)STATE_TYPE::JUMP].iEndIndex = 60;

    m_pAnimInfo[(_uint)STATE_TYPE::FALL].iStartIndex = 61;
    m_pAnimInfo[(_uint)STATE_TYPE::FALL].iEndIndex = 70;

    m_pAnimInfo[(_uint)STATE_TYPE::DASH].iStartIndex = 71;
    m_pAnimInfo[(_uint)STATE_TYPE::DASH].iEndIndex = 80;

    m_iUVTextureIndex = m_pAnimInfo[m_iCurrentAnim].iStartIndex;
}

HRESULT CGrimReaper::Add_Components(void* pArg)
{
    m_pTextureTag = TEXT("Prototype_Component_Sprite_GrimReaperUV");

    if (FAILED(__super::Add_Components(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CGrimReaper::SetUp_ShaderResources()
{
    if (FAILED(__super::SetUp_ShaderResources()))
        return E_FAIL;

    _uint iUVIndexY = m_iUVTextureIndex / m_iUVTexNumX;
    _uint iUVIndexX = m_iUVTextureIndex - (m_iUVTexNumX * iUVIndexY) - 1;
    if (0 > iUVIndexX)
        iUVIndexX = 0;

    if (FAILED(m_pShaderCom->Set_RawValue("g_iUVIndexX", &iUVIndexX, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_iUVIndexY", &iUVIndexY, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_iUVTexNumX", &m_iUVTexNumX, sizeof(_uint))))
        return E_FAIL;

    if (FAILED(m_pShaderCom->Set_RawValue("g_iUVTexNumY", &m_iUVTexNumY, sizeof(_uint))))
        return E_FAIL;

    return S_OK;
}

CGrimReaper* CGrimReaper::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CGrimReaper* pInstance = new CGrimReaper(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created CGrimReaper");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CSpriteObject* CGrimReaper::Clone(const tSpriteInfo& InSpriteInfo, void* pArg) const
{
    CGrimReaper* pInstance = new CGrimReaper(*this);

    if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
    {
        MSG_BOX("Failed to Cloned CGrimReaper");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CGrimReaper::Free()
{
    __super::Free();
}
