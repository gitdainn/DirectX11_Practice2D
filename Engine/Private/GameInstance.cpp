#include "..\Public\GameInstance.h"
#include "Timer_Manager.h"
#include "Graphic_Device.h"
#include "Target_Manager.h"
#include "Input_Device.h"
#include "Level_Manager.h"
#include "Object_Manager.h"
#include "Light_Manager.h"
#include "Font_Manager.h"
#include "PipeLine.h"
#include "Frustum.h"

IMPLEMENT_SINGLETON(CGameInstance)

CGameInstance::CGameInstance()
	: m_pTimer_Manager(CTimer_Manager::GetInstance())
	, m_pGraphic_Device(CGraphic_Device::GetInstance())
	, m_pInput_Device(CInput_Device::GetInstance())
	, m_pLevel_Manager(CLevel_Manager::GetInstance())
	, m_pObject_Manager(CObject_Manager::GetInstance())
	, m_pComponent_Manager(CComponent_Manager::GetInstance())
	, m_pPipeLine(CPipeLine::GetInstance())
	, m_pLight_Manager(CLight_Manager::GetInstance())
	, m_pFont_Manager(CFont_Manager::GetInstance())
	, m_pFrustum(CFrustum::GetInstance())
	, m_pTarget_Manager(CTarget_Manager::GetInstance())
{
	Safe_AddRef(m_pTarget_Manager);
	Safe_AddRef(m_pFrustum);
	Safe_AddRef(m_pFont_Manager);
	Safe_AddRef(m_pLight_Manager);
	Safe_AddRef(m_pPipeLine);
	Safe_AddRef(m_pComponent_Manager);
	Safe_AddRef(m_pObject_Manager);
	Safe_AddRef(m_pLevel_Manager);
	Safe_AddRef(m_pInput_Device);
	Safe_AddRef(m_pGraphic_Device);	
	Safe_AddRef(m_pTimer_Manager);
}

HRESULT CGameInstance::Initialize_Engine(_uint iNumLevels, HINSTANCE hInstance, const GRAPHIC_DESC& GraphicDesc, ID3D11Device** ppDeviceOut, ID3D11DeviceContext** ppContextOut)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;

	/* 엔진을 사용하기위해 필수적으로 초기화가 필요한 매니져들의 기타 초기화작업을 수행한다. */
	/* 그래픽 디바이스 초기화. */
	if (FAILED(m_pGraphic_Device->Ready_Graphic_Device(GraphicDesc.hWnd, GraphicDesc.eWinMode, GraphicDesc.iViewSizeX, GraphicDesc.iViewSizeY, ppDeviceOut, ppContextOut)))
		return E_FAIL;

	/* 다렉인풋 디바이스 초기화. */
	if (FAILED(m_pInput_Device->Ready_DInput(hInstance, GraphicDesc.hWnd)))
		return E_FAIL;

	/* 사운드초기화 */

	/* 오브젝트 매니져의 공간 예약. */
	if (FAILED(m_pObject_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;

	/* 컴포넌트 매니져의 공간 예약. */
	if (FAILED(m_pComponent_Manager->Reserve_Manager(iNumLevels)))
		return E_FAIL;

	if (FAILED(m_pFrustum->Initialize()))
		return E_FAIL;
	return S_OK;
}

/* 호출순서가 x같아요. */
HRESULT CGameInstance::Tick_Engine(_double TimeDelta)
{
	/* 입력장치의 정보 받아오기.  */
	m_pInput_Device->Update_DInput();

	/* 레벨 매니져의 업데이트 */
	m_pLevel_Manager->Tick_Level(TimeDelta);

	/* 오브젝트 매니져의 업데이트 */
	m_pObject_Manager->Tick(TimeDelta);

	m_pPipeLine->Update();

	m_pFrustum->Tick();

	m_pObject_Manager->LateTick(TimeDelta);	

	return S_OK;
}

void CGameInstance::Clear_Engine(_uint iLevelIndex)
{
	if (nullptr == m_pObject_Manager || 
		nullptr == m_pComponent_Manager)
		return;	

	m_pObject_Manager->Clear(iLevelIndex);
	m_pComponent_Manager->Clear(iLevelIndex);
}

HRESULT CGameInstance::Ready_Timer(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return E_FAIL;

	return m_pTimer_Manager->Ready_Timer(pTimerTag);	
}

_double CGameInstance::Get_TimeDelta(const _tchar * pTimerTag)
{
	if (nullptr == m_pTimer_Manager)
		return 0.0;

	return m_pTimer_Manager->Get_Timer(pTimerTag);
}

void CGameInstance::Set_TimeDelta(const _tchar * pTimerTag)
{	
	if (nullptr == m_pTimer_Manager)
		return;

	return m_pTimer_Manager->Set_Timer(pTimerTag);
}

HRESULT CGameInstance::Clear_BackBuffer_View(_float4 vClearColor)
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	return m_pGraphic_Device->Clear_BackBuffer_View(vClearColor);
}

HRESULT CGameInstance::Clear_DepthStencil_View()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	return m_pGraphic_Device->Clear_DepthStencil_View();
}

HRESULT CGameInstance::Present()
{
	if (nullptr == m_pGraphic_Device)
		return E_FAIL;
	return m_pGraphic_Device->Present();
}

_byte CGameInstance::Get_DIKeyState(_ubyte ubyKeyID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIKeyState(ubyKeyID);	
}

_byte CGameInstance::Get_DIMouseState(CInput_Device::MOUSEKEYSTATE eMouseID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseState(eMouseID);
}

_long CGameInstance::Get_DIMouseMove(CInput_Device::MOUSEMOVESTATE eMouseMoveID)
{
	if (nullptr == m_pInput_Device)
		return 0;

	return m_pInput_Device->Get_DIMouseMove(eMouseMoveID);
}

HRESULT CGameInstance::Open_Level(_uint iLevelIndex, CLevel * pNewLevel)
{
	if (nullptr == m_pLevel_Manager)
		return E_FAIL;
	
	return m_pLevel_Manager->Open_Level(iLevelIndex, pNewLevel);
}

CComponent * CGameInstance::Get_Component(_uint iLevelIndex, const _tchar * pLayerTag, const _tchar * pComponentTag, _uint iIndex)
{
	if (nullptr == m_pObject_Manager)
		return nullptr;

	return m_pObject_Manager->Get_Component(iLevelIndex, pLayerTag, pComponentTag, iIndex);	
}

HRESULT CGameInstance::Add_Prototype(const _tchar * pPrototypeTag, CGameObject * pPrototype)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_Prototype(pPrototypeTag, pPrototype);	
}

HRESULT CGameInstance::Add_GameObject(const _tchar * pPrototypeTag, _uint iLevelIndex, const _tchar * pLayerTag, void * pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(pPrototypeTag, iLevelIndex, pLayerTag, pArg);
}

HRESULT CGameInstance::Add_GameObject(const _tchar* pPrototypeTag, _uint iLevelIndex, const _tchar* pLayerTag, const tSpriteInfo& SpriteInfo, void* pArg)
{
	if (nullptr == m_pObject_Manager)
		return E_FAIL;

	return m_pObject_Manager->Add_GameObject(pPrototypeTag, iLevelIndex, pLayerTag, SpriteInfo, pArg);
}

HRESULT CGameInstance::Add_Prototype(_uint iLevelIndex, const _tchar * pPrototypeTag, CComponent * pPrototype)
{
	if (nullptr == m_pComponent_Manager)
		return E_FAIL;

	return m_pComponent_Manager->Add_Prototype(iLevelIndex, pPrototypeTag, pPrototype);	
}

CComponent * CGameInstance::Clone_Component(_uint iLevelIndex, const _tchar * pPrototypeTag, void * pArg)
{
	if (nullptr == m_pComponent_Manager)
		return nullptr;

	return m_pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
}

void CGameInstance::Set_Transform(CPipeLine::TRANSFORMSTATE eState, _fmatrix TransformMatrix)
{
	if (nullptr == m_pPipeLine)
		return;

	return m_pPipeLine->Set_Transform(eState, TransformMatrix);
}

_float4x4 CGameInstance::Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return _float4x4();

	return m_pPipeLine->Get_Transform_Float4x4(eState);
}

_matrix CGameInstance::Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE eState)
{
	if (nullptr == m_pPipeLine)
		return XMMatrixIdentity();

	return m_pPipeLine->Get_Transform_Matrix(eState);
}

_float4 CGameInstance::Get_CamPosition() 
{
	if (nullptr == m_pPipeLine)
		return _float4(0.f, 0.f, 0.f, 1.f);

	return m_pPipeLine->Get_CamPosition();
}

const LIGHTDESC * CGameInstance::Get_Light(_uint iIndex)
{
	if (nullptr == m_pLight_Manager)
		return nullptr;

	return m_pLight_Manager->Get_Light(iIndex);	
}

HRESULT CGameInstance::Add_Light(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const LIGHTDESC & LightDesc)
{
	if (nullptr == m_pLight_Manager)
		return E_FAIL;

	return m_pLight_Manager->Add_Light(pDevice, pContext, LightDesc);	
}

HRESULT CGameInstance::Add_Font(ID3D11Device * pDevice, ID3D11DeviceContext * pContext, const _tchar * pFontTag, const _tchar * pFontFilePath)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Add_Font(pDevice, pContext, pFontTag, pFontFilePath);	
}

HRESULT CGameInstance::Render_Font(const _tchar * pFontTag, const _tchar * pText, const _float2 & vPosition, _fvector vColor, _float fRotation, const _float2 & vRotationOrigin, const _float2 & vScale)
{
	if (nullptr == m_pFont_Manager)
		return E_FAIL;

	return m_pFont_Manager->Render_Font(pFontTag, pText, vPosition, vColor, fRotation, vRotationOrigin, vScale);
}

_bool CGameInstance::isIn_WorldFrustum(_fvector vPosition, _float fRange)
{
	if (nullptr == m_pFrustum)
		return false;

	return m_pFrustum->isIn_World(vPosition, fRange);
}

HRESULT CGameInstance::Set_Shader_RTV(const _tchar * pTargetTag, CShader * pShader, const char * pConstantName)
{
	if (nullptr == m_pTarget_Manager)
		return E_FAIL;

	return m_pTarget_Manager->Set_ShaderResourceView(pTargetTag, pShader, pConstantName);	
}

void CGameInstance::Release_Engine()
{
	CGameInstance::GetInstance()->DestroyInstance();

	CObject_Manager::GetInstance()->DestroyInstance();

	CComponent_Manager::GetInstance()->DestroyInstance();
	
	CLevel_Manager::GetInstance()->DestroyInstance();

	CTimer_Manager::GetInstance()->DestroyInstance();

	CPipeLine::GetInstance()->DestroyInstance();

	CLight_Manager::GetInstance()->DestroyInstance();

	CInput_Device::GetInstance()->DestroyInstance();

	CFont_Manager::GetInstance()->DestroyInstance();

	CFrustum::GetInstance()->DestroyInstance();

	CTarget_Manager::GetInstance()->DestroyInstance();

	CGraphic_Device::GetInstance()->DestroyInstance();
}

void CGameInstance::Free()
{
	Safe_Release(m_pTarget_Manager);
	Safe_Release(m_pFrustum);
	Safe_Release(m_pFont_Manager);
	Safe_Release(m_pLight_Manager);
	Safe_Release(m_pPipeLine);
	Safe_Release(m_pComponent_Manager);
	Safe_Release(m_pObject_Manager);
	Safe_Release(m_pLevel_Manager);
	Safe_Release(m_pInput_Device);
	Safe_Release(m_pGraphic_Device);
	Safe_Release(m_pTimer_Manager);
}
