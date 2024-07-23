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
    static HRESULT WriteFile();

public:
    virtual void Free() override;
};

END