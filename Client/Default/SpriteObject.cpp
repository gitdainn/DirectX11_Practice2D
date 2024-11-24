#include "stdafx.h"
#include "SpriteObject.h"
#include "State.h"
#include "PlayerIdle.h"
#include "FileLoader.h"
#include "Widget.h"

CSpriteObject::CSpriteObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CGameObject(pDevice, pContext)
	, m_bIsAnimUV(false)
	, m_pState(nullptr)
	, m_eSpriteDirection(SPRITE_DIRECTION::LEFT)
	, m_eCurrentState(STATE_TYPE::IDLE)
	, m_bIsInAir(false)
	, m_pAirState(nullptr)
{
	ZeroMemory(&m_tSpriteInfo, sizeof(SPRITE_INFO));
	m_tSpriteInfo.vColor = { 1.f, 1.f, 1.f, 1.f };
	m_tSpriteInfo.fSize = { 1.f, 1.f };
	m_tSpriteInfo.fPosition = { 0.f, 0.f };
}

// 컴포넌트는 프로토타입에서 넘겨받지 않을거라 생략했음.
CSpriteObject::CSpriteObject(const CSpriteObject& rhs)
	: CGameObject(rhs)
	, m_pDevice(rhs.m_pDevice), m_pContext(rhs.m_pContext)
	, m_bIsAnimUV(rhs.m_bIsAnimUV), m_bIsEndSprite(rhs.m_bIsEndSprite)
	, m_vColor(rhs.m_vColor)
	, m_bIsInAir(rhs.m_bIsInAir)
	, m_eCurrentState(rhs.m_eCurrentState), m_eSpriteDirection(rhs.m_eSpriteDirection)
	, m_eSkulRank(rhs.m_eSkulRank), m_eSkulType(rhs.m_eSkulType)
	, m_iLevel(rhs.m_iLevel)

{
	memcpy(&m_tBaseStats, &rhs.m_tBaseStats, sizeof(BASE_STATS));
	memcpy(&m_tSpriteInfo, &rhs.m_tSpriteInfo, sizeof(SPRITE_INFO));

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return;
	Safe_AddRef(pGameInstance);

	if (nullptr != rhs.m_tSpriteInfo.pPrototypeTag)
	{
		m_tSpriteInfo.pPrototypeTag = DeepCopy(rhs.m_tSpriteInfo.pPrototypeTag);
	}

	if (nullptr != rhs.m_tSpriteInfo.pTextureComTag)
	{
		m_tSpriteInfo.pTextureComTag = DeepCopy(rhs.m_tSpriteInfo.pTextureComTag);
		pGameInstance->Add_Garbage(m_tSpriteInfo.pTextureComTag);
	}

	if (nullptr != rhs.m_pSpriteFileName)
	{
		m_pSpriteFileName = DeepCopy(rhs.m_pSpriteFileName);
		pGameInstance->Add_Garbage(m_pSpriteFileName);
	}

	Safe_Release(pGameInstance);
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
		Safe_Release(pFileLoader);
		return E_FAIL;
	}
	_float4x4 WorldMatrix;
	XMStoreFloat4x4(&WorldMatrix, XMMatrixIdentity());

	// 크기 및 위치 지정
	WorldMatrix._11 = (tObjectTransform.fSize.x * tObjectTransform.fSizeRatio.x);
	WorldMatrix._22 = (tObjectTransform.fSize.y * tObjectTransform.fSizeRatio.y);
	WorldMatrix._41 = tObjectTransform.fPosition.x;
	WorldMatrix._42 = tObjectTransform.fPosition.y;
	
	m_pTransformCom->Set_WorldMatrix(XMLoadFloat4x4(&WorldMatrix));

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f)); // 직교투영 함수

	CTransform::TRANSFORM_DESC TransformDesc = { 5.f, XMConvertToRadians(360.f) };
	m_pTransformCom->Set_TransformDesc(TransformDesc);

	Safe_Release(pFileLoader);

	return S_OK;
}

// @qurious - 매개변수 &가 원본 참조..인데 주소 참조는아닌가? 그렇기에 memcpy에서 & 또 써줘야함?
HRESULT CSpriteObject::Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg)
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

HRESULT CSpriteObject::Late_Initialize(void* pArg)
{
	if (nullptr != m_pLineRiderCom)
	{
		_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float2 fPositon = _float2(XMVectorGetX(vPosition), XMVectorGetY(vPosition));
		m_pLineRiderCom->Initialize(&fPositon);
	}

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
	{
		MSG_BOX("CSpriteObject - Render() - SetUp_ShaderResources is FAILED");
		return E_FAIL;
	}

	m_pShaderCom->Begin(m_iShaderPassIndex);

	m_pVIBufferCom->Render();

	return S_OK;
}

HRESULT CSpriteObject::Change_TextureComponent(const _tchar* pPrototypeTag)
{
	if (nullptr == pPrototypeTag)
		return E_FAIL;

	if (FAILED(CGameObject::Change_Component(LEVEL_STATIC, pPrototypeTag, TAG_TEXTURE, (CComponent**)&m_pTextureCom)))
	{
		MSG_BOX("CSpriteObject - ChangeTextureComponent() - FAILED");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpriteObject::Add_Components(void* pArg)
{
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
			return E_FAIL;
		}
	}

	return S_OK;
}

HRESULT CSpriteObject::SetUp_Shader_Camera()
{
	if (FAILED(SetUp_Shader_Default()))
		return E_FAIL;

	_matrix WorldMatrix = m_pTransformCom->Get_WorldMatrix();
	_float4x4 WorldMatrixFloat;
	XMStoreFloat4x4(&WorldMatrixFloat, WorldMatrix);
	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &WorldMatrixFloat)))
	{
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	XMStoreFloat4x4(&m_ViewMatrix, pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW));
	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	XMStoreFloat4x4(&m_ProjMatrix, pGameInstance->Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE::D3DTS_PROJ));
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	{
		Safe_Release(pGameInstance);
		return E_FAIL;
	}

	Safe_Release(pGameInstance);

	return S_OK;
}

HRESULT CSpriteObject::SetUp_Shader_Orthographic()
{
	if (FAILED(SetUp_Shader_Default()))
		return E_FAIL;

	_float4x4 WorldMatrix = m_pTransformCom->Get_WorldMatrixFloat();
	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &WorldMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &m_ViewMatrix)))
	{
		return E_FAIL;
	}

	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &m_ProjMatrix)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpriteObject::SetUp_Shader_NonCamera()
{
	if (FAILED(SetUp_Shader_Default()))
		return E_FAIL;

	_float4x4 WorldMatrix = m_pTransformCom->Get_WorldMatrixFloat();
	if (FAILED(m_pShaderCom->Set_Matrix("g_WorldMatrix", &WorldMatrix)))
	{
		return E_FAIL;
	}

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);

	_float4x4 ViewMatrix, ProjMatrix;
	ViewMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);

	// 위치만 재역행렬 해서 카메라 영향을 안 받도록 하고. 나머지는 카메라 역행렬인 ViewMatrix를 그대로 적용. (직교 시 무조건 맨 앞에 출력되므로 이를 사용함)
	_matrix NonCameraMatrix = XMMatrixInverse(nullptr, XMLoadFloat4x4(&ViewMatrix));
	ViewMatrix._41 = XMVectorGetX(NonCameraMatrix.r[3]);
	ViewMatrix._42 = XMVectorGetY(NonCameraMatrix.r[3]);

	if (FAILED(m_pShaderCom->Set_Matrix("g_ViewMatrix", &ViewMatrix)))
	{
		return E_FAIL;
	}

	ProjMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::TRANSFORMSTATE::D3DTS_PROJ);
	if (FAILED(m_pShaderCom->Set_Matrix("g_ProjMatrix", &ProjMatrix)))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CSpriteObject::SetUp_Shader_Wrap()
{
	if (FAILED(m_pShaderCom->Set_RawValue("g_ObjectSize", &m_pTransformCom->Get_Scaled(), sizeof(_float3))))
		return E_FAIL;

	_float2 fSize = { 0.f, 0.f };
	if (nullptr == m_pSpriteFileName)
	{
		fSize = m_pTextureCom->Get_OriginalTextureSize(m_iTextureIndex);
	}
	else
	{
		fSize = m_pTextureCom->Get_OriginalTextureSize(m_pSpriteFileName);
	}

	if (FAILED(m_pShaderCom->Set_RawValue("g_TextureSize", &fSize, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

HRESULT CSpriteObject::JumpableLineRider(_double TImeDelta)
{
	return E_NOTIMPL;
}

HRESULT CSpriteObject::DefaultLineRider(const _vector vPosition, const _float fOffsetY)
{
	if (nullptr == m_pLineRiderCom)
		return E_FAIL;

	_float fLandingY = { 0.f };
	if (FAILED((m_pLineRiderCom->Collision_Line(vPosition, fLandingY))))
	{
		// 착지할 라인이 없습니다.
		return E_FAIL;
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSetY(vPosition, fLandingY + fOffsetY));
	}

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

HRESULT CSpriteObject::Attach_Collider(const _tchar* pLayerTag, CCollider* pCollider)
{
	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	if (nullptr == pGameInstance)
		return E_FAIL;
	Safe_AddRef(pGameInstance);

	if (FAILED(pGameInstance->Attach_Collider(pLayerTag, pCollider)))
	{
		MSG_BOX("CSpriteObject - LateTick() - FAILED");
		Safe_Release(pGameInstance);
		return E_FAIL;
	}
	Safe_Release(pGameInstance);

	return S_OK;
}

_vector CSpriteObject::Adjust_PositionUp_Radius(const _float& RadiusY)
{
	_vector vPosition = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	return XMVectorSetY(vPosition, XMVectorGetY(vPosition) + RadiusY * 0.5f);
}

void CSpriteObject::MoveToDirection(const SPRITE_DIRECTION& Direction, _double TimeDelta)
{
	if (Direction == SPRITE_DIRECTION::LEFT)
	{
		m_pTransformCom->Go_Left(TimeDelta);
	}
	else if (Direction == SPRITE_DIRECTION::RIGHT)
	{
		m_pTransformCom->Go_Right(TimeDelta);
	}
	else
	{
		MSG_BOX("CSolider - Walk() - ERROR");
	}

	return;
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
			m_iOrder = m_pTextureCom->Get_Order();
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

void CSpriteObject::OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	if (nullptr == pTargetCollider || nullptr == pTarget)
		return;
}

void CSpriteObject::OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
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

void CSpriteObject::OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer)
{
	if (nullptr == pTargetCollider || nullptr == pTarget)
		return;
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

HRESULT CSpriteObject::SetUp_Shader_Default()
{
	if (FAILED(m_pShaderCom->Set_RawValue("g_vColor", &m_vColor, sizeof(_vector))))
		return E_FAIL;

	if (nullptr == m_pSpriteFileName)
	{
		if (FAILED(m_pTextureCom->Set_ShaderResource(m_pShaderCom, "g_Texture", m_iTextureIndex)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(m_pTextureCom->Set_ShaderResource(m_pShaderCom, "g_Texture", m_pSpriteFileName)))
			return E_FAIL;
	}

	return S_OK;
}

void CSpriteObject::Free()
{
	__super::Free();

	Safe_Delete(m_pState);

	/** @note - m_pSpriteFileName 해제하면 안되는 이유
	현재 m_pSpriteFileName는 문자열 리터럴을 가리키므로 읽기 전용 데이터에 저장되어 자동으로 삭제되기 때문 (동적할당 해준 경우만 해제해줄 것)*/
	//Safe_Delete_Array(m_tSpriteInfo.pTextureComTag);
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
	Safe_Release(m_pWidgetCom);
	Safe_Release(m_pLineRiderCom);
}