#pragma once
#include "Base.h"
#include "Tool_Defines.h"
#include "SpriteObject.h"

BEGIN(Tool)

class CFileLoader : public CBase
{
    DECLARE_SINGLETON(CFileLoader)

private:
    explicit CFileLoader();
    ~CFileLoader();

public:
    HRESULT Load_FIle(const _tchar* pFilePath, LEVEL eLevel);
    HRESULT Load_Excel(const _tchar* pFilePath, LEVEL eLevel, vector<CSpriteObject*>& LoadObjectVec);

    HRESULT Load_OriginalData_Excel(const _tchar* pFilePath, LEVEL eLevel);
    HRESULT Load_ObjectTransform_Excel(const _tchar* pFilePath);
    HRESULT Load_ComponentInfo_Excel(const _tchar* pFilePath);

    HRESULT Write_ObjectMetaData_Excel(const _tchar* pFilePath, const OBJECT_METADATA& tMetaData, bool& bIsReset) const;
    HRESULT Write_ObjectTransform_Excel(const _tchar* pFilePath, const OBJECT_TRANSFORM& tMetaData, bool& bIsReset) const;
    HRESULT Write_ComponentInfo_Excel(const _tchar* pFilePath, const COMPONENT_INFO& tMetaData, bool& bIsReset) const;

public:
    /** @qurious - 구조체도 반한 시 실패인지 성공인지 구분 불가라 반환값으로 보내면 안되나? */
    HRESULT Get_ObjectTransform(const int& iObjectID, OBJECT_TRANSFORM& tTransform) const
    {
        //unordered_map<int, SPRITE_INFO>::iterator iter = m_ObjectTransformMap.find(iObjectID);
        auto iter = m_ObjectTransformMap.find(iObjectID);

        if (iter == m_ObjectTransformMap.end())
        {
            return E_FAIL;
        }

        memcpy(&tTransform, &(*iter).second, sizeof(OBJECT_TRANSFORM));
        return S_OK;
    }
    HRESULT Get_ComponentInfoList(const int& iObjectID, list<COMPONENT_INFO>& ComponentList) const
    {
        auto iter = m_ComponentInfoMap.find(iObjectID);
        if (m_ComponentInfoMap.end() == iter)
        {
            return E_FAIL;
        }

        /** @note - list<COMPONENT_INFO>*로 반환하고 싶은데 iter->second로 접근 시 &로 반환이 불가하다. 멤버 변수만 가능 (지역변수는 소멸이라 X) */
        ComponentList = iter->second;
        return S_OK;
    }

    HRESULT Get_OriginalData(const _tchar* pClass, ORIGINAL_DATA& tOriginalData) const
    {
        auto iter = m_OriginalDataMap.find(pClass);
        if (m_OriginalDataMap.end() == iter)
        {
            return E_FAIL;
        }

        /** @note - list<COMPONENT_INFO>*로 반환하고 싶은데 iter->second로 접근 시 &로 반환이 불가하다. 멤버 변수만 가능 (지역변수는 소멸이라 X) */
        memcpy(&tOriginalData, &(*iter).second, sizeof(ORIGINAL_DATA));
        return S_OK;
    }

    //@note - const 함수는 멤버 값 변경이 아니더라도 주소 참조 불가능 (주소 참조하여 포인터 반환X)
    unordered_map<const _tchar*, ORIGINAL_DATA>* Get_OriginalDatas()
    {
        return &m_OriginalDataMap;
    }

public:
    void    Set_ObjectTransformMap(const unordered_map<int, OBJECT_TRANSFORM>& ObjectTransformMap) { m_ObjectTransformMap = ObjectTransformMap; }

    void    Set_ComponentInfoMap(const unordered_map<int, list<COMPONENT_INFO>>& ComponentInfoMap) { m_ComponentInfoMap = ComponentInfoMap; }

private:
    _tchar* Copy_WChar(const _tchar* pWChar);
    void Clear_LoadInfo();

private:
    const int m_iFirstCol;
    const int m_iFirstRow;

private:
    unordered_map<int, OBJECT_TRANSFORM> m_ObjectTransformMap;
    unordered_map<int, list<COMPONENT_INFO>> m_ComponentInfoMap;
    unordered_map<const _tchar*, ORIGINAL_DATA> m_OriginalDataMap;

private: // Sheet Num //
    const int m_iOriginalData = { 0 };
    const int m_iObjectMetaData = { 0 };
    const int m_iObjectTransform = { 1 };
    const int m_iComponentInfo = { 2 };

protected:
    inline virtual void	Free(void) override;
};

END