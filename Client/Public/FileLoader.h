#pragma once
#include "Base.h"
#include "Client_Defines.h"

BEGIN(Client)

class CFileLoader : public CBase
{
    DECLARE_SINGLETON(CFileLoader)

private:
    explicit CFileLoader();
    ~CFileLoader();

private:
    enum SHEET { SH_OBJECT_METADATA, SH_OBJECT_INFO, SH_COMPONENT };

public:
    HRESULT Load_FIle(const _tchar* pFilePath, LEVEL eLevel);
    HRESULT	Load_Line(const _tchar* pFilePath, ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
    HRESULT Load_Excel(const _tchar* pFilePath, const LEVEL eLevel);

    HRESULT Load_ObjectTransform_Excel(const _tchar* pFilePath);
    HRESULT Load_ComponentInfo_Excel(const _tchar* pFilePath);

public:
    HRESULT Load_SkulData_Excel(const _tchar* pFilePath);
    HRESULT Load_SkillData_Excel(const _tchar* pFilePath);

public:
    HRESULT Add_PrototypeName(const _tchar* pID, const _tchar* pPrototype)
    {
        if (nullptr == pID || nullptr == pPrototype)
            return E_FAIL;

        auto iter = find_if(m_IDPrototypeNameMap.begin(), m_IDPrototypeNameMap.end(), CTag_Finder(pID));
        if (iter != m_IDPrototypeNameMap.end())
            return E_FAIL;

        m_IDPrototypeNameMap.insert({ pID, pPrototype });
        return S_OK;
    }

public:
    HRESULT Get_OpenFileName(OPENFILENAME& tOpenFileName);

public:
    const _tchar* Get_PrototypeName(const _tchar* pID)
    {
        if (nullptr == pID)
            return nullptr;

        auto iter = find_if(m_IDPrototypeNameMap.begin(), m_IDPrototypeNameMap.end(), CTag_Finder(pID));
        if (iter == m_IDPrototypeNameMap.end())
            return nullptr;

        return iter->second;
    }
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

    HRESULT Get_SkulData(const _tchar* pObjectID, SKUL_EXCEL& SkulData) const
    {
        if (nullptr == pObjectID)
            return E_FAIL;

        auto iter = find_if(m_SkulDataMap.begin(), m_SkulDataMap.end(), CTag_Finder(pObjectID));

        if (iter == m_SkulDataMap.end())
            return E_FAIL;

        memcpy(&SkulData, &(iter->second), sizeof(SKUL_EXCEL));

        return S_OK;
    }

    HRESULT Get_SkillData(const _tchar* pObjectID, SKILL_EXCEL& SkillData) const
    {
        if (nullptr == pObjectID)
            return E_FAIL;

        auto iter = find_if(m_SkillDataMap.begin(), m_SkillDataMap.end(), CTag_Finder(pObjectID));

        if (iter == m_SkillDataMap.end())
            return E_FAIL;

        memcpy(&SkillData, &(iter->second), sizeof(SKILL_EXCEL));

        return S_OK;
    }

private:
    // @note - readStr로 읽으면 여러 개로 선언한 const _tchar*이 모두 같은 주소를 가리키는 현상 때문에 동적할당
    _tchar* Copy_WChar(const _tchar* pWChar);
    SKUL_RANK Translate_SkulRank(const _tchar* pRank) const;
    SKUL_TYPE Translate_SkulType(const _tchar* pType) const;
    SKILL_TYPE Translate_SkillType(const _tchar* pType) const;

private:
    const int m_iFirstCol;
    const int m_iFirstRow;

private:
    unordered_map<int, OBJECT_TRANSFORM> m_ObjectTransformMap;
    unordered_map<int, list<COMPONENT_INFO>> m_ComponentInfoMap;

private: // Sheet Num //
    const int m_iOriginalData = { 0 };
    const int m_iObjectMetaData = { 0 };
    const int m_iObjectTransform = { 1 };
    const int m_iComponentInfo = { 2 };

private: // 기획자에게 넘겨받는 엑셀 //
    unordered_map<const _tchar*, SKUL_EXCEL> m_SkulDataMap;
    unordered_map<const _tchar*, SKILL_EXCEL> m_SkillDataMap;
    unordered_map<const _tchar*, const _tchar*> m_IDPrototypeNameMap;

protected:
    inline virtual void	Free(void) override;
};

END