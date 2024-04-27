#include "stdafx.h"
#include "Player.h"

// @qurious. 부모 생성자도 꼭 호출해줘야하는 이유가 궁금함. (매개변수로)
CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSpriteObject(pDevice, pContext)
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

	Add_Animation();
	CTransform::TRANSFORM_DESC tTransDesc;
	tTransDesc.SpeedPerSec = 20.f;
	m_pTransformCom->Set_TransformDesc(tTransDesc);

	m_iShaderPassIndex = (_uint)VTXTEXPASS::Default;
	m_iCurrentAnim = (_uint)PLAYER_ANIM::WAIT;
	return S_OK;
}

_uint CPlayer::Tick(_double TimeDelta)
{
	Play_Animation(TimeDelta);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	if (pGameInstance->Get_KeyStay(DIK_A))
	{
		m_pTransformCom->Go_Left(TimeDelta);
	}
	if (pGameInstance->Get_KeyStay(DIK_D))
	{
		m_pTransformCom->Go_Right(TimeDelta);
	}
	Safe_Release(pGameInstance);

	static _vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_vector vCurrentPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	if (XMVectorGetX(vCurrentPos) != XMVectorGetX(vPos))
	{
		vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		int i = 0;
	}
	return __super::Tick(TimeDelta);
}

_uint CPlayer::LateTick(_double TimeDelta)
{
	return __super::LateTick(TimeDelta);
}

HRESULT CPlayer::Render()
{
	return __super::Render();
}

void CPlayer::Add_Animation()
{
	m_pAnimInfo = new ANIM_INFO[(_uint)PLAYER_ANIM::ANIM_END];

	m_pAnimInfo[(_uint)PLAYER_ANIM::WAIT].iStartIndex = 0;
	m_pAnimInfo[(_uint)PLAYER_ANIM::WAIT].iEndIndex = 47;

	m_pAnimInfo[(_uint)PLAYER_ANIM::RUN].iStartIndex = 0;
	m_pAnimInfo[(_uint)PLAYER_ANIM::RUN].iEndIndex = 47;
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
	if (FAILED(CGameObject::Add_Components(LEVEL_LOGO, TEXT("Prototype_Component_Sprite_LittleBorn"),
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

CPlayer* CPlayer::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayer* pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CPlayer::Clone(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	CPlayer* pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
	{
		MSG_BOX("Failed to Cloned CPlayer");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	Safe_Delete_Array(m_pAnimInfo);
}
