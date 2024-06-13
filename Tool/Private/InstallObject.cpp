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
    m_pLayer = new char[MAX_PATH] {"LAYER_DEFAULT"};
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
    fOriginalSize.x *= m_tSpriteInfo.fSizeRatio.x;
    fOriginalSize.y *= m_tSpriteInfo.fSizeRatio.y;
    m_tSpriteInfo.fSize = fOriginalSize;
    m_pTransformCom->Set_Scaled(_float3(fOriginalSize.x, fOriginalSize.y, 1.f));

    return S_OK;
}

_uint CInstallObject::Tick(_double TimeDelta)
{
    //Play_Animation(m_tSpriteInfo.iTextureIndex, TimeDelta);
    m_pTextureCom->Get_TexturePathVec();
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
