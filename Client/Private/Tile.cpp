#include "stdafx.h"
#include "..\Public\Tile.h"

#include "GameInstance.h"

CTile::CTile(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSpriteObject(pDevice, pContext)
{

}

CTile::CTile(const CTile& rhs)
	: CSpriteObject(rhs)
{

}

HRESULT CTile::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CTile::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_PRIORITY;
	m_iOrder += 10;

	return S_OK;
}

HRESULT CTile::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	m_iShaderPassIndex = (_uint)VTXTEX_PASS::Default;
	m_eRenderGroup = CRenderer::RENDER_PRIORITY;
	m_iOrder += 10;

	return S_OK;
}

_uint CTile::Tick(_double TimeDelta)
{
	return __super::Tick(TimeDelta);
}

_uint CTile::LateTick(_double TimeDelta)
{
	return __super::LateTick(TimeDelta);
}

HRESULT CTile::Render()
{
	return __super::Render();
}

HRESULT CTile::Add_Components(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;

	return __super::Add_Components(pArg);
}

HRESULT CTile::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CTile::Add_Animation()
{
	return;
}

void CTile::OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget)
{
}

void CTile::OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget)
{
}

void CTile::OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget)
{
}

CTile* CTile::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CTile* pInstance = new CTile(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CTile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CTile::Clone(const tSpriteInfo& InSpriteInfo, void* pArg) const
{
	CTile* pInstance = new CTile(*this);

	if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
	{
		MSG_BOX("Failed to Cloned CTile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CTile::Clone(void* pArg) const
{
	CTile* pInstance = new CTile(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CTile");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CTile::Free()
{
	__super::Free();
}
