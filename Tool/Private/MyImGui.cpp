#include "..\Public\MyImGui.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "DInput_Manager.h"
#include "Utility.h"
#include <array>
#include <memory>
#include "FileLoader.h"

typedef void (*ImGuiDemoMarkerCallback)(const char* file, int line, const char* section, void* user_data);
extern ImGuiDemoMarkerCallback      GImGuiDemoMarkerCallback;
extern void* GImGuiDemoMarkerCallbackUserData;
//ImGuiDemoMarkerCallback             GImGuiDemoMarkerCallback = NULL;
#define IMGUI_DEMO_MARKER(section)  do { if (GImGuiDemoMarkerCallback != NULL) GImGuiDemoMarkerCallback(__FILE__, __LINE__, section, GImGuiDemoMarkerCallbackUserData); } while (0)

USING(Tool)

IMPLEMENT_SINGLETON(CMyImGui);

CMyImGui::CMyImGui()
    : m_pSelectedObject(nullptr)
    , m_pSpriteListIndex(nullptr)
    , m_iFolderIndex(0), m_iClassIndex(0)
{
    m_CreateObjectVec.reserve(100);
}

HRESULT CMyImGui::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    if (nullptr == pDevice || nullptr == pContext)
    {
        return E_FAIL;
    }

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(pDevice, pContext);

    CFileLoader* pFileLoader = CFileLoader::GetInstance();
    if (nullptr == pFileLoader)
    {
        return E_FAIL;
    }
    Safe_AddRef(pFileLoader);
    unordered_map<const _tchar*, ORIGINAL_DATA>* pOriginalData = pFileLoader->Get_OriginalDatas();
    m_OriginalDataMap = *pOriginalData;
    Safe_Release(pFileLoader);

#pragma region 폴더명
    m_FolderNameVec.emplace_back("GrimReaperUV");
    m_FolderNameVec.emplace_back("Enemy_Solider");
    m_FolderNameVec.emplace_back("ForestTile");
    m_FolderNameVec.emplace_back("ForestEnvironment");
    m_FolderNameVec.emplace_back("Background");
    m_FolderNameVec.emplace_back("UI_Skul");
    m_FolderNameVec.emplace_back("UI_HealthBar");
    m_FolderNameVec.emplace_back("UI_Frame");
    m_FolderNameVec.emplace_back("UI_Key");
    m_pSpriteListIndex = make_unique<_uint[]>(m_FolderNameVec.size());
#pragma endregion

#pragma region 클래스명
    m_ClassNameVec.emplace_back("Environment");
    m_ClassNameVec.emplace_back("Tile");
    m_ClassNameVec.emplace_back("BackGround");
    m_ClassNameVec.emplace_back("Player"); // 기본 플레이어 시작위치 (원래는 LittleBorn)
    m_ClassNameVec.emplace_back("Solider"); // Enemy클래스로 생성 시 알아서 각 객체로 만들어지는 기능 만들면 좋을듯
#pragma endregion

#pragma region 렌더그룹
    m_RenderGroupVec.reserve((_uint)CRenderer::RENDER_END);
    m_RenderGroupVec.emplace_back("RENDER_PRIORITY");
    m_RenderGroupVec.emplace_back("RENDER_NONBLEND");
    m_RenderGroupVec.emplace_back("RENDER_TILE");
    m_RenderGroupVec.emplace_back("RENDER_BLEND");
    m_RenderGroupVec.emplace_back("RENDER_UI");
#pragma endregion

#pragma region 레이어
    const _uint iLayerNum = { 7 };
    m_LayerVec.reserve(iLayerNum);
    m_LayerVec.emplace_back(make_pair("Layer_Default", 0));
    m_LayerVec.emplace_back(make_pair("Layer_Background", 0));
    m_LayerVec.emplace_back(make_pair("Layer_Effect", 0));
    m_LayerVec.emplace_back(make_pair("Layer_Item", 0));
    m_LayerVec.emplace_back(make_pair("Layer_UI", 0));
    m_LayerVec.emplace_back(make_pair("Layer_UI_MainSkul_State", 0));
    m_LayerVec.emplace_back(make_pair("LAYER_SUBSKUL_UI", 0));
#pragma endregion

#pragma region 충돌
    m_ComponentsVec.reserve(m_iComponentsNum);
    m_ComponentsVec.emplace_back("Collider_AABB");
    m_ComponentsVec.emplace_back("Collider_OBB");
#pragma endregion

#pragma region 충돌 레이어
    const _uint iColliderLayerNum = { 4 };
    m_ColliderLayer.reserve(iColliderLayerNum);
    m_ColliderLayer.emplace_back("Coll_Default");
    m_ColliderLayer.emplace_back("Coll_UI");
    m_ColliderLayer.emplace_back("Coll_Player");
    m_ColliderLayer.emplace_back("Coll_Enemy");
#pragma endregion

    Ready_ContainerResource(m_FolderNameVec);

    return S_OK;
}

_uint CMyImGui::Tick(_double TimeDelta)
{
    Key_Input(TimeDelta);

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    if (pGameInstance->Get_KeyDown(DIK_H))
    {
        //CFile_Handler::WriteFile();
    }

    return _uint();
}

_uint CMyImGui::LateTick(_double TimeDelta)
{
    return _uint();
}

HRESULT CMyImGui::Render()
{
    ImGui_ImplDX11_NewFrame();
    ImGui_ImplWin32_NewFrame();
    ImGui::NewFrame();

    if (FAILED(ShowDemoWindow()))
    {
        MSG_BOX("CMyImGui - Render - FAILED");
    }

    if (FAILED(ShowInstalledWindow()))
    {
        MSG_BOX("CMyImGui - Render - FAILED");
    }

    if (FAILED(ShowSpriteWindow()))
    {
        MSG_BOX("CMyImGui - Render - FAILED");
    }

    if (FAILED(ShowInspectorWindow()))
    {
        MSG_BOX("CMyImGui - Render - FAILED");
    }

    // @error - 레이어 충돌 행렬은 일단 보류
    //if (FAILED(ShowSettings()))
    //{
    //    MSG_BOX("CMyImGui - Render - FAILED");
    //}

    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return S_OK;
}

HRESULT CMyImGui::Save_Line()
{
    OPENFILENAME OFN;
    if (FAILED(Get_OpenFileName(OFN)))
        return E_FAIL;

    const _tchar* pFileName = OFN.lpstrFile;
    HANDLE hFile = CreateFile(pFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (INVALID_HANDLE_VALUE == hFile)
        return E_FAIL;

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    list<Engine::LINE_INFO> LineList;
    if (FAILED(pGameInstance->Get_LineList(LineList)))
    {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }
    Safe_Release(pGameInstance);

    if (LineList.empty())
        return S_OK;

    DWORD dwByte = 0;
    _uint iListSize = (_uint)LineList.size();
    WriteFile(hFile, &iListSize, sizeof(_uint), &dwByte, nullptr); // 길이 저장

    for (LINE_INFO tLine : LineList) // 현재 선택된 그룹 저장
    {
        // 저장 전 스크롤 값 제거해야함.
        tLine.tLeftVertex.position.x -= pGameInstance->Get_ScrollX();
        tLine.tRightVertex.position.x -= pGameInstance->Get_ScrollX();
        tLine.tLeftVertex.position.y -= pGameInstance->Get_ScrollY();
        tLine.tRightVertex.position.y -= pGameInstance->Get_ScrollY();
        WriteFile(hFile, &tLine, sizeof(LINE_INFO), &dwByte, nullptr);
    }

    CloseHandle(hFile);
    MSG_BOX("Save SUCCESS");

    return S_OK;
}

HRESULT CMyImGui::Load_Line()
{
    OPENFILENAME OFN;
    if (FAILED(Get_OpenFileName(OFN)))
        return E_FAIL;

    const TCHAR* pFileName = OFN.lpstrFile;

    HANDLE hFile = CreateFile(pFileName, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
        return E_FAIL;

    DWORD   dwByte = 0;
    _bool      bRes = { false };

    _uint iListSize = { 0 };
    bRes = ReadFile(hFile, &iListSize, sizeof(_uint), &dwByte, nullptr);

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    for (_uint i = 0; i < iListSize; ++i)
    {
        LINE_INFO tLine;
        bRes = ReadFile(hFile, &tLine, sizeof(LINE_INFO), &dwByte, nullptr);
        pGameInstance->Add_Vertex(tLine.tLeftVertex);
        pGameInstance->Add_Vertex(tLine.tRightVertex);
    }
    Safe_Release(pGameInstance);

    CloseHandle(hFile);

    return S_OK;
}

HRESULT CMyImGui::Save_FileName_Object()
{
    if (m_CreateObjectVec.empty())
    {
        MSG_BOX("Save_Object is Empty");
        return S_OK;
    }

    OPENFILENAME OFN;
    if (FAILED(Get_OpenFileName(OFN)))
        return E_FAIL;

    const _tchar* pFileName = OFN.lpstrFile;
    HANDLE hFile = CreateFile(pFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (INVALID_HANDLE_VALUE == hFile)
        return E_FAIL;

    DWORD dwByte = 0;

    const _uint iVecSize = (_uint)m_CreateObjectVec.size();
    WriteFile(hFile, &iVecSize, sizeof(_uint), &dwByte, nullptr); // 길이 저장

    _uint iLength = { 0 };
    _tchar szBuffer[MAX_PATH]{};

    static _uint iObjectInstance = { 0 };
    for (CSpriteObject* pObject : m_CreateObjectVec)
    {
        if (nullptr == pObject)
            continue;

#pragma region 텍스처 파일명 저장
        CTexture* pTextureCom = dynamic_cast<CTexture*>(pObject->Get_Component(TAG_TEXTURE));
        if (nullptr == pTextureCom)
            return S_OK;

        const vector<const _tchar*>* pTexturePathVec = pTextureCom->Get_TexturePathVec();
        if (nullptr == pTexturePathVec)
            return S_OK;

        const _uint iTextureIndex = pObject->Get_TextureIndex();
        const _tchar* pFilePath = (*pTexturePathVec)[iTextureIndex]; // 경로 받아오기
        _tchar		szFileName[MAX_PATH] = TEXT("");
        _wsplitpath_s(pFilePath, nullptr, 0, nullptr, 0, szFileName, MAX_PATH, nullptr, 0); // 파일명 받아오기
#pragma endregion

        ++iObjectInstance;
#pragma region 메타 데이터 저장
        SPRITE_INFO tSpriteInfo = pObject->Get_SpriteInfo();
        OBJECT_METADATA tMetaData;
        tMetaData.iInstanceID = iObjectInstance;
        tMetaData.pObjectID = szFileName;
        tMetaData.pNameTag = pObject->Get_NameTag();
        tMetaData.pClassName = pObject->Get_ClassName();
        tMetaData.pLayerTag = pObject->Get_Layer();
        tMetaData.iOrder = pObject->Get_Order();
        WriteFile(hFile, &tMetaData, sizeof(OBJECT_METADATA), &dwByte, nullptr); // 구조체 저장

        iLength = (lstrlen(tMetaData.pObjectID) + 1) * sizeof(_tchar);
        WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        lstrcpy(szBuffer, tMetaData.pObjectID);
        WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);

        iLength = (lstrlen(tMetaData.pNameTag) + 1) * sizeof(_tchar);
        WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        lstrcpy(szBuffer, tMetaData.pNameTag);
        WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);

        iLength = (lstrlen(tMetaData.pClassName) + 1) * sizeof(_tchar);
        WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        lstrcpy(szBuffer, tMetaData.pClassName);
        WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);

        lstrcpy(szBuffer, tMetaData.pLayerTag);
        iLength = (lstrlen(szBuffer) + 1) * sizeof(_tchar);
        WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);
#pragma endregion

#pragma region 트랜스폼 저장
        OBJECT_TRANSFORM tTransform;
        tTransform.iInstanceID = iObjectInstance;
        tTransform.pObjectID = { nullptr };
        tTransform.fSize.x = tSpriteInfo.fSize.x;
        tTransform.fSize.y = tSpriteInfo.fSize.y;
        tTransform.fSizeRatio.x = tSpriteInfo.fSizeRatio.x;
        tTransform.fSizeRatio.y = tSpriteInfo.fSizeRatio.y;

        CTransform* pObjectTransCom = pObject->Get_TransformCom();
        if (nullptr == pObjectTransCom)
        {
            MSG_BOX("CMyImGui - Save_Object_Excel() - FAILED");
            continue;
        }
        _vector vPosition = pObjectTransCom->Get_State(CTransform::STATE_POSITION);
        tTransform.fPosition = _float2(XMVectorGetX(vPosition), XMVectorGetY(vPosition));
        tTransform.fRotation = _float2(0.f, 0.f);

        WriteFile(hFile, &tTransform, sizeof(OBJECT_TRANSFORM), &dwByte, nullptr); // 구조체 저장

        iLength = (lstrlen(tTransform.pObjectID) + 1) * sizeof(_tchar);
        WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        lstrcpy(szBuffer, L" ");
        WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);

#pragma endregion

#pragma region 컴포넌트 저장
        unordered_map<const _tchar*, CComponent*>* pComponents = pObject->Get_Components();
        if (nullptr == pComponents)
        {
            MSG_BOX("CMyImGui - Save_Object_Excel - NULL");
            continue;
        }

        list<CComponent*> SaveComponentList;
        SaveComponentList.emplace_back(pObject->Get_Component(TAG_COLL_AABB));
        SaveComponentList.emplace_back(pObject->Get_Component(TAG_TEXTURE));
        SaveComponentList.remove(nullptr); // List는 remove에 erase + remove 기능 제공

        _uint iComponentNum = SaveComponentList.size();
        WriteFile(hFile, &iComponentNum, sizeof(_uint), &dwByte, nullptr); // 구조체 저장
        for (const auto& iter : *pComponents)
        {
            CComponent* pComponent = iter.second;
            if (nullptr == pComponent)
            {
                MSG_BOX("CMyImGui - Save_Object_Excel - NULL");
                continue;
            }

            COMPONENT_INFO tComponentInfo;
            ZeroMemory(&tComponentInfo, sizeof(COMPONENT_INFO));
            if (FAILED(pComponent->Get_ComponentInfo(tComponentInfo)))
            {
                continue;
            }
            tComponentInfo.iInstanceID = iObjectInstance;
            tComponentInfo.pComponentTag = iter.first;

            WriteFile(hFile, &tComponentInfo, sizeof(COMPONENT_INFO), &dwByte, nullptr); // 구조체 저장

            iLength = (lstrlen(tComponentInfo.pComponentTag) + 1) * sizeof(_tchar);
            WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
            lstrcpy(szBuffer, tComponentInfo.pComponentTag);
            WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);

            iLength = (lstrlen(tComponentInfo.pPrototypeTag) + 1) * sizeof(_tchar);
            WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
            lstrcpy(szBuffer, tComponentInfo.pPrototypeTag);
            WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);

            iLength = lstrlen(L" ") * sizeof(_tchar); // pSortingLayer
            WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
            lstrcpy(szBuffer, L" ");
            WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);
        }
#pragma endregion
    }

    MSG_BOX("SAVE SUCCESS");
    CloseHandle(hFile);

    return S_OK;
}

HRESULT CMyImGui::Load_FileName_Object()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    OPENFILENAME    OFN;
    if (FAILED(Get_OpenFileName(OFN)))
    {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    /* 로드 순서 (저장 순서와 동일): 메타 데이터 - 트랜스폼 - 컴포넌트 순 */
    const TCHAR* pFilePath = OFN.lpstrFile;
    HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        CloseHandle(hFile);
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    DWORD   dwByte = 0;
    _bool   bRes = { false };

    _uint iListSize = { 0 };
    bRes = ReadFile(hFile, &iListSize, sizeof(_uint), &dwByte, nullptr);

    CFileLoader* pFileLoader = CFileLoader::GetInstance();
    if (nullptr == pFileLoader)
    {
        MSG_BOX("CMyImGui - Load_Object_Excel() - FileLoader is NULL");
        CloseHandle(hFile);
        Safe_Release(pGameInstance);
        Safe_Release(pFileLoader);
        return E_FAIL;
    }
    Safe_AddRef(pFileLoader);

    _uint iLength = { 0 };
    unordered_map<int, OBJECT_TRANSFORM> ObjectTransformMap;
    unordered_map<int, list<COMPONENT_INFO>> ComponentInfoMap;

    for (_uint i = 0; i < iListSize; ++i)
    {
        _tchar szObjectID[MAX_PATH]{};
        _tchar szNameTag[MAX_PATH]{};
        _tchar szClassName[MAX_PATH]{};
        _tchar szLayer[MAX_PATH]{};

        _tchar szComponentInfo[MAX_PATH]{};

#pragma region 메타 데이터 로드
        OBJECT_METADATA tMetaData;
        bRes = ReadFile(hFile, &tMetaData, sizeof(OBJECT_METADATA), &dwByte, nullptr);

        bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        bRes = ReadFile(hFile, szObjectID, iLength, &dwByte, nullptr);

        bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        bRes = ReadFile(hFile, szNameTag, iLength, &dwByte, nullptr);

        bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        bRes = ReadFile(hFile, szClassName, iLength, &dwByte, nullptr);

        bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        bRes = ReadFile(hFile, szLayer, iLength, &dwByte, nullptr);
#pragma endregion

#pragma region 트랜스폼 로드
        OBJECT_TRANSFORM tTransform;
        bRes = ReadFile(hFile, &tTransform, sizeof(OBJECT_TRANSFORM), &dwByte, nullptr);

        bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        bRes = ReadFile(hFile, &tTransform.pObjectID, iLength, &dwByte, nullptr);

        ObjectTransformMap.emplace(tTransform.iInstanceID, tTransform);
        pFileLoader->Set_ObjectTransformMap(ObjectTransformMap);
#pragma endregion

#pragma region 컴포넌트 로드
        _uint iComponentNum = { 0 };
        bRes = ReadFile(hFile, &iComponentNum, sizeof(_uint), &dwByte, nullptr);

        COMPONENT_INFO tComponentInfo;
        for (_uint i = 0; i < iComponentNum; ++i)
        {
            bRes = ReadFile(hFile, &tComponentInfo, sizeof(COMPONENT_INFO), &dwByte, nullptr);

            bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
            bRes = ReadFile(hFile, szComponentInfo, iLength, &dwByte, nullptr);
            _tchar* pComponent = new _tchar[lstrlen(szComponentInfo) + 1]{ };
            lstrcpy(pComponent, szComponentInfo);
            tComponentInfo.pComponentTag = pComponent;
            pGameInstance->Add_Garbage(pComponent);

            bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
            bRes = ReadFile(hFile, szComponentInfo, iLength, &dwByte, nullptr);
            pComponent = new _tchar[lstrlen(szComponentInfo) + 1]{ };
            lstrcpy(pComponent, szComponentInfo);
            tComponentInfo.pPrototypeTag = pComponent;
            pGameInstance->Add_Garbage(pComponent);

            bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
            bRes = ReadFile(hFile, szComponentInfo, iLength, &dwByte, nullptr);
            pComponent = new _tchar[lstrlen(szComponentInfo) + 1]{ };
            lstrcpy(pComponent, szComponentInfo);
            tComponentInfo.pSortingLayer = pComponent;
            pGameInstance->Add_Garbage(pComponent);

            auto iter = ComponentInfoMap.find(tComponentInfo.iInstanceID);
            if (ComponentInfoMap.end() != iter)
            {
                (*iter).second.emplace_back(tComponentInfo);
            }
            else
            {
                list<COMPONENT_INFO> ComponentList;
                ComponentList.emplace_back(tComponentInfo);
                /** @qurious - 컨테이너에 삽입할 땐 복제되어 들어가는 것인가? (지역 변수면 소멸되니까 사라질테니!) */
                ComponentInfoMap.emplace(tComponentInfo.iInstanceID, ComponentList);
            }
        }
        pFileLoader->Set_ComponentInfoMap(ComponentInfoMap);
#pragma endregion

#pragma region 오브젝트 생성
        _tchar* pLayer = new _tchar[lstrlen(szLayer) + 1]{ };
        lstrcpy(pLayer, szLayer);

        if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Install"), (_uint)LEVEL_TOOL, pLayer, &tMetaData.iInstanceID)))
        {
            MSG_BOX("CMyImGui - Load_Object() - FAILED");
            CloseHandle(hFile);
            Safe_Release(pGameInstance);
            Safe_Release(pFileLoader);
            return E_FAIL;
        }

        list<CGameObject*>* pObjectList = pGameInstance->Get_ObjectList((_uint)LEVEL_TOOL, pLayer);
        if (nullptr == pObjectList)
        {
            MSG_BOX("CFileLoader - Load_Excel() - NULL");
        }
        CSpriteObject* pAddObject = dynamic_cast<CSpriteObject*>(pObjectList->back());
        if (nullptr != pAddObject)
        {
            static int iIndex = { 1 };

            char* pClassNameC = { nullptr };
            WCToC(szClassName, pClassNameC);

            char* pTag = new char[MAX_PATH] {};
            strcpy(pTag, pClassNameC);
            strcat(pTag, to_string(++iIndex).c_str());

            pAddObject->Set_SpriteTag(pTag);
            _tchar* pFileName = new _tchar[lstrlen(szObjectID) + 1]{};
            lstrcpy(pFileName, szObjectID);
            pAddObject->Set_SpriteFileName(pFileName); // 현재 ObjectID에 FileName에 저장해둠.

            pGameInstance->Add_Garbage(pFileName);
            pAddObject->Set_NameTag(szNameTag);
            pAddObject->Set_ClassName(szClassName);
            pAddObject->Set_TextureIndex(tComponentInfo.iTextureIndex);

            // 클래스, 레이어, 렌더그룹 지정
            CRenderer::RENDERGROUP eRenderGroup = CRenderer::RENDER_END;
            _tchar* pClassName = { nullptr };
            if (!strcmp("Background", pClassNameC))
            {
                eRenderGroup = CRenderer::RENDER_PRIORITY;
            }
            else if (!strcmp("Tile", pClassNameC))
            {
                eRenderGroup = CRenderer::RENDER_TILE;
            }
            else
            {
                eRenderGroup = CRenderer::RENDER_NONBLEND;
            }

            pAddObject->Set_RenderGroup(eRenderGroup);
            pAddObject->Set_Layer(pLayer, true);

            m_CreateObjectVec.emplace_back(dynamic_cast<CSpriteObject*>(pAddObject));
        }
#pragma endregion

        ComponentInfoMap.clear();
        ObjectTransformMap.clear();
    }
    CloseHandle(hFile);
    Safe_Release(pGameInstance);
    Safe_Release(pFileLoader);

    return S_OK;

}

HRESULT CMyImGui::Save_Object()
{
    // 저장은 전체 한번에 저장하면, Dat파일을 나눌 수 없어서 안됨.
    // 로드는 파일별로 자동으로 들어갈 수 있게 모델 인덱스까지 저장한다.
    if (m_CreateObjectVec.empty())
    {
        MSG_BOX("Save_Object is Empty");
        return S_OK;
    }

    OPENFILENAME OFN;
    if (FAILED(Get_OpenFileName(OFN)))
        return E_FAIL;

    const _tchar* pFileName = OFN.lpstrFile;
    HANDLE hFile = CreateFile(pFileName, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
    if (INVALID_HANDLE_VALUE == hFile)
        return E_FAIL;

     DWORD dwByte = 0;

     const _uint iVecSize = (_uint)m_CreateObjectVec.size();
     WriteFile(hFile, &iVecSize, sizeof(_uint), &dwByte, nullptr); // 길이 저장

     _uint iLength = { 0 };
     _tchar szBuffer[MAX_PATH]{};

     static _uint iObjectInstance = { 0 };
     for (CSpriteObject* pObject : m_CreateObjectVec)
     {
         if (nullptr == pObject)
             continue;

         ++iObjectInstance;
#pragma region 메타 데이터 저장
         SPRITE_INFO tSpriteInfo = pObject->Get_SpriteInfo();
         OBJECT_METADATA tMetaData;
         tMetaData.iInstanceID = iObjectInstance;
         tMetaData.pObjectID = { nullptr };
         tMetaData.pNameTag = pObject->Get_NameTag();
         tMetaData.pClassName = pObject->Get_ClassName();
         tMetaData.pLayerTag = pObject->Get_Layer();
         tMetaData.iOrder = pObject->Get_Order();
         WriteFile(hFile, &tMetaData, sizeof(OBJECT_METADATA), &dwByte, nullptr); // 구조체 저장

         iLength = (lstrlen(L" ") + 1) * sizeof(_tchar); // pObjectID
         WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
         lstrcpy(szBuffer, L" ");
         WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);

         iLength = (lstrlen(tMetaData.pNameTag) + 1) * sizeof(_tchar);
         WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
         lstrcpy(szBuffer, tMetaData.pNameTag);
         WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);

         iLength = (lstrlen(tMetaData.pClassName) + 1) * sizeof(_tchar);
         WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
         lstrcpy(szBuffer, tMetaData.pClassName);
         WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);

         lstrcpy(szBuffer, tMetaData.pLayerTag);
         iLength = (lstrlen(szBuffer) + 1) * sizeof(_tchar);
         WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
         WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);
#pragma endregion

#pragma region 트랜스폼 저장
         OBJECT_TRANSFORM tTransform;
         tTransform.iInstanceID = iObjectInstance;
         tTransform.pObjectID = { nullptr };
         tTransform.fSize.x = tSpriteInfo.fSize.x;
         tTransform.fSize.y = tSpriteInfo.fSize.y;
         tTransform.fSizeRatio.x = tSpriteInfo.fSizeRatio.x;
         tTransform.fSizeRatio.y = tSpriteInfo.fSizeRatio.y;

         CTransform* pObjectTransCom = pObject->Get_TransformCom();
         if (nullptr == pObjectTransCom)
         {
             MSG_BOX("CMyImGui - Save_Object_Excel() - FAILED");
             continue;
         }
         _vector vPosition = pObjectTransCom->Get_State(CTransform::STATE_POSITION);
         tTransform.fPosition = _float2(XMVectorGetX(vPosition), XMVectorGetY(vPosition));
         tTransform.fRotation = _float2(0.f, 0.f);
         
         WriteFile(hFile, &tTransform, sizeof(OBJECT_TRANSFORM), &dwByte, nullptr); // 구조체 저장

         iLength = (lstrlen(tTransform.pObjectID) + 1) * sizeof(_tchar);
         WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
         lstrcpy(szBuffer, L" ");
         WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);
#pragma endregion

#pragma region 컴포넌트 저장
         unordered_map<const _tchar*, CComponent*>* pComponents = pObject->Get_Components();
         if (nullptr == pComponents)
         {
             MSG_BOX("CMyImGui - Save_Object_Excel - NULL");
             continue;
         }

         list<CComponent*> SaveComponentList;
         SaveComponentList.emplace_back(pObject->Get_Component(TAG_COLL_AABB));
         SaveComponentList.emplace_back(pObject->Get_Component(TAG_TEXTURE));
         SaveComponentList.remove(nullptr); // List는 remove에 erase + remove 기능 제공

         _uint iComponentNum = SaveComponentList.size();
         WriteFile(hFile, &iComponentNum, sizeof(_uint), &dwByte, nullptr); // 구조체 저장
         for (const auto& iter : *pComponents)
         {
             CComponent* pComponent = iter.second;
             if (nullptr == pComponent)
             {
                 MSG_BOX("CMyImGui - Save_Object_Excel - NULL");
                 continue;
             }

             COMPONENT_INFO tComponentInfo;
             ZeroMemory(&tComponentInfo, sizeof(COMPONENT_INFO));
             if (FAILED(pComponent->Get_ComponentInfo(tComponentInfo)))
             {
                 continue;
             }
             tComponentInfo.iInstanceID = iObjectInstance;
             tComponentInfo.pComponentTag = iter.first;
             tComponentInfo.iTextureIndex = pObject->Get_TextureIndex();

#ifdef _DEBUG 폴더 내 이미지 추가로 리뉴얼된 인덱스로 셋팅 (파일 이름으로 이미지 가져올 때 사용함.)
            const _tchar* pFileName = pObject->Get_SpriteFileName();
            if (nullptr != pFileName)
            {
                CTexture* pTextureCom = dynamic_cast<CTexture*>(pComponent);
                if (nullptr != pTextureCom)
                {
                    const _uint iNewIndex = pTextureCom->Get_FileIndex(pObject->Get_SpriteFileName());
                    if (0 > iNewIndex)
                    {
                        MSG_BOX("CMyImGui - Save_Object() - 파일 이름이 없습니다.");
                        return S_OK;
                    }
                    tComponentInfo.iTextureIndex = iNewIndex;
                }
            }
#endif

             WriteFile(hFile, &tComponentInfo, sizeof(COMPONENT_INFO), &dwByte, nullptr); // 구조체 저장

             iLength = (lstrlen(tComponentInfo.pComponentTag) + 1) * sizeof(_tchar);
             WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
             lstrcpy(szBuffer, tComponentInfo.pComponentTag);
             WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);

             iLength = (lstrlen(tComponentInfo.pPrototypeTag) + 1) * sizeof(_tchar);
             WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
             lstrcpy(szBuffer, tComponentInfo.pPrototypeTag);
             WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);

             iLength = lstrlen(L" ") * sizeof(_tchar); // pSortingLayer
             WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
             lstrcpy(szBuffer, L" ");
             WriteFile(hFile, szBuffer, iLength, &dwByte, nullptr);


         }
#pragma endregion
     }

     MSG_BOX("SAVE SUCCESS");
     CloseHandle(hFile);

    return S_OK;
}

HRESULT CMyImGui::Load_Object()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    OPENFILENAME    OFN;
    if (FAILED(Get_OpenFileName(OFN)))
    {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    /* 로드 순서 (저장 순서와 동일): 메타 데이터 - 트랜스폼 - 컴포넌트 순 */
    const TCHAR* pFilePath = OFN.lpstrFile;
    HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        CloseHandle(hFile);
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    DWORD   dwByte = 0;
    _bool   bRes = { false };

    _uint iListSize = { 0 };
    bRes = ReadFile(hFile, &iListSize, sizeof(_uint), &dwByte, nullptr);

    CFileLoader* pFileLoader = CFileLoader::GetInstance();
    if (nullptr == pFileLoader)
    {
        MSG_BOX("CMyImGui - Load_Object_Excel() - FileLoader is NULL");
        CloseHandle(hFile);
        Safe_Release(pGameInstance);
        Safe_Release(pFileLoader);
        return E_FAIL;
    }
    Safe_AddRef(pFileLoader);

    _uint iLength = { 0 };
    unordered_map<int, OBJECT_TRANSFORM> ObjectTransformMap;
    unordered_map<int, list<COMPONENT_INFO>> ComponentInfoMap;

    for (_uint i = 0; i < iListSize; ++i)
    {
        _tchar szObjectID[MAX_PATH]{};
        _tchar szNameTag[MAX_PATH]{};
        _tchar szClassName[MAX_PATH]{};
        _tchar szLayer[MAX_PATH]{};

        _tchar szComponentInfo[MAX_PATH]{};

#pragma region 메타 데이터 로드
        OBJECT_METADATA tMetaData;
        bRes = ReadFile(hFile, &tMetaData, sizeof(OBJECT_METADATA), &dwByte, nullptr);

        bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        bRes = ReadFile(hFile, szObjectID, iLength, &dwByte, nullptr);

        bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        bRes = ReadFile(hFile, szNameTag, iLength, &dwByte, nullptr);

        bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        bRes = ReadFile(hFile, szClassName, iLength, &dwByte, nullptr);

        bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        bRes = ReadFile(hFile, szLayer, iLength, &dwByte, nullptr);
#pragma endregion

#pragma region 트랜스폼 로드
        OBJECT_TRANSFORM tTransform;
        bRes = ReadFile(hFile, &tTransform, sizeof(OBJECT_TRANSFORM), &dwByte, nullptr);

        bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        bRes = ReadFile(hFile, &tTransform.pObjectID, iLength, &dwByte, nullptr);

        ObjectTransformMap.emplace(tTransform.iInstanceID, tTransform);
        pFileLoader->Set_ObjectTransformMap(ObjectTransformMap);
#pragma endregion

#pragma region 컴포넌트 로드
        _uint iComponentNum = { 0 };
        bRes = ReadFile(hFile, &iComponentNum, sizeof(_uint), &dwByte, nullptr);

        COMPONENT_INFO tComponentInfo;
        for (_uint i = 0; i < iComponentNum; ++i)
        {
            bRes = ReadFile(hFile, &tComponentInfo, sizeof(COMPONENT_INFO), &dwByte, nullptr);

            bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
            bRes = ReadFile(hFile, szComponentInfo, iLength, &dwByte, nullptr);
            _tchar* pComponent = new _tchar[lstrlen(szComponentInfo) + 1]{ };
            lstrcpy(pComponent, szComponentInfo);
            tComponentInfo.pComponentTag = pComponent;
            pGameInstance->Add_Garbage(pComponent);

            bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
            bRes = ReadFile(hFile, szComponentInfo, iLength, &dwByte, nullptr);
            pComponent = new _tchar[lstrlen(szComponentInfo) + 1]{ };
            lstrcpy(pComponent, szComponentInfo);
            tComponentInfo.pPrototypeTag = pComponent;
            pGameInstance->Add_Garbage(pComponent);

            bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
            bRes = ReadFile(hFile, szComponentInfo, iLength, &dwByte, nullptr);
            pComponent = new _tchar[lstrlen(szComponentInfo) + 1]{ };
            lstrcpy(pComponent, szComponentInfo);
            tComponentInfo.pSortingLayer = pComponent;
            pGameInstance->Add_Garbage(pComponent);

            auto iter = ComponentInfoMap.find(tComponentInfo.iInstanceID);
            if (ComponentInfoMap.end() != iter)
            {
                (*iter).second.emplace_back(tComponentInfo);
            }
            else
            {
                list<COMPONENT_INFO> ComponentList;
                ComponentList.emplace_back(tComponentInfo);
                /** @qurious - 컨테이너에 삽입할 땐 복제되어 들어가는 것인가? (지역 변수면 소멸되니까 사라질테니!) */
                ComponentInfoMap.emplace(tComponentInfo.iInstanceID, ComponentList);
            }
        }
        pFileLoader->Set_ComponentInfoMap(ComponentInfoMap);
#pragma endregion

#pragma region 오브젝트 생성
        _tchar* pLayer = new _tchar[lstrlen(szLayer) + 1]{ };
        lstrcpy(pLayer, szLayer);

        if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Install"), (_uint)LEVEL_TOOL, pLayer, &tMetaData.iInstanceID)))
        {
            MSG_BOX("CMyImGui - Load_Object() - FAILED");
            CloseHandle(hFile);
            Safe_Release(pGameInstance);
            Safe_Release(pFileLoader);
            return E_FAIL;
        }

        list<CGameObject*>* pObjectList = pGameInstance->Get_ObjectList((_uint)LEVEL_TOOL, pLayer);
        if (nullptr == pObjectList)
        {
            MSG_BOX("CFileLoader - Load_Excel() - NULL");
        }
        CSpriteObject* pAddObject = dynamic_cast<CSpriteObject*>(pObjectList->back());
        if (nullptr != pAddObject)
        {
            static int iIndex = { 1 };

            char* pClassNameC = { nullptr };
            WCToC(szClassName, pClassNameC);

            char* pTag = new char[MAX_PATH] {};
            strcpy(pTag, pClassNameC);
            strcat(pTag, to_string(++iIndex).c_str());

            pAddObject->Set_SpriteTag(pTag);
            pAddObject->Set_NameTag(szNameTag);
            pAddObject->Set_ClassName(szClassName);
            pAddObject->Set_TextureIndex(tComponentInfo.iTextureIndex);

            // 클래스, 레이어, 렌더그룹 지정
            CRenderer::RENDERGROUP eRenderGroup = CRenderer::RENDER_END;
            _tchar* pClassName = { nullptr };
            if (!strcmp("Background", pClassNameC))
            {
                eRenderGroup = CRenderer::RENDER_PRIORITY;
            }
            else if (!strcmp("Tile", pClassNameC))
            {
                eRenderGroup = CRenderer::RENDER_TILE;
            }
            else
            {
                eRenderGroup = CRenderer::RENDER_NONBLEND;
            }

            pAddObject->Set_RenderGroup(eRenderGroup);
            pAddObject->Set_Layer(pLayer, true);
            pAddObject->Set_Order(tMetaData.iOrder);

            m_CreateObjectVec.emplace_back(dynamic_cast<CSpriteObject*>(pAddObject));
        }
#pragma endregion

        ComponentInfoMap.clear();
        ObjectTransformMap.clear();
    }
    CloseHandle(hFile);
    Safe_Release(pGameInstance);
    Safe_Release(pFileLoader);

    return S_OK;
}

HRESULT CMyImGui::Save_Widget()
{
    return E_NOTIMPL;
}

HRESULT CMyImGui::Load_Widget()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    OPENFILENAME    OFN;
    if (FAILED(Get_OpenFileName(OFN)))
    {
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    /* 로드 순서 (저장 순서와 동일): 메타 데이터 - 트랜스폼 - 컴포넌트 순 */
    const TCHAR* pFilePath = OFN.lpstrFile;
    HANDLE hFile = CreateFile(pFilePath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

    if (INVALID_HANDLE_VALUE == hFile)
    {
        CloseHandle(hFile);
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    DWORD   dwByte = 0;
    _bool   bRes = { false };

    _uint iListSize = { 0 };
    bRes = ReadFile(hFile, &iListSize, sizeof(_uint), &dwByte, nullptr);

    CFileLoader* pFileLoader = CFileLoader::GetInstance();
    if (nullptr == pFileLoader)
    {
        MSG_BOX("CMyImGui - Load_Object_Excel() - FileLoader is NULL");
        CloseHandle(hFile);
        Safe_Release(pGameInstance);
        Safe_Release(pFileLoader);
        return E_FAIL;
    }
    Safe_AddRef(pFileLoader);

    _uint iLength = { 0 };
    unordered_map<int, OBJECT_TRANSFORM> ObjectTransformMap;
    unordered_map<int, list<COMPONENT_INFO>> ComponentInfoMap;

    for (_uint i = 0; i < iListSize; ++i)
    {
        _tchar szObjectID[MAX_PATH]{};
        _tchar szNameTag[MAX_PATH]{};
        _tchar szClassName[MAX_PATH]{};
        _tchar szLayer[MAX_PATH]{};

        _tchar szComponentInfo[MAX_PATH]{};

#pragma region 메타 데이터 로드
        OBJECT_METADATA tMetaData;
        bRes = ReadFile(hFile, &tMetaData, sizeof(OBJECT_METADATA), &dwByte, nullptr);

        bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        bRes = ReadFile(hFile, szObjectID, iLength, &dwByte, nullptr);

        bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        bRes = ReadFile(hFile, szNameTag, iLength, &dwByte, nullptr);

        bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        bRes = ReadFile(hFile, szClassName, iLength, &dwByte, nullptr);

        bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        bRes = ReadFile(hFile, szLayer, iLength, &dwByte, nullptr);
#pragma endregion

#pragma region 트랜스폼 로드
        OBJECT_TRANSFORM tTransform;
        bRes = ReadFile(hFile, &tTransform, sizeof(OBJECT_TRANSFORM), &dwByte, nullptr);

        bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
        bRes = ReadFile(hFile, &tTransform.pObjectID, iLength, &dwByte, nullptr);

        ObjectTransformMap.emplace(tTransform.iInstanceID, tTransform);
        pFileLoader->Set_ObjectTransformMap(ObjectTransformMap);
#pragma endregion

#pragma region 컴포넌트 로드
        _uint iComponentNum = { 0 };
        bRes = ReadFile(hFile, &iComponentNum, sizeof(_uint), &dwByte, nullptr);

        COMPONENT_INFO tComponentInfo;
        for (_uint i = 0; i < iComponentNum; ++i)
        {
            bRes = ReadFile(hFile, &tComponentInfo, sizeof(COMPONENT_INFO), &dwByte, nullptr);

            bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
            bRes = ReadFile(hFile, szComponentInfo, iLength, &dwByte, nullptr);
            _tchar* pComponent = new _tchar[lstrlen(szComponentInfo) + 1]{ };
            lstrcpy(pComponent, szComponentInfo);
            tComponentInfo.pComponentTag = pComponent;
            pGameInstance->Add_Garbage(pComponent);

            bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
            bRes = ReadFile(hFile, szComponentInfo, iLength, &dwByte, nullptr);
            pComponent = new _tchar[lstrlen(szComponentInfo) + 1]{ };
            lstrcpy(pComponent, szComponentInfo);
            tComponentInfo.pPrototypeTag = pComponent;
            pGameInstance->Add_Garbage(pComponent);

            bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
            bRes = ReadFile(hFile, szComponentInfo, iLength, &dwByte, nullptr);
            pComponent = new _tchar[lstrlen(szComponentInfo) + 1]{ };
            lstrcpy(pComponent, szComponentInfo);
            tComponentInfo.pSortingLayer = pComponent;
            pGameInstance->Add_Garbage(pComponent);

            auto iter = ComponentInfoMap.find(tComponentInfo.iInstanceID);
            if (ComponentInfoMap.end() != iter)
            {
                (*iter).second.emplace_back(tComponentInfo);
            }
            else
            {
                list<COMPONENT_INFO> ComponentList;
                ComponentList.emplace_back(tComponentInfo);
                /** @qurious - 컨테이너에 삽입할 땐 복제되어 들어가는 것인가? (지역 변수면 소멸되니까 사라질테니!) */
                ComponentInfoMap.emplace(tComponentInfo.iInstanceID, ComponentList);
            }
        }
        pFileLoader->Set_ComponentInfoMap(ComponentInfoMap);
#pragma endregion

#pragma region 오브젝트 생성
        if (FAILED(pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Install"), (_uint)LEVEL_TOOL, szLayer, &tMetaData.iInstanceID)))
        {
            MSG_BOX("CMyImGui - Load_Object() - FAILED");
            CloseHandle(hFile);
            Safe_Release(pGameInstance);
            Safe_Release(pFileLoader);
            return E_FAIL;
        }

        list<CGameObject*>* pObjectList = pGameInstance->Get_ObjectList((_uint)LEVEL_TOOL, szLayer);
        if (nullptr == pObjectList)
        {
            MSG_BOX("CFileLoader - Load_Excel() - NULL");
        }
        CSpriteObject* pAddObject = dynamic_cast<CSpriteObject*>(pObjectList->back());
        if (nullptr != pAddObject)
        {
            static int iIndex = { 1 };
            char* pTag = new char[MAX_PATH] {};
            strcpy(pTag, "pObjectID");
            strcat(pTag, to_string(++iIndex).c_str());
            pAddObject->Set_SpriteTag(pTag);
            pAddObject->Set_NameTag(szNameTag);
            pAddObject->Set_ClassName(szClassName);

            _tchar* pLayer = new _tchar[lstrlen(szLayer) + 1]{ };
            lstrcpy(pLayer, szLayer);
            pAddObject->Set_Layer(pLayer, true);
            pAddObject->Set_Order(tMetaData.iOrder);

            m_CreateObjectVec.emplace_back(dynamic_cast<CSpriteObject*>(pAddObject));
        }
#pragma endregion

        ComponentInfoMap.clear();
        ObjectTransformMap.clear();
    }
    CloseHandle(hFile);
    Safe_Release(pGameInstance);
    Safe_Release(pFileLoader);

    return S_OK;
}

HRESULT CMyImGui::Save_Object_Excel()
{
    if (m_CreateObjectVec.empty())
    {
        MSG_BOX("CMyImGui - Save_Object_Excel - EMPTY");
        return S_OK;
    }

    OPENFILENAME OFN;
    if (FAILED(Get_OpenFileName(OFN)))
        return E_FAIL;

    CFileLoader* pFileLoader = CFileLoader::GetInstance();
    if (nullptr == pFileLoader)
    {
        MSG_BOX("CMyImGui - Save_Object_Excel - NULL");
        return E_FAIL;
    }
    Safe_AddRef(pFileLoader);

    const TCHAR* pFilePath = OFN.lpstrFile; // TEXT("../Bin/DataFiles/Level_Logo.xlsx");
    bool bIsReset = { true };

    static _uint iExcelInstance = { 5000 };
    for (CSpriteObject* pObject : m_CreateObjectVec)
    {
        if (nullptr == pObject)
            continue;

        ++iExcelInstance;
        SPRITE_INFO tSpriteInfo = pObject->Get_SpriteInfo();

        OBJECT_METADATA tMetaData;
        tMetaData.iInstanceID = iExcelInstance;
        tMetaData.pObjectID = { nullptr };
        tMetaData.pNameTag = pObject->Get_NameTag();
        tMetaData.pClassName = pObject->Get_ClassName();
        tMetaData.pLayerTag = pObject->Get_Layer();
        tMetaData.iOrder = pObject->Get_Order();

        if (FAILED(pFileLoader->Write_ObjectMetaData_Excel(pFilePath, tMetaData, bIsReset)))
        {
            MSG_BOX("CMyImGui - Save_Object_Excel() - FAILED");
        }

        OBJECT_TRANSFORM tTransform;
        tTransform.iInstanceID = iExcelInstance;
        tTransform.pObjectID = { nullptr };
        tTransform.fSize.x = tSpriteInfo.fSize.x;
        tTransform.fSize.y = tSpriteInfo.fSize.y;
        tTransform.fSizeRatio.x = tSpriteInfo.fSizeRatio.x;
        tTransform.fSizeRatio.y = tSpriteInfo.fSizeRatio.y;

        CTransform* pObjectTransCom = pObject->Get_TransformCom();
        if (nullptr == pObjectTransCom)
        {
            MSG_BOX("CMyImGui - Save_Object_Excel() - FAILED");
            continue;
        }
        _vector vPosition = pObjectTransCom->Get_State(CTransform::STATE_POSITION);
        tTransform.fPosition = _float2(XMVectorGetX(vPosition), XMVectorGetY(vPosition));
        tTransform.fRotation = _float2(0.f, 0.f);


        if (FAILED(pFileLoader->Write_ObjectTransform_Excel(pFilePath, tTransform, bIsReset)))
        {
            MSG_BOX("CMyImGui - Save_Object_Excel() - FAILED");
            continue;
        }

        unordered_map<const _tchar*, CComponent*>* pComponents = pObject->Get_Components();
        if (nullptr == pComponents)
        {
            MSG_BOX("CMyImGui - Save_Object_Excel - NULL");
            continue;
        }

        //@refactoring - 컴포넌트 하나당 매번 Sheet를 여는 건 매우 비효율적... 한번에 담아서 보내는 게 나을듯!
        for (auto iter : *pComponents)
        {
            CComponent* pComponent = iter.second;
            if (nullptr == pComponent)
            {
                MSG_BOX("CMyImGui - Save_Object_Excel - NULL");
                continue;
            }

            COMPONENT_INFO tComponentInfo;
            ZeroMemory(&tComponentInfo, sizeof(COMPONENT_INFO));
            if (FAILED(pComponent->Get_ComponentInfo(tComponentInfo)))
            {
                continue;
            }
            tComponentInfo.iInstanceID = iExcelInstance;
            tComponentInfo.pComponentTag = iter.first;
            tComponentInfo.iTextureIndex = pObject->Get_TextureIndex();

            if (FAILED(pFileLoader->Write_ComponentInfo_Excel(pFilePath, tComponentInfo, bIsReset)))
            {
                MSG_BOX("Save FAILED");
            }
        }

        bIsReset = false;
    }

    MSG_BOX("SAVE SUCCESS");
    Safe_Release(pFileLoader);

    return S_OK;
}

HRESULT CMyImGui::Load_Object_Excel()
{
    CFileLoader* pFileLoader = CFileLoader::GetInstance();
    if (nullptr == pFileLoader)
    {
        MSG_BOX("CMyImGui - Load_Object_Excel() - FileLoader is NULL");
        return E_FAIL;
    }
    Safe_AddRef(pFileLoader);

    OPENFILENAME    OFN;
    if (FAILED(Get_OpenFileName(OFN)))
        return E_FAIL;

    const TCHAR* pFilePath = OFN.lpstrFile;
    if (FAILED(pFileLoader->Load_ObjectTransform_Excel(pFilePath)))
    {
        MSG_BOX("CMyImGui - Load_Object_Excel() - Load_Excel FAILED");
        return E_FAIL;
    }

    if (FAILED(pFileLoader->Load_ComponentInfo_Excel(pFilePath)))
    {
        MSG_BOX("CMyImGui - Load_Object_Excel() - Load_Excel FAILED");
        return E_FAIL;
    }

    if (FAILED(pFileLoader->Load_Excel(pFilePath, LEVEL::LEVEL_TOOL, m_CreateObjectVec)))
    {
        MSG_BOX("CMyImGui - Load_Object_Excel() - Load_Excel FAILED");
        return E_FAIL;
    }

    Safe_Release(pFileLoader);
    return S_OK;
}

//HRESULT CMyImGui::Load_PreviousData()
//{
//    CGameInstance* pGameInstance = CGameInstance::GetInstance();
//    Safe_AddRef(pGameInstance);
//
//    /* 저장순서 : 게임오브젝트명, 벡터원소갯수, 텍스처프로토이름, 텍스처인덱스, UIRECTDESC 구조체, 색상, 선형보간, 액션 타입, 액션 구조체 */
//    if (ImGui::Button("Load"))
//    {
//        OPENFILENAME OFN;
//        TCHAR filePathName[128] = L"";
//        TCHAR lpstrFile[256] = L".data";
//        static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.BattleUIdat)\0*.BattleUIdat";
//
//        ZeroMemory(&OFN, sizeof(OPENFILENAME));
//        OFN.lStructSize = sizeof(OPENFILENAME);
//        OFN.hwndOwner = g_hWnd;
//        OFN.lpstrFilter = filter;
//        OFN.lpstrFile = lpstrFile;
//        OFN.nMaxFile = 256;
//        OFN.lpstrInitialDir = L"..\\Bin\\DataFiles";
//
//        if (GetOpenFileName(&OFN) != 0)
//        {
//            const TCHAR* pGetPath = OFN.lpstrFile;
//
//            HANDLE hFile = CreateFile(pGetPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);
//
//            if (INVALID_HANDLE_VALUE == hFile)
//                return E_FAIL;
//
//            DWORD   dwByte = 0;
//            _bool      bRes = { false };
//
//#pragma region LOAD
//            // Load : 현재 모델이 가지고 있는 이펙트 사이즈
//            _uint iVecSize = { 0 };
//            bRes = ReadFile(hFile, &iVecSize, sizeof(_uint), &dwByte, nullptr);
//
//            _uint	iTagIndex = { 0 };
//            for (_uint i = 0; i < iVecSize; ++i)
//            {
//                PREVIOUS_SPRITE_INFO tPreviousSpriteInfo;
//                bRes = ReadFile(hFile, &tPreviousSpriteInfo, sizeof(PREVIOUS_SPRITE_INFO), &dwByte, nullptr);
//
//                _uint iLength = { 0 };
//                bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
//                _tchar* pTag = new _tchar[iLength]{};
//                if (-1 != iLength)
//                {
//                    bRes = ReadFile(hFile, pTag, iLength, &dwByte, nullptr);
//                    tPreviousSpriteInfo.pTextureComTag = pTag;
//                }
//
//                tSpriteInfo tSpriteInfo;
//                tSpriteInfo.fPosition = tPreviousSpriteInfo.fPosition;
//                tSpriteInfo.fSize = tPreviousSpriteInfo.fSize;
//                tSpriteInfo.fSizeRatio = tPreviousSpriteInfo.fSizeRatio;
//                tSpriteInfo.iOrder = tPreviousSpriteInfo.iOrder;
//                tSpriteInfo.iTextureIndex = tPreviousSpriteInfo.iTextureIndex;
//                tSpriteInfo.pTextureComTag = tPreviousSpriteInfo.pTextureComTag;
//                tSpriteInfo.pPrototypeTag = nullptr;
//                tSpriteInfo.vColor = tPreviousSpriteInfo.vColor;
//
//                if (FAILED(Install_GameObject(tSpriteInfo)))
//                {
//                    MSG_BOX("CMyImGui - Load_Object() - FAILED");
//                    CloseHandle(hFile);
//                    Safe_Release(pGameInstance);
//                    return E_FAIL;
//                }
//            }
//            CloseHandle(hFile);
//        }
//    }
//
//    Safe_Release(pGameInstance);
//
//    return S_OK;
//}

inline HRESULT CMyImGui::ShowDemoWindow()
{
    static bool bIsShow = true;
    ImGui::ShowDemoWindow(&bIsShow);
    
    return S_OK;
}

HRESULT CMyImGui::ShowInstalledWindow()
{
    static bool bIsShow = true;
    ImGui::Begin("Create", &bIsShow);

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    static int iLayerSelectIndex = { 0 };
    static const char* Layers[] =
    {
        m_LayerVec[0].first,
        m_LayerVec[1].first,
        m_LayerVec[2].first,
        m_LayerVec[3].first,
        m_LayerVec[4].first,
        m_LayerVec[5].first,
        m_LayerVec[6].first,
    };

    ImGui::Combo("Layer", &iLayerSelectIndex, Layers, IM_ARRAYSIZE(Layers));
    m_pLayerC = m_LayerVec[iLayerSelectIndex].first;

    static int iSelectIndex = { 0 };
    if (ImGui::BeginListBox("InstalledObject"))
    {
        for (int iListIndex = 0; iListIndex < m_CreateObjectVec.size(); iListIndex++)
        {
            const bool is_selected = (iSelectIndex == iListIndex);
            // Selectable은 콜백 함수, const char* 첫번째 인자와 동일한 리스트를 선택 (즉, 문자열이 동일하면 먼저 찾은 리스트가 선택됨)  
            if (ImGui::Selectable(m_CreateObjectVec[iListIndex]->Get_SpriteTag(), is_selected))
            {
                if (nullptr != m_pSelectedObject)
                    m_pSelectedObject->Set_ShaderPass((_uint)VTXTEX_PASS::Default);
                m_pSelectedObject = m_CreateObjectVec[iListIndex];

                m_pSelectedObject->Set_ShaderPass((_uint)VTXTEX_PASS::Color);

                iSelectIndex = iListIndex; // 선택한 항목의 인덱스를 저장
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    if (ImGui::Button("Save Object"))
    {
        if (FAILED(Save_Object()))
        {
            MSG_BOX("CMyImGui - Save_Object - FAILED");
        }
    }
    ImGui::SameLine();

    if (ImGui::Button("Load Object"))
    {
        if (FAILED(Load_Object()))
        {
            MSG_BOX("CMyImGui - Load_Object - FAILED");
        }
    }

    if (ImGui::Button("Save Line"))
    {
        if (FAILED(Save_Line()))
       {
           MSG_BOX("CMyImGui - Save_Line - FAILED");
       }
    }
    ImGui::SameLine();

    if (ImGui::Button("Load Line"))
    {
        if (FAILED(Load_Line()))
        {
            MSG_BOX("CMyImGui - Load_Line - FAILED");
        }
    }

    if (ImGui::Button("Save Excel"))
    {
        if (FAILED(Save_Object_Excel()))
        {
            MSG_BOX("CMyImGui - Save_Line - FAILED");
        }
    }
    ImGui::SameLine();

    if (ImGui::Button("Load Excel"))
    {
        if (FAILED(Load_Object_Excel()))
        {
            MSG_BOX("CMyImGui - Load_Line - FAILED");
        }
    }


 /*   if(FAILED(Load_PreviousData()))
    {
        MSG_BOX("CMyImGui - Load_PreviousData - FAILED");
    }*/

    Safe_Release(pGameInstance);

    ImGui::End();
    return S_OK;
}

HRESULT CMyImGui::ShowSpriteWindow()
{
    static bool bIsShow = true;
    ImGui::Begin("Sprite", &bIsShow);

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

#pragma region ImGui 이미지 설명
    // 아래는 데모에서 접근할 수 있는 유일한 텍스처인 폰트 텍스처를 표시합니다.
    // 'ImTextureID' 타입을 사용하여 포인터나 텍스처 데이터를 식별할 수 있는 ID를 저장하세요.
    // 텍스처 위에 마우스를 올리면 확대된 뷰를 볼 수 있습니다.

    // 아래는 데모에서 표시할 수 있는 유일한 텍스처인 폰트 텍스처를 표시합니다.
    // ImTextureID는 데이터를 저장하는 용도로 사용되며, 렌더링 백엔드로 전달됩니다.
    // 각 imgui_impl_XXXX.cpp 파일의 주석에서 백엔드가 ImTextureID에 무엇을 기대하는지 확인하세요.
    // 예:
    // - imgui_impl_dx11.cpp: ID3D11ShaderResourceView* 포인터
    // - imgui_impl_opengl3.cpp: GLuint OpenGL 텍스처 ID
    //
    // 참고:
    // - ImTextureID를 사용자 정의 포인터로 설정한 경우, 이를 ImGui::Image()에 전달할 수 있습니다.
    // - ShowMetricsWindow()를 사용하여 렌더링 데이터를 디버깅할 수 있습니다.
    // - ImDrawList::AddImage()를 사용해 직접 이미지 렌더링도 가능합니다.

    // - Read https://github.com/ocornut/imgui/blob/master/docs/FAQ.md
    // - Read https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
#pragma endregion

    /** @note - Tree는 열어야 내부 코드 실행됨 */
    ImGui::Text("Installed upon double-click.");
    for (_uint iFolderIndex = 0; iFolderIndex < m_FolderNameVec.size(); ++iFolderIndex)
    {
        const char* pFolderName = m_FolderNameVec[iFolderIndex];
        if (ImGui::TreeNode(pFolderName))
        {
            auto CompareLamda = [pFolderName](const pair<const char*, CTexture*>& Pair)
                {
                    return !strcmp(Pair.first, pFolderName);
                };
            auto iter = find_if(m_FolderTextureMap.begin(), m_FolderTextureMap.end(), CompareLamda);
            if (iter == m_FolderTextureMap.end())
                continue;

            CTexture* pTexture = iter->second;
            if (nullptr == pTexture)
                continue;

            const vector<ID3D11ShaderResourceView*>* pSRVVec = pTexture->Get_ResourceViewVec();
            if (nullptr == pSRVVec)
                continue;

            _uint iIndex = { 0 };
            for (ID3D11ShaderResourceView* pSRV : *pSRVVec)
            {
                if(0 == (iIndex % 5))
                    ImGui::NewLine();

                // ImtextureID는 void*의 typedef임. imgui_impl_dx11.cpp에서 ID3D11ShaderResourceView*를 기대하고 있음.
                ImTextureID my_tex_id = (ImTextureID*)pSRV;
                _float2 fSize = pTexture->Get_OriginalTextureSize(iIndex);
                float my_tex_w = (float)fSize.x;
                float my_tex_h = (float)fSize.y;

                ImGui::PushID(iIndex);
                ImVec2 size = ImVec2(64.f, 64.f);                         // Size of the image we want to make visible
                ImVec2 uv0 = ImVec2(0.0f, 0.0f);                            // UV coordinates for lower-left
                //ImVec2 uv1 = ImVec2(64.f / my_tex_w, 64.f / my_tex_h);    // UV coordinates for (32,32) in our texture
                ImVec2 uv1 = ImVec2(1.f, 1.f);    // UV coordinates for (32,32) in our texture
                ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             // Black background
                ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint

                if (ImGui::ImageButton("", my_tex_id, size, uv0, uv1, bg_col, tint_col)) // 이미지 버튼 클릭 시 실행됨
                {
                    m_pSpriteListIndex[iFolderIndex] = iIndex;
                    m_iFolderIndex = iFolderIndex;

                    // 오브젝트 생성
                    SPRITE_INFO tSpriteInfo;
                    tSpriteInfo.iTextureIndex = m_pSpriteListIndex[m_iFolderIndex];
                    // 1. 클래스 이름을 누르면 .second에 있는 Texture, TextureIndex 정보로 생성됨.
                    tSpriteInfo.pTextureComTag = ConvertCWithWC(m_FolderNameVec[m_iFolderIndex], TEXT("Prototype_Component_Sprite_"));
                    pGameInstance->Add_Garbage(tSpriteInfo.pTextureComTag);
                    Install_GameObject(tSpriteInfo);
                }
                ImGui::PopID();
                ImGui::SameLine();

                ++iIndex;
            }
            ImGui::TreePop();
        }
    }
    // Add는 한 번만 해야함
    /** @note - unique_ptr
    1. 레퍼런스 카운팅을 하지 않고, 스코프에서 벗어나면 즉시 해제되는 스마트 포인터
    2. unique_ptr은 기존 C 배열 초기화 가능, shared_ptr은 불가능 -> std::array나 std::vector 권장
    3. unique_ptr<자료형> 변수명(new 클래스()); 로도 생성 가능하나 make_unique로 생성할 것을 추천
    4. make_unique<bool[]>(개수); 배열 초기화는 memset 권장 (for문보다 속도 빠름)*/

    Safe_Release(pGameInstance);

    ImGui::End();
    return S_OK;
}

HRESULT CMyImGui::ShowInspectorWindow()
{
    static bool bIsShow = true;
    ImGui::Begin("Inspector", &bIsShow);

    if (nullptr == m_pSelectedObject)
    {
        ImGui::End();
        return S_OK;
    }

    // 클래스 이름 지정
    // 레이어 지정
    //char* pTag = new char[strlen(pObject->Get_SpriteTag() + 1)] {};
    static char* pTag = { nullptr };
    //if (bIsSelectionChanged)
    //{
    //    strcpy(pTag, pObject->Get_SpriteTag());
    //}
    //ImGui::InputText("Tag", pTag, strlen(pTag) + 1);
    //pObject->Set_SpriteTag(pTag);

    //char* pLayer = nullptr;
    //WCToC(m_pSelectedObject->Get_Layer(), pLayer);
    //ImGui::Text("Layer: "); ImGui::SameLine(); ImGui::Text(pLayer);
    //Safe_Delete_Array(pLayer);

    const _bool bIsSelectionChanged = CheckSelectionChanged();

    Default_Info(bIsSelectionChanged);

    if (FAILED(Inspector_Transform(bIsSelectionChanged)))
    {
        MSG_BOX("CMyImGui - ShowInspectorWindow() - FAILED");
        return E_FAIL;
    }

    if (FAILED(Inspector_SpriteRenderer(bIsSelectionChanged)))
        return E_FAIL;

    ImGui::Text("-------------------------------------");
    if (FAILED(Inspector_Components(bIsSelectionChanged)))
        return E_FAIL;

    ImGui::End();
    return S_OK;
}

HRESULT CMyImGui::ShowSettings()
{
    static bool bIsShow = true;
    ImGui::Begin("Settings", &bIsShow);

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    static _bool m_bIsIgnoreLayer[5][5];
    for (_uint i = 0; i < m_LayerVec.size(); ++i)
    {
        ImGui::Text(" ");
        ImGui::Text(m_LayerVec[i].first);
        for (_uint j = 0; j < m_LayerVec.size(); ++j)
        {
            // 체크박스 이름 다 달라야 해...
            if (ImGui::Checkbox(/*m_LayerVec[j]*/" ", &m_bIsIgnoreLayer[i][j]))
            {
                int i = 0;
            }
            ImGui::SameLine();
        }
    }

    Safe_Release(pGameInstance);

    ImGui::End();
    return S_OK;
}

HRESULT CMyImGui::Default_Info(const _bool bIsSelectionChanged)
{
    static char szName[MAX_PATH] = { "" };

    if (bIsSelectionChanged)
    {
        char* pClassName = { nullptr };
        WCToC(m_pSelectedObject->Get_ClassName(), pClassName);
        for (int iIndex = 0; iIndex < m_ClassNameVec.size(); iIndex++)
        {
            if (!strcmp(pClassName, m_ClassNameVec[iIndex]))
            {
                m_iClassIndex = iIndex;
                ImGui::SetItemDefaultFocus();
                break;
            }
        }
        Safe_Delete_Array(pClassName);

        char* pName = { nullptr };
        const _tchar* pNameTag = m_pSelectedObject->Get_NameTag();
        if (nullptr == pNameTag)
        {
            strcpy(szName, "");
        }
        else
        {
            WCToC(pNameTag, pName);
            strcpy(szName, pName);
            Safe_Delete_Array(pName);
        }
    }

    /* Layer 고르기 */
    /* NameTag 지정하기 */
    ImGui::InputText("NameTag", szName, IM_ARRAYSIZE(szName));
    _tchar* pName = { nullptr };
    CToWC(szName, pName); // @error - 여기서 왜 const 상수가 값이 바뀌냐...?
    m_pSelectedObject->Set_NameTag(pName);
    Safe_Delete_Array(pName); // 현재 Set_NameTag 안에서 동적할당 중


    static ImGuiComboFlags flags = 0;
    if (ImGui::BeginCombo("Class", m_ClassNameVec[m_iClassIndex], flags))
    {
        for (int iIndex = 0; iIndex < m_ClassNameVec.size(); iIndex++)
        {
            const bool is_selected = (m_iClassIndex == iIndex);
            // Selectable은 콜백 함수, const char* 첫번째 인자와 동일한 리스트를 선택 (즉, 문자열이 동일하면 먼저 찾은 리스트가 선택됨)  
            if (ImGui::Selectable(m_ClassNameVec[iIndex], is_selected))
            {
                m_iClassIndex = iIndex; // 선택한 항목의 인덱스를 저장  
                _tchar* pDest = { nullptr };
                CToWC(m_ClassNameVec[m_iClassIndex], pDest);
                const _tchar* pClassName = pDest;
                m_pSelectedObject->Set_ClassName(pClassName);
                // 콜라이더명과 각 콜라이더는... 보관하고 있는 게 낫지 않을까?//
                // 현재 그냥 동일한 컴포넌트 하나만 가져오는 듯함..! 맨 앞에 있는 컴포넌트 .!
            }

            if (is_selected)
            {
                ImGui::SetItemDefaultFocus();
            }

        }
        ImGui::EndCombo();
    }

    return S_OK;
}

HRESULT CMyImGui::Inspector_Transform(const _bool bIsSelectionChanged)
{
    if (ImGui::TreeNode("Transform"))
    {
        static bool bUsedOnce = { false };

        static enum eTransform { POSITION, ROTATION, SCALE, SCALE_RATIO, TRANS_END };
        static const _float fSpeed[eTransform::TRANS_END] = { 0.5f, 0.5f, 1.f };
        static const _float fMin[eTransform::TRANS_END] = { -10000.f, 0.f, 1.f, 1.f };
        static const _float fMax[eTransform::TRANS_END] = { 10000.f, 360.f, 1000.f, 20.f };

        static CTransform* pSelectedObjectTransform = m_pSelectedObject->Get_TransformCom();
        static _float fScale[3] = {10.f, 10.f, 1.f};
        static _float fScaleRatio[3] = {1.f, 1.f, 1.f};

        if (bIsSelectionChanged || !bUsedOnce)
        {
            pSelectedObjectTransform = m_pSelectedObject->Get_TransformCom();

            if (nullptr == pSelectedObjectTransform)
            {
                ImGui::End();
                return S_OK;
            }

            CTexture* pTexture = dynamic_cast<CTexture*>(m_pSelectedObject->Get_Component(TAG_TEXTURE));
            if (nullptr == pTexture)
            {
                ImGui::End();
                return S_OK;
            }

            //_float3 vScale = pSelectedObjectTransform->Get_Scaled();

            //if (!m_pSelectedObject->Get_bIsAnimUV())
            //{
            //    const int iTextureIndex = m_pSelectedObject->Get_TextureIndex();
            //    _float2 vScale = pTexture->Get_OriginalTextureSize(iTextureIndex);
            //}

            _float2 vScaleRatio = m_pSelectedObject->Get_ScaleRatio();
            fScaleRatio[0] = vScaleRatio.x;
            fScaleRatio[1] = vScaleRatio.y;
        };

        _vector vPosition = pSelectedObjectTransform->Get_State(CTransform::STATE_POSITION);
        _float fPosition[3] = { XMVectorGetX(vPosition), XMVectorGetY(vPosition), 0.f };

        ImGui::DragFloat3("Position", fPosition
            , fSpeed[eTransform::POSITION], fMin[eTransform::POSITION], fMax[eTransform::POSITION], "%.1f");
        //ImGui::DragFloat3("Rotation", vPosition
        //    , fSpeed[eTransform::ROTATION], fMin[eTransform::ROTATION], fMax[eTransform::ROTATION]);
        //ImGui::DragFloat3("Scale", fScale
        //    , fSpeed[eTransform::SCALE], fMin[eTransform::SCALE], fMax[eTransform::SCALE], "%.1f");
        ImGui::DragFloat3("ScaleRatio", fScaleRatio
            , fSpeed[eTransform::SCALE_RATIO], fMin[eTransform::SCALE_RATIO], fMax[eTransform::SCALE_RATIO], "%.1f");

        if(nullptr != pSelectedObjectTransform)
            pSelectedObjectTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosition[0], fPosition[1], fPosition[2], 1.f));
        m_pSelectedObject->Set_ScaleRatio(_float2(fScaleRatio[0], fScaleRatio[1]));

        bUsedOnce = { true };
        ImGui::TreePop();
    }

    return S_OK;
}

HRESULT CMyImGui::Inspector_Components(const _bool bIsSelectionChanged)
{
#pragma region 컴포넌트 추가
    static ImGuiComboFlags flags = 0;
    static _uint iComponentIndex = { 0 };
    if (ImGui::BeginCombo("Component List", m_ComponentsVec[iComponentIndex], flags))
    {
        for (int iIndex = 0; iIndex < m_ComponentsVec.size(); iIndex++)
        {
            const bool is_selected = (iComponentIndex == iIndex);
            // Selectable은 콜백 함수, const char* 첫번째 인자와 동일한 리스트를 선택 (즉, 문자열이 동일하면 먼저 찾은 리스트가 선택됨)  
            if (ImGui::Selectable(m_ComponentsVec[iIndex], is_selected))
            {
                iComponentIndex = iIndex; // 선택한 항목의 인덱스를 저장  
                // 콜라이더명과 각 콜라이더는... 보관하고 있는 게 낫지 않을까?//
                // 현재 그냥 동일한 컴포넌트 하나만 가져오는 듯함..! 맨 앞에 있는 컴포넌트 .!
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
    }

    static char ComponentTag[128] = "";
    _tchar* pComponentTag = nullptr;

    if (ImGui::Button("Add Component"))
    {
        if (!lstrcmp(TEXT(""), pComponentTag))
        {
            MSG_BOX("Part Tag is NULL");
            return S_OK;
        }

        CTransform* pTransformCom = m_pSelectedObject->Get_TransformCom();
        if (nullptr == pTransformCom)
        {
            MSG_BOX("CMyImGui - Inspector_Components() - NULL");
            return S_OK;
        }

        COMPONENT_INFO tComponentInfo;
        ZeroMemory(&tComponentInfo, sizeof(COMPONENT_INFO));
        _vector vPos = pTransformCom->Get_State(CTransform::STATE_POSITION);
        tComponentInfo.fPosition = _float2(XMVectorGetX(vPos), XMVectorGetY(vPos));
        _float3 vSize = pTransformCom->Get_Scaled();
        tComponentInfo.fSize = _float2(vSize.x, vSize.y);
        tComponentInfo.fOffset = _float2(0.f, 0.f);

        _tchar* pPrototypeTag = ConvertCWithWC(m_ComponentsVec[iComponentIndex], TEXT("Prototype_Component_"));
        CComponent* pComponent = { nullptr };
        /** @qurious. CGameObject 상속을 받아 함수 오버로딩이 될텐데 .. 왜 CGameObject*의 Add_Components를 인식 못하지? */
        if (FAILED(m_pSelectedObject->CGameObject::Add_Components(LEVEL_STATIC, pPrototypeTag, TAG_COLL_AABB, &pComponent, &tComponentInfo)))
        {
            MSG_BOX("CMyImGui - ShowInspectorWindow - Add_Component - FAILED");
        }
        Safe_Delete_Array(pPrototypeTag);
        m_pSelectedObject->Mapping_Component(TAG_COLL_AABB);
        
        CGameInstance* pGameInstance = CGameInstance::GetInstance();
        Safe_AddRef(pGameInstance);
        if (nullptr != pGameInstance)
        {
            pGameInstance->Add_Garbage(pPrototypeTag);
        }
        Safe_Release(pGameInstance);
        
        if (nullptr == pComponent)
        {
            MSG_BOX("CMyImGui - Inspector_Components() - NULL");
        }
        else
        {
            pComponent->Set_Owner(m_pSelectedObject);
            m_ComponentGarbageList.emplace_back(pComponent);
        }
    }
#pragma endregion
    unordered_map<const _tchar*, CComponent*>* m_Components = m_pSelectedObject->Get_Components();
    if (nullptr != m_Components)
    {
        if (m_Components->empty())
            return S_OK;

        for (auto iter = m_Components->begin(); iter != m_Components->end(); ++iter)
        {
            CCollider* pCollider = dynamic_cast<CCollider*>(iter->second);
            if (nullptr != pCollider)
            {
                Inpsector_Collider(pCollider, bIsSelectionChanged);
            }
        }
    }

    return S_OK;
}

HRESULT CMyImGui::Inspector_SpriteRenderer(const _bool bIsSelectionChanged)
{

    if (ImGui::TreeNode("SpriteRenderer"))
    {
        CComponent* pComponent = m_pSelectedObject->Get_Component(TAG_TEXTURE);
        CTexture* pTexture = dynamic_cast<CTexture*>(pComponent);

        static int iOrder = { 0 };
        if (bIsSelectionChanged)
            iOrder = m_pSelectedObject->Get_Order();
        ImGui::InputInt("Order in Layer", &iOrder);

        CGameInstance* pGameInstance = CGameInstance::GetInstance();
        Safe_AddRef(pGameInstance);
        if (pGameInstance->Get_KeyDown(DIK_UP))
        {
            ++iOrder;
        }
        else if (pGameInstance->Get_KeyDown(DIK_DOWN))
        {
            --iOrder;
        }
        Safe_Release(pGameInstance);
        m_pSelectedObject->Set_Order(iOrder);

        static ImGuiComboFlags flags = 0;
        static _uint iRenderIndex = { 0 };

        if (bIsSelectionChanged)
            iRenderIndex = (_uint)m_pSelectedObject->Get_RenderGroup();

        if (ImGui::BeginCombo("Sorting Layer(RenderGroup)", m_RenderGroupVec[iRenderIndex], flags))
        {
            for (int iListIndex = 0; iListIndex < m_RenderGroupVec.size(); iListIndex++)
            {
                const bool is_selected = (iRenderIndex == iListIndex);
                // Selectable은 콜백 함수, const char* 첫번째 인자와 동일한 리스트를 선택 (즉, 문자열이 동일하면 먼저 찾은 리스트가 선택됨)  
                if (ImGui::Selectable(m_RenderGroupVec[iListIndex], is_selected))
                {
                    iRenderIndex = iListIndex; // 선택한 항목의 인덱스를 저장
                    m_pSelectedObject->Set_RenderGroup(CRenderer::RENDERGROUP(iRenderIndex));
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        static _uint iLayerIndex = { 0 };
#pragma region 주석
        //static unique_ptr<char> pLayer = make_unique<char>(MAX_PATH);

        //if (bIsSelectionChanged)
        //{
        //    for (_uint i = 0; i < m_LayerVec.size(); ++i)
        //    {
        //        if (!strcmp(m_LayerVec[i], m_pSelectedObject->Get_Layer()))
        //        {
        //            iLayerIndex = i;
        //            break;
        //        }
        //    }
        //}

        //if (ImGui::BeginCombo("Layer", m_LayerVec[iLayerIndex], flags))
        //{
        //    for (int iListIndex = 0; iListIndex < m_LayerVec.size(); iListIndex++)
        //    {
        //        const bool is_selected = (iLayerIndex == iListIndex);
        //        // Selectable은 콜백 함수, const char* 첫번째 인자와 동일한 리스트를 선택 (즉, 문자열이 동일하면 먼저 찾은 리스트가 선택됨)  
        //        if (ImGui::Selectable(m_LayerVec[iListIndex], is_selected))
        //        {
        //            iLayerIndex = iListIndex; // 선택한 항목의 인덱스를 저장
        //            m_pSelectedObject->Set_Layer(m_LayerVec[iLayerIndex]);
        //        }

        //        if (is_selected)
        //            ImGui::SetItemDefaultFocus();
        //    }
        //    ImGui::EndCombo();
        //}
#pragma endregion

        // CheckBox는 클릭된 시점을 true
        static bool bIsMultiple = false;
        if (bIsSelectionChanged)
        {
            if (m_pSelectedObject->Get_bIsAnimUV())
            {
                bIsMultiple = true;
            }
            else
            {
                bIsMultiple = false;
            }
        }

        if (ImGui::Checkbox("Multiple", &bIsMultiple))
        {
            m_pSelectedObject->Set_IsAnimUV(!m_pSelectedObject->Get_bIsAnimUV());

            if (m_pSelectedObject->Get_bIsAnimUV())
                m_pSelectedObject->Set_ShaderPass((_uint)VTXTEX_PASS::UV_Anim);
            else
                m_pSelectedObject->Set_ShaderPass((_uint)VTXTEX_PASS::Default);
        }



        if (bIsMultiple)
            Inspector_Multiple(bIsSelectionChanged);

        ImGui::TreePop();
    }

    return S_OK;
}

HRESULT CMyImGui::Inspector_Multiple(const _bool bIsSelectionChanged)
{
    if (nullptr == m_pSelectedObject)
        return E_FAIL;

    if (ImGui::TreeNode("Sprite Slice"))
    {
        static int vec4i[4] = { 1, 1, 0, 0 };

        ImGui::InputInt2("Column : Row", vec4i);

        if (bIsSelectionChanged)
        {
            vec4i[0] = m_pSelectedObject->Get_UVTexNumX();
            vec4i[1] = m_pSelectedObject->Get_UVTexNumY();
        }

        if (ImGui::Button("Slice"))
        {
            m_pSelectedObject->Set_UVTexNumX(vec4i[0]);
            m_pSelectedObject->Set_UVTexNumY(vec4i[1]);
            m_pSelectedObject->Set_SliceTextureSize(vec4i[0], vec4i[1]);
        }


        ImGui::TreePop();
    }

    if (ImGui::TreeNode("Animation"))
    {
        ImGui::TreePop();
    }
    return S_OK;
}

HRESULT CMyImGui::Inpsector_Collider(CCollider* pCollider, const _bool bIsSelectionChanged)
{
    if (nullptr == pCollider)
    {
        return E_FAIL;
    }

    if (ImGui::TreeNode("ColliderAABB"))
    {
        /** @qurious - 미해결 문제(열 받음ㅜ)
        * 시도 내용: 1. 2차원 배열을 하자니 m_ColliderSize()는 상수가 아니라 불가능
        * 2. Collider 종류마다 함수를 파서 if(nullptr != dynamic_cast<ColliderAABB*>)처럼 일일이 비교는 비효율적)
        */
        //ImGui::InputTextWithHint("Component Tag", "Write Part Name", ComponentTag, IM_ARRAYSIZE(ComponentTag));
        //CToWC(ComponentTag, pComponentTag);

        static _bool bIsBlock = { false };
        if (bIsSelectionChanged)
        {
            CComponent* pComponent = m_pSelectedObject->Find_Component(TAG_COLL_AABB);
            if (nullptr != pCollider)
            {
                CCollider* pCollider = dynamic_cast<CCollider*>(pComponent);
                if (nullptr != pCollider)
                {
                    bIsBlock = pCollider->Get_IsBlock();
                }
            }
        }
        if (ImGui::Checkbox("IsBlock", &bIsBlock))
        {
            CComponent* pComponent = m_pSelectedObject->Find_Component(TAG_COLL_AABB);
            if (nullptr != pCollider)
            {
                CCollider* pCollider = dynamic_cast<CCollider*>(pComponent);
                if (nullptr != pCollider)
                {
                    pCollider->Set_IsBlock(bIsBlock);
                }
            }
        }

        static CCollider::COLLIDER_DESC tColliderDesc = pCollider->Get_ColliderDesc();
        static _float fOffset[2] = { tColliderDesc.vOffset.x, tColliderDesc.vOffset.y};
        static _float fScale[2] = { tColliderDesc.vScale.x, tColliderDesc.vScale.y};

        if (bIsSelectionChanged)
        {
            tColliderDesc = pCollider->Get_ColliderDesc();
            fOffset[0] = tColliderDesc.vOffset.x;
            fOffset[1] = tColliderDesc.vOffset.y;
            fScale[0] = tColliderDesc.vScale.x;
            fScale[1] = tColliderDesc.vScale.y;
        }

        enum INFO { OFFSET, SIZE, INS_END };
        const float fSpeed[INS_END] = { 1.f, 1.f };
        const float fMin[INS_END] = { -10000.f, 1.f };
        const float fMax[INS_END] = { 10000.f, 300.f };
        ImGui::DragFloat2("Offset", fOffset
            , fSpeed[INFO::OFFSET], fMin[INFO::OFFSET], fMax[INFO::OFFSET]);
        tColliderDesc.vOffset = _float3(fOffset[0], fOffset[1], 0.f);

        ImGui::DragFloat2("Size", fScale
            , fSpeed[INFO::SIZE], fMin[INFO::SIZE], fMax[INFO::SIZE]);
        tColliderDesc.vScale = _float3(fScale[0], fScale[1], 1.f);

        static ImGuiComboFlags flags = 0;
        static _uint iColliderIndex = { 0 };
        pCollider->Set_ColliderDesc(tColliderDesc);

        if (ImGui::BeginCombo("Collider Layer", m_ColliderLayer[iColliderIndex], flags))
        {
            for (int iIndex = 0; iIndex < m_ColliderLayer.size(); iIndex++)
            {
                const bool is_selected = (iColliderIndex == iIndex);
                // Selectable은 콜백 함수, const char* 첫번째 인자와 동일한 리스트를 선택 (즉, 문자열이 동일하면 먼저 찾은 리스트가 선택됨)  
                if (ImGui::Selectable(m_ColliderLayer[iIndex], is_selected))
                {
                    iColliderIndex = iIndex;
                    // 콜라이더를 해당 레이어로 설정
                }

                if (is_selected)
                    ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }
        ImGui::TreePop();
    }

    return S_OK;
}

void CMyImGui::Key_Input(_double TimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (pGameInstance->Get_MouseStay(CDInput_Manager::MOUSEKEYSTATE::DIMK_RB))
    {
        /* 직교 투영의 경우, 내부적으로 변화는 일어나지만 월드 좌표와 뷰포트 좌표의 차이가 없기에 뷰포트 좌표를 월드로 셋팅 */
        if (nullptr == m_pSelectedObject)
        {
            Safe_Release(pGameInstance);
            return;
        }
        CTransform* pObjectTransformCom = m_pSelectedObject->Get_TransformCom();
        if (nullptr == pObjectTransformCom)
            return;

        _vector vMousePos = { 0.f, 0.f, 0.f, 0.f };
        Get_IntersectMouseWithZPlane(vMousePos, XMVectorGetZ(pObjectTransformCom->Get_State(CTransform::STATE_POSITION)));
        
        m_pSelectedObject->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vMousePos);
    }


    if (pGameInstance->Get_KeyStay(DIK_L) && pGameInstance->Get_MouseDown(CDInput_Manager::MOUSEKEYSTATE::DIMK_LB))
    {
        _vector vMousePos = { 0.f, 0.f, 0.f, 0.f };
        Get_IntersectMouseWithZPlane(vMousePos, 0.f);
        _float3 vPosition = _float3(XMVectorGetX(vMousePos), XMVectorGetY(vMousePos), 0.f);
        VertexPositionColor tVertex(vPosition, XMFLOAT4(0.f, 1.f, 0.f, 1.f));

        pGameInstance->Add_Vertex(tVertex);
    }

    if (pGameInstance->Get_KeyStay(DIK_L) && pGameInstance->Get_KeyDown(DIK_BACKSPACE))
    {
        pGameInstance->DeleteBack_Line();
    }

#pragma region 설치는 이미지 버튼을 클릭하세요
    //// 설치
    //if (pGameInstance->Get_KeyStay(DIK_Z) && pGameInstance->Get_MouseDown(CDInput_Manager::MOUSEKEYSTATE::DIMK_LB))
    //{
    //    SPRITE_INFO tSpriteInfo;
    //    tSpriteInfo.iTextureIndex = m_pSpriteListIndex[m_iFolderIndex];
    //    // 1. 클래스 이름을 누르면 .second에 있는 Texture, TextureIndex 정보로 생성됨.
    //    tSpriteInfo.pTextureComTag = ConvertCWithWC(m_FolderNameVec[m_iFolderIndex], TEXT("Prototype_Component_Sprite_"));
    //    pGameInstance->Add_Garbage(tSpriteInfo.pTextureComTag);
    //    Install_GameObject(tSpriteInfo);
    //}
#pragma endregion

    // 복제
    if (pGameInstance->Get_KeyStay(DIK_LSHIFT) && pGameInstance->Get_MouseDown(CDInput_Manager::MOUSEKEYSTATE::DIMK_LB))
    {
        CSpriteObject* pObject = dynamic_cast<CSpriteObject*>(m_pSelectedObject);
        if (nullptr == pObject)
        {
            Safe_Release(pGameInstance);
            return;
        }

        CTransform* pTransform = pObject->Get_TransformCom();
        _vector vPosition = pTransform->Get_State(CTransform::STATE_POSITION);
        _float3 vScale = pTransform->Get_Scaled();
        _float2 vScaleRatio = m_pSelectedObject->Get_SpriteInfo().fSizeRatio;

        SPRITE_INFO tSpriteInfo = pObject->Get_SpriteInfo();
        tSpriteInfo.fPosition.x = XMVectorGetX(vPosition) + (vScale.x/* * vScaleRatio.x*/);
        tSpriteInfo.fPosition.y = XMVectorGetY(vPosition);

        /** @note - const char* 새로운 메모리 공간에 값만 복사해서 다른 const char*이 가리키도록 하는 방법 */
        _tchar* pTemp = new _tchar[lstrlen(pObject->Get_SpriteInfo().pTextureComTag) + 1]{};
        lstrcpy(pTemp, pObject->Get_SpriteInfo().pTextureComTag);
        tSpriteInfo.pTextureComTag = pTemp;
        pGameInstance->Add_Garbage(tSpriteInfo.pTextureComTag);
        Install_GameObject(tSpriteInfo);
    }

    if (pGameInstance->Get_KeyStay(DIK_LCONTROL) && pGameInstance->Get_KeyDown(DIK_Z))
    {
        if (m_CreateObjectVec.empty())
        {
            Safe_Release(pGameInstance);
            return;
        }

        // 사실 이러면 실질적인 객체는 삭제가 안된 상태
        list<CGameObject*>* pObjectList = pGameInstance->Get_ObjectList(LEVEL_TOOL, TEXT("Layer_Default"));
        if (nullptr == pObjectList)
        {
            Safe_Release(pGameInstance);
            return;
        }

        (*pObjectList).remove(m_CreateObjectVec.back());
        m_CreateObjectVec.pop_back();
        Safe_Release(m_pSelectedObject);

        if (m_CreateObjectVec.empty())
        {
            m_pSelectedObject = nullptr;
            Safe_Release(pGameInstance);
            return;
        }
        else
        {
            if (nullptr != m_pSelectedObject)
                m_pSelectedObject->Set_ShaderPass((_uint)VTXTEX_PASS::Default);

            m_pSelectedObject = m_CreateObjectVec.back();
            m_pSelectedObject->Set_ShaderPass((_uint)VTXTEX_PASS::Color);
        }
    }

    if (pGameInstance->Get_KeyDown(DIK_DELETE))
    {
        if (m_CreateObjectVec.empty())
        {
            Safe_Release(pGameInstance);
            return;
        }

        /** @error - 오브젝트 지워질 때 LayerMap 안의 Layer 명도 지워져서 이후부터는 Get_ObjectList가 정상적으로 안됨 */
        list<CGameObject*>* pObjectList = pGameInstance->Get_ObjectList(LEVEL_TOOL, m_pSelectedObject->Get_Layer());
        if (nullptr == pObjectList)
        {
            Safe_Release(pGameInstance);
            return;
        }

        /** @note - remove와 erase
        * 1. erase는 모든 컨테이너가 보관하고 있는 멤버, 인자값 iter와 동일한 원소를 삭제하고 뒤의 원소를 앞으로 당긴 다음 ++iter를 반한함.
        * 2. <algorithm>의 remove 함수는 삭제할 요소에 유지할 요소를 덮어씌우는 방식으로 맨 뒤에는 필요없는 기존값들이 그대로 유지되어 있음 (용량은 그대로란 뜻)
        * -> 이를 지우기 위해 erase(remove(begin, end, 요소), end)를 사용하여 remove의 반환값인 new end 부터 end까지 해당 범위를 erase하는 방식 채택
        * 3.  list는 erase + remove 형태를 void remove 멤버 함수로 보유, list의 경우 erase + remove보다 remove 멤버 함수를 사용할 것 (시간복잡도상 유리)
        * */
        (*pObjectList).remove(m_pSelectedObject);
        m_CreateObjectVec.erase(remove(m_CreateObjectVec.begin(), m_CreateObjectVec.end(), m_pSelectedObject), m_CreateObjectVec.end());

        /** @note - new로 생성한 포인터의 컨테이너를 생성할 땐 컨테이너 소멸 전에 포인터를 delete 할 것 
        * 1. 컨테이너는 소멸 시 각 요소 자체를 없애줌
        * 2. 그러나 포인터의 소멸자는 아무 역할을 하지 않기 때문에 포인터가 가리키는 대상은 소멸되지 못함.
        * 3. 따라서 컨테이너가 포인터를 보유한 경우 컨테이너 소멸 전 직접 delete 해줄 것 */
        Safe_Release(m_pSelectedObject);

        if(m_CreateObjectVec.empty())
        {
            m_pSelectedObject = nullptr;
            Safe_Release(pGameInstance);
            return;
        }
        else
            m_pSelectedObject = m_CreateObjectVec.back();
    }

    Safe_Release(pGameInstance);
    return;
}

void CMyImGui::Get_MousePosInViewport(_vector& vOutPos) const
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    if (nullptr == pGameInstance)
        return;

    Safe_AddRef(pGameInstance);
    POINT ptMouse{};
    GetCursorPos(&ptMouse); // 스크린 공간의 마우스 좌표를 가져옵니다.
    ScreenToClient(g_hWnd, &ptMouse); // 스크린 공간을 클라이언트 공간으로 변환합니다. 화면 크기가 동일하면(?) 클라공간은 뷰포트(=픽셀 단위)와 동일

    // 뷰스 == 카메라 월드 역행렬
    _float4x4 CameraWordInverseMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);

    /** x는 -이고, y는 +인 이유
    * API 좌표계는 좌측 상단 (0,0)에 x는 우측으로 갈수록 +, y는 아래로 갈수록 +임을 인지해야 함.
    * 뷰스페이스는 카메라의 월드 역행렬이므로 카메라 이동의 반대방향을 내포함.
    * 우리는 카메라가 이동한 위치의 좌표를 가져올 거기 때문에 카메라의 반대가 아니라 동일하게 움직여야 해서 -와 +로 해준 것 */
    _float fX = ptMouse.x - CameraWordInverseMatrix._41;
    _float fY = ptMouse.y + CameraWordInverseMatrix._42;

    /* API 좌표계를 DirectX 좌표계로 보정한 것 (변환X = NDC 좌표계X) */
    vOutPos = XMVectorSet(fX - (g_iWinSizeX >> 1), -fY + (g_iWinSizeY >> 1), 0.f, 1.f);

    Safe_Release(pGameInstance);
    return;
}

void CMyImGui::Get_IntersectMouseWithZPlane(_vector& vOutPos, const _uint iZIndex) const
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    //_vector vMousePos = CUtility::Get_MousePosInViewport(g_hWnd, g_iWinSizeY, g_iWinSizeY); 
    POINT ptMouse{};
    GetCursorPos(&ptMouse); // 스크린 공간의 마우스 좌표를 가져옵니다.
    ScreenToClient(g_hWnd, &ptMouse); // 스크린 -> 클라이언트 공간(크기 동일 시 뷰포트와 동일)

    // 뷰포트 -> 투영
    _vector vMousePos = XMVectorSet(ptMouse.x / (g_iWinSizeX * 0.5f) - 1.f, ptMouse.y / -(g_iWinSizeY * 0.5f) + 1.f, 0.f, 1.f);

    // 투영 -> 뷰스페이스
    _matrix ProjectionMatrix = pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE::D3DTS_PROJ);
    ProjectionMatrix = XMMatrixInverse(nullptr, ProjectionMatrix); // 투영 역행렬
    vMousePos = XMVector3TransformCoord(XMVectorSet(XMVectorGetX(vMousePos), XMVectorGetY(vMousePos), 0.f, 1.f), ProjectionMatrix);

    _vector vRayPos = { 0.f, 0.f, 0.f };
    _vector vRayDir = vMousePos - vRayPos;

    // 3. 뷰스 -> 월드
    _matrix ViewMatrix = pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);
    ViewMatrix = XMMatrixInverse(nullptr, ViewMatrix);

    vRayPos = XMVector3TransformCoord(vRayPos, ViewMatrix);
    vRayDir = XMVector3Normalize(XMVector3TransformNormal(vRayDir, ViewMatrix));

    // 광선-평면 교차 계산 (z=1 평면)
    float t = (iZIndex - XMVectorGetZ(vRayPos)) / XMVectorGetZ(vRayDir); // t = (zPlaneIndex - vRayPos) / vRayDir
    vOutPos = XMVectorAdd(vRayPos, XMVectorScale(vRayDir, t)); // vRayPos + t * vRayDir

#pragma region 공부할 부분
    ////@qurious - 마우스 스크롤 왜 X는 -고, Y는 + 인지 분석, 왜 엔진에 넘기면 마우스가 제대로 안 따라오는지!
    // // 이거 HiFiRush Tool의 직교투영 시 코드도 같이 보면서 원근 투영 시 마우스 클리과 직교 투영 시 마우스 클릭 어케 하는지 보기 (블로그 참고)
    // _float fX = XMVectorGetX(vMousePos) - pGameInstance->Get_ScrollX();
    // _float fY = XMVectorGetY(vMousePos) + pGameInstance->Get_ScrollY();
#pragma endregion

    Safe_Release(pGameInstance);
    return;
}

HRESULT CMyImGui::Get_OpenFileName(OPENFILENAME& tOpenFileName)
{
    OPENFILENAME OFN;
    TCHAR filePathName[128] = L"";
    TCHAR* lpstrFile = new TCHAR[256]{ L".data" };
    static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.BattleUIdat)\0*.BattleUIdat";

    ZeroMemory(&OFN, sizeof(OPENFILENAME));
    OFN.lStructSize = sizeof(OPENFILENAME);
    OFN.hwndOwner = g_hWnd;
    OFN.lpstrFilter = filter;
    OFN.lpstrFile = lpstrFile;
    OFN.nMaxFile = 256;
    OFN.lpstrInitialDir = L"..\\Bin\\DataFiles";

    if (GetOpenFileName(&OFN) == 0)
        return E_FAIL;

    // @note - 포인터 TCHAR lpstrFile[MAX_PATH] 지역변수면 함수 나가면 사라짐
    // 내가 알기로 리터럴 상수만 참조하는 곳이 없을 때까지 존재하는 것으로 앎.
    memcpy(&tOpenFileName, &OFN, sizeof(OPENFILENAME));

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);
    pGameInstance->Add_Garbage(OFN.lpstrFile);
    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CMyImGui::Install_GameObject(SPRITE_INFO& tSpriteInfo)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    static int iIndex = m_CreateObjectVec.size();

    _tchar* pLayer = { nullptr };
    CToWC(m_pLayerC, pLayer);
    // 고르는 Layer 목록에 따라 알맞은 enum의 Layer를 줘야함 !
    if (FAILED((pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Install"), LEVEL_TOOL, pLayer, tSpriteInfo))))
    {
        MSG_BOX("CMyImGui - Install_GameObject() - NULL");
        Safe_Release(pGameInstance);
        return E_FAIL;
    }
    // @qurious - 이러면.. 오브젝트 안의 pTextureComTag가 문자열은 없는데 공간은 참조 중이라 nullptr해줘야하긴해..    
    if (nullptr != m_pSelectedObject)
        m_pSelectedObject->Set_ShaderPass((_uint)VTXTEX_PASS::Default);

    m_pSelectedObject = dynamic_cast<CSpriteObject*>(pGameInstance->Get_LastObject(LEVEL_TOOL, pLayer));
    if (nullptr == m_pSelectedObject)
    {
        MSG_BOX("CMyImGui - Install_GameObject() - NULL");
        Safe_Release(pGameInstance);
        return E_FAIL;
    }

    // 스프라이트 이름 지정
    char* pTag = new char[MAX_PATH] {};
    strcpy(pTag, m_FolderNameVec[m_iFolderIndex]);
    strcat(pTag, to_string(++iIndex).c_str());
    m_pSelectedObject->Set_SpriteTag(pTag);
    m_pSelectedObject->Set_ShaderPass((_uint)VTXTEX_PASS::Color);

    // 클래스, 레이어, 렌더그룹 지정
    CRenderer::RENDERGROUP eRenderGroup = CRenderer::RENDER_END;
    _tchar* pClassName = { nullptr };
    if(!strcmp("Background", m_FolderNameVec[m_iFolderIndex]))
    {
        eRenderGroup = CRenderer::RENDER_PRIORITY;
        pClassName = new _tchar[strlen("BackGround") + 1];
        lstrcpy(pClassName, TEXT("BackGround"));
    }
    else if (!strcmp("Tile", m_FolderNameVec[m_iFolderIndex]))
    {
        eRenderGroup = CRenderer::RENDER_TILE;
        pClassName = new _tchar[strlen("Tile") + 1];
        lstrcpy(pClassName, TEXT("Tile"));
    }
    else
    {
        eRenderGroup = CRenderer::RENDER_NONBLEND;
        pClassName = new _tchar[strlen("Environment") + 1];
        lstrcpy(pClassName, TEXT("Environment"));
    }

    m_pSelectedObject->Set_RenderGroup(eRenderGroup);
    m_pSelectedObject->Set_ClassName(pClassName);
    m_pSelectedObject->Set_Layer(pLayer, true);

    Safe_Delete_Array(pClassName); // 현재 Set_Class 함수 내에서 동적할당 수행함.

    m_CreateObjectVec.emplace_back(m_pSelectedObject);

    Safe_Release(pGameInstance);
    return S_OK;
}

_tchar* CMyImGui::ConvertCWithWC(const char* pFolderName, const _tchar* pConvertText) const
{
    // @note - 동적할당 문자열은 _tchar로 선언 가능. 문자열 리터럴은 무조건 const로 선언
    _tchar* pPrototypeTag = new _tchar[MAX_PATH];
    ZeroMemory(pPrototypeTag, sizeof(_tchar) * MAX_PATH);
    // @note - lstrcat은 첫 번째 인자의 '\0' 부분에 붙이는 것이므로 초기화 안하고 붙이면 췍췍췍 뒤에 붙음
    lstrcat(pPrototypeTag, pConvertText);
    _tchar* pFolderNameWC = { nullptr };
    CToWC(pFolderName, pFolderNameWC);
    lstrcat(pPrototypeTag, pFolderNameWC);
    Safe_Delete_Array(pFolderNameWC);
    return pPrototypeTag;
}

const _bool& CMyImGui::CheckSelectionChanged() const
{
    static CSpriteObject* pPreviousObject = { nullptr };

    if (m_pSelectedObject != pPreviousObject)
    {
        pPreviousObject = m_pSelectedObject;
        return true;
    }
    return false;
}

HRESULT CMyImGui::Ready_ContainerResource(const vector<const char*>& FolderNameVec)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    // 텍스처 컴포넌트를 얻기 위해 오브젝트를 추가합니다.
    SPRITE_INFO tSpriteInfo;
    tSpriteInfo.fPosition = _float2(-(g_iWinSizeX * 0.5f) + 20.f, g_iWinSizeY * 0.5f - 20.f);
    _tchar* pTag = new _tchar[MAX_PATH];
    lstrcpy(pTag, TEXT("Prototype_Component_Sprite_ForestTile"));
    tSpriteInfo.pTextureComTag = pTag;
    pGameInstance->Add_Garbage(tSpriteInfo.pTextureComTag);
    if (FAILED((pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Install"), LEVEL_STATIC, TEXT("Layer_Default"), tSpriteInfo))))
    {
        MSG_BOX("CMyImGui - Tick() - NULL");
    }

    CSpriteObject* pObject = dynamic_cast<CSpriteObject*>(pGameInstance->Get_LastObject(LEVEL_STATIC, TEXT("Layer_Default")));
    if (nullptr == pObject)
    {
        MSG_BOX("CMyImGui - Initialize - NULL");
        Safe_Release(pGameInstance);
    }
    //pObject->Set_Dead();

    _uint iIndex = { 0 };
    for (const char* pFolderName : FolderNameVec)
    {
        // 텍스처 폴더 수만큼 텍스처 컴포넌트를 복사하여 m_FolderTextureMap에 보관합니다.
        const _tchar* pTextureComTag = ConvertCWithWC(FolderNameVec[iIndex], TEXT("Prototype_Component_Sprite_"));
        pGameInstance->Add_Garbage(pTextureComTag);
        pObject->Change_TextureComponent(pTextureComTag);

        CTexture* pTexture = dynamic_cast<CTexture*>(pObject->Get_Component(TAG_TEXTURE));
        if (nullptr == pTexture)
            continue;

        CTexture* pCloneTexture = dynamic_cast<CTexture*>(pTexture->Clone());
        if (nullptr == pCloneTexture)
            continue;

        m_FolderTextureMap.emplace(pFolderName, pCloneTexture);
        ++iIndex;

        // 해당 텍스처 컴포넌트가 지닌 텍스처들의 주소도 보관합니다.
        const vector<const _tchar*>* pTexturePathVec = pTexture->Get_TexturePathVec();
        if (nullptr == pTexturePathVec)
            continue;

        vector<const char*> CPathVec;
        CPathVec.reserve(pTexturePathVec->size());

        for (_uint i = 0; i < pTexturePathVec->size(); ++i)
        {
            char* pPath = { nullptr };
            WCToC((*pTexturePathVec)[i], pPath);
            CPathVec.emplace_back(pPath);
        }
        m_SpritePathMap.insert({ pFolderName, CPathVec });
    }

    Safe_Release(pGameInstance);

    return S_OK;
}

void CMyImGui::Free()
{
    /** @note - const vector<자료형>하면 값을 바꿀 수 없기에 clear도 불가능 */
     //폴더명은 상수라 자동 해제, 파일명은 WCToC로 동적할당해서 담은 문자열이므로 직접 해제
    unordered_map<const char*, vector<const char*>>::iterator iter = m_SpritePathMap.begin();
    for (iter; iter != m_SpritePathMap.end(); ++iter)
    {
        for (const char* pFilePath : iter->second)
        {
            Safe_Delete_Array(pFilePath);
        }
        (iter->second).clear();
    }

    auto TextureIter = m_FolderTextureMap.begin();
    for (TextureIter; TextureIter != m_FolderTextureMap.end(); ++TextureIter)
    {
        Safe_Release(TextureIter->second);
    }

    // Add_Component 시 내부에서 Safe_AddRef 하기 때문에 제거해줄 것
    for (CComponent* pComponent: m_ComponentGarbageList)
    {
        Safe_Release(pComponent);
    }

    /** @note - string의 c_str() 주의사항 // c_str()은 string으로부터 const 문자열을 얻기 위해 사용
    * - string 객체는 일반 자료형처럼 스코프 벗어나면 자동으로 소멸자가 호출되어 삭제되나, 동적할당 시 당연하게도 직접 해제해줘야함.
    * 1. c_str()은 string 문자열의 문자열 재할당(+=), 삭제가 이뤄지면 바로 무효됨.
    * 2. string이 지역변수인 경우 절대 c_str()을 반환값으로 넘기면 안됨. -> string은 스코프를 벗어나면 소멸자가 호출되어 객체가 삭제되기 때문
    * 3. c_str()은 사용할 타이밍에만 최근 string을 c_str()로 변환해서 사용하는 것 (vector에 c_str()을 담는 것은 X) 
    * */
    m_CreateObjectVec.clear();
    m_RenderGroupVec.clear();
    m_LayerVec.clear();

    m_SpritePathMap.clear();

    CFileLoader::GetInstance()->DestroyInstance();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
