#include "stdafx.h"
#include "Player.h"
#include "PlayerIdle.h"
#include "PlayerJump.h"


// @qurious. 부모 생성자도 꼭 호출해줘야하는 이유가 궁금함. (매개변수로)
CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSpriteObject(pDevice, pContext)
	, m_bIsEquipped(false)
	, m_bIsInAir(false)
	, m_pAirState(nullptr)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	__super::Initialize_Prototype();

	//m_ID = "P";
	return S_OK;
}

HRESULT CPlayer::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
	{
		return E_FAIL;
	}

	m_eRenderGroup = CRenderer::RENDER_UI;

	m_pState = new CPlayerIdle();
	m_pAirState = new CPlayerJump();
	return S_OK;
}

HRESULT CPlayer::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	m_eRenderGroup = CRenderer::RENDER_UI;

	m_pState = new CPlayerIdle();
	m_pAirState = new CPlayerJump();
	return S_OK;
}

_uint CPlayer::Tick(_double TimeDelta)
{
	if (!m_bIsEquipped)
		return _uint();

	m_pState->Update(this, TimeDelta);

	if(m_bIsInAir)
		m_pAirState->Update(this, TimeDelta);

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

void CPlayer::Input_Handler(const STATE_TYPE Input, const SPRITE_DIRECTION eDirection)
{
	CState* pState = m_pState->Input_Handler(this, Input, eDirection);
	CState* pAirState = m_pAirState->Input_Handler(this, Input, eDirection);

	if (nullptr != pState)
	{
		//m_eCurrentState = Input;
		delete m_pState;

		m_pState = pState;
		m_pState->Enter(this);
	}

	if (nullptr != pAirState)
	{
		delete m_pAirState;
		m_pAirState = pAirState;
		m_pAirState->Enter(this);
	}
}

HRESULT CPlayer::Add_Components(void* pArg)
{
	if (FAILED(__super::Add_Components(pArg)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;

	/* For.Com_Collider */
	CComponent* pComponent = Find_Component(TAG_COLL_AABB);
	if (nullptr == pComponent)
	{
		MSG_BOX("CSpriteObject - Add_Component - Find Component is NULL");
		return E_FAIL;
	}

	m_pColliderCom = dynamic_cast<CCollider*>(pComponent);
	if (nullptr == pComponent)
	{
		MSG_BOX("CSpriteObject - Add_Component - Find Component is NULL");
		return E_FAIL;
	}

	///* For.Com_Texture */
	//if (FAILED(CGameObject::Add_Components(LEVEL_LOGO, m_pTextureTag,
	//	TAG_TEXTURE, (CComponent**)&m_pTextureCom, nullptr)))
	//	return E_FAIL;

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

	Safe_Delete(m_pAirState);
}
