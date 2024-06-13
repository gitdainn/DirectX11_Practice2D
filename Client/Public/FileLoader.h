#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CFileLoader : public CBase
{
private:
    explicit CFileLoader();
    ~CFileLoader();

public:
    static HRESULT Load_FIle(const _tchar* pFilePath, LEVEL eLevel);
};

END