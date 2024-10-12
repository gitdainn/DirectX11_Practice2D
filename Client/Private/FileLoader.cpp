#include "stdafx.h"
#include "FileLoader.h"
#include "GameInstance.h"
#include "GameObject.h"
#include "Line_Manager.h"
#include "libxl.h"
#include <functional>

using namespace libxl;

IMPLEMENT_SINGLETON(CFileLoader)

CFileLoader::CFileLoader()
	: m_iFirstRow(4), m_iFirstCol(1)
{
}

CFileLoader::~CFileLoader()
{
}

HRESULT CFileLoader::Load_FIle(const _tchar* pFilePath, LEVEL eLevel)
{
	if (nullptr == pFilePath)
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* 저장순서 : 게임오브젝트명, 벡터원소갯수, 텍스처프로토이름, 텍스처인덱스, UIRECTDESC 구조체, 색상, 선형보간, 액션 타입, 액션 구조체 */
	OPENFILENAME OFN;
	TCHAR filePathName[128] = L"";
	TCHAR lpstrFile[256] = L".data";
	static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.BattleUIdat)\0*.data";

	ZeroMemory(&OFN, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\DataFiles";

	if (GetOpenFileName(&OFN) != 0)
	{
		const TCHAR* pGetPath = OFN.lpstrFile;

		HANDLE hFile = CreateFile(pGetPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

		if (INVALID_HANDLE_VALUE == hFile)
			return E_FAIL;

		DWORD   dwByte = 0;
		_bool      bRes = { false };

#pragma region LOAD
		_uint iVecSize = { 0 };
		bRes = ReadFile(hFile, &iVecSize, sizeof(_uint), &dwByte, nullptr);

		_uint	iTagIndex = { 0 };
		for (_uint i = 0; i < iVecSize; ++i)
		{
			SPRITE_INFO tSpriteInfo;
			bRes = ReadFile(hFile, &tSpriteInfo, sizeof(SPRITE_INFO), &dwByte, nullptr);

			_uint iLength = { 0 };
			bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_tchar* pPrototypeTag = new _tchar[iLength]{};
			if (-1 != iLength)
			{
				bRes = ReadFile(hFile, pPrototypeTag, iLength, &dwByte, nullptr);
				tSpriteInfo.pPrototypeTag = pPrototypeTag;
			}

			bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
			_tchar* pTextureComTag = new _tchar[iLength]{};
			if (-1 != iLength)
			{
				bRes = ReadFile(hFile, pTextureComTag, iLength, &dwByte, nullptr);
				tSpriteInfo.pTextureComTag = pTextureComTag;
			}

			_uint iLayerBitset = { 0 };
			if (FAILED(pGameInstance->Add_GameObject(tSpriteInfo.pPrototypeTag, (_uint)eLevel, iLayerBitset, tSpriteInfo)))
			{
				MSG_BOX("CFileLoader - Load_FIle() - FAILED");
				CloseHandle(hFile);
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
		}
		CloseHandle(hFile);
	}

	Safe_Release(pGameInstance);
}

HRESULT CFileLoader::Load_Excel(const _tchar* pFilePath, const LEVEL eLevel)
{
	if (nullptr == pFilePath)
		return E_FAIL;

	Book* pBook = xlCreateXMLBookW();
	if (nullptr == pBook)
	{
		pBook->release();
		return E_FAIL;
	}

	tObjectTransform tObjectTransform;
	if (pBook->load(pFilePath))
	{
		const int iSheetNum = { 0 };
		Sheet* pSheet = pBook->getSheet(iSheetNum);
		if (nullptr == pSheet)
		{
			pBook->release();
			return E_FAIL;
		}
		const int iLastRow = pSheet->lastRow() - 1;
		const int iLastCol = 4;
		for (_uint iRow = m_iFirstRow; iRow <= iLastRow; ++iRow)
		{
			int iCol = { 0 };
			int iInstanceID = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			const _tchar* pObjectID = pSheet->readStr(iRow, m_iFirstCol + iCol++);
			const _tchar* pNameTag = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));
			const _tchar* pClassName = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));
			_tchar* pLayer = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));
			_uint iLayerBitset = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			_uint iOrder = pSheet->readNum(iRow, m_iFirstCol + iCol++);

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);

			_tchar pPrototypeTag[MAX_PATH] = TEXT("Prototype_GameObject_");
			lstrcat(pPrototypeTag, pClassName);
			Safe_Delete_Array(pClassName);
			Safe_Delete_Array(pLayer);

			if (FAILED(pGameInstance->Add_GameObject(pPrototypeTag, (_uint)eLevel, iLayerBitset, &iInstanceID)))
			{
				MSG_BOX("CFileLoader - Load_Excel() - FAILED");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}

			if (nullptr != pNameTag)
			{
				CGameObject* pObject = pGameInstance->Get_ObjectList((_uint)eLevel, iLayerBitset)->back();
				if (nullptr != pObject)
				{
					pObject->Set_NameTag(pNameTag);
					Safe_Delete_Array(pNameTag);
					pObject->Set_Order(iOrder);
				}
			}

			Safe_Release(pGameInstance);

			//Sheet* pTransformSheet = pBook->getSheet(1); // Transform
			// pTransformSheet로 동일한 ObjectID 찾아서 해당 Row 전부 읽어오기.
			// 그리고 tSpriteObject구조체에 모두 담아서 Add->GameObject //
			// 흠.. 구조체에 들어갈 내용을 한번에 보관하고, 생성할 때 필요한 정보만 MetaData에 관리한느게 어떨까..?
		}
	}

	pBook->release();

	return S_OK;
}


HRESULT CFileLoader::Load_Line(const _tchar* pFilePath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	if (nullptr == pFilePath || nullptr == pDevice || nullptr == pContext)
		return E_FAIL;

	HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

	if (INVALID_HANDLE_VALUE == hFile)
		return E_FAIL;

	DWORD   dwByte = 0;
	_bool      bRes = { false };

	_uint iListSize = { 0 };
	bRes = ReadFile(hFile, &iListSize, sizeof(_uint), &dwByte, nullptr);

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);

	for (_uint i = 0; i < iListSize; ++i)
	{
		Engine::LINE_INFO tLine;
		bRes = ReadFile(hFile, &tLine, sizeof(LINE_INFO), &dwByte, nullptr);
		pGameInstance->Add_Vertex(tLine.tLeftVertex);
		pGameInstance->Add_Vertex(tLine.tRightVertex);
	}

	Safe_Release(pGameInstance);
	CloseHandle(hFile);

	return S_OK;
}

HRESULT CFileLoader::Load_ObjectTransform_Excel(const _tchar* pFilePath)
{
	if (nullptr == pFilePath)
		return E_FAIL;

	Book* pBook = xlCreateXMLBookW();
	if (nullptr == pBook)
	{
		pBook->release();
		return E_FAIL;
	}

	OBJECT_TRANSFORM tTransform;
	if (pBook->load(pFilePath))
	{
		Sheet* pSheet = pBook->getSheet(m_iObjectTransform);
		if (nullptr == pSheet)
		{
			pBook->release();
			return E_FAIL;
		}
		const int iLastRow = pSheet->lastRow() - 1;
		const int iLastCol = 8;
		for (_uint iRow = m_iFirstRow; iRow <= iLastRow; ++iRow)
		{
			int iCol = { 0 };
			const int iInstanceID = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			const _tchar* pObjectID = pSheet->readStr(iRow, m_iFirstCol + iCol++);

			if (m_ObjectTransformMap.end() != m_ObjectTransformMap.find(iInstanceID))
			{
				MSG_BOX("CFileLoader - Load_SpriteInfo_Excel() - FAIL");
				continue;
			}

			_float2 fSizeRatio = { 1.f, 1.f };
			tTransform.fSize.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tTransform.fSize.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			fSizeRatio.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			fSizeRatio.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tTransform.fPosition.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tTransform.fPosition.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tTransform.fRotation.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tTransform.fRotation.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);

			tTransform.fSize.x *= fSizeRatio.x;
			tTransform.fSize.y *= fSizeRatio.y;
			m_ObjectTransformMap.emplace(iInstanceID, tTransform);
		}
	}

	pBook->release();

	return S_OK;
}

HRESULT CFileLoader::Load_ComponentInfo_Excel(const _tchar* pFilePath)
{
	if (nullptr == pFilePath)
		return E_FAIL;

	Book* pBook = xlCreateXMLBookW();
	if (nullptr == pBook)
	{
		pBook->release();
		return E_FAIL;
	}

	COMPONENT_INFO tComponentInfo;
	if (pBook->load(pFilePath))
	{
		Sheet* pSheet = pBook->getSheet(m_iComponentInfo);
		if (nullptr == pSheet)
		{
			pBook->release();
			return E_FAIL;
		}
		const int iLastRow = pSheet->lastRow() - 1;
		const int iLastCol = 13;
		for (_uint iRow = m_iFirstRow; iRow <= iLastRow; ++iRow)
		{
			int iCol = { 0 };
			tComponentInfo.iInstanceID = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			if (0 == tComponentInfo.iInstanceID)
			{
				continue;
			}
			const _tchar* pObjectID = pSheet->readStr(iRow, m_iFirstCol + iCol++);

			tComponentInfo.pPrototypeTag = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));
			tComponentInfo.pComponentTag = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));
			tComponentInfo.pSortingLayer = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));
			tComponentInfo.iTextureIndex = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fSize.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fSize.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fOffset.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fOffset.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fPosition.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fPosition.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);

			auto iter = m_ComponentInfoMap.find(tComponentInfo.iInstanceID);
			if (m_ComponentInfoMap.end() != iter)
			{
				(*iter).second.emplace_back(tComponentInfo);
			}
			else
			{
				list<COMPONENT_INFO> ComponentList;
				ComponentList.emplace_back(tComponentInfo);
				/** @qurious - 컨테이너에 삽입할 땐 복제되어 들어가는 것인가? (지역 변수면 소멸되니까 사라질테니!) */
				m_ComponentInfoMap.emplace(tComponentInfo.iInstanceID, ComponentList);
			}
		}
	}

	pBook->release();

	return S_OK;
}

HRESULT CFileLoader::Load_SkulData_Excel(const _tchar* pFilePath)
{
	if (nullptr == pFilePath)
		return E_FAIL;

	Book* pBook = xlCreateXMLBookW();
	if (nullptr == pBook)
	{
		pBook->release();
		return E_FAIL;
	}

	COMPONENT_INFO tComponentInfo;
	if (pBook->load(pFilePath))
	{
		const int iSheetIndex = { 0 };
		Sheet* pSheet = pBook->getSheet(iSheetIndex);
		if (nullptr == pSheet)
		{
			pBook->release();
			return E_FAIL;
		}
		const int iLastRow = pSheet->lastRow() - 1;
		const int iLastCol = pSheet->lastCol();
		const int iFirstRow = { 4 };
		const int iFirstCol = { 1 };
		for (_uint iRow = iFirstRow; iRow <= iLastRow; ++iRow)
		{
			SKUL_EXCEL tSkulInfo;
			ZeroMemory(&tSkulInfo, sizeof(SKUL_EXCEL));

			int iCol = { 0 };
			const _tchar* pID = pSheet->readStr(iRow, iFirstCol + iCol++);
			tSkulInfo.pName = Copy_WChar(pSheet->readStr(iRow, iFirstCol + iCol++));
			tSkulInfo.eRank = Translate_SkulRank(pSheet->readStr(iRow, iFirstCol + iCol++));
			tSkulInfo.eType = Translate_SkulType(pSheet->readStr(iRow, iFirstCol + iCol++));
			
			const int iSkillNum = { 4 };
			for (_uint i = 0; i < iSkillNum; ++i)
			{
				tSkulInfo.pSkill[i] = Copy_WChar(pSheet->readStr(iRow, iFirstCol + iCol++));
			}

			tSkulInfo.iMagicAttackIncrease = pSheet->readNum(iRow, iFirstCol + iCol++);
			tSkulInfo.iPhysicalAttackIncrease = pSheet->readNum(iRow, iFirstCol + iCol++);
			tSkulInfo.iDefense = pSheet->readNum(iRow, iFirstCol + iCol++);

			tSkulInfo.iBone = pSheet->readNum(iRow, iFirstCol + iCol++);

			tSkulInfo.pExplanation = Copy_WChar(pSheet->readStr(iRow, iFirstCol + iCol++));

			auto iter = m_SkulDataMap.find(tSkulInfo.pName);
			if (m_SkulDataMap.end() == iter)
			{
				/** @qurious - 컨테이너에 삽입할 땐 복제되어 들어가는 것인가? (지역 변수면 소멸되니까 사라질테니!) */
				m_SkulDataMap.emplace(tSkulInfo.pName, tSkulInfo);
			}
			else
			{
				Safe_Delete_Array(tSkulInfo.pName);
				Safe_Delete_Array(tSkulInfo.pExplanation);
				for (_uint i = 0; i < iSkillNum; ++i)
				{
					Safe_Delete_Array(tSkulInfo.pSkill[i]);
				}
			}
		}
	}

	pBook->release();

	return S_OK;
}

HRESULT CFileLoader::Load_SkillData_Excel(const _tchar* pFilePath)
{
	if (nullptr == pFilePath)
		return E_FAIL;

	Book* pBook = xlCreateXMLBookW();
	if (nullptr == pBook)
	{
		pBook->release();
		return E_FAIL;
	}

	COMPONENT_INFO tComponentInfo;
	if (pBook->load(pFilePath))
	{
		const int iSheetIndex = { 1 };
		Sheet* pSheet = pBook->getSheet(iSheetIndex);
		if (nullptr == pSheet)
		{
			pBook->release();
			return E_FAIL;
		}
		const int iLastRow = pSheet->lastRow() - 1;
		const int iLastCol = pSheet->lastCol();
		const int iFirstRow = { 4 };
		const int iFirstCol = { 1 };
		for (_uint iRow = iFirstRow; iRow <= iLastRow; ++iRow)
		{
			SKILL_EXCEL tSkillInfo;
			ZeroMemory(&tSkillInfo, sizeof(SKILL_EXCEL));

			int iCol = { 0 };
			const _tchar* pID = pSheet->readStr(iRow, iFirstCol + iCol++);
			tSkillInfo.pName = Copy_WChar(pSheet->readStr(iRow, iFirstCol + iCol++));
			tSkillInfo.eType = Translate_SkillType(pSheet->readStr(iRow, iFirstCol + iCol++));

			const _uint iDamageLevel = { 3 };
			for (_uint i = 0; i < iDamageLevel; ++i)
			{
				tSkillInfo.iDamage[i] = pSheet->readNum(iRow, iFirstCol + iCol++);
			}

			tSkillInfo.CoolDown = pSheet->readNum(iRow, iFirstCol + iCol++);
			tSkillInfo.DelayTime = pSheet->readNum(iRow, iFirstCol + iCol++);
			tSkillInfo.LifeTime = pSheet->readNum(iRow, iFirstCol + iCol++);

			tSkillInfo.pStatusChange = Copy_WChar(pSheet->readStr(iRow, iFirstCol + iCol++));
			tSkillInfo.pExplanation = Copy_WChar(pSheet->readStr(iRow, iFirstCol + iCol++));

			auto iter = m_SkillDataMap.find(tSkillInfo.pName);
			if (m_SkillDataMap.end() == iter)
			{
				/** @qurious - 컨테이너에 삽입할 땐 복제되어 들어가는 것인가? (지역 변수면 소멸되니까 사라질테니!) */
				m_SkillDataMap.emplace(tSkillInfo.pName, tSkillInfo);
			}
			else
			{
				Safe_Delete_Array(tSkillInfo.pName);
				Safe_Delete_Array(tSkillInfo.pStatusChange);
				Safe_Delete_Array(tSkillInfo.pExplanation);
			}
		}
	}

	pBook->release();

	return S_OK;
}

HRESULT CFileLoader::Get_OpenFileName(OPENFILENAME& tOpenFileName)
{
	OPENFILENAME OFN;
	TCHAR filePathName[128] = L"";
	TCHAR* lpstrFile = new TCHAR[256]{ L".data" };
	static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.BattleUIdat)\0*.BattleUIdat";

	ZeroMemory(&OFN, sizeof(OPENFILENAME));
	OFN.lStructSize = sizeof(OPENFILENAME);
	OFN.hwndOwner = g_hWnd;
	OFN.lpstrFilter = filter;
	OFN.lpstrFile = lpstrFile;
	OFN.nMaxFile = 256;
	OFN.lpstrInitialDir = L"..\\Bin\\DataFiles";

	if (GetOpenFileName(&OFN) == 0)
		return E_FAIL;

	// @note - 포인터 TCHAR lpstrFile[MAX_PATH] 지역변수면 함수 나가면 사라짐
	// 내가 알기로 리터럴 상수만 참조하는 곳이 없을 때까지 존재하는 것으로 앎.
	memcpy(&tOpenFileName, &OFN, sizeof(OPENFILENAME));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);
	pGameInstance->Add_Garbage(OFN.lpstrFile);
	Safe_Release(pGameInstance);

	return S_OK;
}


_tchar* CFileLoader::Copy_WChar(const _tchar* pWChar)
{
	if (nullptr == pWChar)
	{
		return nullptr;
	};

	_tchar* pChar = new _tchar[lstrlen(pWChar) + 1];
	if (nullptr == pChar)
	{
		return nullptr;
	};

	lstrcpy(pChar, pWChar);

	return pChar;
}

SKILL_TYPE CFileLoader::Translate_SkillType(const _tchar* pType) const
{
	if (nullptr == pType)
		return SKILL_TYPE();

	unordered_map<const _tchar*, SKILL_TYPE> TypeMap;
	TypeMap.emplace(L"마법데미지", SKILL_TYPE::PHYISIC);
	TypeMap.emplace(L"물리데미지", SKILL_TYPE::MAGIC);
	TypeMap.emplace(L"복합데미지", SKILL_TYPE::BALANCE);

	// @note - 문자열은 find로 찾으면 주소가 달라서 못 찾음. find_if 문으로 함수 객체 넣어서 lstrcpy로 비교할 것
	// @note - map의 멤버함수 find, find_if는 멤버 함수 X
	auto iter = find_if(TypeMap.begin(), TypeMap.end(), CTag_Finder(pType));
	if (iter == TypeMap.end())
	{
		MSG_BOX("CFileLoader - Translate_Rank() - FAILED");
		return SKILL_TYPE();
	}

	return iter->second;
}

SKUL_RANK CFileLoader::Translate_SkulRank(const _tchar* pRank) const
{
	if (nullptr == pRank)
		return SKUL_RANK();

	unordered_map<const _tchar*, SKUL_RANK> RankMap;
	RankMap.emplace(L"일반", SKUL_RANK::NORMAL);
	RankMap.emplace(L"레어", SKUL_RANK::RARE);
	RankMap.emplace(L"유니크", SKUL_RANK::UNIQUE);
	RankMap.emplace(L"레전더리", SKUL_RANK::LEGENDARY);

	// @note - 문자열은 find로 찾으면 주소가 달라서 못 찾음. find_if 문으로 함수 객체 넣어서 lstrcpy로 비교할 것
	// @note - map의 멤버함수 find, find_if는 멤버 함수 X
	auto iter = find_if(RankMap.begin(), RankMap.end(), CTag_Finder(pRank));
	if (iter == RankMap.end())
	{
		MSG_BOX("CFileLoader - Translate_Rank() - FAILED");
		return SKUL_RANK();
	}

	return iter->second;
}

SKUL_TYPE CFileLoader::Translate_SkulType(const _tchar* pType) const
{
	unordered_map<const _tchar*, SKUL_TYPE> TypeMap;
	TypeMap.emplace(TEXT("밸런스"), SKUL_TYPE::BALANCE);
	TypeMap.emplace(TEXT("파워"), SKUL_TYPE::POWER);
	TypeMap.emplace(TEXT("스피드"), SKUL_TYPE::SPEED);

	auto iter = find_if(TypeMap.begin(), TypeMap.end(), CTag_Finder(pType));
	if (iter == TypeMap.end())
	{
		MSG_BOX("CFileLoader - Translate_Rank() - FAILED");
		return SKUL_TYPE();
	}

	return iter->second;
}

inline void CFileLoader::Free(void)
{
	__super::Free();

	//for (auto iter : m_ObjectTransformMap)
	//{
	//	Safe_Delete_Array(iter.second.pObjectID);
	//}
	m_ObjectTransformMap.clear();

	for (auto iter : m_ComponentInfoMap)
	{
		for (auto List : iter.second)
		{
			//Safe_Delete_Array(List.pObjectID); // @error. pObjectID는 null도 아니고 그냥 이상한 없는 값이라서 오류
			Safe_Delete_Array(List.pPrototypeTag);
			Safe_Delete_Array(List.pComponentTag);
			Safe_Delete_Array(List.pSortingLayer);
		}
	}
	m_ComponentInfoMap.clear();

	for (const pair<const _tchar*, SKUL_EXCEL>& Pair : m_SkulDataMap)
	{
		// Safe_Delete_Array(Pair.first); // first가 tPlayerInfo.pName;
		SKUL_EXCEL tPlayerInfo = Pair.second;
		Safe_Delete_Array(tPlayerInfo.pName);
		for (_uint i = 0; i < 4; ++i)
		{
			Safe_Delete_Array(tPlayerInfo.pSkill[i]);
		}
		Safe_Delete_Array(tPlayerInfo.pExplanation);
	}
	m_SkulDataMap.clear();

	for (const pair<const _tchar*, SKILL_EXCEL>& Pair : m_SkillDataMap)
	{
		// Safe_Delete_Array(Pair.first); // first가 tPlayerInfo.pName;
		SKILL_EXCEL tSkillInfo = Pair.second;
		Safe_Delete_Array(tSkillInfo.pName);
		Safe_Delete_Array(tSkillInfo.pStatusChange);
		Safe_Delete_Array(tSkillInfo.pExplanation);
	}
	m_SkillDataMap.clear();

	m_IDPrototypeNameMap.clear();
}
