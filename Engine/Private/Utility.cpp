#include "Utility.h"
#include "GameInstance.h"

CUtility::CUtility()
{
}

CUtility::~CUtility()
{
}

CTexture* CUtility::Load_Texture_Folder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pFolderPath)
{
	// _pLoadFilePath
		/*"../Bin/Resources/Sound/ ���� �޾ƿ���, starcat���� *.* ���̱�*/

	// _finddata_t : <io.h>���� �����ϸ� ���� ������ �����ϴ� ����ü
	// <io.h>�� char ������ ����ϱ⿡ char�� ����Ѵ�.
	_finddata_t fd;

	char szCurPath[128] = "";
	strcpy_s(szCurPath, pFolderPath); // ../BIn/Texture/

	strcat_s(szCurPath, "*.*"); // ../BIn/Texture/*.*

	// _findfirst : <io.h>���� �����ϸ� ����ڰ� ������ ��� ������ ���� ù ��° ������ ã�� �Լ�
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

	// �ӽ� ����Ʈ. �� �߰��ϰ� �����ؼ� �������
	// list<const _tchar*> ResourceTagList = {};
	// list<const _tchar*> MemoryList = {}; // ���ڿ� ��ȯ���� ���� �����Ҵ� ������ ���ؼ�.

	_tchar szTexNum[10];

	_uint iTexType = 0;
	_uint iTexNum = 0; 

	vector<TCHAR*> FilePathVec;
	while (iResult != -1)
	{
		// ../Sound/"
		strcpy_s(szFullPath, pFolderPath);

		// "../ Sound/Success.wav"
		strcat_s(szFullPath, fd.name);

		TCHAR* pFullPath = new TCHAR[MAX_PATH];
		CToWC(szFullPath, pFullPath); // wchar_t �� ��° ���ڰ� ������ �����Ϳ��ߵ�
		FilePathVec.emplace_back(pFullPath);

		//if (!lstrcmp(L".png", szExt)) // DAINN. �ش� Ȯ���ڴ� �� �ҷ�����
		//{
		//	iResult = _findnext(handle, &fd);
		//	continue;
		//}

		// ���� ���� Continue
		if ('\0' == szFileName[0] || '.' == szFileName[0])
		{
			iResult = _findnext(handle, &fd);
			continue;
		}

		//_findnext : <io.h>���� �����ϸ� ���� ��ġ�� ������ ã�� �Լ�, �� ���� �ȿ� ���̻� ã�� �� ���ٸ� -1�� ����
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
