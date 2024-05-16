#include "..\Public\MyImGui.h"
#include "GameObject.h"
#include "SpriteObject.h"
#include "GameInstance.h"
#include "DInput_Manager.h"
#include "Utility.h"
#include <array>
#include <memory>

typedef void (*ImGuiDemoMarkerCallback)(const char* file, int line, const char* section, void* user_data);
extern ImGuiDemoMarkerCallback      GImGuiDemoMarkerCallback;
extern void* GImGuiDemoMarkerCallbackUserData;
//ImGuiDemoMarkerCallback             GImGuiDemoMarkerCallback = NULL;
#define IMGUI_DEMO_MARKER(section)  do { if (GImGuiDemoMarkerCallback != NULL) GImGuiDemoMarkerCallback(__FILE__, __LINE__, section, GImGuiDemoMarkerCallbackUserData); } while (0)

USING(Tool)

IMPLEMENT_SINGLETON(CMyImGui);

CMyImGui::CMyImGui()
    : m_pPreviewObject(nullptr)
    , m_pSelectedObject(nullptr)
    , m_pSpriteListIndex(nullptr)
    , m_iFolderIndex(0)
{
    m_CreateObjectVec.reserve(100);
}

HRESULT CMyImGui::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(pDevice, pContext);

    m_FolderNameVec.emplace_back("Tile");
    m_FolderNameVec.emplace_back("ForestEnvironment");
    m_FolderNameVec.emplace_back("Background");
    m_pSpriteListIndex = make_unique<_uint[]>(m_FolderNameVec.size());

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    SPRITE_INFO tSpriteInfo;
    tSpriteInfo.fPosition = _float2(-(g_iWinSizeX * 0.5f) + 20.f, g_iWinSizeY * 0.5f - 20.f);
    if (FAILED((pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Install"), LEVEL_STATIC, LAYER_DEFAULT, tSpriteInfo))))
    {
        MSG_BOX("CMyImGui - Tick() - NULL");
    }

    m_pPreviewObject = dynamic_cast<CSpriteObject*>(pGameInstance->Get_ObjectList(LEVEL_STATIC, LAYER_DEFAULT)->front());
    m_pPreviewObject->Set_IsScroll(false);

    if (nullptr == m_pPreviewObject)
    {
        MSG_BOX("CMyImGui - Initialize - NULL");
        Safe_Release(pGameInstance);
    }

    Safe_Release(pGameInstance);

    return S_OK;
}

_uint CMyImGui::Tick(_double TimeDelta)
{
    Key_Input(TimeDelta);

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

    ImGui::Render();

    ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());

    return S_OK;
}

HRESULT CMyImGui::Save_Object()
{
    // 저장은 전체 한번에 저장하면, Dat파일을 나눌 수 없어서 안됨.
    // 로드는 파일별로 자동으로 들어갈 수 있게 모델 인덱스까지 저장한다.
    if (ImGui::Button("Save"))
    {
        if (m_CreateObjectVec.empty())
        {
            MSG_BOX("Save_Object is Empty");
            return S_OK;
        }

        // Win Api Save File Dialog
        OPENFILENAME OFN;
        TCHAR filePathName[128] = L"";
        TCHAR lpstrFile[256] = L".data";
        static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.BattleUIdat)\0*.BattleUIdat";
        //static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.BattleUIdat)\0*.BattleUIdat";

        ZeroMemory(&OFN, sizeof(OPENFILENAME));
        OFN.lStructSize = sizeof(OPENFILENAME);
        OFN.hwndOwner = g_hWnd;
        OFN.lpstrFilter = filter;
        OFN.lpstrFile = lpstrFile;
        OFN.nMaxFile = 256;
        OFN.lpstrInitialDir = L"..\\Bin\\DataFiles";

        if (GetSaveFileName(&OFN) != 0)
        {
            const TCHAR* pGetPath = OFN.lpstrFile;

            HANDLE hFile = CreateFile(pGetPath, GENERIC_WRITE, 0, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);

            if (INVALID_HANDLE_VALUE == hFile)
                return E_FAIL;

            DWORD dwByte = 0;

#pragma region SAVE
            _uint iVecSize = (_uint)m_CreateObjectVec.size();
            WriteFile(hFile, &iVecSize, sizeof(_uint), &dwByte, nullptr); // 길이 저장

            for (CSpriteObject* pObject : m_CreateObjectVec) // 현재 선택된 그룹 저장
            {
                _vector vPos = pObject->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
                SPRITE_INFO tSpriteInfo = pObject->Get_SpriteInfo();
                tSpriteInfo.fPosition = _float2(XMVectorGetX(vPos), XMVectorGetY(vPos));
                WriteFile(hFile, &tSpriteInfo, sizeof(SPRITE_INFO), &dwByte, nullptr);

                _uint iLength = sizeof(_tchar) * lstrlen(tSpriteInfo.pTextureComTag) + 1;
                WriteFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr); // 길이 저장
                WriteFile(hFile, tSpriteInfo.pTextureComTag, iLength, &dwByte, nullptr); // 길이 저장

            }
            CloseHandle(hFile);
        }
    }

    return S_OK;
}

HRESULT CMyImGui::Load_Object()
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    /* 저장순서 : 게임오브젝트명, 벡터원소갯수, 텍스처프로토이름, 텍스처인덱스, UIRECTDESC 구조체, 색상, 선형보간, 액션 타입, 액션 구조체 */
    if (ImGui::Button("Load"))
    {
        OPENFILENAME OFN;
        TCHAR filePathName[128] = L"";
        TCHAR lpstrFile[256] = L".data";
        static TCHAR filter[] = L"모두(*.*)\0*.*\0데이터 파일(*.BattleUIdat)\0*.BattleUIdat";

        ZeroMemory(&OFN, sizeof(OPENFILENAME));
        OFN.lStructSize = sizeof(OPENFILENAME);
        OFN.hwndOwner = g_hWnd;
        OFN.lpstrFilter = filter;
        OFN.lpstrFile = lpstrFile;
        OFN.nMaxFile = 256;
        OFN.lpstrInitialDir = L"..\\Bin\\DataFiles";

        if (GetOpenFileName(&OFN) != 0)
        {
            const TCHAR* pGetPath = OFN.lpstrFile;

            HANDLE hFile = CreateFile(pGetPath, GENERIC_READ, 0, 0, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, 0);

            if (INVALID_HANDLE_VALUE == hFile)
                return E_FAIL;

            DWORD   dwByte = 0;
            _bool      bRes = { false };

#pragma region LOAD
            // Load : 현재 모델이 가지고 있는 이펙트 사이즈
            _uint iVecSize = { 0 };
            bRes = ReadFile(hFile, &iVecSize, sizeof(_uint), &dwByte, nullptr);

            _uint	iTagIndex = { 0 };
            for (_uint i = 0; i < iVecSize; ++i)
            {
                SPRITE_INFO tSpriteInfo;
                bRes = ReadFile(hFile, &tSpriteInfo, sizeof(SPRITE_INFO), &dwByte, nullptr);

                _uint iLength = { 0 };
                bRes = ReadFile(hFile, &iLength, sizeof(_uint), &dwByte, nullptr);
                _tchar* pTag = new _tchar[iLength]{};
                if (-1 != iLength)
                {
                    bRes = ReadFile(hFile, pTag, iLength, &dwByte, nullptr);
                    tSpriteInfo.pTextureComTag = pTag;
                }

                if (FAILED(Install_GameObject(tSpriteInfo)))
                {
                    MSG_BOX("CMyImGui - Load_Object() - FAILED");
                    CloseHandle(hFile);
                    Safe_Release(pGameInstance);
                    return E_FAIL;
                }
            }
            CloseHandle(hFile);
        }
    }

    Safe_Release(pGameInstance);

    return S_OK;
}

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

    static int iSelectIndex = { 0 };

    if (ImGui::BeginListBox("InstalledObject"))
    {
        for (int iListIndex = 0; iListIndex < m_CreateObjectVec.size(); iListIndex++)
        {
            const bool is_selected = (iSelectIndex == iListIndex);
            // Selectable은 콜백 함수, const char* 첫번째 인자와 동일한 리스트를 선택 (즉, 문자열이 동일하면 먼저 찾은 리스트가 선택됨)  
            if (ImGui::Selectable(m_CreateObjectVec[iListIndex]->Get_SpriteTag(), is_selected))
            {
                m_pSelectedObject = m_CreateObjectVec[iListIndex];
                iSelectIndex = iListIndex; // 선택한 항목의 인덱스를 저장
            }

            if (is_selected)
                ImGui::SetItemDefaultFocus();
        }
        ImGui::EndListBox();
    }

    if (FAILED(Save_Object()))
    {
        MSG_BOX("CMyImGui - ShowInspectorWindow - FAILED");
    }
    ImGui::SameLine();
    if (FAILED(Load_Object()))
    {
        MSG_BOX("CMyImGui - ShowInspectorWindow - FAILED");
    }

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

    /** @note - Tree는 열어야 내부 코드 실행됨 */
    for (_uint iFolderIndex = 0; iFolderIndex < m_FolderNameVec.size(); ++iFolderIndex)
    {
        if (ImGui::TreeNode(m_FolderNameVec[iFolderIndex]))
        {
            // Add는 한 번만 해야함
            /** @note - unique_ptr
            1. 레퍼런스 카운팅을 하지 않고, 스코프에서 벗어나면 즉시 해제되는 스마트 포인터
            2. unique_ptr은 기존 C 배열 초기화 가능, shared_ptr은 불가능 -> std::array나 std::vector 권장
            3. unique_ptr<자료형> 변수명(new 클래스()); 로도 생성 가능하나 make_unique로 생성할 것을 추천
            4. make_unique<bool[]>(개수); 배열 초기화는 memset 권장 (for문보다 속도 빠름)*/
            static unique_ptr<bool[]> bIsOnce = make_unique<bool[]>(m_FolderNameVec.size());
            //memset(&bIsOnce, false, sizeof(bool) * m_FolderNameVec.size());

            if (!bIsOnce[iFolderIndex])
            {
                Add_SpriteListBox(m_FolderNameVec[iFolderIndex]);
                bIsOnce[iFolderIndex] = true;
            }

            vector<const char*> PathVec = m_SpritePathMap.find(m_FolderNameVec[iFolderIndex])->second;

            if (ImGui::BeginListBox("Sprite"))
            {
                if (!PathVec.empty())
                {
                    for (_uint iListIndex = 0; iListIndex < PathVec.size(); ++iListIndex)
                    {
                        const bool is_selected = !strcmp(PathVec[iListIndex], PathVec[m_pSpriteListIndex[iFolderIndex]]);
                        // Selectable(const char* 리스트에 띄울 이름, bool 선택 여부) 
                        if (ImGui::Selectable(PathVec[iListIndex], is_selected))
                        {
                            SPRITE_INFO tSpriteInfo = m_pPreviewObject->Get_SpriteInfo();
                            tSpriteInfo.iTextureIndex = m_pSpriteListIndex[iFolderIndex] = iListIndex;
                            m_pPreviewObject->Set_SpriteInfo(tSpriteInfo);
                            m_iFolderIndex = iFolderIndex;
                        }

                        // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
                        if (is_selected)
                            ImGui::SetItemDefaultFocus();
                    }
                }
                ImGui::EndListBox();
            }

            ImGui::TreePop();
        }
    }

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

    CSpriteObject* pObject = dynamic_cast<CSpriteObject*>(m_pSelectedObject);
    if (nullptr == pObject)
    {
        ImGui::End();
        return S_OK;
    }

    const _bool bIsSelectionChanged = CheckSelectionChanged();
    //char* pTag = new char[strlen(pObject->Get_SpriteTag() + 1)] {};
    static char* pTag = { nullptr };
    //if (bIsSelectionChanged)
    //{
    //    strcpy(pTag, pObject->Get_SpriteTag());
    //}
    //ImGui::InputText("Tag", pTag, strlen(pTag) + 1);
    //pObject->Set_SpriteTag(pTag);

    if (ImGui::TreeNode("Transform"))
    {
        static enum eTransform { POSITION, ROTATION, SCALE, TRANS_END };
        static const _float fSpeed[eTransform::TRANS_END] = { 0.5f, 0.5f, 1.f };
        static const _float fMin[eTransform::TRANS_END] = { -10000.f, 0.f, 1.f };
        static const _float fMax[eTransform::TRANS_END] = { 10000.f, 360.f, 10.f };
        
        CTransform* pSelectedObjectTransform = m_pSelectedObject->Get_TransformCom();
        CTexture* pTexture = dynamic_cast<CTexture*>(m_pSelectedObject->Get_Component(TAG_TEXTURE));

        if (nullptr == pTexture)
        {
            ImGui::End();
            return S_OK;
        }

        _vector vPosition = pSelectedObjectTransform->Get_State(CTransform::STATE_POSITION);
        const int iTextureIndex = m_pSelectedObject->Get_SpriteInfo().iTextureIndex;
        _float2 vScale = pTexture->Get_OriginalTextureSize(iTextureIndex);
        _float2 vScaleRatio = m_pSelectedObject->Get_ScaleRatio();

        _float fPosition[3] = { XMVectorGetX(vPosition), XMVectorGetY(vPosition), 0.f };
        _float fScaleRatio[3] = { vScaleRatio.x, vScaleRatio.y, 1.f };

        ImGui::DragFloat3("Position", fPosition
            , fSpeed[eTransform::POSITION], fMin[eTransform::POSITION], fMax[eTransform::POSITION]);
        //ImGui::DragFloat3("Rotation", vPosition
        //    , fSpeed[eTransform::ROTATION], fMin[eTransform::ROTATION], fMax[eTransform::ROTATION]);
        ImGui::DragFloat3("Scale", fScaleRatio
            , fSpeed[eTransform::SCALE], fMin[eTransform::SCALE], fMax[eTransform::SCALE]);

        pSelectedObjectTransform->Set_State(CTransform::STATE_POSITION, XMVectorSet(fPosition[0], fPosition[1], fPosition[2], 1.f));
        m_pSelectedObject->Set_ScaleRatio(_float2(fScaleRatio[0], fScaleRatio[1]));
        ImGui::TreePop();
    }

    if (ImGui::TreeNode("SpriteRenderer"))
    {
        static int iOrder = { 0 };
        if(bIsSelectionChanged)
            iOrder = m_pSelectedObject->Get_Order();
        ImGui::InputInt("Order in Layer", &iOrder);
        m_pSelectedObject->Set_Order(iOrder);
        ImGui::TreePop();
    }



    ImGui::End();
    return S_OK;
}

void CMyImGui::Key_Input(_double TimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    if (pGameInstance->Get_MouseStay(CDInput_Manager::MOUSEKEYSTATE::DIMK_RB))
    {
        //_vector vMousePos = CUtility::Get_MousePos(g_hWnd, g_iWinSizeY, g_iWinSizeY); 
        POINT ptMouse{};
        GetCursorPos(&ptMouse);
        ScreenToClient(g_hWnd, &ptMouse);

        _float2 MousePos = _float2((_float)ptMouse.x, (_float)ptMouse.y);

        _vector vMousePos = XMVectorSet(MousePos.x, MousePos.y, 0.0f, 1.f);

        // @qurious - 마우스 스크롤 왜 X는 -고, Y는 + 인지 분석, 왜 엔진에 넘기면 마우스가 제대로 안 따라오는지!
        _float fX = XMVectorGetX(vMousePos) - pGameInstance->Get_ScrollX();
        _float fY = XMVectorGetY(vMousePos) + pGameInstance->Get_ScrollY();

        /* 투영 변환 X, API 뷰포트 좌표를 DirectX 뷰포트로 보정한 것 */
        vMousePos = XMVectorSet(fX - (g_iWinSizeX >> 1), -fY + (g_iWinSizeY >> 1), 0.f, 1.f);

        /* 직교 투영의 경우, 내부적으로 변화는 일어나지만 월드 좌표와 뷰포트 좌표의 차이가 없기에 뷰포트 좌표를 월드로 셋팅 */
        if (nullptr == m_pSelectedObject)
            return;
        m_pSelectedObject->Get_TransformCom()->Set_State(CTransform::STATE_POSITION, vMousePos);
    }
    // 설치
    if (pGameInstance->Get_KeyStay(DIK_Z) && pGameInstance->Get_MouseDown(CDInput_Manager::MOUSEKEYSTATE::DIMK_LB))
    {
        SPRITE_INFO tSpriteInfo;
        tSpriteInfo.iTextureIndex = m_pSpriteListIndex[m_iFolderIndex];
        tSpriteInfo.pTextureComTag = ConvertSpriteComponentWithFolderName(m_FolderNameVec[m_iFolderIndex]);
        Install_GameObject(tSpriteInfo);
    }

    // 복제
    if (pGameInstance->Get_KeyStay(DIK_LSHIFT) && pGameInstance->Get_MouseDown(CDInput_Manager::MOUSEKEYSTATE::DIMK_LB))
    {
        CSpriteObject* pObject = dynamic_cast<CSpriteObject*>(m_pSelectedObject);
        if (nullptr == pObject)
            return;

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
        list<CGameObject*>* pObjectList = pGameInstance->Get_ObjectList(LEVEL_TOOL, LAYER_DEFAULT);
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
            m_pSelectedObject = m_CreateObjectVec.back();
    }

    if (pGameInstance->Get_KeyDown(DIK_DELETE))
    {
        if (m_CreateObjectVec.empty())
        {
            Safe_Release(pGameInstance);
            return;
        }


        list<CGameObject*>* pObjectList = pGameInstance->Get_ObjectList(LEVEL_TOOL, LAYER_DEFAULT);
        if (nullptr == pObjectList)
        {
            Safe_Release(pGameInstance);
            return;
        }

        /** @note - remove와 erase
        * 1. list는 void remove를 멤버로 보유, 인자값과 동일한 요소를 list상에서 삭제 (vector는 remove X)
        * 2. erase는 모든 컨테이너가 보관하고 있는 멤버, 인자값 iter와 동일한 원소를 삭제하고 뒤의 원소를 앞으로 당긴 다음 ++iter를 반한함.
        * 3. <algorithm>의 remove 함수는 삭제할 요소에 유지할 요소를 덮어씌우는 방식으로 맨 뒤에는 필요없는 기존값들이 그대로 유지되어 있음 (용량은 그대로란 뜻)
        * -> 이를 지우기 위해 erase(remove(begin, end, 요소), end)를 사용하여 remove의 반환값인 new end 부터 end까지 해당 범위를 erase하는 방식 채택
        * */
        (*pObjectList).remove(m_pSelectedObject);
        m_CreateObjectVec.erase(remove(m_CreateObjectVec.begin(), m_CreateObjectVec.end(), m_pSelectedObject), m_CreateObjectVec.end());
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

HRESULT CMyImGui::Install_GameObject(SPRITE_INFO& tSpriteInfo)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    static int iIndex = 0;

    if (FAILED((pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Install"), LEVEL_TOOL, LAYER_DEFAULT, tSpriteInfo))))
    {
        MSG_BOX("CMyImGui - Install_GameObject() - NULL");
        Safe_Release(pGameInstance);
        return E_FAIL;
    }
    // @qurious - 이러면.. 오브젝트 안의 pTextureComTag가 문자열은 없는데 공간은 참조 중이라 nullptr해줘야하긴해..
    const list<CGameObject*>* pObjectList = pGameInstance->Get_ObjectList(LEVEL_TOOL, LAYER_DEFAULT);
    if (nullptr == pObjectList)
    {
        MSG_BOX("CMyImGui - Install_GameObject() - NULL");
        Safe_Release(pGameInstance);
        return E_FAIL;
    }
    m_pSelectedObject = dynamic_cast<CSpriteObject*>(pObjectList->back());
    if (nullptr == m_pSelectedObject)
    {
        MSG_BOX("CMyImGui - Install_GameObject() - NULL");
        Safe_Release(pGameInstance);
        return E_FAIL;
    }
    char* pTag = new char[MAX_PATH] {};
    strcpy(pTag, m_FolderNameVec[m_iFolderIndex]);
    strcat(pTag, to_string(++iIndex).c_str());
    m_pSelectedObject->Set_SpriteTag(pTag);
    m_CreateObjectVec.emplace_back(m_pSelectedObject);

    //if (nullptr != m_pSelectedObject)
    //{
    //    m_InstalledList.emplace_back(m_pSelectedObject);
    //}

    Safe_Release(pGameInstance);
    return S_OK;
}

void CMyImGui::Add_SpriteListBox(const char* pFolderName)
{
    _tchar* pPrototypeTag = ConvertSpriteComponentWithFolderName(pFolderName);
    m_pPreviewObject->Change_TextureComponent(pPrototypeTag);
    vector<const _tchar*> m_TexturePathVec = *m_pPreviewObject->Get_TexturePathVec();

    vector<const char*> CPathVec;
    CPathVec.reserve(m_TexturePathVec.size());

    for (_uint i = 0; i < m_TexturePathVec.size(); ++i)
    {
        char* pPath = { nullptr };
        WCToC(m_TexturePathVec[i], pPath);
        CPathVec.emplace_back(pPath);
    }
    m_SpritePathMap.insert({ pFolderName, CPathVec });
}

_tchar* CMyImGui::ConvertSpriteComponentWithFolderName(const char* pFolderName) const
{
    // @note - 동적할당 문자열은 _tchar로 선언 가능. 문자열 리터럴은 무조건 const로 선언
    _tchar* pPrototypeTag = new _tchar[MAX_PATH];
    ZeroMemory(pPrototypeTag, sizeof(_tchar) * MAX_PATH);
    // @note - lstrcat은 첫 번째 인자의 '\0' 부분에 붙이는 것이므로 초기화 안하고 붙이면 췍췍췍 뒤에 붙음
    lstrcat(pPrototypeTag, TEXT("Prototype_Component_Sprite_"));
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

void CMyImGui::Free()
{
    /** @note - const vector<자료형>하면 값을 바꿀 수 없기에 clear도 불가능 */
    // 폴더명은 상수라 자동 해제, 파일명은 WCToC로 동적할당해서 담은 문자열이므로 직접 해제
    unordered_map<const char*, vector<const char*>>::iterator iter = m_SpritePathMap.begin();
    for (iter; iter != m_SpritePathMap.end(); ++iter)
    {
        for (const char* pFilePath : iter->second)
        {
            Safe_Delete_Array(pFilePath);
        }
        (iter->second).clear();
    }

    /** @note - string의 c_str() 주의사항 // c_str()은 string으로부터 const 문자열을 얻기 위해 사용
    * - string 객체는 일반 자료형처럼 스코프 벗어나면 자동으로 소멸자가 호출되어 삭제되나, 동적할당 시 당연하게도 직접 해제해줘야함.
    * 1. c_str()은 string 문자열의 문자열 재할당(+=), 삭제가 이뤄지면 바로 무효됨.
    * 2. string이 지역변수인 경우 절대 c_str()을 반환값으로 넘기면 안됨. -> string은 스코프를 벗어나면 소멸자가 호출되어 객체가 삭제되기 때문
    * 3. c_str()은 사용할 타이밍에만 최근 string을 c_str()로 변환해서 사용하는 것 (vector에 c_str()을 담는 것은 X) 
    * */
    m_CreateObjectVec.clear();

    m_SpritePathMap.clear();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
