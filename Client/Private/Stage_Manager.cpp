#include "stdafx.h"
#include "Stage_Manager.h"
#include "Door.h"

IMPLEMENT_SINGLETON(CStage_Manager)

CStage_Manager::CStage_Manager()
{
}

CStage_Manager::~CStage_Manager()
{
}

_uint CStage_Manager::Tick(_double TimeDelta)
{
	if (ROOM_TYPE::BLACKMARKET == m_eRoomType || m_RewardList.empty())
		return _uint();

	// 보상 중 하나라도 사망(=획득)하면 문을 활성화합니다.
	for (CGameObject* pObject : m_RewardList)
	{
		if (nullptr == pObject || pObject->Get_IsDead())
		{
			Activate_Door();
			m_RewardList.clear();
			break;
		}
	}

	return _uint();
}

void CStage_Manager::Enter(const ROOM_TYPE& eRoomType, const LEVEL& eLevel)
{
	Initialize_Member();

	m_eRoomType = eRoomType;
	m_eLevel = eLevel;

	if (ROOM_TYPE::BLACKMARKET == m_eRoomType)
		Activate_Door();
}

void CStage_Manager::Give_ClearReward()
{
	if (m_bHasGivenReward)
		return;

	switch (m_eRoomType)
	{
	case ROOM_TYPE::GOLD:
		Give_Coin();
		break;

	case ROOM_TYPE::SKUL:
		Give_Skul();
		break;

	case ROOM_TYPE::ITEM_ESSENCE:
		Give_Essence();
		break;

	case ROOM_TYPE::AWAKENING_BOSS:
		Give_Skul();
		Give_Essence();
		Give_Item();
		break;

	default:
		break;
	}

	m_bHasGivenReward = true;
}

void CStage_Manager::Give_Skul()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;
	Safe_AddRef(pGameInstance);

	SPRITE_INFO tSpriteInfo;
	tSpriteInfo.fSize = _float2{ 20.f, 20.f };
	tSpriteInfo.fPosition = _float2{ 30, 30.f };

	if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_SkulItem"), m_eLevel, LAYER_ITEM, tSpriteInfo)))
	{
		Safe_Release(pGameInstance);
		return;
	}
	CGameObject* pObject = pGameInstance->Get_LastObject(m_eLevel, LAYER_ITEM);
	if (nullptr == pObject)
	{
		Safe_Release(pGameInstance);
		return;
	}

	m_RewardList.emplace_back(pObject);
	Safe_Release(pGameInstance);

	return;
}
void CStage_Manager::Give_Essence()
{
}
void CStage_Manager::Give_Item()
{
}
void CStage_Manager::Give_Coin()
{
}
void CStage_Manager::Initialize_Member()
{
	m_RewardList.clear();
	m_bHasGivenReward = false;
}

HRESULT CStage_Manager::Activate_Door()
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);

	list<CGameObject*>* pObjectList = pGameInstance->Get_ObjectList(m_eLevel, LAYER_DOOR);
	if (nullptr == pObjectList)
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	for (CGameObject* pObject : *pObjectList)
	{
		CDoor* pDoor = dynamic_cast<CDoor*>(pObject);
		if (nullptr == pDoor)
			continue;

		pDoor->Set_Activate();
	}

	Safe_Release(pGameInstance);
	return S_OK;
}
void CStage_Manager::Free()
{
	__super::Free();
}
