#include "..\Public\MyImGui.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "DInput_Manager.h"

USING(Tool)

IMPLEMENT_SINGLETON(CMyImGui);

CMyImGui::CMyImGui()
    : m_pSelectedObject(nullptr)
{
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
    ImGui::Begin("Installed Object", &bIsShow);
    
    ImGuiIO& io = ImGui::GetIO();
    ImTextureID my_tex_id = io.Fonts->TexID;
    float my_tex_w = (float)io.Fonts->TexWidth;
    float my_tex_h = (float)io.Fonts->TexHeight;

    static int pressed_count = 0;
    for (int i = 0; i < 8; i++)
    {
        // UV coordinates are often (0.0f, 0.0f) and (1.0f, 1.0f) to display an entire textures.
        // Here are trying to display only a 32x32 pixels area of the texture, hence the UV computation.
        // Read about UV coordinates here: https://github.com/ocornut/imgui/wiki/Image-Loading-and-Displaying-Examples
        ImGui::PushID(i);
        if (i > 0)
            ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(i - 1.0f, i - 1.0f));
        ImVec2 size = ImVec2(32.f, 32.f);                         // Size of the image we want to make visible
        ImVec2 uv0 = ImVec2(0.0f, 0.0f);                            // UV coordinates for lower-left
        ImVec2 uv1 = ImVec2(32.0f / my_tex_w, 32.0f / my_tex_h);    // UV coordinates for (32,32) in our texture
        ImVec4 bg_col = ImVec4(0.0f, 0.0f, 0.0f, 1.0f);             // Black background
        ImVec4 tint_col = ImVec4(1.0f, 1.0f, 1.0f, 1.0f);           // No tint
        if (ImGui::ImageButton("", my_tex_id, size, uv0, uv1, bg_col, tint_col))
            pressed_count += 1;
        if (i > 0)
            ImGui::PopStyleVar();
        ImGui::PopID();
        ImGui::SameLine();
    }

    // @Dainn.
    /* For.Prototype_Component_Sprite_LittleBorn */
    //if (FAILED(pGameInstance->Add_Prototype(LEVEL_LOGO, TEXT("Prototype_Component_Sprite_LittleBorn"),
    //    CTexture::Create(m_pDevice, m_pContext, TEXT("../Bin/Resources/Skul/Player/LittleBorn/Wait_%d.png"), 48))))
    //{
    //    Safe_Release(pGameInstance);
    //    return E_FAIL;
    //};

    //// ->  폴더명 LittleBorn들만 FolderList에 넣어서 출력한 다음에 클릭하면 해당 
    //if (ImGui::BeginListBox("Group_UI"))
    //{
    //    if (0 < m_GroupUIVec.size())
    //    {
    //        for (auto it = m_GroupUIVec[m_iGroupIndex].begin(); it != m_GroupUIVec[m_iGroupIndex].end(); ++it)
    //        {
    //            const bool is_selected = (iter == it);
    //            if (ImGui::Selectable((*it).pTag, is_selected))
    //            {
    //                iter = it;
    //                m_iPickedGroupUI = _uint(std::distance(m_GroupUIVec[m_iGroupIndex].begin(), it));
    //            }

    //            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
    //            if (is_selected)
    //                ImGui::SetItemDefaultFocus();
    //        }
    //    }
    //    ImGui::EndListBox();
    //}

    ImGui::End();
    return S_OK;
}

HRESULT CMyImGui::ShowSpriteWindow()
{
    static bool bIsShow = true;
    ImGui::Begin("Sprite", &bIsShow);


    // m_pTextureCom .. 만큼 만들어야 한다..? 흠.......... 
    // 내가 눌렀을 때, 해당 폴더로 오브젝트 생성 Add_GameObject(Prototype_TileFolder) 하고
    // 이 타일 폴더의 m_iMaxTextureNum만큼 띄우기? 그러면 이 개수만큼 오브젝트 있어야함 .. (이미지로 띄우려면)
    // 클릭한 해당 이미지로 생성 버튼 누르면 해당 폴더명과 인덱스로 오브젝트 생성됨.
    // 설치 목록들은 pair<이름, CGameObject*>로 있어서 리스트박스 이름 누르면 그 오브젝트 선택됨

    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    //pGameInstance->Add_GameObject(TEXT("Prototype_Component_Sprite_Tile"), LEVEL_STATIC, LAYER_PLAYER);
    //if (ImGui::BeginListBox("Group_UI"))
    //{
    //    if (0 < m_GroupUIVec.size())
    //    {
    //        for (auto it = m_GroupUIVec[m_iGroupIndex].begin(); it != m_GroupUIVec[m_iGroupIndex].end(); ++it)
    //        {
    //            const bool is_selected = (iter == it);
    //            if (ImGui::Selectable((*it).pTag, is_selected))
    //            {
    //                iter = it;
    //                m_iPickedGroupUI = _uint(std::distance(m_GroupUIVec[m_iGroupIndex].begin(), it));
    //            }

    //            // Set the initial focus when opening the combo (scrolling + keyboard navigation focus)
    //            if (is_selected)
    //                ImGui::SetItemDefaultFocus();
    //        }
    //    }
    //    ImGui::EndListBox();
    //}

    //ImGui::End();

    Safe_Release(pGameInstance);

    return S_OK;
}

HRESULT CMyImGui::ShowInspectorWindow()
{
    static bool bIsShow = true;
    ImGui::Begin("Inspector", &bIsShow);

    ImGui::End();
    return S_OK;
}

HRESULT CMyImGui::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGui::StyleColorsDark();

    ImGui_ImplWin32_Init(g_hWnd);
    ImGui_ImplDX11_Init(pDevice, pContext);

    return S_OK;
}

_uint CMyImGui::Tick(_double TimeDelta)
{
    CGameInstance* pGameInstance = CGameInstance::GetInstance();
    Safe_AddRef(pGameInstance);

    SPRITE_INFO tSpriteInfo;
    tSpriteInfo.fPosition = _float2(0.f, 0.f); // 마우스 클릭 위치
    //if (pGameInstance->Get_KeyStay(DIK_Z) && pGameInstance->Get_MouseDown(CDInput_Manager::MOUSEKEYSTATE::DIMK_LB))
    //{
    //    // 왜 m_Prototypes에 없지?
    //    m_pSelectedObject = pGameInstance->Clone_GameObject(TEXT("Prototype_GameObject_Install"), &tSpriteInfo);

    //    if(nullptr != m_pSelectedObject)
    //    {
    //        m_InstalledList.emplace_back(m_pSelectedObject);
    //    }
    //}

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

    ImGui::End();
    ImGui::Render();

    return S_OK;
}

void CMyImGui::Free()
{
    ImGui_ImplDX11_Shutdown();
    ImGui_ImplWin32_Shutdown();
    ImGui::DestroyContext();
}
