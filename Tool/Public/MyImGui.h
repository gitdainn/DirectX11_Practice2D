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

class CSpriteObject;

class CMyImGui : public CBase
{
	DECLARE_SINGLETON(CMyImGui)

private:
    explicit CMyImGui();
	/** @qurious - 자식이 없는데 virtual 써도 되는가? */
    virtual ~CMyImGui() = default;

private:
	struct SAVE_DESC
	{
		SPRITE_INFO	tSpriteInfo;
		const _tchar* pLayer = { nullptr };

	};

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
	void	Key_Input(_double TimeDelta);
	HRESULT Install_GameObject(SPRITE_INFO tSpriteInfo);
	void Add_SpriteListBox(const char* pFolderName);
	_tchar* ConvertSpriteComponentWithFolderName(const char* pFolderName) const;
	const _bool&		CheckSelectionChanged() const;

private:
	CSpriteObject* m_pPreviewObject;
	CSpriteObject* m_pSelectedObject;
	vector<const char*>		m_FolderNameVec;
	unique_ptr<_uint[]>		m_pSpriteListIndex;
	_uint					m_iFolderIndex;
	vector<CSpriteObject*>	m_CreateObjectVec;

	/** @note - 자료구조 선택 이유
	* 1. 토글 열 때마다 추가해야하는 폴더 경우 map으로 key값으로 찾아 상수 시간복잡도이고, 삽입 부담X
	* 2. 폴더 내 파일 경우 크기를 미리 알 수 있으므로 삽입 시 재할당 방지 가능하고, 인덱스 검색 시 상수시간복잡도인 vector
	* */
	unordered_map<const char*, vector<const char*>>	m_SpritePathMap; // <폴더명, 폴더 내 파일명>

private:
	virtual void Free() override;
};

END