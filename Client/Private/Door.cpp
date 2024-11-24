#include "stdafx.h"
#include "..\Public\Door.h"

#include "GameInstance.h"

CDoor::CDoor(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CAnimObject(pDevice, pContext)
{

}

CDoor::CDoor(const CDoor& rhs)
	: CAnimObject(rhs)
{

}

HRESULT CDoor::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Set_Layer(LAYER_ENVIRONMENT, false);

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::UV_Anim;
	m_iAnimType = (_uint)ACTIVATE_TYPE::ACTIVATE;
	m_eRenderGroup = CRenderer::RENDER_NONBLEND;

	m_iTextureIndex = 0;

	m_bIsAnimUV = true;
	m_iUVTexNumX = 8;
	m_iUVTexNumY = 3;
	m_bIsLoop = true;

	m_tSpriteInfo.pTextureComTag = TEXT("Prototype_Component_Sprite_Door");
	m_pSpriteFileName = TEXT("BlackMarketDoor");

	return S_OK;
}

HRESULT CDoor::Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
		return E_FAIL;

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vScaled = m_pTransformCom->Get_Scaled();
	if (FAILED(DefaultLineRider(vPosition, vScaled.y / 2.f)))
		return E_FAIL;

	Add_Animation();

	return S_OK;
}

HRESULT CDoor::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float3 vScaled = m_pTransformCom->Get_Scaled();
	if (FAILED(DefaultLineRider(vPosition, vScaled.y / 2.f)))
		return E_FAIL;
	
	Add_Animation();

	return S_OK;
}

_uint CDoor::Tick(_double TimeDelta)
{
	if((_uint)ACTIVATE_TYPE::ACTIVATE == m_iAnimType)
		Play_Animation(TimeDelta, m_iUVTextureIndex, m_iAnimType);

	return __super::Tick(TimeDelta);
}

_uint CDoor::LateTick(_double TimeDelta)
{
	if (nullptr != m_pColliderCom)
		Attach_Collider(m_pLayerTag, m_pColliderCom);

	return __super::LateTick(TimeDelta);
}

HRESULT CDoor::Render()
{
	return __super::Render();
}

HRESULT CDoor::Add_Components(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;

	/* For.Com_LineRider */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_LineRider"),
		TAG_LINERIDER, (CComponent**)&m_pLineRiderCom, &m_tSpriteInfo.fPosition)))
		return E_FAIL;

	return __super::Add_Components(pArg);
}

HRESULT CDoor::SetUp_ShaderResources()
{
	if (FAILED(SetUp_Shader_Camera()))
		return E_FAIL;

	if (FAILED(SetUp_Shader_UVAnim()))
		return E_FAIL;

	return S_OK;
}

void CDoor::Add_Animation()
{
	m_iAnimTypeNum = (_uint)ACTIVATE_TYPE::ACTIVATE_END;
	m_pAnimInfo = new ANIM_INFO[m_iAnimTypeNum];

	int iRow = { 0 };
	m_pAnimInfo[(_uint)ACTIVATE_TYPE::DEACTIVATE].iStartIndex = 0;
	m_pAnimInfo[(_uint)ACTIVATE_TYPE::DEACTIVATE].iEndIndex = 0;
	m_pAnimInfo[(_uint)ACTIVATE_TYPE::DEACTIVATE].fAnimTime = 1.f;

	++iRow;
	m_pAnimInfo[(_uint)ACTIVATE_TYPE::ACTIVATE].iStartIndex = 0 + m_iUVTexNumX * iRow;
	m_pAnimInfo[(_uint)ACTIVATE_TYPE::ACTIVATE].iEndIndex = 7 + m_iUVTexNumX * iRow;
	m_pAnimInfo[(_uint)ACTIVATE_TYPE::ACTIVATE].fAnimTime = 1.f;

	m_iUVTextureIndex = m_pAnimInfo[m_iAnimType].iStartIndex;

	return;
}

void CDoor::OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	// 만약 스테이지 깬 상태인데 Enter이면 ACTIVATE로 변경
}

void CDoor::OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	// ACTIVATE이고, 플레이어 있는데 입장버튼 누르면 스테이지 전환
	__super::OnCollisionStay(pTargetCollider, pTarget, pTargetLayer);
}

void CDoor::OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
}

CDoor* CDoor::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CDoor* pInstance = new CDoor(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CDoor::Clone(const SPRITE_INFO& InSpriteInfo, void* pArg) const
{
	CDoor* pInstance = new CDoor(*this);

	if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
	{
		MSG_BOX("Failed to Cloned CDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CDoor::Clone(void* pArg) const
{
	CDoor* pInstance = new CDoor(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CDoor");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CDoor::Free()
{
	__super::Free();
}
