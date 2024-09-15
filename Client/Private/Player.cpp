#include "stdafx.h"
#include "Player.h"
#include "PlayerIdle.h"
#include "PlayerJump.h"
#include "Line_Manager.h"
#include "FileLoader.h"
#include "Skill.h"

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

	return S_OK;
}

HRESULT CPlayer::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	if (FAILED(__super::Initialize(InSpriteInfo)))
	{
		return E_FAIL;
	}

	// pFileLoader->Get_Data(pObjectID); 로 모든 정보 가져와서 맵핑 ... 해야되는데 ...
	m_pLayer = LAYER_PLAYER;
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

	/* false == IsCurrentLineOccupied()이면 playerJump로 변환 + 추락 상태 만들기 */
	if (m_bIsInAir)
	{
		m_pAirState->Update(this, TimeDelta);
	}
	// @ error - 공중 상태일 때, 추락하는 시점에 땅이 있으면 자연스럽게 안착해야 함.
	else
	{
		// 안 뛰고 있으면 무조건 Landing_Ground해서 땅에 붙여버리기
		Landing_Ground();
	}
	
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

void CPlayer::OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget)
{
}

void CPlayer::OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget)
{
	__super::OnCollisionStay(pTargetCollider, pTarget);
}

void CPlayer::OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget)
{
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

void CPlayer::Mapping_SkulData(const _tchar* pObjectID)
{
	if (nullptr == pObjectID)
		return;

	CFileLoader* pFileLoader = CFileLoader::GetInstance();
	if (nullptr == pFileLoader)
		return;
	Safe_AddRef(pFileLoader);

	LOAD_SKUL_INFO tSkulInfo;
	pFileLoader->Get_SkulData(pObjectID, tSkulInfo);

	m_iMagicAttackIncrease = tSkulInfo.iMagicAttackIncrease;
	m_iPhysicalAttackIncrease = tSkulInfo.iPhysicalAttackIncrease;
	m_iMagicDefenseIncrease = tSkulInfo.iMagicDefenseIncrease;
	m_iPhysicalDefenseIncrease = tSkulInfo.iPhysicalDefenseIncrease;

	m_eSkulRank = tSkulInfo.eRank;
	m_eSkulType = tSkulInfo.eType;

	if (SKUL_TYPE::BALANCE == m_eSkulType)
	{
		m_iMaxJumpCount = 2;
	}
	else if (SKUL_TYPE::POWER == m_eSkulType)
	{
		m_iMaxJumpCount = 3;
	}
	else
		m_iMaxJumpCount = 1;

	for (_uint i = 0; i < iSkillNum; ++i)
	{
		m_pSkill[i] = Mapping_Skill(tSkulInfo.pSkill1);
	}

	Safe_Release(pFileLoader);
}

CSkill* CPlayer::Mapping_Skill(const _tchar* pObjectID)
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
	if (nullptr == pGameInstance)
		return;
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Add_GameObject(/*Get_PrototypeName(pObjectID)*/TEXT("~~"), LEVEL_STATIC, LAYER_SKILL)))
		return nullptr;

	list<CGameObject*>* pObjectList = pGameInstance->Get_ObjectList(LEVEL_STATIC, LAYER_SKILL);
	if (nullptr == pObjectList)
		return nullptr;

	if (pObjectList->empty())
		return nullptr;

	Safe_Release(pGameInstance);

	return dynamic_cast<CSkill*>(pObjectList->back());
}

void CPlayer::Landing_Ground()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;
	Safe_AddRef(pGameInstance);

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	_float fLandingY = { 0.f };
	if (pGameInstance->IsCurrentLineOccupied(_float2(XMVectorGetX(vPosition), XMVectorGetY(vPosition)), fLandingY))
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, fLandingY));
	}
	else
	{
		m_bIsInAir = true;
		Input_Handler(STATE_TYPE::JUMP);
		CPlayerJump* pPlayerJump = dynamic_cast<CPlayerJump*>(m_pAirState);
		if (nullptr != pPlayerJump)
		{
			pPlayerJump->Set_IsFalling(true);
		}
	}

	Safe_Release(pGameInstance);
	
	return;
}

HRESULT CPlayer::Add_Components(void* pArg)
{
	if (FAILED(__super::Add_Components(pArg)))
		return E_FAIL;

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
		return E_FAIL;

	if (nullptr == m_pColliderCom)
	{
		COMPONENT_INFO tComponentInfo;
		ZeroMemory(&tComponentInfo, sizeof(COMPONENT_INFO));
		tComponentInfo.fPosition = m_tSpriteInfo.fPosition;
		tComponentInfo.fSize = _float2(m_tSpriteInfo.fSize.x / m_iUVTexNumX, m_tSpriteInfo.fSize.y / m_iUVTexNumY);
		//tComponentInfo.fSize = _float2(60.f, 60.f);
		tComponentInfo.fOffset = _float2(0.f, 0.f);

		if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Collider_AABB"),
			TAG_COLL_AABB, (CComponent**)&m_pColliderCom, &tComponentInfo)))
		{
			MSG_BOX("CSpriteObject - Add_Components() - FAILED");
			return E_FAIL;
		}
		m_pColliderCom->Set_Owner(this);
	}

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
