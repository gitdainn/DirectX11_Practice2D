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
class CCollider;
class CComponent;
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

	typedef struct tPreviousSpriteInfo
	{
		_uint		iOrder = { 0 };
		_float2	fSize = { 300.f, 320.f };
		_float2	fSizeRatio = { 1.f, 1.f };
		_float2	fPosition = { 0.f, 0.f };

		_uint		iTextureIndex = { 0 };
		/** @note - const타입을 nullptr로 지정했는데도 구조체 생성 시 값을 넣을 수 있는 이유는 구조체는 호출 시 그때 생성되어 초기화하는 것이기 때문 */
		const _tchar* pTextureComTag = { nullptr };
		_float4	vColor = { 1.f, 1.f, 1.f, 1.f };
	}PREVIOUS_SPRITE_INFO;

private:
	HRESULT	ShowDemoWindow();
	HRESULT	ShowInstalledWindow();
	HRESULT	ShowSpriteWindow();
	HRESULT	ShowInspectorWindow();
	HRESULT ShowSettings();

private: // Inspector //
	HRESULT Default_Info(const _bool& bIsSelectionChanged);
	HRESULT Inspector_Transform(const _bool& bIsSelectionChanged);
	HRESULT Inspector_SpriteRenderer(const _bool& bIsSelectionChanged);
	HRESULT Inspector_Components(const _bool& bIsSelectionChanged);

private:
	HRESULT Inpsector_Collider(CCollider* pCollider, const _bool& bIsSelectionChanged);

public:
	virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual _uint Tick(_double TimeDelta);
	virtual _uint LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
	HRESULT Save_Object();
	HRESULT Load_Object();
	HRESULT	Save_Line();
	HRESULT	Load_Line();
	HRESULT Save_Object_Excel();
	HRESULT Load_Object_Excel();
	HRESULT Load_PreviousData(); // 구조체 수정 전에 저장했떤 파일 불러오기

private:
	void	Key_Input(_double TimeDelta);
	void	Get_MousePos(_vector& vMousePos) const;

	HRESULT Get_OpenFileName(OPENFILENAME& tOpenFileName);
	HRESULT Install_GameObject(SPRITE_INFO& tSpriteInfo);
	void Add_SpriteListBox(const char* pFolderName);
	_tchar* ConvertCWithWC(const char* pFolderName, const _tchar* pConvertText) const;
	const _bool&		CheckSelectionChanged() const;

private:
	const int m_iComponentsNum = 2;

private:
	CSpriteObject* m_pPreviewObject;
	CSpriteObject* m_pSelectedObject;
	vector<const char*>		m_FolderNameVec;
	vector<const char*>		m_ClassNameVec;
	vector<const char*>		m_RenderGroupVec;
	vector<pair<const char*, _uint>>		m_LayerVec;
	vector<const char*>		m_ComponentsVec;
	vector<const char*>		m_ColliderLayer;
	vector<CSpriteObject*>	m_CreateObjectVec;
	unique_ptr<_uint[]>		m_pSpriteListIndex;
	list<CComponent*>		m_ComponentGarbageList;

	_uint					m_iFolderIndex;
	_uint					m_iClassIndex;
	const char*				m_pLayerC;
	unordered_map<const _tchar*, ORIGINAL_DATA>		m_OriginalDataMap;

	_uint					m_iInstanceID = { 0 };

	/** @note - 자료구조 선택 이유
	* 1. 토글 열 때마다 추가해야하는 폴더 경우 map으로 key값으로 찾아 상수 시간복잡도이고, 삽입 부담X
	* 2. 폴더 내 파일 경우 크기를 미리 알 수 있으므로 삽입 시 재할당 방지 가능하고, 인덱스 검색 시 상수시간복잡도인 vector
	* */
	unordered_map<const char*, vector<const char*>>	m_SpritePathMap; // <폴더명, 폴더 내 파일명>

private:
	virtual void Free() override;
};

END