#include "stdafx.h"
#include "SpriteObject.h"
#include "State.h"
#include "PlayerIdle.h"
#include "FileLoader.h"

CSpriteObject::CSpriteObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_iUVTextureIndex(0)
	, m_iUVTexNumX(0), m_iUVTexNumY(0)
	, m_bIsAnimUV(false)
	, m_pState(nullptr)
	, m_eSpriteDirection(SPRITE_DIRECTION::LEFT)
	, m_eCurrentState(STATE_TYPE::IDLE)
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

	OBJECT_TRANSFORM tObjectTransform;
	if (FAILED(pFileLoader->Get_ObjectTransform(m_iInstanceID, tObjectTransform)))
	{
		MSG_BOX("CSpriteObject - Initialize - FileLoad FAILED");
		return E_FAIL;
	}
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	// 위치 지정
	WorldMatrix._11 = tObjectTransform.fSize.x;
	WorldMatrix._22 = tObjectTransform.fSize.y;
	WorldMatrix._41 = tObjectTransform.fPosition.x;
	WorldMatrix._42 = tObjectTransform.fPosition.y;
	
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&WorldMatrix));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));

	CTransform::TRANSFORM_DESC TransformDesc = { 5.f, XMConvertToRadians(360.f) };
	m_pTransformCom->Set_TransformDesc(TransformDesc);

	return S_OK;
}

// @qurious - 매개변수 &가 원본 참조..인데 주소 참조는아닌가? 그렇기에 memcpy에서 & 또 써줘야함?
HRESULT CSpriteObject::Initialize(const tSpriteInfo& InSpriteInfo, void* pArg)
{
	memcpy(&m_tSpriteInfo, &InSpriteInfo, sizeof m_tSpriteInfo);

	m_iTextureIndex = m_tSpriteInfo.iTextureIndex;

	if (FAILED(Add_Components(pArg)))
		return E_FAIL;

	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	// 위치 지정
	WorldMatrix._11 = m_tSpriteInfo.fSize.x;
	WorldMatrix._22 = m_tSpriteInfo.fSize.y;
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

	return _uint();
}

_uint CSpriteObject::LateTick(_double TimeDelta)
{
	if (nullptr != m_pColliderCom)
	{
		CGameInstance* pGameInstance = CGameInstance::GetInstance();
		Safe_AddRef(pGameInstance);
		if (FAILED(pGameInstance->Attach_Collider(m_pLayer, m_pColliderCom)))
		{
			MSG_BOX("CSpriteObject - LateTick() - FAILED");
		}
		Safe_Release(pGameInstance);
	}

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
	
	if (nullptr != pState)
	{
		m_eCurrentState = Input;
		delete m_pState;

		m_pState = pState;
		m_pState->Enter(this);
	}
}

HRESULT CSpriteObject::Add_Components(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	/* For.Com_Transform */
	if (FAILED(CGameObject::Add_Components((_uint)LEVEL::LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TAG_TRANSFORM, (CComponent**)(&m_pTransformCom))))
		return E_FAIL;

	/* For.Com_Renderer */
	if (FAILED(CGameObject::Add_Components((_uint)LEVEL::LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TAG_RENDERER, (CComponent**)(&m_pRendererCom))))
		return E_FAIL;

	/* For.Com_VIBuffer_Rect */
	if (FAILED(__super::Add_Components((_uint)LEVEL::LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TAG_BUFFER, (CComponent**)(&m_pVIBufferCom))))
		return E_FAIL;

	if (nullptr == m_pTextureCom)
	{
		if (FAILED(CGameObject::Add_Components(LEVEL_STATIC, m_tSpriteInfo.pTextureComTag,
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

HRESULT CSpriteObject::SetUp_ShaderResources()
{
	//if (FAILED(m_pTransformCom->Set_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
	//	return E_FAIL;
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	_float4x4 WorldMatrix = m_pTransformCom->Get_WorldMatrixFloat();
	//if (m_bIsScroll)
	//{
	//	Scroll_Screen(WorldMatrix);
	//}

	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &WorldMatrix)))
		return E_FAIL;

	Safe_Release(pGameInstance);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Set_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
		return E_FAIL;

	_float4 vColor = { 1.f, 1.f, 1.f, 1.f };
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &vColor, sizeof(_vector))))
		return E_FAIL;

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

HRESULT CSpriteObject::Mapping_Component(const _tchar* pComponentTag)
{
	if (nullptr == pComponentTag)
	{
		return E_FAIL;
	}

	CComponent* pComponent = { nullptr };

	if (!lstrcmp(pComponentTag, TAG_TEXTURE))
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

void CSpriteObject::Play_Animation(_uint& iSpriteIndex, _double TimeDelta)
{
	// 열거체는 객체마다 다르므로 .. 템플릿 가능할까?
	_float fPerAnimTime = m_pAnimInfo[m_iCurrentAnim].fAnimTime / fabs((_float)m_pAnimInfo[m_iCurrentAnim].iEndIndex - (_float)m_pAnimInfo[m_iCurrentAnim].iStartIndex);

	const _uint iCurrentSpriteIndex = m_bIsAnimUV ? m_iUVTextureIndex : m_iTextureIndex;
	unordered_map<_uint, _float>::iterator iter = m_pAnimInfo[m_iCurrentAnim].fDelayTimeMap.find(iCurrentSpriteIndex);
	_float fDelayTime = { 0.f };
	if (iter != m_pAnimInfo[m_iCurrentAnim].fDelayTimeMap.end())
		fDelayTime = iter->second;

	m_AnimAcc += (_float)TimeDelta;
	if (fPerAnimTime + fDelayTime <= m_AnimAcc)
	{
		m_AnimAcc = 0.f;
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

	Safe_Delete(m_pState);

	if (nullptr != m_pAnimInfo)
		m_pAnimInfo->fDelayTimeMap.clear();
	Safe_Delete_Array(m_pAnimInfo);

	/** @note - m_pTextureTag 해제하면 안되는 이유
	현재 m_pTextureTag는 문자열 리터럴을 가리키므로 읽기 전용 데이터에 저장되어 자동으로 삭제되기 때문 (동적할당 해준 경우만 해제해줄 것)*/
	Safe_Delete_Array(m_tSpriteInfo.pTextureComTag);
	Safe_Delete_Array(m_tSpriteInfo.pPrototypeTag);

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