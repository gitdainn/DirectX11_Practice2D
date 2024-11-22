#include "stdafx.h"
#include "SkillGuillotine.h"
#include "Player_Manager.h"

CSkillGuillotine::CSkillGuillotine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSkill(pDevice, pContext)
{
}

HRESULT CSkillGuillotine::Initialize_Prototype()
{
	m_pNameTag = new _tchar[MAX_PATH]{ TEXT("길로틴") };
	m_bIsAnimUV = false;
	m_bIsSkillAvailable = true;
	m_iAnimType = 0;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;

	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CSkillGuillotine::Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
	{
		return E_FAIL;
	}

	Add_Animation();

	return S_OK;
}

HRESULT CSkillGuillotine::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	m_bIsAnimUV = false;

	return S_OK;
}

_uint CSkillGuillotine::Tick(_double TimeDelta)
{
	if (m_DelayTime < m_DelayTimeAcc)
	{
		m_bIsDelayFinished = true;
	}
	else
		m_DelayTimeAcc += TimeDelta;

	Play_Animation(TimeDelta, m_iTextureIndex);

	return __super::Tick(TimeDelta);
}

_uint CSkillGuillotine::LateTick(_double TimeDelta)
{
	if (nullptr != m_pColliderCom)
	{
		Attach_Collider(m_pLayerTag, m_pColliderCom);

	}

	return __super::LateTick(TimeDelta);
}

HRESULT CSkillGuillotine::Render()
{
	return __super::Render();
}

void CSkillGuillotine::Enter(CSpriteObject* pOwner)
{
	__super::Enter(pOwner);

	CTransform* pPlayerTransformCom = pOwner->Get_TransformCom();
	if (nullptr == pPlayerTransformCom)
		return;

	_vector vPlayerPosition = pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat2(&m_tSpriteInfo.fPosition, vPlayerPosition);

	if (FAILED(DefaultLineRider(vPlayerPosition)))
	{
		m_bIsSkillAvailable = true;
		return;
	}

	_float2 vScale = m_pTextureCom->Get_OriginalTextureSize(m_iTextureIndex);
	vScale.y += 5.f;
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, Adjust_PositionUp_Radius(vScale.y));

	CCollider::COLLIDER_DESC tColliderDesc = m_pColliderCom->Get_ColliderDesc();
	XMStoreFloat3(&tColliderDesc.vPosition, vPlayerPosition);
	m_pColliderCom->Set_ColliderDesc(tColliderDesc);

	m_iTextureIndex = m_pAnimInfo->iStartIndex;

	return;
}

HRESULT CSkillGuillotine::Awaken()
{
	return S_OK;
}

void CSkillGuillotine::Add_Animation()
{
	m_pAnimInfo = new ANIM_INFO;

	m_pAnimInfo->iStartIndex = 0;
	m_pAnimInfo->iEndIndex = 25;
	m_pAnimInfo->fAnimTime = m_LifeTime;
}

void CSkillGuillotine::End_Animation(_uint& iSpriteIndex)
{
	__super::End_Animation(iSpriteIndex);
	iSpriteIndex = m_pAnimInfo->iStartIndex;
}

HRESULT CSkillGuillotine::Add_Components(void* pArg)
{
	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_Guillotine"),
		TAG_TEXTURE, (CComponent**)&m_pTextureCom, nullptr)))
		return E_FAIL;

	return __super::Add_Components(pArg);
}

HRESULT CSkillGuillotine::SetUp_ShaderResources()
{
	if (FAILED(SetUp_Shader_Camera()))
		return E_FAIL;

	if (FAILED(SetUp_Shader_UVAnim()))
		return E_FAIL;

	return S_OK;
}

CSkillGuillotine* CSkillGuillotine::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkillGuillotine* pInstance = new CSkillGuillotine(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSkillGuillotine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CSkillGuillotine::Clone(const SPRITE_INFO& InSpriteInfo, void* pArg) const
{
	CSkillGuillotine* pInstance = new CSkillGuillotine(*this);

	if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
	{
		MSG_BOX("Failed to Cloned CSkillGuillotine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CSkillGuillotine::Clone(void* pArg) const
{
	CSkillGuillotine* pInstance = new CSkillGuillotine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSkillGuillotine");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkillGuillotine::Free()
{
	__super::Free();
}
