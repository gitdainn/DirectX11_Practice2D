#include "EnemyHealthBarUI.h"

CEnemyHealthBarUI::CEnemyHealthBarUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CHealthBarUI(pDevice, pContext)
{
}

CEnemyHealthBarUI::CEnemyHealthBarUI(const CEnemyHealthBarUI& rhs)
	: CHealthBarUI(rhs)
{
}

HRESULT CEnemyHealthBarUI::Initialize_Prototype()
{
	return __super::Initialize_Prototype();
}

HRESULT CEnemyHealthBarUI::Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg)
{
	m_fOffset.y = 10.f;
	return __super::Initialize(InSpriteInfo, pArg);
}

HRESULT CEnemyHealthBarUI::Initialize(void* pArg)
{
	return __super::Initialize(pArg);
}

_uint CEnemyHealthBarUI::Tick(_double TimeDelta)
{
	if (nullptr == m_pOwner)
	{
		m_bIsDead = true;
	}
	
	Follow_Owner();

	return __super::Tick(TimeDelta);
}

_uint CEnemyHealthBarUI::LateTick(_double TimeDelta)
{
	return __super::LateTick(TimeDelta);
}

HRESULT CEnemyHealthBarUI::Render()
{
	return __super::Render();
}

HRESULT CEnemyHealthBarUI::Add_Components(void* pArg)
{
	return __super::Add_Components(pArg);
}

HRESULT CEnemyHealthBarUI::SetUp_ShaderResources()
{
	return __super::SetUp_ShaderResources();
}

void CEnemyHealthBarUI::Follow_Owner() const
{
	if (nullptr == m_pOwner)
		return;

	_vector vOwnerPos = m_pOwner->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vOwnerPos, XMVectorGetY(vOwnerPos) + m_fOffset.y));
}

CEnemyHealthBarUI* CEnemyHealthBarUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CEnemyHealthBarUI* pInstance = new CEnemyHealthBarUI(pDevice, pContext);
	if (pInstance)
	{
		if (FAILED(pInstance->Initialize_Prototype()))
		{
			MSG_BOX("CEnemyHealthBarUI - Create() - FAILED");
			Safe_Release(pInstance);
		}
	}

	return pInstance;
}

CSpriteObject* CEnemyHealthBarUI::Clone(const SPRITE_INFO& InSpriteInfo, void* pArg) const
{
	CEnemyHealthBarUI* pInstance = new CEnemyHealthBarUI(*this);

	if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
	{
		MSG_BOX("Failed to Cloned CEnemyHealthBarUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CEnemyHealthBarUI::Clone(void* pArg) const
{
	CEnemyHealthBarUI* pInstance = new CEnemyHealthBarUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CEnemyHealthBarUI");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CEnemyHealthBarUI::Free()
{
	__super::Free();
}
