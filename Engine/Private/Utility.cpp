#include "Utility.h"
#include "GameInstance.h"

CUtility::CUtility()
{
}

CUtility::~CUtility()
{
}

CTexture* CUtility::Load_Texture_Folder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFolderPath)
{
	// _pLoadFilePath
	/*"../Bin/Resources/Sound/ 까지 받아오고, starcat으로 *.* 붙이기 */

	// _finddata_t : <io.h>에서 제공하며 파일 정보를 저장하는 구조체
	// <io.h>는 char 형식을 사용하기에 char을 사용한다.
	_finddata_t fd;

	char szCurPath[128] = "";
	char szPath[128] = "";
	char* pPath = { nullptr };
	WCToC(pFolderPath, pPath);
	strcpy_s(szCurPath, pPath); // ../BIn/Texture/
	strcpy_s(szPath, pPath); // ../BIn/Texture/
	Safe_Delete_Array(pPath);

	strcat_s(szCurPath, "*.*"); // ../BIn/Texture/*.*

	// _findfirst : <io.h>에서 제공하며 사용자가 설정한 경로 내에서 가장 첫 번째 파일을 찾는 함수
	intptr_t handle = _findfirst(szCurPath, &fd);

	if (handle == -1)
		return nullptr;

	int iResult = 0;

	char szFullPath[128] = "";

	_tchar szFileName[MAX_PATH] = TEXT("");
	_tchar szDir[MAX_PATH] = TEXT("");
	_tchar szExt[MAX_PATH] = TEXT("");

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	// 임시 리스트. 다 추가하고 복사해서 집어넣음
	// list<const _tchar*> ResourceTagList = {};
	// list<const _tchar*> MemoryList = {}; // 문자열 변환으로 인한 동적할당 해제를 위해서.

	_uint iTexType = 0;
	_uint iTexNum = 0; 

	vector<TCHAR*> FilePathVec;
	while (iResult != -1)
	{
		// ../Sound/"
		strcpy_s(szFullPath, szPath);

		// "../ Sound/Success.wav"
		strcat_s(szFullPath, fd.name);

		TCHAR* pFullPath = new TCHAR[MAX_PATH];
		CToWC(szFullPath, pFullPath); // wchar_t 두 번째 인자가 무조건 포인터여야됨

		_wsplitpath_s(pFullPath, nullptr, 0, szDir, MAX_PATH, szFileName, MAX_PATH, szExt, MAX_PATH);

		//if (!lstrcmp(L".png", szExt)) // DAINN. 해당 확장자는 안 불러와짐
		//{
		//	iResult = _findnext(handle, &fd);
		//	continue;
		//}

		// 숨김 파일 Continue
		if ('\0' == szFileName[0] || '.' == szFileName[0])
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		//_findnext : <io.h>에서 제공하며 다음 위치의 파일을 찾는 함수, 한 폴더 안에 더이상 찾을 게 없다면 -1을 리턴
		FilePathVec.emplace_back(pFullPath);
		++iTexNum;
		iResult = _findnext(handle, &fd);
	}

	CTexture* pTexture = CTexture::Create(pDevice, pContext, FilePathVec);

	for (TCHAR* pFilePath : FilePathVec)
	{
		Safe_Delete_Array(pFilePath);
	}

	Safe_Release(pGameInstance);

	_findclose(handle);

	return pTexture;
}
