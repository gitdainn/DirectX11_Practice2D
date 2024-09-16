#include "stdafx.h"
#include "SkillGateOfNether.h"
#include "Player_Manager.h"

CSkillGateOfNether::CSkillGateOfNether(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSkill(pDevice, pContext)
{
}

HRESULT CSkillGateOfNether::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	return S_OK;
}

HRESULT CSkillGateOfNether::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	m_iUVTexNumX = 5;
	m_iUVTexNumY = 7;

	if (FAILED(__super::Initialize(InSpriteInfo)))
	{
		return E_FAIL;
	}

	Add_Animation();

	m_bIsAnimUV = true;
	m_bIsSkillAvailable = true;

	_float2 vScale = m_pTextureCom->Get_OriginalTextureSize(m_iTextureIndex);
	vScale.x /= m_iUVTexNumX;
	vScale.y /= m_iUVTexNumY;
	m_pTransformCom->Set_Scaled(_float3(vScale.x, vScale.y, 0.f));
	m_iShaderPassIndex = (_uint)VTXTEX_PASS::UV_Anim;

	return S_OK;
}

HRESULT CSkillGateOfNether::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	m_bIsAnimUV = false;

	return S_OK;
}

_uint CSkillGateOfNether::Tick(_double TimeDelta)
{
	Play_Animation(m_iUVTextureIndex, TimeDelta);

	if (m_bIsEndSprite)
	{
		m_bIsEndSprite = false;
		if ((_uint)GATEOFNETHER_TYPE::SKILL_END <= (m_iTextureIndex + 1))
		{
			m_iTextureIndex = 0;
			m_bIsSkillAvailable = true;
			m_iCurrentAnim = 0;
		}
		else
		{
			++m_iCurrentAnim;
			++m_iTextureIndex;
		}
	}
	return __super::Tick(TimeDelta);

}

_uint CSkillGateOfNether::LateTick(_double TimeDelta)
{
	return __super::LateTick(TimeDelta);
}

HRESULT CSkillGateOfNether::Render()
{
	return __super::Render();
}

void CSkillGateOfNether::Enter(CSpriteObject* pOwner)
{
	if (nullptr == pOwner)
	{
		return;
	}

	CTransform* pPlayerTransformCom = pOwner->Get_TransformCom();
	if (nullptr == pPlayerTransformCom)
		return;

	_vector vPosition = pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	XMStoreFloat2(&m_tSpriteInfo.fPosition, vPosition);

	if (FAILED(Landing_Ground(vPosition)))
	{
		m_bIsSkillAvailable = true;
		return;
	}

	m_iCurrentAnim = 0;
	m_AnimAcc = 0.0;
	m_bIsSkillAvailable = false;
	m_bIsEndSprite = false;

	m_iTextureIndex = 0;
	ResetTime();

	return;
}

HRESULT CSkillGateOfNether::Awaken()
{
	return S_OK;
}

void CSkillGateOfNether::Add_Animation()
{
	m_pAnimInfo = new ANIM_INFO[(_uint)GATEOFNETHER_TYPE::SKILL_END];
	
	m_pAnimInfo[(_uint)GATEOFNETHER_TYPE::LOOP].iStartIndex = 0;
	m_pAnimInfo[(_uint)GATEOFNETHER_TYPE::LOOP].iEndIndex = 34;
	m_pAnimInfo[(_uint)GATEOFNETHER_TYPE::LOOP].fAnimTime = 2;

	m_pAnimInfo[(_uint)GATEOFNETHER_TYPE::START].iStartIndex = 0;
	m_pAnimInfo[(_uint)GATEOFNETHER_TYPE::START].iEndIndex = 34;
	m_pAnimInfo[(_uint)GATEOFNETHER_TYPE::START].fAnimTime = 2;
}

//void CSkillGateOfNether::Play_Animation(_uint& iSpriteIndex, _double TimeDelta)
//{
//	//Play_Animation(m_iTextureIndex, TimeDelta);
//		// 열거체는 객체마다 다르므로 .. 템플릿 가능할까?
//	const _uint iMaxTexNum = m_pAnimInfo->iEndIndex + 1;
//	_float fPerAnimTime = m_pAnimInfo[m_iCurrentAnim].fAnimTime / (_float)iMaxTexNum;
//
//	unordered_map<_uint, _float>::iterator iter = m_pAnimInfo[m_iCurrentAnim].fDelayTimeMap.find(iSpriteIndex);
//	_float fDelayTime = { 0.f };
//	if (iter != m_pAnimInfo[m_iCurrentAnim].fDelayTimeMap.end())
//		fDelayTime = iter->second;
//
//	m_fTimeAcc += (_float)TimeDelta;
//	if (fPerAnimTime + fDelayTime <= m_fTimeAcc)
//	{
//		m_fTimeAcc = 0.f;
//		++iSpriteIndex;
//
//		if (m_pAnimInfo->iEndIndex < iSpriteIndex)
//		{
//			m_bIsEndSprite = true;
//			iSpriteIndex = m_pAnimInfo->iStartIndex;
//		}
//	}
//}

HRESULT CSkillGateOfNether::Add_Components(void* pArg)
{
	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Sprite_GateOfNetherUV"),
		TAG_TEXTURE, (CComponent**)&m_pTextureCom, nullptr)))
		return E_FAIL;

	return __super::Add_Components(pArg);
}

HRESULT CSkillGateOfNether::SetUp_ShaderResources()
{
	if (FAILED(__super::SetUp_ShaderResources()))
		return E_FAIL;

	_uint iUVIndexY = m_iUVTextureIndex / m_iUVTexNumX;
	/** @note - _uint가 있으면 int로 담기 전 계산 과정에서 이미 모두 int로 변환 후 계산해야함. (음수가 되면 엄청 쓰레기값 들어감) */
	_uint iUVIndexX = max(0, (int)m_iUVTextureIndex - (int)(m_iUVTexNumX * iUVIndexY) - 1);

	// 0일 경우 -1을 하면 _uint라 이상한 값 나오기 때문에 체크 후 1 감소 (1감소해야 위치 맞음)
	if (FAILED(m_pShaderCom->Set_RawValue("g_iUVIndexX", &iUVIndexX, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_iUVIndexY", &iUVIndexY, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_iUVTexNumX", &m_iUVTexNumX, sizeof(_uint))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_iUVTexNumY", &m_iUVTexNumY, sizeof(_uint))))
		return E_FAIL;

	return S_OK;
}

CSkillGateOfNether* CSkillGateOfNether::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CSkillGateOfNether* pInstance = new CSkillGateOfNether(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created CSkillGateOfNether");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CSkillGateOfNether::Clone(const tSpriteInfo& InSpriteInfo, void* pArg) const
{
	CSkillGateOfNether* pInstance = new CSkillGateOfNether(*this);

	if (FAILED(pInstance->Initialize(InSpriteInfo, pArg)))
	{
		MSG_BOX("Failed to Cloned CSkillGateOfNether");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CSpriteObject* CSkillGateOfNether::Clone(void* pArg) const
{
	CSkillGateOfNether* pInstance = new CSkillGateOfNether(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CSkillGateOfNether");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CSkillGateOfNether::Free()
{
	__super::Free();
}
