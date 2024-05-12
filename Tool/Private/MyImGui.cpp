#include "..\Public\MyImGui.h"
#include "GameObject.h"
#include "SpriteObject.h"
#include "GameInstance.h"
#include "DInput_Manager.h"
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
}

HRESULT CMyImGui::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(pDevice, pContext);

    m_FolderName.emplace_back("Tile");
    m_FolderName.emplace_back("ForestEnvironment");
    m_pSpriteListIndex = make_unique<_uint[]>(m_FolderName.size());

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    SPRITE_INFO tSpriteInfo;
    if (FAILED((pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Install"), LEVEL_STATIC, LAYER_DEFAULT, tSpriteInfo))))
    {
        MSG_BOX("CMyImGui - Tick() - NULL");
    }

    m_pPreviewObject = dynamic_cast<CSpriteObject*>(pGameInstance->Get_ObjectList(LEVEL_STATIC, LAYER_DEFAULT)->front());
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
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    SPRITE_INFO tSpriteInfo;
    tSpriteInfo.fPosition = _float2(0.f, 0.f); // 마우스 클릭 위치

    if (pGameInstance->Get_KeyStay(DIK_Z) && pGameInstance->Get_MouseDown(CDInput_Manager::MOUSEKEYSTATE::DIMK_LB))
    {
        SPRITE_INFO tSpriteInfo;
        tSpriteInfo.iTextureIndex = m_pSpriteListIndex[m_iFolderIndex];
        tSpriteInfo.pTextureTag = ConvertSpriteComponentWithFolderName(m_FolderName[m_iFolderIndex]);
        if (FAILED((pGameInstance->Add_GameObject(TEXT("Prototype_GameObject_Install"), LEVEL_TOOL, LAYER_DEFAULT, tSpriteInfo))))
        {
            MSG_BOX("CMyImGui - Tick() - NULL");
            Safe_Release(pGameInstance);
            return E_FAIL;
        }
        // @qurious - 이러면.. 오브젝트 안의 pTextureTag가 문자열은 없는데 공간은 참조 중이라 nullptr해줘야하긴해..
        Safe_Delete_Array(tSpriteInfo.pTextureTag); 
        m_pSelectedObject = pGameInstance->Get_ObjectList(LEVEL_TOOL, LAYER_DEFAULT)->back();

        if(nullptr != m_pSelectedObject)
        {
            m_InstalledList.emplace_back(m_pSelectedObject);
        }
    }

    Safe_Release(pGameInstance);

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

inline HRESULT CMyImGui::ShowDemoWindow()
{
    static bool bIsShow = true;
    ImGui::ShowDemoWindow(&bIsShow);
    
    return S_OK;
}

HRESULT CMyImGui::ShowInstalledWindow()
{
    //static bool bIsShow = true;
    //ImGui::Begin("Installed Object", &bIsShow);

    //ImGuiIO& io = ImGui::GetIO();
    //ImTextureID my_tex_id = io.Fonts->TexID;
    //float my_tex_w = (float)io.Fonts->TexWidth;
    //float my_tex_h = (float)io.Fonts->TexHeight;

    //static int pressed_count = 0;
    //for (int i = 0; i < 8; i++)
    //{
    //    // UV coordinates are often (0.0f, 0.0f) and (1.0f, 1.0f) to display an entire textures.
    //    // Here are trying to display only a 32x32 pixels area of the texture, hence the UV computation.
    //    // Read about UV coordinates here: https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
    //    ImGui::PushID(i);
    //    if (i > 0)
    //        ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(i - 1.0f, i - 1.0f));
    //    ImVec2 size = ImVec2(32.f, 32.f);                         // Size of the image we want to make visible
    //    ImVec2 uv0 = ImVec2(0.0f, 0.0f);                            // UV coordinates for lower-left
    //    ImVec2 uv1 = ImVec2(32.0f / my_tex_w, 32.0f / my_tex_h);    // UV coordinates for (32,32) in our texture
    //    ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             // Black background
    //    ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint
    //    if (ImGui::ImageButton("", my_tex_id, size, uv0, uv1, bg_col, tint_col))
    //        pressed_count += 1;
    //    if (i > 0)
    //        ImGui::PopStyleVar();
    //    ImGui::PopID();
    //    ImGui::SameLine();
    //}

    //// @Dainn.
    ///* For.Prototype_Component_Sprite_LittleBorn */
    ////if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Sprite_LittleBorn"),
    ////    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skul/Player/LittleBorn/Wait_%d.png"), 48))))
    ////{
    ////    Safe_Release(pGameInstance);
    ////    return E_FAIL;
    ////};

    ////// ->  폴더명 LittleBorn들만 FolderList에 넣어서 출력한 다음에 클릭하면 해당 
    ////if (ImGui::BeginListBox("Group_UI"))
    ////{
    ////    if (0 < m_GroupUIVec.size())
    ////    {
    ////        for (auto it = m_GroupUIVec[m_iGroupIndex].begin(); it != m_GroupUIVec[m_iGroupIndex].end(); ++it)
    ////        {
    ////            const bool is_selected = (iter == it);
    ////            if (ImGui::Selectable((*it).pTag, is_selected))
    ////            {
    ////                iter = it;
    ////                m_iPickedGroupUI = _uint(std::distance(m_GroupUIVec[m_iGroupIndex].begin(), it));
    ////            }

    ////            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
    ////            if (is_selected)
    ////                ImGui::SetItemDefaultFocus();
    ////        }
    ////    }
    ////    ImGui::EndListBox();
    ////}

    //ImGui::End();
    return S_OK;
}

HRESULT CMyImGui::ShowSpriteWindow()
{
    static bool bIsShow = true;
    ImGui::Begin("Sprite", &bIsShow);

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    /** @note - Tree는 열어야 내부 코드 실행됨 */
    for (_uint iFolderIndex = 0; iFolderIndex < m_FolderName.size(); ++iFolderIndex)
    {
        if (ImGui::TreeNode(m_FolderName[iFolderIndex]))
        {
            // Add는 한 번만 해야함
            /** @note - unique_ptr
            1. 레퍼런스 카운팅을 하지 않고, 스코프에서 벗어나면 즉시 해제되는 스마트 포인터
            2. unique_ptr은 기존 C 배열 초기화 가능, shared_ptr은 불가능 -> std::array나 std::vector 권장
            3. unique_ptr<자료형> 변수명(new 클래스()); 로도 생성 가능하나 make_unique로 생성할 것을 추천
            4. make_unique<bool[]>(개수); 배열 초기화는 memset 권장 (for문보다 속도 빠름)*/
            static unique_ptr<bool[]> bIsOnce = make_unique<bool[]>(m_FolderName.size());
            //memset(&bIsOnce, false, sizeof(bool) * m_FolderName.size());

            if (!bIsOnce[iFolderIndex])
            {
                Add_SpriteListBox(m_FolderName[iFolderIndex]);
                bIsOnce[iFolderIndex] = true;
            }

            vector<const char*> PathVec = m_SpritePathMap.find(m_FolderName[iFolderIndex])->second;

            if (ImGui::BeginListBox("Sprite"))
            {
                if (!PathVec.empty())
                {
                    for (_uint iListIndex = 0; iListIndex < PathVec.size(); ++iListIndex)
                    {
                        const bool is_selected = !strcmp(PathVec[iListIndex], PathVec[m_pSpriteListIndex[iFolderIndex]]);
                        // Selectable(const char* 형태의 첫번째 인자, bool 선택 여부) 
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

    ImGui::End();
    return S_OK;
}

void CMyImGui::Add_SpriteListBox(const char* pFolderName)
{
    _tchar* pPrototypeTag = ConvertSpriteComponentWithFolderName(pFolderName);
    m_pPreviewObject->Change_TextureComponent(pPrototypeTag);
    Safe_Delete_Array(pPrototypeTag);
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
    m_SpritePathMap.clear();

    m_InstalledList.clear();

    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
