#pragma once

#pragma region IMGUI
// 다른 헤더보다 먼저 추가해야 오류 안 뜸
#include "imgui.h"
#include "imgui_impl_win32.h"
#include "imgui_impl_dx11.h"
#pragma endregion

#include "Tool_Defines.h"
#include "Base.h"

BEGIN(Engine)
class CGameObject;
END

BEGIN(Tool)

class CMyImGui : public CBase
{
	DECLARE_SINGLETON(CMyImGui)

private:
    explicit CMyImGui();
	/** @qurious - 자식이 없는데 virtual 써도 되는가? */
    virtual ~CMyImGui() = default;

private:
	HRESULT	ShowDemoWindow();
	HRESULT	ShowInstalledWindow();
	HRESULT	ShowSpriteWindow();
	HRESULT	ShowInspectorWindow();

public:
	virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual _uint Tick(_double TimeDelta);
	virtual _uint LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	CGameObject* m_pSelectedObject;
	list<CGameObject*>		m_InstalledList;

private:
	virtual void Free() override;
};

END