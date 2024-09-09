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

			if (FAILED(pGameInstance->Add_GameObject(tSpriteInfo.pPrototypeTag, (_uint)eLevel, LAYER_DEFAULT, tSpriteInfo)))
			{
				MSG_BOX("CMyImGui - Load_Object() - FAILED");
				CloseHandle(hFile);
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
		}
		CloseHandle(hFile);
	}

	Clear_LoadInfo();
	Safe_Release(pGameInstance);
}

HRESULT CFileLoader::Load_Excel(const _tchar* pFilePath, LEVEL eLevel, vector<CSpriteObject*>& LoadObjectVec)
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
		const int iSheetNum = { 0 };
		Sheet* pSheet = pBook->getSheet(iSheetNum);
		if (nullptr == pSheet)
		{
			pBook->release();
			return E_FAIL;
		}
		const int iLastRow = pSheet->lastRow() - 1;
		const int iLastCol = 5;
		for (_uint iRow = m_iFirstRow; iRow <= iLastRow; ++iRow)
		{
			int iCol = { 0 };
			int iInstanceID = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			const _tchar* pObjectID = pSheet->readStr(iRow, m_iFirstCol + iCol++);
			_tchar* pClassName = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));
			_tchar* pLayer = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			Safe_AddRef(pGameInstance);
			if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Install"), (_uint)eLevel, pLayer, &iInstanceID)))
			{
				MSG_BOX("CMyImGui - Load_Object() - FAILED");
				Safe_Release(pGameInstance);
				return E_FAIL;
			}
			pGameInstance->Add_Garbage(pLayer);

			list<CGameObject*>* pObjectList = pGameInstance->Get_ObjectList((_uint)eLevel, pLayer);
			if (nullptr == pObjectList)
			{
				MSG_BOX("CFileLoader - Load_Excel() - NULL");
			}
			CSpriteObject* pAddObject = dynamic_cast<CSpriteObject*>(pObjectList->back());
			if (nullptr != pAddObject)
			{
				static int iIndex = { 1 };
				char* pTag = new char[MAX_PATH] {};
				strcpy(pTag, "pObjectID");
				strcat(pTag, to_string(++iIndex).c_str());
				pAddObject->Set_SpriteTag(pTag);
				pAddObject->Set_ClassName(pClassName);
				pAddObject->Set_Layer(pLayer);

				LoadObjectVec.emplace_back(dynamic_cast<CSpriteObject*>(pAddObject));
			}
			Safe_Release(pGameInstance);

			//Sheet* pTransformSheet = pBook->getSheet(1); // Transform
			// pTransformSheet로 동일한 ObjectID 찾아서 해당 Row 전부 읽어오기.
			// 그리고 tSpriteObject구조체에 모두 담아서 Add->GameObject //
			// 흠.. 구조체에 들어갈 내용을 한번에 보관하고, 생성할 때 필요한 정보만 MetaData에 관리한느게 어떨까..?
		}
	}

	pBook->release();
	Clear_LoadInfo();

	return S_OK;
}

HRESULT CFileLoader::Load_OriginalData_Excel(const _tchar* pFilePath, LEVEL eLevel)
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
		Sheet* pSheet = pBook->getSheet(m_iOriginalData);
		if (nullptr == pSheet)
		{
			pBook->release();
			return E_FAIL;
		}
		const int iLastRow = pSheet->lastRow() - 1;
		const int iLastCol = { 5 };
		ORIGINAL_DATA tOriginalData;
		for (_uint iRow = m_iFirstRow; iRow <= iLastRow; ++iRow)
		{
			int iCol = { 0 };
			tOriginalData.pObjectID = pSheet->readStr(iRow, m_iFirstCol + iCol++);
			tOriginalData.pClass = pSheet->readStr(iRow, m_iFirstCol + iCol++);
			tOriginalData.pLayer = pSheet->readStr(iRow, m_iFirstCol + iCol++);
			tOriginalData.pTextureTag = pSheet->readStr(iRow, m_iFirstCol + iCol++);
			tOriginalData.iTextureIndex = pSheet->readNum(iRow, m_iFirstCol + iCol++);

			//if (m_OriginalDataMap.end() != m_OriginalDataMap.find(tOriginalData.pClass))
			//{
			//	MSG_BOX("CFileLoader - Load_OriginalData_Excel() - FAIL");
			//	pBook->release();
			//	continue;
			//}

			//@qurious - 지역변수구조체를 컨테이너에 넣으면 소멸이 안되고 복제돼서 들어가는지!
			m_OriginalDataMap.emplace(tOriginalData.pClass, tOriginalData);
		}
	}

	pBook->release();

	return S_OK;
}

HRESULT CFileLoader::Load_ObjectTransform_Excel(const _tchar* pFilePath)
{
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
		const int iLastCol = 11;
		for (_uint iRow = m_iFirstRow; iRow <= iLastRow; ++iRow)
		{
			int iCol = { 0 };
			const int iInstanceID = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			const _tchar* pObjectID = pSheet->readStr(iRow, m_iFirstCol + iCol++);

			//if (m_ObjectTransformMap.end() != m_ObjectTransformMap.find(iInstanceID))
			//{
			//	MSG_BOX("CFileLoader - Load_SpriteInfo_Excel() - FAIL");
			//	continue;
			//}

			tTransform.fSize.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tTransform.fSize.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tTransform.fSizeRatio.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tTransform.fSizeRatio.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tTransform.fPosition.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tTransform.fPosition.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tTransform.fRotation.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tTransform.fRotation.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);

			m_ObjectTransformMap.emplace(iInstanceID, tTransform);
		}
	}

	pBook->release();

	return S_OK;
}

HRESULT CFileLoader::Load_ComponentInfo_Excel(const _tchar* pFilePath)
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
		Sheet* pSheet = pBook->getSheet(m_iComponentInfo);
		if (nullptr == pSheet)
		{
			pBook->release();
			return E_FAIL;
		}
		const int iLastRow = pSheet->lastRow() - 1;
		const int iLastCol = 11;
		for (_uint iRow = m_iFirstRow; iRow <= iLastRow; ++iRow)
		{
			int iCol = { 0 };
			tComponentInfo.iInstanceID = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			const _tchar* pObjectID = pSheet->readStr(iRow, m_iFirstCol + iCol++);

			tComponentInfo.pPrototypeTag = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));
			tComponentInfo.pComponentTag = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));
			tComponentInfo.pSortingLayer = Copy_WChar(pSheet->readStr(iRow, m_iFirstCol + iCol++));
			tComponentInfo.iOrder = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.iTextureIndex = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fSize.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fSize.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fOffset.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fOffset.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fPosition.x = pSheet->readNum(iRow, m_iFirstCol + iCol++);
			tComponentInfo.fPosition.y = pSheet->readNum(iRow, m_iFirstCol + iCol++);

			CGameInstance* pGameInstance = CGameInstance::GetInstance();
			pGameInstance->Add_Garbage(tComponentInfo.pPrototypeTag);
			pGameInstance->Add_Garbage(tComponentInfo.pComponentTag);
			pGameInstance->Add_Garbage(tComponentInfo.pSortingLayer);

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

HRESULT CFileLoader::Write_ObjectTransform_Excel(const _tchar* pFilePath, const OBJECT_TRANSFORM& tTransform, bool& bIsReset) const
{
	Book* pBook = xlCreateXMLBookW();
	if (nullptr == pBook)
	{
		pBook->release();
		return E_FAIL;
	}

	static _uint iRow = m_iFirstRow;

	if (pBook->load(pFilePath))
	{
		Sheet* pSheet = pBook->getSheet(m_iObjectTransform);
		if (nullptr == pSheet)
		{
			pBook->release();
			return E_FAIL;
		}

		const int iLastRow = pSheet->lastRow() - 1;
		const int iLastCol = 11;
		int iCol = { 0 };

		if (bIsReset)
		{
			iRow = m_iFirstRow;
			pSheet->removeRow(m_iFirstRow, iLastRow);
		}

		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tTransform.iInstanceID);
		pSheet->writeStr(iRow, m_iFirstCol + iCol++, tTransform.pObjectID);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tTransform.fSize.x);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tTransform.fSize.y);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tTransform.fSizeRatio.x);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tTransform.fSizeRatio.y);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tTransform.fPosition.x);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tTransform.fPosition.y);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tTransform.fRotation.x);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tTransform.fRotation.y);
		++iRow;
	}

	pBook->save(pFilePath);
	pBook->release();

	return S_OK;
}

HRESULT CFileLoader::Write_ComponentInfo_Excel(const _tchar* pFilePath, const COMPONENT_INFO& tMetaData, bool& bIsReset) const
{
	Book* pBook = xlCreateXMLBookW();
	if (nullptr == pBook)
	{
		pBook->release();
		return E_FAIL;
	}

	static _uint iRow = m_iFirstRow;
	if (pBook->load(pFilePath))
	{
		Sheet* pSheet = pBook->getSheet(m_iComponentInfo);
		if (nullptr == pSheet)
		{
			pBook->release();
			return E_FAIL;
		}
		const int iLastRow = pSheet->lastRow() - 1;
		const int iLastCol = 11;
		int iCol = { 0 };

		if (bIsReset)
		{
			iRow = m_iFirstRow;
			pSheet->removeRow(m_iFirstRow, iLastRow);
		}

		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tMetaData.iInstanceID);
		pSheet->writeStr(iRow, m_iFirstCol + iCol++, tMetaData.pObjectID);
		pSheet->writeStr(iRow, m_iFirstCol + iCol++, tMetaData.pPrototypeTag);
		pSheet->writeStr(iRow, m_iFirstCol + iCol++, tMetaData.pComponentTag);
		pSheet->writeStr(iRow, m_iFirstCol + iCol++, tMetaData.pSortingLayer);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tMetaData.iOrder);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tMetaData.iTextureIndex);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tMetaData.fSize.x);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tMetaData.fSize.y);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tMetaData.fOffset.x);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tMetaData.fOffset.y);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tMetaData.fPosition.x);
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tMetaData.fPosition.y);
		++iRow;
	}

	pBook->save(pFilePath);
	pBook->release();

	return S_OK;
}

HRESULT CFileLoader::Write_ObjectMetaData_Excel(const _tchar* pFilePath, const OBJECT_METADATA& tMetaData, bool& bIsReset) const
{
	Book* pBook = xlCreateXMLBookW();
	if (nullptr == pBook)
	{
		pBook->release();
		return E_FAIL;
	}

	static _uint iRow = m_iFirstRow;
	if (pBook->load(pFilePath))
	{
		Sheet* pSheet = pBook->getSheet(m_iObjectMetaData);
		if (nullptr == pSheet)
		{
			pBook->release();
			return E_FAIL;
		}
		const int iLastRow = pSheet->lastRow() - 1;
		const int iLastCol = 11;
		int iCol = { 0 };

		if (bIsReset)
		{
			iRow = m_iFirstRow;
			pSheet->removeRow(m_iFirstRow, iLastRow);
		}
		
		pSheet->writeNum(iRow, m_iFirstCol + iCol++, tMetaData.iInstanceID);
		pSheet->writeStr(iRow, m_iFirstCol + iCol++, tMetaData.pObjectID);
		pSheet->writeStr(iRow, m_iFirstCol + iCol++, tMetaData.pClassName);
		pSheet->writeStr(iRow, m_iFirstCol + iCol++, tMetaData.pLayer);
		++iRow;
	}

	pBook->save(pFilePath);
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

void CFileLoader::Clear_LoadInfo()
{
	m_OriginalDataMap.clear();
	m_ObjectTransformMap.clear();
	m_ComponentInfoMap.clear();
}

inline void CFileLoader::Free(void)
{
	__super::Free();

	// @error - OriginalData도 해제해주기

	//for (auto iter : m_ObjectTransformMap)
	//{
	//	Safe_Delete_Array(iter.second.pObjectID);
	//}
	m_ObjectTransformMap.clear();
	m_ComponentInfoMap.clear();
}
