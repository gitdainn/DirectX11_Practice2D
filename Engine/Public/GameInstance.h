#pragma once

#include "Component_Manager.h"
#include "PipeLine.h"
#include "DInput_Manager.h"

/* 엔진의 핵심이 되는 싱글턴 클래스.  */
/* 엔진에서 사용되는 다양한 매니져클래스를 가지고 있는다. */
/* 보여줘야할 함수만 정의하여 클라로 보여준다. */

BEGIN(Engine)

class ENGINE_DLL CGameInstance final : public CBase
{
	DECLARE_SINGLETON(CGameInstance)
private:
	CGameInstance();
	virtual ~CGameInstance() = default;

public: /* For.GameInstance */
	HRESULT Initialize_Engine(_uint iNumLevels, HINSTANCE hInstance, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut);
	HRESULT Tick_Engine(_double TimeDelta);
	void Clear_Engine(_uint iLevelIndex);

public: /* For.Timer_Manager */
	HRESULT	Ready_Timer(const _tchar* pTimerTag);
	_double Get_TimeDelta(const _tchar* pTimerTag);
	void Set_TimeDelta(const _tchar* pTimerTag);

public: /* For.Graphic_Device */
	HRESULT Clear_BackBuffer_View(_float4 vClearColor);	
	HRESULT Clear_DepthStencil_View();	
	HRESULT Present();

public: /* For.Dinput_Manager */
	_bool		Get_KeyStay(_ubyte ubyKeyID);
	_bool		Get_KeyDown(_ubyte ubyKeyID);
	_bool		Get_KeyUp(_ubyte ubyKeyID);

	_bool		Get_MouseStay(CDInput_Manager::MOUSEKEYSTATE eMouseID);
	_bool		Get_MouseDown(CDInput_Manager::MOUSEKEYSTATE eMouseID);
	_bool		Get_MouseUp(CDInput_Manager::MOUSEKEYSTATE eMouseID);

	_long		Get_MouseMove(CDInput_Manager::MOUSEMOVESTATE eMouseMoveID);

public: /* For.Level_Manager */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);

public: /* For.Object_Manager */
	class CComponent* Get_Component(_uint iLevelIndex, const _tchar* pLayerTag, const _tchar* pComponentTag, _uint iIndex = 0);
	const list<class CGameObject*>* Get_ObjectList(_uint iLevelIndex, const _tchar* pLayerTag);
	HRESULT Add_Prototype(const _tchar* pPrototypeTag, class CGameObject* pPrototype);
	HRESULT Add_GameObject(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, void* pArg = nullptr); /* 원형을 복제하여 사본을 추가한다. */
	HRESULT Add_GameObject(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, const tSpriteInfo& ObjectInfo, void* pArg = nullptr); /* 원형을 복제하여 사본을 추가한다. */
	class CGameObject* Clone_GameObject(const _tchar * pPrototypeTag, void* pArg = nullptr);
	class CGameObject* Clone_GameObject(const _tchar* pPrototypeTag, const tSpriteInfo& SpriteInfo, void* pArg = nullptr);

public: /* For.Component_Manager */
	HRESULT Add_Prototype(_uint iLevelIndex, const _tchar* pPrototypeTag, class CComponent* pPrototype);
	class CComponent* Clone_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, void* pArg = nullptr);

public: /* For.PipeLine */
	void Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix);
	_float4x4 Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState);
	_matrix Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState);
	_float4 Get_CamPosition();

public: /* For.Light_Manager */
	const LIGHTDESC* Get_Light(_uint iIndex);
	HRESULT Add_Light(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const LIGHTDESC& LightDesc);

public: /* For.Font_Manager */
	HRESULT Add_Font(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pFontTag, const _tchar* pFontFilePath);
	HRESULT Render_Font(const _tchar* pFontTag, const _tchar * pText, const _float2& vPosition, _fvector vColor, _float fRotation = 0.f, const _float2& vRotationOrigin = _float2(0.f, 0.f), const _float2& vScale = _float2(1.f, 1.f));

public: /* For.Frustum */
	_bool isIn_WorldFrustum(_fvector vPosition, _float fRange = 0.f);

public: /* For.Scroll_Manager */
	const _float& Get_ScrollX() const;
	const _float& Get_ScrollY() const;
	void		Set_ScrollX(const _float _fX);
	void		Set_ScrollY(const _float _fY);

//public: /* For.GarbageCollector */
//	template<typename T>
//	void	Add_Garbage(T& Garbage);
//	template<typename T>
//	void	Add_CurLevelGarbage(T& Garbage);
//	template<typename T>
//	void	Clear_CurLevelGarbage(T Null = nullptr); // 이거.. 어케해야할지 몰겠음.

public:
	static void Release_Engine();

private:
	class CTimer_Manager*				m_pTimer_Manager = { nullptr };
	class CGraphic_Device*				m_pGraphic_Device = { nullptr };
	class CLevel_Manager*				m_pLevel_Manager = { nullptr };
	class CObject_Manager*				m_pObject_Manager = { nullptr };
	class CComponent_Manager*			m_pComponent_Manager = { nullptr };
	class CPipeLine*					m_pPipeLine = { nullptr };
	class CLight_Manager*				m_pLight_Manager = { nullptr };
	class CFont_Manager*				m_pFont_Manager = { nullptr };
	class CFrustum*						m_pFrustum = { nullptr };
	class CDInput_Manager*				m_pDInput_Manager = { nullptr };
	class CScroll_Manager*				m_pScroll_Manager = { nullptr };

public:
	virtual void Free() override;
};

END
