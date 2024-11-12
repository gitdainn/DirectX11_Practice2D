#include "stdafx.h"
#include "Skill.h"
#include "FileLoader.h"

CSkill::CSkill(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CSpriteObject(pDevice, pContext)
{
}

HRESULT CSkill::Initialize_Prototype()
{
	Set_Layer(LAYER_PLAYERATK, false);
	__super::Initialize_Prototype();

	return S_OK;
}

HRESULT CSkill::Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
	{
		return E_FAIL;
	}

	Mapping_SkillData(m_pNameTag);

	m_bIsRender = false;
	return S_OK;
}

HRESULT CSkill::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	Mapping_SkillData(m_pNameTag);

	m_bIsRender = false;
	return S_OK;
}

HRESULT CSkill::Late_Initialize(void* pArg)
{
	if (nullptr == m_pSpriteFileName)
		m_tSpriteInfo.fSize = m_pTextureCom->Get_OriginalTextureSize(m_iTextureIndex);
	else
		m_tSpriteInfo.fSize = m_pTextureCom->Get_OriginalTextureSize(m_pSpriteFileName);

	if (m_bIsAnimUV)
	{
		m_tSpriteInfo.fSize.x /= m_iUVTexNumX;
		m_tSpriteInfo.fSize.y /= m_iUVTexNumY;
	}

	m_pTransformCom->Set_Scaled(_float3(m_tSpriteInfo.fSize.x, m_tSpriteInfo.fSize.y/* + m_tSpriteInfo.fSize.y * 0.5f*/, 0.f));

	/* For. Com_Collider */
	COMPONENT_INFO tComponentInfo;
	ZeroMemory(&tComponentInfo, sizeof(COMPONENT_INFO));
	tComponentInfo.fPosition = m_tSpriteInfo.fPosition;
	tComponentInfo.fSize = m_tSpriteInfo.fSize;

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

_uint CSkill::Tick(_double TimeDelta)
{
	m_LifeTimeAcc += TimeDelta;
	m_DelayTimeAcc += TimeDelta;

	if (m_LifeTime <= m_LifeTimeAcc)
	{
		m_LifeTimeAcc = 0.0;
		m_bIsSkillAvailable = true;
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
	ResetTime();

	const ANIM_INFO* pAnim = m_pAnimInfo + m_iAnimType;
	m_iTextureIndex = pAnim->iStartIndex;
	m_bIsSkillAvailable = false;
	m_bIsEndSprite = false;
	m_bIsRender = false;
	m_bIsDelayFinished = false;
}

HRESULT CSkill::Awaken()
{
	return S_OK;
}

HRESULT CSkill::Add_Components(void* pArg)
{
	if (FAILED(__super::Add_Components(pArg)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;

	/* For.Com_LineRider */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_LineRider"),
		TAG_LINERIDER, (CComponent**)&m_pLineRiderCom, &m_tSpriteInfo.fPosition)))
		return E_FAIL;

	return S_OK;
}

void CSkill::Mapping_SkillData(const _tchar* pObjectID)
{
	if (nullptr == pObjectID)
		return;

	CFileLoader* pFileLoader = CFileLoader::GetInstance();
	if (nullptr == pFileLoader)
		return;
	Safe_AddRef(pFileLoader);

	SKILL_EXCEL tSkillInfo;
	ZeroMemory(&tSkillInfo, sizeof(SKILL_EXCEL));
	if (FAILED(pFileLoader->Get_SkillData(pObjectID, tSkillInfo)))
	{
		Safe_Release(pFileLoader);
		return;
	}
	
	Set_NameTag(tSkillInfo.pName); //  tSkillInfo.pName은 FileLoader에서 map으로 보관되어 할당 해제를 하므로, m_pNameTag에 단순 대입하면 안됨.
	m_CoolDown = tSkillInfo.CoolDown;
	m_DelayTime = tSkillInfo.DelayTime;
	m_LifeTime = tSkillInfo.LifeTime;

	for (_uint i = 0; i < 3; ++i)
	{
		m_iDamage[i] = tSkillInfo.iDamage[i];
	}
	Safe_Release(pFileLoader);

	return;
}

void CSkill::Free()
{
	__super::Free();
}
