#include "stdafx.h"
#include "SpriteObject.h"
#include "FileLoader.h"
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
{
	ZeroMemory(&m_tSpriteInfo, sizeof tSpriteInfo);
	m_tSpriteInfo.vColor = { 1.f, 1.f, 1.f, 1.f };
	m_tSpriteInfo.fSize = { 1.f, 1.f };
	m_tSpriteInfo.fSizeRatio = { 1.f, 1.f };
	m_tSpriteInfo.fPosition = { 0.f, 0.f };
}

HRESULT CSpriteObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CSpriteObject::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CSpriteObject - Initialize - Argument is NULL");
		return E_FAIL;
	}
	m_iInstanceID = *(_uint*)pArg;

	if (FAILED(Load_Components_Excel()))
	{
		MSG_BOX("CSpriteObject - Add_Components_Excel() - FAILED");
		return E_FAIL;
	}

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	CFileLoader* pFileLoader = CFileLoader::GetInstance();
	if (nullptr == pFileLoader)
	{
		MSG_BOX("CSpriteObject - Initialize - FileLoader is NULL");
		return E_FAIL;
	}
	Safe_AddRef(pFileLoader);

	OBJECT_TRANSFORM tObjectTransform;
	if (FAILED(pFileLoader->Get_ObjectTransform(m_iInstanceID, tObjectTransform)))
	{
		MSG_BOX("CSpriteObject - Initialize - FileLoad FAILED");
		return E_FAIL;
	}
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	// 위치 지정
	if(nullptr != m_pTextureCom)
		m_tSpriteInfo.fSize = m_pTextureCom->Get_OriginalTextureSize(m_iTextureIndex);
	m_tSpriteInfo.fSizeRatio = tObjectTransform.fSizeRatio;
	WorldMatrix._11 = tObjectTransform.fSize.x * m_tSpriteInfo.fSizeRatio.x;
	WorldMatrix._22 = tObjectTransform.fSize.y * m_tSpriteInfo.fSizeRatio.y;
	WorldMatrix._41 = tObjectTransform.fPosition.x;
	WorldMatrix._42 = tObjectTransform.fPosition.y;

	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&WorldMatrix));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	CTransform::TRANSFORM_DESC TransformDesc = { 5.f, XMConvertToRadians(360.f) };
	m_pTransformCom->Set_TransformDesc(TransformDesc);

	Safe_Release(pFileLoader);
	return S_OK;
}

// @qurious - 매개변수 &가 원본 참조..인데 주소 참조는아닌가? 그렇기에 memcpy에서 & 또 써줘야함?
HRESULT CSpriteObject::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	memcpy(&m_tSpriteInfo, &InSpriteInfo, sizeof m_tSpriteInfo);
	m_iTextureIndex = m_tSpriteInfo.iTextureIndex;

	if (nullptr == m_tSpriteInfo.pTextureComTag)
	{
		MSG_BOX("CSpriteObject - Initialize() - NULL");
		return E_FAIL;
	}
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

HRESULT CSpriteObject::Mapping_Component(const _tchar* pComponentTag)
{
	if (nullptr == pComponentTag)
	{
		return E_FAIL;
	}

	CComponent* pComponent = { nullptr };

	if(!lstrcmp(pComponentTag, TAG_TEXTURE))
	{
		/* For.Com_Texture */
		pComponent = Find_Component(TAG_TEXTURE);
		if (nullptr != pComponent)
		{
			m_pTextureCom = dynamic_cast<CTexture*>(pComponent);
			if (nullptr == m_pTextureCom)
			{
				MSG_BOX("CSpriteObject - Add_Component - TextureCom is NULL");
				return E_FAIL;
			}
			m_iTextureIndex = m_pTextureCom->Get_TextureIndex();

		}
	}

	if (!lstrcmp(pComponentTag, TAG_COLL_AABB))
	{
		/* For.Com_Collider */
		pComponent = Find_Component(TAG_COLL_AABB);
		if (nullptr != pComponent)
		{
			m_pColliderCom = dynamic_cast<Engine::CCollider*>(pComponent);
		}
	}

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

	if (nullptr == m_pTextureCom)
	{
		if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, m_pTextureComTag,
			TAG_TEXTURE, (CComponent**)&m_pTextureCom, nullptr)))
		{
			MSG_BOX("CSpriteObject - Add_Components() - FAILED");
			Safe_Release(pGameInstance);
			return E_FAIL;
		}
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CSpriteObject::Load_Components_Excel()
{
	CFileLoader* pFileLoader = CFileLoader::GetInstance();
	if (nullptr == pFileLoader)
	{
		MSG_BOX("CSpriteObject - Add_Components - FileLoader is NULL");
		return E_FAIL;
	}
	Safe_AddRef(pFileLoader);

	list<COMPONENT_INFO> ComponentList;
	if (FAILED(pFileLoader->Get_ComponentInfoList(m_iInstanceID, ComponentList)))
	{
		MSG_BOX("CSpriteObject - Add_Components - ComponentList is NULL");
		Safe_Release(pFileLoader);
		return E_FAIL;
	}

	CComponent* pComponent = { nullptr };
	for (COMPONENT_INFO tInfo : ComponentList)
	{
		if (FAILED(CGameObject::Add_Components((_uint)LEVEL::LEVEL_STATIC, tInfo.pPrototypeTag,
			tInfo.pComponentTag, &pComponent, &tInfo)))
		{
			MSG_BOX("CSpriteObject - Add_Components - FAILED");
			continue;
		}
		pComponent->Set_Owner(this);
	}

	Mapping_Component(TAG_TEXTURE);
	Mapping_Component(TAG_COLL_AABB);

	Safe_Release(pFileLoader);

	return S_OK;
}

void CSpriteObject::OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget)
{
}

void CSpriteObject::OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget)
{
	if (nullptr == pTargetCollider || nullptr == pTarget)
		return;

	if (pTargetCollider->Get_IsBlock())
	{
		_vector vPushVector = m_pColliderCom->Get_IntersectVectorX(pTargetCollider);
		//// x, y 중 어느 방향이 부딪쳤는지 알아챈다. 
		//// 부딪친 방향으로 겹친 거리만큼 다시 민다. or 블록 객체 위치로 지정한다. (좌우 중 어느 곳에 지정할지 알아야 함)
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition + vPushVector, XMVectorGetY(vPosition)));
	}
}

void CSpriteObject::OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget)
{
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

	if (FAILED(m_pTextureCom->Set_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
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

	const _uint iCurrentSpriteIndex = m_bIsAnimUV ? m_iUVTextureIndex : m_iTextureIndex;
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
	Safe_Release(m_pColliderCom);
}