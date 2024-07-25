#pragma once

#include "Base.h"

BEGIN(Engine)

//class CFile_Handler final : CBase
//{
//    DECLARE_SINGLETON(CFile_Handler)
//
//private:
//    CFile_Handler();
//    virtual ~CFile_Handler() = default;
//
//public:
//    HRESULT WriteFile();
//
//public:
//    virtual void Free() override;
//};

class ENGINE_DLL CFile_Handler final : CBase
{
private:
    CFile_Handler();
    virtual ~CFile_Handler() = default;

public:
    static HRESULT AddSheet(const _tchar* pSheetName, const _tchar* pSavePath);
    static HRESULT GetSheet(const _tchar* pSheetName, const _tchar* pSavePath);

    static list<tObjectMetaData> ReadObjectMetaData(const _tchar* pExcelPath);
    static tObjectTransform ReadObjectTransform();
    static HRESULT WriteObjectMetaData(const _tchar* pExcelPath, const tObjectMetaData& tOMD);
    static HRESULT WriteObjectTransform();

public:
    virtual void Free() override;
};

END