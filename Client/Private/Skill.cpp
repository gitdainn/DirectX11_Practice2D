#include "stdafx.h"
#include "Skill.h"

CSkill::CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSpriteObject(pDevice, pContext)
{
}

HRESULT CSkill::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CSkill::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
	{
		return E_FAIL;
	}

	m_bIsRender = false;
	return S_OK;
}

HRESULT CSkill::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}
	m_bIsRender = false;
	return S_OK;
}

_uint CSkill::Tick(_double TimeDelta)
{
	//if (!m_bIsCoolFinished)
	//{
	//	m_bIsCoolFinished = true;
	//}
	m_LifeTimeAcc += TimeDelta;

	if (m_LifeTime <= m_LifeTimeAcc)
	{
		m_LifeTimeAcc = 0.0;
		//m_bIsRender = false;
	}

	return __super::Tick(TimeDelta);

}

_uint CSkill::LateTick(_double TimeDelta)
{
	return __super::LateTick(TimeDelta);
}

HRESULT CSkill::Render()
{
	return __super::Render();
}

void CSkill::Enter(CSpriteObject* pOwner)
{
	m_iCurrentAnim = 0;
	m_iTextureIndex = m_pAnimInfo[m_iCurrentAnim].iStartIndex;
	m_AnimAcc = 0.0;
	m_bIsSkillAvailable = false;
	m_bIsEndSprite = false;

	m_iTextureIndex = 0;
	ResetTime();
}

HRESULT CSkill::Awaken()
{
	return S_OK;
}

void CSkill::Play_Animation(_uint& iSpriteIndex, _double TimeDelta)
{
	__super::Play_Animation(iSpriteIndex, TimeDelta);
}

HRESULT CSkill::Add_Components(void* pArg)
{
	if (FAILED(__super::Add_Components(pArg)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;

	/* For. Com_Collider */
	COMPONENT_INFO tComponentInfo;
	ZeroMemory(&tComponentInfo, sizeof(COMPONENT_INFO));
	tComponentInfo.fPosition = m_tSpriteInfo.fPosition;
	tComponentInfo.fSize = _float2(m_tSpriteInfo.fSize.x / m_iUVTexNumX, m_tSpriteInfo.fSize.y / m_iUVTexNumY);
	tComponentInfo.fOffset = _float2(0.f, 0.f);

	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TAG_COLL_AABB, (CComponent**)&m_pColliderCom, &tComponentInfo)))
	{
		MSG_BOX("CSpriteObject - Add_Components() - FAILED");
		return E_FAIL;
	}
	m_pColliderCom->Set_Owner(this);

	return S_OK;
}

HRESULT CSkill::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

HRESULT CSkill::Landing_Ground(const _vector& vPosition)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);

	_float fLandingY = { 0.f };
	if (!pGameInstance->IsCurrentLineOccupied(_float2(XMVectorGetX(vPosition), XMVectorGetY(vPosition)), fLandingY))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, fLandingY));
	Safe_Release(pGameInstance);

	return S_OK;
}

void CSkill::Free()
{
	__super::Free();
}
