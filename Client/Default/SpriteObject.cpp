#include "stdafx.h"
#include "SpriteObject.h"

CSpriteObject::CSpriteObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_bIsDead(false), m_bIsRender(true)
	, m_eRenderGroup(CRenderer::RENDERGROUP::RENDER_PRIORITY)
{
	ZeroMemory(&m_tSpriteInfo, sizeof tSpriteInfo);
	m_tSpriteInfo.vColor = { 1.f, 1.f, 1.f, 1.f };
	m_tSpriteInfo.fSizeRatio = { 1.f, 1.f };
	m_tSpriteInfo.fPosition = { 0.f, 0.f };

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
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

// @qurious. 매개변수 &가 원본 참조..인데 주소 참조는아닌가? 그렇기에 memcpy에서 & 또 써줘야함?
HRESULT CSpriteObject::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	memcpy(&m_tSpriteInfo, &InSpriteInfo, sizeof m_tSpriteInfo);

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());

	// 4는 위치
	Change_TextureSize();
	m_WorldMatrix._41 = m_tSpriteInfo.fPosition.x;
	m_WorldMatrix._42 = m_tSpriteInfo.fPosition.y;

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&m_WorldMatrix));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	CTransform::TRANSFORMDESC TransformDesc = { 5.f, XMConvertToRadians(360.f) };
	m_pTransformCom->Set_TransformDesc(TransformDesc);

	return S_OK;
}

_uint CSpriteObject::Tick(_double TimeDelta)
{
	if (m_bIsDead)
		return OBJ_DEAD;

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

void CSpriteObject::Play_Animation(_double TimeDelta)
{
	// 열거체는 객체마다 다르므로 .. 템플릿 가능할까?
	float fPerAnimTime = m_pAnimInfo[m_iCurrentAnim].fAnimTime / (float)abs(m_pAnimInfo[m_iCurrentAnim].iEndIndex - m_pAnimInfo[m_iCurrentAnim].iStartIndex);

	m_fTimeAcc += (_float)TimeDelta;
	if (fPerAnimTime <= m_fTimeAcc)
	{
		m_fTimeAcc = 0.f;
		++m_tSpriteInfo.iTextureIndex;

		if (m_pAnimInfo[m_iCurrentAnim].iEndIndex < m_tSpriteInfo.iTextureIndex)
		{
			m_tSpriteInfo.iTextureIndex = m_pAnimInfo[m_iCurrentAnim].iStartIndex;
		}

		Change_TextureSize();
	}
}

CGameObject* CSpriteObject::Clone(void* pArg)
{
	MSG_BOX("CSpriteObject - Clone(void* pArg) - 2D용 Clone 함수를 사용해주세요");
	return nullptr;
}

void CSpriteObject::Free()
{
	__super::Free();

	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);


}