#include "stdafx.h"
#include "InstallObject.h"

/* @note - 생성자에 부모 생성자도 초기화해야 하는 이유
* */
CInstallObject::CInstallObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CSpriteObject(pDevice, pContext)
{
}

CInstallObject::CInstallObject(const CInstallObject& rhs)
    : CSpriteObject(rhs)
{
    *this = rhs;
    /** @note - 디폴트 복사생성자
    * 1. 복사 생성자 미정의 시 단순 1대1 대입하는 기본 복사생성자 실행됨
    * 2. 포인터, 문자열의 경우 같은 문자열을 참조하는 문제 발생 (한 객체에서 delete[] 시 모든 객체가 다 삭제된 공간 참조)
    * 3. 복사 생성자를 하나라도 정의했다면 무조건 모든 멤버 변수 1대1 대입 등 직접 명시해서 적어야 함
    * */
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

    m_eRenderGroup = CRenderer::RENDER_PRIORITY;
    m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
    m_iAnimType = (_uint)STATE_TYPE::IDLE;

    m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;

    _float2 fOriginalSize = m_pTextureCom->Get_OriginalTextureSize(m_iTextureIndex);
    fOriginalSize.x *= m_tSpriteInfo.fSizeRatio.x;
    fOriginalSize.y *= m_tSpriteInfo.fSizeRatio.y;
    m_tSpriteInfo.fSize = fOriginalSize;
    m_pTransformCom->Set_Scaled(_float3(m_tSpriteInfo.fSize.x, m_tSpriteInfo.fSize.y, 1.f));

    return S_OK;
}

HRESULT CInstallObject::Initialize(void* pArg)
{
    if (FAILED(__super::Initialize(pArg)))
    {
        return E_FAIL;
    }

    CTransform::TRANSFORM_DESC tTransDesc;
    tTransDesc.SpeedPerSec = 20.f;
    m_pTransformCom->Set_TransformDesc(tTransDesc);

    return S_OK;
}

_uint CInstallObject::Tick(_double TimeDelta)
{
    //Play_Animation(m_tSpriteInfo.iTextureIndex, TimeDelta);
    //m_pTextureCom->Get_TexturePathVec();

    if (nullptr != m_pColliderCom)
    {
        CGameInstance* pGameInstance = CGameInstance::GetInstance();
        if (FAILED(pGameInstance->Attach_Collider(m_LayerBitset, m_pColliderCom)))
        {
            MSG_BOX("CInstallObject - Tick() - FAILED");
        }
    }

#pragma region 임시 코드
    if (!lstrcmp(TEXT("BackGround"), m_pClassName))
    {
        m_eRenderGroup = CRenderer::RENDER_PRIORITY;
        m_pTransformCom->Set_Scaled(_float3(g_iWinSizeX, g_iWinSizeY, 0.f));
        m_bIsScroll = false;
    }
    else
    {
        m_eRenderGroup = CRenderer::RENDER_NONBLEND;
    }



#pragma endregion

    return __super::Tick(TimeDelta);
}

_uint CInstallObject::LateTick(_double TimeDelta)
{
    return __super::LateTick(TimeDelta);
}

HRESULT CInstallObject::Render()
{
    return __super::Render();
    return S_OK;
}


HRESULT CInstallObject::Add_Components(void* pArg)
{
    /* For.Com_Shader */
    if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
        TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
        return E_FAIL;

    return __super::Add_Components(pArg);
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

CSpriteObject* CInstallObject::Clone(void* pArg) const
{
    CInstallObject* pInstance = new CInstallObject(*this);

    if (FAILED(pInstance->Initialize(pArg)))
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
