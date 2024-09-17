#pragma once
#include "Engine_Defines.h"
#include "Texture.h"

class ENGINE_DLL CUtility
{
private:
    explicit CUtility();
    ~CUtility();

public:
    /** 폴더 안의 이미지를 이름 상관없이 모두 불러오는 함수입니다. */
    static CTexture* Load_Texture_Folder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFolderPath);
    static _vector Get_MousePos(HWND hWnd, const _uint& iWinSizeX, const _uint& iWinSizeY);

private:
    static wstring Get_LastPart(const wstring& WString, const wstring Delimiter);

};

