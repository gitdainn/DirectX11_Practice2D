#include "stdafx.h"
#include "SkillThrowSkul.h"
#include "Player_Manager.h"

CSkillThrowSkul::CSkillThrowSkul(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSkill(pDevice, pContext)
{
}

HRESULT CSkillThrowSkul::Initialize_Prototype()
{
	m_pNameTag = new _tchar[MAX_PATH]{ TEXT("명계의균열") };
	m_iUVTexNumX = 5;
	m_iUVTexNumY = 7;
	m_bIsAnimUV = true;
	m_iShaderPassIndex = (_uint)VTXTEX_PASS::UV_Anim;

	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CSkillThrowSkul::Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
	{
		return E_FAIL;
	}

	Add_Animation();

	m_bIsSkillAvailable = true;

	return S_OK;
}

HRESULT CSkillThrowSkul::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	m_bIsAnimUV = false;

	return S_OK;
}

_uint CSkillThrowSkul::Tick(_double TimeDelta)
{
	if (m_DelayTime < m_DelayTimeAcc)
	{
		m_bIsDelayFinished = true;
	}
	else
		m_DelayTimeAcc += TimeDelta;

	Play_Animation(TimeDelta, m_iUVTextureIndex);

	return __super::Tick(TimeDelta);

}

_uint CSkillThrowSkul::LateTick(_double TimeDelta)
{
	if (nullptr != m_pColliderCom)
	{
		Attach_Collider(m_pLayerTag, m_pColliderCom);

	}

	return __super::LateTick(TimeDelta);
}

HRESULT CSkillThrowSkul::Render()
{
	return __super::Render();
}

void CSkillThrowSkul::Enter(CSpriteObject* pOwner)
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

	CCollider::COLLIDER_DESC tColliderDesc = m_pColliderCom->Get_ColliderDesc();
	XMStoreFloat3(&tColliderDesc.vPosition, vPlayerPosition);
	m_pColliderCom->Set_ColliderDesc(tColliderDesc);

	m_iTextureIndex = (_uint)GATEOFNETHER_TYPE::LOOP;

	return;
}

HRESULT CSkillThrowSkul::Awaken()
{
	return S_OK;
}

void CSkillThrowSkul::Add_Animation()
{
	m_pAnimInfo = new ANIM_INFO;

	m_pAnimInfo->iStartIndex = 0;
	m_pAnimInfo->iEndIndex = 34;
	m_pAnimInfo->fAnimTime = 2;
}

HRESULT CSkillThrowSkul::Add_Components(void* pArg)
{
	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_GateOfNetherUV"),
		TAG_TEXTURE, (CComponent**)&m_pTextureCom, nullptr)))
		return E_FAIL;

	return __super::Add_Components(pArg);
}

HRESULT CSkillThrowSkul::SetUp_ShaderResources()
{
	if (FAILED(SetUp_ShaderDefault()))
		return E_FAIL;

	if (FAILED(SetUp_Shader_UVAnim()))
		return E_FAIL;

	return S_OK;
}

CSkillThrowSkul* CSkillThrowSkul::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkillThrowSkul* pInstance = new CSkillThrowSkul(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSkillThrowSkul");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CSkillThrowSkul::Clone(const SPRITE_INFO& InSpriteInfo, void* pArg) const
{
	CSkillThrowSkul* pInstance = new CSkillThrowSkul(*this);

	if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
	{
		MSG_BOX("Failed to Cloned CSkillThrowSkul");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CSkillThrowSkul::Clone(void* pArg) const
{
	CSkillThrowSkul* pInstance = new CSkillThrowSkul(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSkillThrowSkul");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkillThrowSkul::Free()
{
	__super::Free();
}
