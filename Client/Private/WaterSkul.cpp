#include "stdafx.h"
#include "WaterSkul.h"
#include "Player_Manager.h"

CWaterSkul::CWaterSkul(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPlayer(pDevice, pContext)
{
}

HRESULT CWaterSkul::Initialize_Prototype()
{
    m_pObjectID = TEXT("P003");
    m_pNameTag = new _tchar[MAX_PATH]{ TEXT("워터스컬") };

    m_iShaderPassIndex = (_uint)VTXTEX_PASS::UV_Anim;
    m_iAnimType = (_uint)STATE_TYPE::IDLE;
    m_iTextureIndex = 0;

    m_bIsAnimUV = true;
    m_iUVTexNumX = 10;
    m_iUVTexNumY = 9;

    return __super::Initialize_Prototype();
}

HRESULT CWaterSkul::Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg)
{
    if (FAILED(__super::Initialize(InSpriteInfo)))
    {
        return E_FAIL;
    }

    CTransform::TRANSFORM_DESC tTransDesc;
    tTransDesc.SpeedPerSec = 20.f;
    m_pTransformCom->Set_TransformDesc(tTransDesc);

    Add_Animation();

    return S_OK;
}

HRESULT CWaterSkul::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
    {
        return E_FAIL;
    }

    CTransform::TRANSFORM_DESC tTransDesc;
    tTransDesc.SpeedPerSec = 20.f;
    m_pTransformCom->Set_TransformDesc(tTransDesc);

    Add_Animation();

    //CPlayer_Manager::GetInstance()->Acquire_Skul(this);

    return S_OK;
}

_uint CWaterSkul::Tick(_double TimeDelta)
{    
    Play_Animation(TimeDelta, m_iUVTextureIndex, m_iAnimType);

    if (STATE_TYPE::SWAP == m_eCurrentState)
        SwapSkill_Deluge(TimeDelta);

    if (STATE_TYPE::ATK1 == m_eCurrentState)
        Add_DefaultAtkCollider(m_pAnimInfo[(_uint)STATE_TYPE::ATK1].iStartIndex + 4);
    if (STATE_TYPE::DEFAULT_ATK == m_eCurrentState)
        Add_DefaultAtkCollider(m_pAnimInfo[(_uint)STATE_TYPE::DEFAULT_ATK].iStartIndex + 4);

    return CPlayer::Tick(TimeDelta);
}

_uint CWaterSkul::LateTick(_double TimeDelta)
{
    return CPlayer::LateTick(TimeDelta);
}

HRESULT CWaterSkul::Render()
{
    return CPlayer::Render();
}

void CWaterSkul::Add_Animation()
{
    m_iAnimTypeNum = (_uint)STATE_TYPE::MOTION_END;
    m_pAnimInfo = new ANIM_INFO[m_iAnimTypeNum];

    int iRow = { 0 };
    m_pAnimInfo[(_uint)STATE_TYPE::IDLE].iStartIndex = 0;
    m_pAnimInfo[(_uint)STATE_TYPE::IDLE].iEndIndex = 5;
    m_pAnimInfo[(_uint)STATE_TYPE::IDLE].fAnimTime = 1.f;


    ++iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::WALK].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::WALK].iEndIndex = 5 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::WALK].fAnimTime = 1.f ;

    ++iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::ATK1].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::ATK1].iEndIndex = 6 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::ATK1].fAnimTime = 0.5f;

    ++iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::ATK2].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::ATK2].iEndIndex = 7 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::ATK2].fAnimTime = 0.6f ;

    ++iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::SKILL1].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::SKILL1].iEndIndex = 4 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::SKILL1].fAnimTime = 0.6f;

    ++iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::SKILL2].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::SKILL2].iEndIndex = 5 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::SKILL2].fAnimTime = 0.6f;

    m_pAnimInfo[(_uint)STATE_TYPE::SWAP].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::SWAP].iEndIndex = 5 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::SWAP].fAnimTime = 1.f;

    ++iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::JUMP].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::JUMP].iEndIndex = 2 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::JUMP].fAnimTime = 1.f;

    ++iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::FALL].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::FALL].iEndIndex = 3 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::FALL].fAnimTime = 1.f;

    ++iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::DASH].iStartIndex = 0 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::DASH].iEndIndex = 2 + m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)STATE_TYPE::DASH].fAnimTime = 0.3f;
    m_pAnimInfo[(_uint)STATE_TYPE::DASH].fDelayTimeMap.emplace( 1, 2.2f );

    m_iUVTextureIndex = m_pAnimInfo[m_iAnimType].iStartIndex;
}

void CWaterSkul::SwapSkill_Deluge(_double TimeDelta)
{
    if (m_pAnimInfo[(_uint)STATE_TYPE::SWAP].fAnimTime <= m_SwapTimeAcc)
    {
        m_SwapTimeAcc = 0.0;
        Input_Handler(STATE_TYPE::IDLE, m_eSpriteDirection);
    }
    m_SwapTimeAcc += TimeDelta;
}


HRESULT CWaterSkul::Add_Components(void* pArg)
{
    if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_WaterSkulUV"),
        TAG_TEXTURE, (CComponent**)&m_pTextureCom, nullptr)))
        return E_FAIL;

    return __super::Add_Components(pArg);
}

HRESULT CWaterSkul::SetUp_ShaderResources()
{
    if (FAILED(__super::SetUp_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

CWaterSkul* CWaterSkul::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CWaterSkul* pInstance = new CWaterSkul(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created CWaterSkul");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CSpriteObject* CWaterSkul::Clone(const SPRITE_INFO& InSpriteInfo, void* pArg) const
{
    CWaterSkul* pInstance = new CWaterSkul(*this);

    if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
    {
        MSG_BOX("Failed to Cloned CWaterSkul");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CSpriteObject* CWaterSkul::Clone(void* pArg) const
{
    CWaterSkul* pInstance = new CWaterSkul(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned CWaterSkul");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CWaterSkul::Free()
{
    __super::Free();
}
