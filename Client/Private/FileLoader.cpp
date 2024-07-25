#include "stdafx.h"
#include "FileLoader.h"
#include "GameInstance.h"
#include "libxl.h"

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

			if (FAILED(pGameInstance->Add_GameObject(tSpriteInfo.pPrototypeTag, (_uint)eLevel, LAYER_UI, tSpriteInfo)))
			{
				MSG_BOX("CMyImGui - Load_Object() - FAILED");
				CloseHandle(hFile);
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
		}
		CloseHandle(hFile);
	}

	Safe_Release(pGameInstance);
}

HRESULT CFileLoader::Load_Excel(const _tchar* pFilePath, LEVEL eLevel)
{
	Book* pBook = xlCreateXMLBookW();
	if (nullptr == pBook)
	{
		pBook->release();
		return E_FAIL;
	}

	tObjectTransform tObjectTransform;
	if (pBook->load(pFilePath))
	{
		Sheet* pSheet = pBook->getSheet(SHEET::OBJECT_METADATA);
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
			const int iObjectID = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			_tchar* pPrototypeTag = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));
			_tchar* pLayer = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));
			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);
			if (FAILED(pGameInstance->Add_GameObject(pPrototypeTag, (_uint)eLevel, pLayer)))
			{
				MSG_BOX("CMyImGui - Load_Object() - FAILED");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			/** @error - 프로토는 지워도 되지만.. Layer는 지우면 컨테이너에서 값이 사라짐.. ㅜ */
			Safe_Delete_Array(pPrototypeTag);
			pGameInstance->Add_Garbage(pLayer);
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

HRESULT CFileLoader::Load_SpriteInfo_Excel(const _tchar* pFilePath, LEVEL eLevel)
{
	Book* pBook = xlCreateXMLBookW();
	if (nullptr == pBook)
	{
		pBook->release();
		return E_FAIL;
	}

	SPRITE_INFO tSpriteInfo;
	if (pBook->load(pFilePath))
	{
		Sheet* pSheet = pBook->getSheet(SHEET::OBJECT_INFO);
		if (nullptr == pSheet)
		{
			pBook->release();
			return E_FAIL;
		}
		const int iLastRow = pSheet->lastRow() - 1;
		const int iLastCol = 10;
		for (_uint iRow = m_iFirstRow; iRow <= iLastRow; ++iRow)
		{
			int iCol = { 0 };
			int iObjectID = pSheet->readNum(iRow, m_iFirstCol + iCol++);

			if (m_SpriteInfoMap.end() != m_SpriteInfoMap.find(iObjectID))
			{
				MSG_BOX("CFileLoader - Load_SpriteInfo_Excel() - FAIL");
				continue;
			}

			tSpriteInfo.iOrder = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tSpriteInfo.fSize.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tSpriteInfo.fSize.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tSpriteInfo.fSizeRatio.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tSpriteInfo.fSizeRatio.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tSpriteInfo.fPosition.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tSpriteInfo.fPosition.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);

			m_SpriteInfoMap.emplace(iObjectID, tSpriteInfo);
		}
	}

	pBook->release();

	return S_OK;
}

HRESULT CFileLoader::Load_ComponentInfo_Excel(const _tchar* pFilePath, LEVEL eLevel)
{
	Book* pBook = xlCreateXMLBookW();
	if (nullptr == pBook)
	{
		pBook->release();
		return E_FAIL;
	}

	COMPONENT_INFO tComponentInfo;
	if (pBook->load(pFilePath))
	{
		Sheet* pSheet = pBook->getSheet(SHEET::COMPONENT);
		if (nullptr == pSheet)
		{
			pBook->release();
			return E_FAIL;
		}
		const int iLastRow = pSheet->lastRow() - 1;
		const int iLastCol = 10;
		for (_uint iRow = m_iFirstRow; iRow <= iLastRow; ++iRow)
		{
			int iCol = { 0 };
			int iObjectID = pSheet->readNum(iRow, m_iFirstCol + iCol++);

			tComponentInfo.pPrototypeTag = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));
			tComponentInfo.pComponentTag = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));
			tComponentInfo.pLayer = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));
			tComponentInfo.fSize.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fSize.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fOffset.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fOffset.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fPosition.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fPosition.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);

			auto iter = m_ComponentInfoMap.find(iObjectID);
			if (m_ComponentInfoMap.end() != iter)
			{
				(*iter).second.emplace_back(tComponentInfo);
			}
			else
			{
				list<COMPONENT_INFO> ComponentList;
				ComponentList.emplace_back(tComponentInfo);
				/** @qurious - 컨테이너에 삽입할 땐 복제되어 들어가는 것인가? (지역 변수면 소멸되니까 사라질테니!) */
				m_ComponentInfoMap.emplace(iObjectID, ComponentList);
			}
		}
	}

	pBook->release();

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

inline void CFileLoader::Free(void)
{
	__super::Free();

	m_SpriteInfoMap.clear();

	for (auto iter : m_ComponentInfoMap)
	{
		for (auto List : iter.second)
		{
			Safe_Delete_Array(List.pPrototypeTag);
			Safe_Delete_Array(List.pComponentTag);
			Safe_Delete_Array(List.pLayer);
		}
	}
	m_ComponentInfoMap.clear();
}
