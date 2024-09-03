#include "stdafx.h"
#include "WaterSkul.h"
#include "PlayerInfo.h"

CWaterSkul::CWaterSkul(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CPlayer(pDevice, pContext)
{
}

HRESULT CWaterSkul::Initialize_Prototype()
{
    return __super::Initialize_Prototype();
}

HRESULT CWaterSkul::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
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

    m_iUVTexNumX = 8;
    m_iUVTexNumY = 9;
    m_bIsAnimUV = true;

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

    m_iShaderPassIndex = (_uint)VTXTEX_PASS::UV_Anim;
    m_iCurrentAnim = (_uint)STATE_TYPE::IDLE;
    m_iTextureIndex = 0;

    m_iUVTexNumX = 8;
    m_iUVTexNumY = 9;
    m_bIsAnimUV = true;

    // @error - 장착한 플레이어만 뜨는데 이거 일단 고정으로 장착하게 해뒀으니 추후 수정해야함! //
    CPlayerInfo::GetInstance()->Set_EquippedSkul(this);
    Add_Animation();

    return S_OK;
}

_uint CWaterSkul::Tick(_double TimeDelta)
{    
    Play_Animation(m_iUVTextureIndex, TimeDelta);
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
    m_pAnimInfo = new ANIM_INFO[(_uint)STATE_TYPE::MOTION_END];

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

    m_iUVTextureIndex = m_pAnimInfo[m_iCurrentAnim].iStartIndex;
}

HRESULT CWaterSkul::Add_Components(void* pArg)
{
    if (FAILED(__super::Add_Components(pArg)))
        return E_FAIL;

    return S_OK;
}

HRESULT CWaterSkul::SetUp_ShaderResources()
{
    if (FAILED(__super::SetUp_ShaderResources()))
        return E_FAIL;

    _uint iUVIndexY = m_iUVTextureIndex / m_iUVTexNumX;
    /** @note - _uint가 있으면 int로 담기 전 계산 과정에서 이미 모두 int로 변환 후 계산해야함. (음수가 되면 엄청 큰 수처럼 이상수 나옴) */
    _uint iUVIndexX = max(0, (int)m_iUVTextureIndex - (int)(m_iUVTexNumX * iUVIndexY) - 1);

    // 0일 경우 -1을 하면 _uint라 이상한 값 나오기 때문에 체크 후 1 감소 (1감소해야 위치 맞음)
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

CSpriteObject* CWaterSkul::Clone(const tSpriteInfo& InSpriteInfo, void* pArg) const
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
