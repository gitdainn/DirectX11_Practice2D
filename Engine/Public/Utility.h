#pragma once
#include "Engine_Defines.h"
#include "Texture.h"

class ENGINE_DLL CUtility
{
private:
    explicit CUtility();
    ~CUtility();

public:
    /** ���� ���� �̹����� �̸� ������� ��� �ҷ����� �Լ��Դϴ�. */
    static CTexture* Load_Texture_Folder(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const char* pFolderPath);
};

