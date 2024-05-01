#include "stdafx.h"
#include "SpriteObject.h"
#include "State.h"
#include "PlayerIdle.h"

CSpriteObject::CSpriteObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_bIsDead(false), m_bIsRender(true)
	, m_iUVTextureIndex(0)
	, m_iUVTexNumX(0), m_iUVTexNumY(0)
	, m_eRenderGroup(CRenderer::RENDERGROUP::RENDER_PRIORITY)
	, m_bIsAnimUV(false)
	, m_pState(nullptr)
	, m_eSpriteDirection(SPRITE_DIRECTION::LEFT)
{
	ZeroMemory(&m_tSpriteInfo, sizeof tSpriteInfo);
	m_tSpriteInfo.vColor = { 1.f, 1.f, 1.f, 1.f };
	m_tSpriteInfo.fSize = { 1.f, 1.f };
	m_tSpriteInfo.fPosition = { 0.f, 0.f };

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

HRESULT CSpriteObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpriteObject::Initialize(void* pArg)
{
	return S_OK;
}

// @qurious - 매개변수 &가 원본 참조..인데 주소 참조는아닌가? 그렇기에 memcpy에서 & 또 써줘야함?
HRESULT CSpriteObject::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	memcpy(&m_tSpriteInfo, &InSpriteInfo, sizeof m_tSpriteInfo);

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	// 위치 지정
	WorldMatrix._11 = m_tSpriteInfo.fSize.x;
	WorldMatrix._22 = m_tSpriteInfo.fSize.x;
	WorldMatrix._41 = m_tSpriteInfo.fPosition.x;
	WorldMatrix._42 = m_tSpriteInfo.fPosition.y;
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&WorldMatrix));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	CTransform::TRANSFORM_DESC TransformDesc = { 5.f, XMConvertToRadians(360.f) };
	m_pTransformCom->Set_TransformDesc(TransformDesc);

	return S_OK;
}

_uint CSpriteObject::Tick(_double TimeDelta)
{
	if (m_bIsDead)
		return OBJ_DEAD;

	//if (m_bIsJump)
	//{
	//	/** 엔진에서 제공해야할 것:
	//	* 1. 현재 하락세인지 검사
	//	* 2. 파라볼라 종료 함수
	//	*/
	//	m_pTransformCom->ParabolaY(TimeDelta);

	//	/** @note - 윗층으로 점프하는 경우 대비
	//	* 하락세일 때, 가장 가까이 있는 땅으로 착지 */
	//	_float fLandingY = 0.0;
	//	if (true == m_pTransformCom->CheckParabolicDecline())
	//	{
	//		// 하락세일 때 가장 가까운 땅 검사해서 착지
	//		if (fLandingY > XMVectorGetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION)))
	//		{
	//			m_pTransformCom->Set_State(CTransform::STATE_POSITION
	//				, XMVectorSetY(m_pTransformCom->Get_State(CTransform::STATE_POSITION), fLandingY));

	//			m_pTransformCom->End_Parabola();
	//			m_bIsJump = false;
	//		}
	//	}
	//}

	return _uint();
}

_uint CSpriteObject::LateTick(_double TimeDelta)
{
	if (m_bIsRender)
	{
		//@qurious. enum class가 아니라 일반 enum이면 engine의 열거체를 멤버로 선언 시 사용불가임.
		m_pRendererCom->Add_RenderGroup(m_eRenderGroup, this);
	}

	return _uint();
}

HRESULT CSpriteObject::Render()
{
	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iShaderPassIndex);

	m_pVIBufferCom->Render();

	return S_OK;
}

void CSpriteObject::Input_Handler(const STATE_TYPE Input, const SPRITE_DIRECTION eDirection)
{
	CState* pState = m_pState->Input_Handler(this, Input, eDirection);
	
	if (nullptr == pState)
		return;

	if (nullptr != m_pState)
		delete m_pState;

	m_pState = pState;
	m_pState->Enter(this);
}

HRESULT CSpriteObject::Add_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(CGameObject::Add_Components((_uint)LEVEL::LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TAG_TRANSFORM, (CComponent**)(&m_pTransformCom))))
		return E_FAIL;

	if (FAILED(CGameObject::Add_Components((_uint)LEVEL::LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TAG_RENDERER, (CComponent**)(&m_pRendererCom))))
		return E_FAIL;

	if (FAILED(__super::Add_Components((_uint)LEVEL::LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TAG_BUFFER, (CComponent**)(&m_pVIBufferCom))))
		return E_FAIL;

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CSpriteObject::SetUp_ShaderResources()
{
	if (FAILED(m_pTransformCom->Set_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_ShaderResource(m_pShaderCom, "g_Texture", m_tSpriteInfo.iTextureIndex)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_tSpriteInfo.vColor, sizeof(_vector))))
		return E_FAIL;

	return S_OK;
}

void CSpriteObject::Play_Animation(_uint& iSpriteIndex, _double TimeDelta)
{
	// 열거체는 객체마다 다르므로 .. 템플릿 가능할까?
	float fPerAnimTime = m_pAnimInfo[m_iCurrentAnim].fAnimTime / (float)abs(m_pAnimInfo[m_iCurrentAnim].iEndIndex - m_pAnimInfo[m_iCurrentAnim].iStartIndex);

	m_fTimeAcc += (_float)TimeDelta;
	if (fPerAnimTime <= m_fTimeAcc)
	{
		m_fTimeAcc = 0.f;
		++iSpriteIndex;

		if (m_pAnimInfo[m_iCurrentAnim].iEndIndex < iSpriteIndex)
		{
			m_bIsEndSprite = true;
			iSpriteIndex = m_pAnimInfo[m_iCurrentAnim].iStartIndex;
		}
	}
}

CGameObject* CSpriteObject::Clone(void* pArg) const
{
	MSG_BOX("CSpriteObject - Clone(void* pArg) - 2D용 Clone 함수를 사용해주세요");
	return nullptr;
}

void CSpriteObject::Free()
{
	__super::Free();

	Safe_Delete(m_pState);

	// @note - Add_Prototype으로 만든 원본 객체들은 m_Prototypes에서 삭제해줌. (원본은 삭제해야하니까 AddRef X)
	// @note - 각 오브젝트의 컴포넌트들은 Add_Component 시 AddRef하기 때문에 m_Component에서 다 Release 해줌
	// @note - Add_Component의 Clone()할 때, new로 생성 또는 AddRef()해서 사본 주기 때문에 얘네는 따로 여기서 해제해줘야함.
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);


}