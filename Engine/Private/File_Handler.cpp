#include "File_Handler.h"
#include "libxl.h"

//IMPLEMENT_SINGLETON(CFile_Handler)

using namespace libxl;

HRESULT CFile_Handler::AddSheet(const _tchar* pSheetName, const _tchar* pSavePath)
{
    Book* pBook = xlCreateBook();

    if (nullptr == pBook)
    {
        pBook->release();
        return E_FAIL;
    }

    Sheet* pSheet = pBook->addSheet(pSheetName);
    if (nullptr == pSheet)
    {
        pBook->release();
        return E_FAIL;
    }

    //pSheet->writeStr(2, 1, L"Hello, World!");
    //pSheet->writeNum(4, 1, 1000);

    //Font* pFont = pBook->addFont();
    //if (nullptr == pFont)
    //{
    //    pBook->release();
    //    return E_FAIL;
    //}

    //pFont->setColor(COLOR_BLUE);

    pBook->save(pSavePath);
    pBook->release();

    return S_OK;
}

HRESULT CFile_Handler::GetSheet(const _tchar* pSheetName, const _tchar* pSavePath)
{
    return E_NOTIMPL;
}

/** @error - 지역변수를 반환값으로 보내면 안될텐디ㅔ.. 그리고 list나 vector같은 컨테이너는 단순 반환하면 안될텐데..? */
list<tObjectMetaData> CFile_Handler::ReadObjectMetaData(const _tchar* pExcelPath)
{
    Book* pBook = xlCreateXMLBookW();
    if (nullptr == pBook)
    {
        pBook->release();
        return list<tObjectMetaData>();
    }

    list<tObjectMetaData> OMDList;
    if (pBook->load(pExcelPath))
    {
        Sheet* pSheet = pBook->getSheet(0);
        if (nullptr == pSheet)
        {
            pBook->release();
            return list<tObjectMetaData>();
        }

        const int iFirstRow = pSheet->firstRow();
        const int iFirstCol = pSheet->firstCol();
        const int iLastRow = pSheet->lastRow();
        const int iLastCol = pSheet->lastCol();

        tObjectMetaData tOMD;
        for (_uint iCol = iFirstCol; iCol < iLastCol; ++iCol)
        {
            for (_uint iRow = iFirstRow; iRow <= iLastRow; ++iRow)
            {
                tOMD.iObjectID = pSheet->readNum(iRow, iFirstCol + (int)METADATA::OBJECT_ID);
                tOMD.iParentID = pSheet->readNum(iRow, iFirstCol + (int)METADATA::PARENT_ID);
                tOMD.pClassName = pSheet->readStr(iRow, iFirstCol + (int)METADATA::CLASSNAME);
                tOMD.pPrototypeTag = pSheet->readStr(iRow, iFirstCol + (int)METADATA::PROTOTYPETAG);
                tOMD.pTextureTag = pSheet->readStr(iRow, iFirstCol + (int)METADATA::TEXTURETAG);
                tOMD.pLayer = pSheet->readStr(iRow, iFirstCol + (int)METADATA::LAYER);
                OMDList.emplace_back(tOMD);
            }
        }
    }

    pBook->release();
    return OMDList;
}

tObjectTransform CFile_Handler::ReadObjectTransform()
{
    return tObjectTransform();
}

HRESULT CFile_Handler::WriteObjectMetaData(const _tchar* pExcelPath, const tObjectMetaData& tOMD)
{
    Book* pBook = xlCreateXMLBookW();
    if (nullptr == pBook)
    {
        pBook->release();
        return E_FAIL;
    }

    if (pBook->load(pExcelPath))
    {
        Sheet* pSheet = pBook->getSheet(0);
        if (nullptr == pSheet)
        {
            pBook->release();
            return E_FAIL;
        }

        const int iRow = pSheet->lastRow() + 1;
        const int iFirstCol = pSheet->firstCol();

        pSheet->writeNum(iRow, iFirstCol + (int)METADATA::OBJECT_ID, tOMD.iObjectID);
        pSheet->writeNum(iRow, iFirstCol + (int)METADATA::PARENT_ID, tOMD.iParentID);
        pSheet->writeStr(iRow, iFirstCol + (int)METADATA::CLASSNAME, tOMD.pClassName);
        pSheet->writeStr(iRow, iFirstCol + (int)METADATA::PROTOTYPETAG, tOMD.pPrototypeTag);
        pSheet->writeStr(iRow, iFirstCol + (int)METADATA::TEXTURETAG, tOMD.pTextureTag);
        pSheet->writeStr(iRow, iFirstCol + (int)METADATA::LAYER, tOMD.pLayer);
    }

    pBook->release();
    return S_OK;
}

HRESULT CFile_Handler::WriteObjectTransform()
{
    return S_OK;
}

void CFile_Handler::Free()
{
    __super::Free();
}
