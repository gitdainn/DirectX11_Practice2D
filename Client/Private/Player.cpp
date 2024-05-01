#include "stdafx.h"
#include "Player.h"

// @qurious. 부모 생성자도 꼭 호출해줘야하는 이유가 궁금함. (매개변수로)
CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSpriteObject(pDevice, pContext)
	, m_pTextureTag(nullptr)
	, m_bIsEquipped(false)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CPlayer::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
	{
		return E_FAIL;
	}
	return S_OK;
}

_uint CPlayer::Tick(_double TimeDelta)
{
	if (!m_bIsEquipped)
		return _uint();

	return __super::Tick(TimeDelta);
}

_uint CPlayer::LateTick(_double TimeDelta)
{
	if (!m_bIsEquipped)
		return _uint();

	return __super::LateTick(TimeDelta);
}

HRESULT CPlayer::Render()
{
	return __super::Render();
}

HRESULT CPlayer::Add_Components(void* pArg)
{
	if (FAILED(__super::Add_Components(pArg)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Components(LEVEL_LOGO, m_pTextureTag,
		TAG_TEXTURE, (CComponent**)&m_pTextureCom, nullptr)))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Free()
{
	__super::Free();

	/** @qurious - 왜 _tchar*을 메모리 해제 하면 안되는가? */
	//Safe_Delete_Array(m_pTextureTag);
	Safe_Delete_Array(m_pAnimInfo);
}
