#include "stdafx.h"
#include "Player.h"
#include "PlayerIdle.h"
#include "PlayerJump.h"
#include "Line_Manager.h"
#include "FileLoader.h"
#include "Skill.h"
#include "Player_Manager.h"
#include "PlayerDamaged.h"
#include "LineRider.h"

// @qurious. 부모 생성자도 꼭 호출해줘야하는 이유가 궁금함. (매개변수로)
CPlayer::CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CSpriteObject(pDevice, pContext)
	, m_bIsEquipped(false)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	__super::Initialize_Prototype();
	Set_Layer(LAYER_PLAYER, false);
	m_bIsScroll = false;

	return S_OK;
}

HRESULT CPlayer::Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
	{
		return E_FAIL;
	}

	Mapping_SkulData(m_pNameTag);

	// pFileLoader->Get_Data(pObjectID); 로 모든 정보 가져와서 맵핑 ... 해야되는데 ...
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

	Mapping_SkulData(m_pNameTag);

	m_eRenderGroup = CRenderer::RENDER_UI;

	m_pState = new CPlayerIdle();
	m_pAirState = new CPlayerJump();

	return S_OK;
}

_uint CPlayer::Tick(_double TimeDelta)
{
	if (!m_bIsEquipped)
		return _uint();

	if(STATE_TYPE::SWAP != m_eCurrentState)
		m_pState->Update(this, TimeDelta);

	//CGameInstance* pGameInstance = CGameInstance::GetInstance();
	//static _vector vPastPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	//if (XMVectorGetX(vPastPosition) != XMVectorGetX(vPosition)
	//	|| XMVectorGetY(vPastPosition) != XMVectorGetY(vPastPosition))
	//{
	//	_vector vDir = vPosition - vPastPosition;
	//	_float fCameraSpeed = m_fMovementSpeed * 0.7f * TimeDelta;
	//	vDir = XMVector4Length(vDir);
	//	pGameInstance->Set_ScrollX(XMVectorGetX(vDir) * fCameraSpeed);
	//	pGameInstance->Set_ScrollY(XMVectorGetY(vDir) * fCameraSpeed);
	//	vPastPosition = vPosition;
	//}
	
	JumpableLineRider(TimeDelta);

	SkillTick(TimeDelta);

	return __super::Tick(TimeDelta);
}

_uint CPlayer::LateTick(_double TimeDelta)
{
	if (!m_bIsEquipped)
		return _uint();

	Attach_Collider(m_pLayerTag, m_pColliderCom);

	SkillLateTick(TimeDelta);

	return __super::LateTick(TimeDelta);
}

HRESULT CPlayer::Render()
{
	SkillRender();
	return __super::Render();
}

void CPlayer::SkillTick(_double TimeDelta)
{
	if (m_SkillAvailableList.empty())
		return;

	for (CSkill* pSkill : m_SkillAvailableList)
	{
		if (nullptr == pSkill)
			continue;

		pSkill->Tick(TimeDelta);
	}
}

void CPlayer::SkillLateTick(_double TimeDelta)
{
	if (m_SkillAvailableList.empty())
		return;

	// @note - 컨테이너를 돌리는 중에 remove 등 컨테이너 개수를 변동해버리면 문제 발생!
	list<CSkill*> RemoveList;
	for (CSkill* pSkill : m_SkillAvailableList)
	{
		if (nullptr == pSkill)
			continue;

		if (pSkill->Get_IsSkillAvailable())
		{
			RemoveList.emplace_back(pSkill);
			// @note - list의 remove는 erase와 remove 결합한 형태로 제공 (algorithm의 remove는 erase와 결합 사용 추천)
		}
		else
		{
			if(pSkill->Get_IsDelayFinished())
				pSkill->LateTick(TimeDelta);
		}
	}

	for (CSkill* pSkill : RemoveList)
	{
		m_SkillAvailableList.remove(pSkill);
	}
}

void CPlayer::SkillRender()
{
	if (m_SkillAvailableList.empty())
		return;

	for (CSkill* pSkill : m_SkillAvailableList)
	{
		if (nullptr == pSkill)
			continue;

		if (pSkill->Get_IsDelayFinished())
			pSkill->Render();
	}
}

void CPlayer::OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	CSpriteObject* pTargetObject = dynamic_cast<CSpriteObject*>(pTarget);
	if (nullptr == pTargetObject || nullptr == pTargetCollider)
		return;

	// 무적 상태가 아닐 때 공격을 받으면
	if ((!m_bIsInvulnerable) && (!lstrcmp(LAYER_ENEMYATK, pTargetLayer)))
	{
		Input_Handler(STATE_TYPE::DAMAGED);
		CPlayerDamaged* pPlayerDamaged = dynamic_cast<CPlayerDamaged*>(m_pState);
		if (nullptr != pPlayerDamaged)
			pPlayerDamaged->Set_TargetDirection(pTargetObject->Get_SpriteDirection());
		
		CPlayer_Manager* pPlayer_Manager = CPlayer_Manager::GetInstance();
		if (nullptr != pPlayer_Manager)
		{
			Safe_AddRef(pPlayer_Manager);
			pPlayer_Manager->Set_Damaged(pTargetObject->Get_Attack());
		}
		Safe_Release(pPlayer_Manager);
	}

	__super::OnCollisionEnter(pTargetCollider, pTarget, pTargetLayer);
}

void CPlayer::OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	__super::OnCollisionStay(pTargetCollider, pTarget, pTargetLayer);
}

void CPlayer::OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
}

void CPlayer::Input_Handler(const STATE_TYPE Input, const SPRITE_DIRECTION eDirection)
{
	CState* pState = m_pState->Input_Handler(this, Input, eDirection);
 	CState* pAirState = m_pAirState->Input_Handler(this, Input, eDirection);

	if (nullptr != pState)
	{
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

void CPlayer::Execute_Skill(_uint iSkillIndex)
{
	if (iSkillNum <= iSkillIndex)
		return;

	if (nullptr == m_pSkill[iSkillIndex])
		return;

	if (m_pSkill[iSkillIndex]->Get_IsSkillAvailable())
	{
		STATE_TYPE Input = (0 == iSkillIndex ? STATE_TYPE::ATK1 : STATE_TYPE::ATK2);
		__super::Input_Handler(Input);
		m_pSkill[iSkillIndex]->Enter(this);
		m_SkillAvailableList.emplace_back(m_pSkill[iSkillIndex]);
	}
		// 해당 스킬 Available 상태이면 SkillManager에 넣어버려서 그곳에서 한번에 돌려버리기!
		// 들어오자마자 Available = false 되고, 스킬 다 하다가 CoolDown되면 초기화와 함께 다시 Available
		// Available 되면 매니저는 삭제 시킴.
}

void CPlayer::End_Animation(_uint& iSpriteIndex)
{
	__super::End_Animation(iSpriteIndex);
	iSpriteIndex = m_pAnimInfo[m_iAnimType].iStartIndex;
}

HRESULT CPlayer::JumpableLineRider(_double TimeDelta)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fLandingY = { 0.f };

	if (m_bIsInAir)
	{
		// 대쉬 중에는 점프 상태를 잠시 멈춥니다.
		if (m_eCurrentState == STATE_TYPE::DASH)
		{
			CPlayerJump* pJump = dynamic_cast<CPlayerJump*>(m_pAirState);
			if (nullptr != pJump)
				pJump->Set_IsFalling(true);
		}
		else
		{
			m_pAirState->Update(this, TimeDelta);
		}

		CPlayerJump* pPlayerJump = dynamic_cast<CPlayerJump*>(m_pAirState);
		if (nullptr == pPlayerJump)
			return E_FAIL;

		if (pPlayerJump->Get_IsFalling())
		{
			// 착지 시점을 체크합니다.
			if (m_pLineRiderCom->CheckLineLanding(vPosition, fLandingY))
			{
				m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, fLandingY));

				Set_IsInAir(false);

				STATE_TYPE eCurrentState = Get_CurrentState();
				//if (STATE_TYPE::ATK1 != eCurrentState
				//	&& STATE_TYPE::ATK2 != eCurrentState
				//	&& STATE_TYPE::DEFAULT_ATK != eCurrentState
				//	&& STATE_TYPE::JUMP_ATK != eCurrentState)
				{
					Input_Handler(STATE_TYPE::IDLE, Get_SpriteDirection());
					pPlayerJump->Set_IsDead(true);
				}
			}
		}
	}
	else
	{
		// 안 뛰고 있으면 무조건 Landing_Ground해서 땅에 붙여버리기
		// 붙일 땅이 없으면 추락 상태로 변경합니다.
		if (FAILED((m_pLineRiderCom->Collision_Line(vPosition, fLandingY))))
		{
			Input_Handler(STATE_TYPE::JUMP, Get_SpriteDirection());
			dynamic_cast<CPlayerJump*>(m_pAirState)->Set_IsFalling(true);
		}
		else
		{
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, fLandingY));
		}
	}

	return S_OK;
}

void CPlayer::Mapping_SkulData(const _tchar* pObjectID)
{
	if (nullptr == pObjectID)
		return;

	CFileLoader* pFileLoader = CFileLoader::GetInstance();
	if (nullptr == pFileLoader)
		return;
	Safe_AddRef(pFileLoader);

	SKUL_EXCEL tSkulInfo;
	if (FAILED(pFileLoader->Get_SkulData(pObjectID, tSkulInfo)))
	{
		Safe_Release(pFileLoader);
		return;
	}

	m_iMagicAttackIncrease = tSkulInfo.iMagicAttackIncrease;
	m_iPhysicalAttackIncrease = tSkulInfo.iPhysicalAttackIncrease;
	m_iDefense = tSkulInfo.iDefense;

	m_eSkulRank = tSkulInfo.eRank;
	m_eSkulType = tSkulInfo.eType;
	Mapping_Type(m_eSkulType);


	for (_uint i = 0; i < iSkillNum; ++i)
	{
		m_pSkill[i] = Get_Skill(tSkulInfo.pSkill[i]);
	}

	Safe_Release(pFileLoader);
}

CSkill* CPlayer::Get_Skill(const _tchar* pObjectID)
{
	// 이름에 알맞은 클래스를 추가하기. (해당 번호에 있는.... 클래스를 어케 갖고옴?
	// 1. Prototype을 조합한다. (기획자가 클래스명을 미리 안다고.....?)
	// 2. 미리 해당 프로토타입을 각 스컬에서 넣어둔다. (이러면 맵핑의 의미가없음... 알아야해..)
	// 3. 모든 스킬 만들어두고 .... 이것도 결국 클래스명 지정해줘야 하는데?
	// 흠.. 뭐가 되든 프로로타입은 알고있어야 함..
	// pair<스킬 이름, 프로토타입명>으로 각 객체마다 미리 해두는 게 좋을 듯.... 
	// 그러면 미리 모든 스킬 이름과 프로토타입명을 어디선가 관리해둬야 함... (Add_PrototypeName으로 ,, )

	if (nullptr == pObjectID)
		return nullptr;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	CFileLoader* pFileLoader = CFileLoader::GetInstance();
	if (nullptr == pGameInstance || nullptr == pFileLoader)
	{
		return nullptr;
	}
	Safe_AddRef(pGameInstance);
	Safe_AddRef(pFileLoader);

	SPRITE_INFO tSpriteInfo;
	ZeroMemory(&tSpriteInfo, sizeof(SPRITE_INFO));
	tSpriteInfo.fSize = _float2(200.f, 200.f);
	CGameObject* pObject = pGameInstance->Clone_GameObject(pFileLoader->Get_PrototypeName(pObjectID), tSpriteInfo);
	if (nullptr == pObject)
	{
		Safe_Release(pGameInstance);
		Safe_Release(pFileLoader);
		return nullptr;
	}

	Safe_Release(pGameInstance);
	Safe_Release(pFileLoader);

	CSkill* pSkill = dynamic_cast<CSkill*>(pObject);
	return pSkill;
}

void CPlayer::Awaken()
{
	++m_iLevel;
	for (_uint i = 0; i < iSkillNum; ++i)
	{
		if (nullptr == m_pSkill[i])
			continue;
		m_pSkill[i]->Awaken();
	}
}

void CPlayer::Mapping_Type(const SKUL_TYPE& tType)
{
	if (SKUL_TYPE::BALANCE == tType)
	{
		m_fMovementSpeed += m_fMovementSpeed * 0.2f;
		m_fAttackSpeed += m_fAttackSpeed * 0.2f;
		m_iMaxJumpCount = 2;
	}
	else if (SKUL_TYPE::POWER == tType)
	{
		m_iDefense -= m_iDefense * 0.2f;
		m_iMaxJumpCount = 1;
	}
	else if (SKUL_TYPE::SPEED == tType)
	{
		m_fReduceCoolDownSpeed += m_fReduceCoolDownSpeed * 0.6f;
		m_iMaxJumpCount = 3;
	}
	else
		MSG_BOX("CPlayer - Mapping_Type() - FAILED");

}

void CPlayer::Add_DefaultAtkCollider(const _uint iTextureIndex)
{
	if (iTextureIndex == m_iUVTextureIndex)
	{
		CCollider::COLLIDER_DESC tColliderDesc = m_pDefaultAtkColliderCom->Get_ColliderDesc();
		tColliderDesc.vOffset.y = 30.f;
		tColliderDesc.vOffset.x = (SPRITE_DIRECTION::LEFT == m_eSpriteDirection ? -20.f : 20.f);
		m_pDefaultAtkColliderCom->Set_ColliderDesc(tColliderDesc);
		Attach_Collider(LAYER_PLAYERATK, m_pDefaultAtkColliderCom);
		m_pDefaultAtkColliderCom->Set_Owner(this);
	}
}

HRESULT CPlayer::Add_Components(void* pArg)
{
	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;

	/* For.Com_LineRider */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_LineRider"),
		TAG_LINERIDER, (CComponent**)&m_pLineRiderCom, &m_tSpriteInfo.fPosition)))
		return E_FAIL;

#pragma region COLLIDER
	if (nullptr == m_pColliderCom)
	{
		COMPONENT_INFO tComponentInfo;
		ZeroMemory(&tComponentInfo, sizeof(COMPONENT_INFO));
		tComponentInfo.fPosition = m_tSpriteInfo.fPosition;
		tComponentInfo.fSize = _float2(70.f, 70.f); // 한 이미지 속 캐릭터 크기가 70, 70임
		tComponentInfo.fOffset = _float2(0.f, tComponentInfo.fSize.y * 0.5f);

		if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
			TAG_COLL_AABB, (CComponent**)&m_pColliderCom, &tComponentInfo)))
		{
			MSG_BOX("CSpriteObject - Add_Components() - FAILED");
			return E_FAIL;
		}
		m_pColliderCom->Set_Owner(this);
	}

	COMPONENT_INFO tComponentInfo;
	ZeroMemory(&tComponentInfo, sizeof(COMPONENT_INFO));
	tComponentInfo.fPosition = m_tSpriteInfo.fPosition;
	tComponentInfo.fSize = _float2(130.f, 130.f);
	tComponentInfo.fOffset = _float2(0.f, 0.f);

	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
		TAG_COLL_DEFAULTATK, (CComponent**)&m_pDefaultAtkColliderCom, &tComponentInfo)))
	{
		MSG_BOX("CSpriteObject - Add_Components() - FAILED");
		return E_FAIL;
	}
	m_pDefaultAtkColliderCom->Set_Owner(this);
#pragma endregion

	return __super::Add_Components(pArg);
}

HRESULT CPlayer::SetUp_ShaderResources()
{
	if (FAILED(SetUp_ShaderDefault()))
		return E_FAIL;

	if (FAILED(SetUp_Shader_UVAnim()))
		return E_FAIL;

	return S_OK;
}

void CPlayer::Free()
{
	__super::Free();

	m_SkillAvailableList.clear();

	for (_uint i = 0; i < iSkillNum; ++i)
	{
		Safe_Release(m_pSkill[i]);
	}
	Safe_Delete(m_pAirState);
	Safe_Delete(m_pState);

	Safe_Release(m_pDefaultAtkColliderCom);
}
