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
	HRESULT Default_Info(const _bool bIsSelectionChanged);
	HRESULT Inspector_Transform(const _bool bIsSelectionChanged);
	HRESULT Inspector_SpriteRenderer(const _bool bIsSelectionChanged);
	HRESULT	Inspector_Multiple(const _bool bIsSelectionChanged);
	HRESULT Inspector_Components(const _bool bIsSelectionChanged);

private:
	HRESULT Inpsector_Collider(CCollider* pCollider, const _bool bIsSelectionChanged);
public:
	virtual HRESULT Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual _uint Tick(_double TimeDelta);
	virtual _uint LateTick(_double TimeDelta);
	virtual HRESULT Render();

private:
#ifdef _DEBUG // 폴더에 이미지 추가 시 기존 데이터 복구 방법
	/** 폴더에 이미지 추가 시 기존의 이미지 인덱스가 변경되므로 아래 내용을 읽어주세요.
	* 1. 이미지 추가 전 파일명으로 기존 데이터를 저장합니다. (Load_Object() -> Save_FileName_Object())
	* 2. 이미지 추가 후 기존 데이터를 파일명으로 불러온 뒤, 저장 시 인덱스로 저장합니다. (이미지 추가 -> Load_FileName_Object() -> Save_Object())
	* 3. 폴더에 이미지 추가로 변경된 인덱스에 알맞게 기존 데이터를 사용할 수 있게 됩니다. (Load_Object())*/

	// 고유한 형식의 해쉬(=지문)으로 얻는 방법도 있다고 함
	//constexpr uint32_t FNV1a_Hash(const std::string& str, uint32_t hash = 2166136261u);
	HRESULT Save_FileName_Object();
	HRESULT Load_FileName_Object();
#endif

	/** 현재 렌더그룹은 저장을 안하고 있습니다. 클라이언트의 Initialize에서 셋팅한 값이 적용되고 있습니다. */
	HRESULT Save_Object();
	HRESULT Load_Object();
	HRESULT Save_Widget();
	HRESULT Load_Widget();
	HRESULT	Save_Line();
	HRESULT	Load_Line();
	HRESULT Save_Object_Excel();
	HRESULT Load_Object_Excel();

private:
	void	Key_Input(_double TimeDelta);

private: // Utility //
	/** 뷰포트상의 마우스 위치를 반환합니다. *
	* @notice - 직교투영 시 월드 위치와 뷰포트 상의 위치는 동일함으로 직교투영한 오브젝트를 다루고자 할 때 사용해주세요. */
	void	Get_MousePosInViewport(_vector& vOutPos) const;
	/** 인자값으로 받은 인덱스의 Z 평면과 마우스의 교차 지점의 위치를 반환합니다. *
	* @param iZIndex - n ~ f 사이의 z 인덱스를 넣어주세요. (ex. z=1로 원근투영 중인 오브젝트를 마우스 위치로 위치시키고 싶을 때 1 넘김)*/
	void	Get_IntersectMouseWithZPlane(_vector& vOutPos, const _uint iZIndex) const;

private:
	HRESULT Get_OpenFileName(OPENFILENAME& tOpenFileName);
	HRESULT Install_GameObject(SPRITE_INFO& tSpriteInfo);
	_tchar* ConvertCWithWC(const char* pFolderName, const _tchar* pConvertText) const;
	const _bool&		CheckSelectionChanged() const;

	HRESULT	Ready_ContainerResource(const vector<const char*>& FolderNameVec);
private:
	const int m_iComponentsNum = 2;

private:
	CSpriteObject* m_pSelectedObject;
	unordered_map<const char*, CTexture*>	m_FolderTextureMap;
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

	/** @note - 자료구조 선택 이유
	* 1. 토글 열 때마다 추가해야하는 폴더 경우 map으로 key값으로 찾아 상수 시간복잡도이고, 삽입 부담X
	* 2. 폴더 내 파일 경우 크기를 미리 알 수 있으므로 삽입 시 재할당 방지 가능하고, 인덱스 검색 시 상수시간복잡도인 vector
	* */
	unordered_map<const char*, vector<const char*>>	m_SpritePathMap; // <폴더명, 폴더 내 파일명>

private:
	virtual void Free() override;
};

END