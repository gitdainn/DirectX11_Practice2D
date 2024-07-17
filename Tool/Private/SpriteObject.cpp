#include "stdafx.h"
#include "SpriteObject.h"

#include "Collider.h"

USING(Tool)

CSpriteObject::CSpriteObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_iUVTextureIndex(0)
	, m_iUVTexNumX(0), m_iUVTexNumY(0)
	, m_bIsAnimUV(false)
	, m_eSpriteDirection(SPRITE_DIRECTION::LEFT)
	, m_pTextureComTag(nullptr), m_pSpriteTag(nullptr)
	, m_bIsScroll(true)
	, m_pLayer(nullptr)
{
	ZeroMemory(&m_tSpriteInfo, sizeof tSpriteInfo);
	m_tSpriteInfo.vColor = { 1.f, 1.f, 1.f, 1.f };
	m_tSpriteInfo.fSize = { 1.f, 1.f };
	m_tSpriteInfo.fPosition = { 0.f, 0.f };
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
	if (nullptr == m_tSpriteInfo.pTextureComTag)
		m_pTextureComTag = TEXT("Prototype_Component_Sprite_Tile");
	else
		m_pTextureComTag = m_tSpriteInfo.pTextureComTag;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	// 위치 지정
	WorldMatrix._11 = m_tSpriteInfo.fSize.x * m_tSpriteInfo.fSizeRatio.x;
	WorldMatrix._22 = m_tSpriteInfo.fSize.y * m_tSpriteInfo.fSizeRatio.y;
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
	if (FAILED(__super::Tick(TimeDelta)))
	{
		MSG_BOX("CSpriteObject - Tick - __super Error");
		return E_FAIL;
	};

	return _uint();
}

_uint CSpriteObject::LateTick(_double TimeDelta)
{
	if (FAILED(__super::LateTick(TimeDelta)))
	{
		MSG_BOX("CSpriteObject - LateTick - __super Error");
		return E_FAIL;
	};

	return _uint();
}

HRESULT CSpriteObject::Render()
{
	if (FAILED(__super::Render()))
	{
		MSG_BOX("CSpriteObject - Render - __super Error");
		return E_FAIL;
	};

	if (FAILED(SetUp_ShaderResources()))
		return E_FAIL;

	m_pShaderCom->Begin(m_iShaderPassIndex);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CSpriteObject::Change_TextureComponent(const _tchar* pPrototypeTag)
{
	if (FAILED(CGameObject::Change_Component(LEVEL_STATIC, pPrototypeTag, TAG_TEXTURE, (CComponent**)&m_pTextureCom)))
	{
		MSG_BOX("CSpriteObject - ChangeTextureComponent() - FAILED");
		return E_FAIL;
	}
	Safe_Delete_Array(m_tSpriteInfo.pTextureComTag);
	m_tSpriteInfo.pTextureComTag = pPrototypeTag;

	return S_OK;
}

HRESULT CSpriteObject::Add_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	if (FAILED(CGameObject::Add_Components((_uint)LEVEL::LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TAG_TRANSFORM, (CComponent**)(&m_pTransformCom))))
	{
		MSG_BOX("CSpriteObject - Add_Components() - FAILED");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(CGameObject::Add_Components((_uint)LEVEL::LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TAG_RENDERER, (CComponent**)(&m_pRendererCom))))
	{
		MSG_BOX("CSpriteObject - Add_Components() - FAILED");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	if (FAILED(__super::Add_Components((_uint)LEVEL::LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TAG_BUFFER, (CComponent**)(&m_pVIBufferCom))))
	{
		MSG_BOX("CSpriteObject - Add_Components() - FAILED");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	/* For.Com_Shader */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxTex"),
		TAG_SHADER, (CComponent**)&m_pShaderCom, nullptr)))
	{
		MSG_BOX("CSpriteObject - Add_Components() - FAILED");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	/* For.Com_Texture */
	if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, m_pTextureComTag,
		TAG_TEXTURE, (CComponent**)&m_pTextureCom, nullptr)))
	{
		MSG_BOX("CSpriteObject - Add_Components() - FAILED");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CSpriteObject::SetUp_ShaderResources()
{
	// 실제 Transform에 더해버리면 계속누적해서 더해버리니까 실제 위치에는 적용 안되도록 출려갛ㄹ 때만 + Scroll
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 WorldMatrix = m_pTransformCom->Get_WorldMatrixFloat();
	if (m_bIsScroll)
	{
		Scroll_Screen(WorldMatrix);
	}

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	//if (FAILED(m_pTransformCom->Set_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;

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

void CSpriteObject::Scroll_Screen(_float4x4& WorldMatrix) const
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	WorldMatrix._41 = XMVectorGetX(vPosition) + pGameInstance->Get_ScrollX();
	WorldMatrix._42 = XMVectorGetY(vPosition) + pGameInstance->Get_ScrollY();

	Safe_Release(pGameInstance);
}

void CSpriteObject::Play_Animation(_uint& iSpriteIndex, _double TimeDelta)
{
	// 열거체는 객체마다 다르므로 .. 템플릿 가능할까?
	_float fPerAnimTime = m_pAnimInfo[m_iCurrentAnim].fAnimTime / fabs((_float)m_pAnimInfo[m_iCurrentAnim].iEndIndex - (_float)m_pAnimInfo[m_iCurrentAnim].iStartIndex);

	const _uint iCurrentSpriteIndex = m_bIsAnimUV ? m_iUVTextureIndex : m_tSpriteInfo.iTextureIndex;
	unordered_map<_uint, _float>::iterator iter = m_pAnimInfo[m_iCurrentAnim].fDelayTimeMap.find(iCurrentSpriteIndex);
	_float fDelayTime = { 0.f };
	if (iter != m_pAnimInfo[m_iCurrentAnim].fDelayTimeMap.end())
		fDelayTime = iter->second;

	m_fTimeAcc += (_float)TimeDelta;
	if (fPerAnimTime + fDelayTime <= m_fTimeAcc)
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

	if (nullptr != m_pAnimInfo)
	{
		m_pAnimInfo->fDelayTimeMap.clear();
		Safe_Delete_Array(m_pAnimInfo);
	}

	Safe_Delete_Array(m_tSpriteInfo.pTextureComTag);
	Safe_Delete_Array(m_tSpriteInfo.pPrototypeTag);

	Safe_Delete_Array(m_pSpriteTag);
	Safe_Delete_Array(m_pLayer);

	/** @note - _tchar* m_pTextureComTage를 TEXT("상수"); 리터럴 상수로 넣으면 메모리 Code 영역에 저장되어 상수들은 자동으로 해제하기에 해제해주면 안됨 */
	// Safe_Delete_Array(m_pTextureComTag);
	// 
	// @note - Add_Prototype으로 만든 원본 객체들은 m_Prototypes에서 삭제해줌. (원본은 삭제해야하니까 AddRef X)
	// @note - 각 오브젝트의 컴포넌트들은 Add_Component 시 AddRef하기 때문에 m_Component에서 다 Release 해줌
	// @note - Add_Component의 Clone()할 때, new로 생성 또는 AddRef()해서 사본 주기 때문에 얘네는 따로 여기서 해제해줘야함.
	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
}