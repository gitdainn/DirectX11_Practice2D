#include "stdafx.h"
#include "GrimReaper.h"
#include "Solider.h"

/* @note - 생성자에 부모 생성자도 초기화해야 하는 이유
* */
CSolider::CSolider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CEnemy(pDevice, pContext)
{
}

HRESULT CSolider::Initialize_Prototype()
{
    m_pNameTag = TEXT("병사");
    m_iShaderPassIndex = (_uint)VTXTEX_PASS::UV_Anim;
    m_eSpriteDirection = SPRITE_DIRECTION::RIGHT;
    m_bIsAnimUV = true;
    m_iUVTexNumX = 10;
    m_iUVTexNumY = 6;

    return __super::Initialize_Prototype();
}

HRESULT CSolider::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
    if (FAILED(__super::Initialize(InSpriteInfo)))
    {
        return E_FAIL;
    }

    CTransform::TRANSFORM_DESC tTransDesc;
    tTransDesc.SpeedPerSec = 20.f;
    m_pTransformCom->Set_TransformDesc(tTransDesc);

    _float2 vScale = m_pTextureCom->Get_OriginalTextureSize(m_iTextureIndex);
    vScale.x /= m_iUVTexNumX;
    vScale.y /= m_iUVTexNumY;
    m_pTransformCom->Set_Scaled(_float3(vScale.x * 1.5f, vScale.y * 1.5f, 0.f));

    Add_Animation();

    m_StateFunctionMap.emplace(ENEMY_STATE::IDLE, bind(&CSolider::Walk, this, 1));
    m_StateFunc = bind(&CSolider::Idle, this, 0.1f);

    return S_OK;
}

HRESULT CSolider::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
    {
        return E_FAIL;
    }

    CTransform::TRANSFORM_DESC tTransDesc;
    tTransDesc.SpeedPerSec = 20.f;
    m_pTransformCom->Set_TransformDesc(tTransDesc);

    _float2 vScale = m_pTextureCom->Get_OriginalTextureSize(m_iTextureIndex);
    vScale.x /= m_iUVTexNumX;
    vScale.y /= m_iUVTexNumY;
    m_pTransformCom->Set_Scaled(_float3(vScale.x * 1.5f, vScale.y * 1.5f, 0.f));

    Add_Animation();

    return S_OK;
}

_uint CSolider::Tick(_double TimeDelta)
{
    Play_Animation(TimeDelta, m_iUVTextureIndex, m_iAnimType);

    return __super::Tick(TimeDelta);
}

_uint CSolider::LateTick(_double TimeDelta)
{
    return __super::LateTick(TimeDelta);
}

HRESULT CSolider::Render()
{
    return __super::Render();
}

void CSolider::Add_Animation()
{
    m_iAnimTypeNum = (_uint)ENEMY_STATE::ENEMY_END;
    m_pAnimInfo = new ANIM_INFO[m_iAnimTypeNum];
    m_iAnimType = (_uint)ENEMY_STATE::IDLE;

    _uint iRow = { 0 };
    m_pAnimInfo[(_uint)ENEMY_STATE::IDLE].iStartIndex = 0;
    m_pAnimInfo[(_uint)ENEMY_STATE::IDLE].iEndIndex = 4;
    m_pAnimInfo[(_uint)ENEMY_STATE::IDLE].fAnimTime = 2.f;

    ++iRow;
    m_pAnimInfo[(_uint)ENEMY_STATE::WALK].iStartIndex = m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)ENEMY_STATE::WALK].iEndIndex = m_iUVTexNumX * iRow + 8;
    m_pAnimInfo[(_uint)ENEMY_STATE::WALK].fAnimTime = 2.f;

    ++iRow;
    m_pAnimInfo[(_uint)ENEMY_STATE::ATK1].iStartIndex = m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)ENEMY_STATE::ATK1].iEndIndex = m_iUVTexNumX * iRow + 6;
    m_pAnimInfo[(_uint)ENEMY_STATE::ATK1].fAnimTime = 1.f;

    ++iRow;
    m_pAnimInfo[(_uint)ENEMY_STATE::DAMAGED].iStartIndex = m_iUVTexNumX * iRow;
    m_pAnimInfo[(_uint)ENEMY_STATE::DAMAGED].iEndIndex = m_iUVTexNumX * iRow + 6;
    m_pAnimInfo[(_uint)ENEMY_STATE::DAMAGED].fAnimTime = 1.f;

    m_iUVTextureIndex = m_pAnimInfo[m_iAnimType].iStartIndex;
}

HRESULT CSolider::Add_Components(void* pArg)
{
    if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_Enemy_Solider"),
        TAG_TEXTURE, (CComponent**)&m_pTextureCom, nullptr)))
        return E_FAIL;

    return __super::Add_Components(pArg);
}

HRESULT CSolider::SetUp_ShaderResources()
{
    if (FAILED(__super::SetUp_ShaderResources()))
        return E_FAIL;

    return S_OK;
}

void CSolider::Idle(_double TimeDelta)
{
    _double IdleTime = { 2.f };
    
    if (IdleTime <= m_IdleTimeAcc)
    {
        Input_Handler(ENEMY_STATE::WALK);
        m_IdleTimeAcc = 0.0;
    }
    else
    {
        m_IdleTimeAcc += TimeDelta;
    }

    return;
}

void CSolider::Walk(_double TimeDelta)
{
    const _uint iSearchRoundsNum = { 2 };
    const _float fSearchDistance = { 10.f };
    const _float fSwitchDistance = { 3.f };
    const _float fPlayerAtkDistance = { 100.f };

    if (iSearchRoundsNum <= m_iSearchRoundsCount)
    {
        Input_Handler(ENEMY_STATE::IDLE);
        m_iSearchRoundsCount = 0;
    }

    // 이동 //
    if (m_eSpriteDirection == SPRITE_DIRECTION::LEFT)
    {
        m_pTransformCom->Go_Left(TimeDelta);
    }
    else if (m_eSpriteDirection == SPRITE_DIRECTION::RIGHT)
    {
        m_pTransformCom->Go_Right(TimeDelta);
    }
    else
    {
        MSG_BOX("CSolider - Walk() - ERROR");
    }

    // 플레이어와의 거리 측정 //
    _vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
    _vector fCurrentPlayerDistance = XMVector3Length(vPosition - Get_PlayerTransformCom()->Get_State(CTransform::STATE_POSITION));
    if (fPlayerAtkDistance >= XMVectorGetX(fCurrentPlayerDistance))
    {
        Input_Handler(ENEMY_STATE::ATK1);
        m_iSearchRoundsCount = 0;
    }

    // 라인 끝점과의 거리 측정 //
    _float3 vLineEndPoint = (m_eSpriteDirection == SPRITE_DIRECTION::LEFT ? m_LineEndPoints.first : m_LineEndPoints.second);
    _float fCurrentLineDistance = Get_DistanceX(vPosition, XMLoadFloat3(&vLineEndPoint));

    // 라인 끝에 부딪쳤거나 iSearchDistance 이상 움직였거나 Block에 부딪쳤으면 Switch
    if (fSwitchDistance >= fCurrentLineDistance)
    {
        ++m_iSearchRoundsCount;
        Switch_SpriteDirection();
    }

    return;
}

void CSolider::Attack(_double TimeDelta)
{
    if (m_bIsEndSprite)
        Input_Handler(ENEMY_STATE::IDLE);

    CCollider::COLLIDER_DESC tColliderDesc = m_pDefaultAtkColliderCom->Get_ColliderDesc();
    tColliderDesc.vOffset.y = 30.f;
    tColliderDesc.vOffset.x = (SPRITE_DIRECTION::LEFT == m_eSpriteDirection ? -20.f : 20.f);
    m_pDefaultAtkColliderCom->Set_ColliderDesc(tColliderDesc);
    Attach_Collider(m_LayerBitset, m_pDefaultAtkColliderCom);
}

void CSolider::Chase(_double TimeDelta)
{
}

void CSolider::Damaged(_double TimeDelta)
{
    if (m_bIsEndSprite)
        Input_Handler(ENEMY_STATE::IDLE);

    // 넉백 //
    //if (m_eSpriteDirection == SPRITE_DIRECTION::LEFT)
    //{
    //    m_pTransformCom->Go_Right(TimeDelta);
    //}
    //else if (m_eSpriteDirection == SPRITE_DIRECTION::RIGHT)
    //{
    //    m_pTransformCom->Go_Left(TimeDelta);
    //}
    //else
    //{
    //    MSG_BOX("CSolider - Walk() - ERROR");
    //}
}

CSolider* CSolider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    CSolider* pInstance = new CSolider(pDevice, pContext);

    if (FAILED(pInstance->Initialize_Prototype()))
    {
        MSG_BOX("Failed to Created CSolider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CSpriteObject* CSolider::Clone(const tSpriteInfo& InSpriteInfo, void* pArg) const
{
    CSolider* pInstance = new CSolider(*this);

    if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
    {
        MSG_BOX("Failed to Cloned CSolider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

CSpriteObject* CSolider::Clone(void* pArg) const
{
    CSolider* pInstance = new CSolider(*this);

    if (FAILED(pInstance->Initialize(pArg)))
    {
        MSG_BOX("Failed to Cloned CSolider");
        Safe_Release(pInstance);
    }

    return pInstance;
}

void CSolider::Free()
{
    __super::Free();
}
