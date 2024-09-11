#include "stdafx.h"
#include "GrimReaper.h"
#include "PlayerInfo.h"

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

HRESULT CGrimReaper::Initialize(void* pArg)
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

    //CPlayerInfo::GetInstance()->Set_EquippedSkul(this);

    Add_Animation();

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

    int iRow = { 0 };
    m_pAnimInfo[(_uint)STATE_TYPE::IDLE].iStartIndex = 0;
    m_pAnimInfo[(_uint)STATE_TYPE::IDLE].iEndIndex = 9;
    m_pAnimInfo[(_uint)STATE_TYPE::IDLE].fAnimTime = 1.f ;

    ++iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::WALK].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::WALK].iEndIndex = 9 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::WALK].fAnimTime =  1.f ;

    ++iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::ATK1].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::ATK1].iEndIndex = 9 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::ATK1].fAnimTime = 0.5f ;

    ++iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::SKILL1].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::SKILL1].iEndIndex = 9 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::SKILL1].fAnimTime = 0.6f ;

    ++iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::ATK2].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::ATK2].iEndIndex = 8 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::ATK2].fAnimTime = 0.6f ;

    ++iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::JUMP].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::JUMP].iEndIndex = 6 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::JUMP].fAnimTime = 1.f ;

    ++iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::FALL].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::FALL].iEndIndex = 7 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::FALL].fAnimTime = 1.f ;

    ++iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::DASH].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::DASH].iEndIndex = 5 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::DASH].fAnimTime = 0.6f;

    m_iUVTextureIndex = m_pAnimInfo[m_iCurrentAnim].iStartIndex;
}

HRESULT CGrimReaper::Add_Components(void* pArg)
{
    if (FAILED(CGameObject::Add_Components(LEVEL_LOGO, TEXT("Prototype_Component_Sprite_GrimReaperUV"),
        TAG_TEXTURE, (CComponent**)&m_pTextureCom, nullptr)))
        return E_FAIL;

    return __super::Add_Components(pArg);
}

HRESULT CGrimReaper::SetUp_ShaderResources()
{
    if (FAILED(__super::SetUp_ShaderResources()))
        return E_FAIL;

    _uint iUVIndexY = m_iUVTextureIndex / m_iUVTexNumX;
    _uint iUVIndexX = max(0, (int)m_iUVTextureIndex - (int)(m_iUVTexNumX * iUVIndexY) - 1);

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

CSpriteObject* CGrimReaper::Clone(void* pArg) const
{
    CGrimReaper* pInstance = new CGrimReaper(*this);

    if (FAILED(pInstance->Initialize(pArg)))
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
