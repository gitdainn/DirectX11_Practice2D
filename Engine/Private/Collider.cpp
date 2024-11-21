#include "..\Public\Collider.h"
#include "DebugDraw.h"
#include "PipeLine.h"
#include "GameObject.h"
#include "GameInstance.h"
#include "Transform.h"

#include "ColliderAABB2D.h"
#include "ColliderOBB2D.h"
#include "ColliderSphere2D.h"

_uint CCollider::g_iColliderID = { 0 };

CCollider::CCollider(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CComponent(pDevice, pContext)
{
	XMStoreFloat4x4(&m_WorldMatrix, XMMatrixIdentity());
	ZeroMemory(&m_tColliderDesc, sizeof(COLLIDER_DESC));
}

CCollider::CCollider(const CCollider & rhs)
	: CComponent(rhs)
	, m_WorldMatrix(rhs.m_WorldMatrix)
	, m_tColliderDesc(rhs.m_tColliderDesc)
	, m_iID(++g_iColliderID)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG
{

#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif _DEBUG
}

HRESULT CCollider::Initialize_Prototype()
{
#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<DirectX::VertexPositionColor>(m_pContext);
	if (nullptr == m_pBatch)
		return E_FAIL;

	m_pEffect = new BasicEffect(m_pDevice);
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->SetVertexColorEnabled(true);

	const void*		pShaderByteCode = { nullptr };
	size_t			iShaderByteCodeLength = { 0 };

	m_pEffect->GetVertexShaderBytecode(&pShaderByteCode, &iShaderByteCodeLength);

	/** IA(입력-어셈블러) 단계에 대한 입력 버퍼 데이터를 설명하는 입력 레이아웃 객체 생성 */
	// 각 단일 버텍스(정점)에 대한 데이터 요소(위치, 색상, 벡터 등) 지정
	// D3D11_INPUT_ELEMENT_DESC: 입력 레이아웃 정의 (Position, Color등 단일 꼭짓점 버텍스가 가지는 요소들에 대한 설명인듯)
	// 
	if (FAILED(m_pDevice->CreateInputLayout(DirectX::VertexPositionColor::InputElements,
		DirectX::VertexPositionColor::InputElementCount,
		pShaderByteCode, iShaderByteCodeLength, &m_pInputLayout)))
		return E_FAIL;
#endif // _DEBUG
	
	return S_OK;
}

HRESULT CCollider::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CColider - Initialize - Argument is NULL");
		return E_FAIL;
	}

	/** @note - dynamic_cast는 완전한 클래스 형식에만 가능하므로 void형에는 불가능 (C타입의 형변환 사용할 것) */
	COMPONENT_INFO tComponentInfo;
	memcpy(&tComponentInfo, (COMPONENT_INFO*)pArg, sizeof(COMPONENT_INFO));

	m_tColliderDesc.vScale.x = tComponentInfo.fSize.x;
	m_tColliderDesc.vScale.y = tComponentInfo.fSize.y;
	m_tColliderDesc.vOffset.x = tComponentInfo.fOffset.x;
	m_tColliderDesc.vOffset.y = tComponentInfo.fOffset.y;
	m_tColliderDesc.vPosition.x = tComponentInfo.fPosition.y;
	m_tColliderDesc.vPosition.x = tComponentInfo.fPosition.y;

	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}
	
	return S_OK;
}

_uint CCollider::Tick(_double TimeDelta)
{
	if (nullptr == m_pOwner || m_pOwner->Get_IsDead())
	{
		m_bIsDead = true;
		return OBJ_DEAD;
	}

	m_bIsCollision = false;

	CGameInstance* pGameInstance = CGameInstance::GetInstance();
	Safe_AddRef(pGameInstance);

	CTransform* pOwnerTransformCom = m_pOwner->Get_TransformCom();
	if (nullptr != pOwnerTransformCom)
	{
		_vector vOwnerPos = pOwnerTransformCom->Get_State(CTransform::STATE_POSITION);
		m_tColliderDesc.vPosition = _float3(XMVectorGetX(vOwnerPos) + m_tColliderDesc.vOffset.x
											, XMVectorGetY(vOwnerPos) + m_tColliderDesc.vOffset.y, 1.f);

		//if (m_pOwner->Get_IsScroll())
		//{
		//	m_tColliderDesc.vPosition.x += pGameInstance->Get_ScrollX();
		//	m_tColliderDesc.vPosition.y += pGameInstance->Get_ScrollY();
		//}
	}

	_matrix ScaleMatrix = XMMatrixScaling(m_tColliderDesc.vScale.x, m_tColliderDesc.vScale.y, 1.f);
	_matrix TranslationMatrix = XMMatrixTranslation(m_tColliderDesc.vPosition.x, m_tColliderDesc.vPosition.y, 0.f);
	XMStoreFloat4x4(&m_WorldMatrix, ScaleMatrix * TranslationMatrix);

	Safe_Release(pGameInstance);
}

const _bool CCollider::IsCollision(CCollider* pTargetCollider)
{
	if (nullptr == pTargetCollider)
		return false;

	_bool bIsCollision = { false };
	CColliderAABB2D* pAABB = dynamic_cast<CColliderAABB2D*>(pTargetCollider);
	if (nullptr != pAABB)
		bIsCollision = Intersects(pAABB);

	CColliderOBB2D* pOBB = dynamic_cast<CColliderOBB2D*>(pTargetCollider);
	if (nullptr != pOBB)
		bIsCollision = Intersects(pOBB);

	CColliderSphere2D* pSphere = dynamic_cast<CColliderSphere2D*>(pTargetCollider);
	if (nullptr != pSphere)
		bIsCollision = Intersects(pSphere);


	return bIsCollision;
}

void CCollider::OnCollisionEnter(CCollider* pTargetCollider, const _tchar* pTargetLayer)
{
	if (nullptr == pTargetCollider || nullptr == m_pOwner)
		return;

	m_pOwner->OnCollisionEnter(pTargetCollider, pTargetCollider->Get_Owner(), pTargetLayer);
}

void CCollider::OnCollisionStay(CCollider* pTargetCollider, const _tchar* pTargetLayer)
{
	if (nullptr == pTargetCollider || nullptr == m_pOwner)
		return;

	m_pOwner->OnCollisionStay(pTargetCollider, pTargetCollider->Get_Owner(), pTargetLayer);
}

void CCollider::OnCollisionExit(CCollider* pTargetCollider, const _tchar* pTargetLayer)
{
	if (nullptr == pTargetCollider || nullptr == m_pOwner)
		return;

	m_pOwner->OnCollisionExit(pTargetCollider, pTargetCollider->Get_Owner(), pTargetLayer);
}

void CCollider::Set_Owner(CGameObject* pOwner)
{
	__super::Set_Owner(pOwner);

	// @error - 이거 어차피 Transform 컴포넌트 할당 전이라 의미 없음 
	CTransform* pOwnerTransformCom = m_pOwner->Get_TransformCom();
	if (nullptr != pOwnerTransformCom)
	{
		m_WorldMatrix = pOwnerTransformCom->Get_WorldMatrixFloat();
	}
}

//#ifdef _DEBUG
//HRESULT CCollider::Render()
//{
//	_float4x4 WorldMatrix = m_pOwner->Get_TransformCom()->Get_WorldMatrixFloat();
//	CGameInstance* pGameInstance = CGameInstance::GetInstance();
//	Safe_AddRef(pGameInstance);
//
//	_vector vPosition = m_pOwner->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
//	WorldMatrix._41 = XMVectorGetX(vPosition) + pGameInstance->Get_ScrollX();
//	WorldMatrix._42 = XMVectorGetY(vPosition) + pGameInstance->Get_ScrollY();
//
//	Safe_Release(pGameInstance);
//
//	m_pEffect->SetWorld(DirectX::XMLoadFloat4x4(&WorldMatrix));
//
//	CPipeLine* pPipeLine = CPipeLine::GetInstance();
//	Safe_AddRef(pPipeLine);
//
//	_float4x4 ViewMatrix;
//	_float4x4 ProjMatrix;
//	DirectX::XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
//	DirectX::XMStoreFloat4x4(&ProjMatrix, XMMatrixOrthographicLH(1280, 720, 0.f, 1.f));
//
//	m_pEffect->SetView(DirectX::XMLoadFloat4x4(&ViewMatrix));
//	m_pEffect->SetProjection(DirectX::XMLoadFloat4x4(&ProjMatrix));
//
//	Safe_Release(pPipeLine);
//
//	/** 입력 레이아웃 객체를 IA(Input-Assembler) 단계에 바인딩한다.
//	입력 레이아웃 객체: 꼭짓점 버퍼 데이터가 IA 파이프라인 단계로 스트리밍되는 방법 설명
//	입력 레이아웃 객체는 CreateInputLayout으로 생성*/
//	m_pContext->IASetInputLayout(m_pInputLayout);
//	/** 기하 도형 쉐이더를 디바이스로 설정한다.
//	(기하 도형 셰이더에 대한 포인터 null이면 쉐이더 X, 클래스 인스턴스 인터페이스 배열 - 셰이더 각 인터페이스는 인스턴스 필요, 배열 수)*/
//	m_pContext->GSSetShader(nullptr, nullptr, 0);
//
//	/* 출력 전에 셰이더에 반드시 던져야할 리소스(텍스쳐, 행렬, 벡터 etc) 던지기. */
//	/** 한 번 지정한 출력 상태 등은 모든 렌더링 상태에 적용되므로 각 오브젝트마다
//	* 자기 자신의 입력 레이아웃 객체 등으로 설정한 후 매번 출력 전에 Apply로 던져줘야 한다. */
//	m_pEffect->Apply(m_pContext);
//
//	// Begin batch drawing
//	m_pBatch->Begin();
//
//	XMFLOAT4 vColor = m_bIsCollision ? XMFLOAT4(1.f, 0.f, 0.f, 1.f) : XMFLOAT4(1.f, 1.f, 1.f, 1.f);
//
//	VertexPositionColor vertices[] =
//	{
//		// 상단 선분
//		{ XMFLOAT3(-0.5f, 0.5f, 0.0f), vColor },    // 왼쪽 상단
//		{ XMFLOAT3(0.5f, 0.5f, 0.0f), vColor },     // 오른쪽 상단
//
//		// 우측 선분
//		{ XMFLOAT3(0.5f, 0.5f, 0.0f), vColor },     // 오른쪽 상단
//		{ XMFLOAT3(0.5f, -0.5f, 0.0f), vColor },    // 오른쪽 하단
//
//		// 하단 선분
//		{ XMFLOAT3(0.5f, -0.5f, 0.0f), vColor },    // 오른쪽 하단
//		{ XMFLOAT3(-0.5f, -0.5f, 0.0f), vColor },   // 왼쪽 하단
//
//		// 좌측 선분
//		{ XMFLOAT3(-0.5f, -0.5f, 0.0f), vColor },   // 왼쪽 하단
//		{ XMFLOAT3(-0.5f, 0.5f, 0.0f), vColor }     // 왼쪽 상단
//	};
//
//	/** DirectX Tool Kit의 PrimitiveBatch는 SetVertex, SetIndex 할 필요없이
//	* 내부적으로 이미 구현되어 있어 매개변수로 (토폴로지, 정점 한 번씩만 선언, 정점 개수)
//	* 이렇게만 알려주면 간단하게 폴리곤 정점을 그릴 수 있다. */
//	m_pBatch->Draw(D3D11_PRIMITIVE_TOPOLOGY_LINELIST, vertices, 8);
//
//	// End batch drawing
//	m_pBatch->End();
//
//	return S_OK;
//}
//#endif // _DEBUG

#ifdef _DEBUG
HRESULT CCollider::Render()
{
	m_pEffect->SetWorld(DirectX::XMLoadFloat4x4(&m_WorldMatrix));
	//m_pEffect->SetWorld(XMMatrixIdentity());

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	Safe_AddRef(pPipeLine);

	_float4x4 ViewMatrix;
	_float4x4 ProjMatrix;
	DirectX::XMStoreFloat4x4(&ViewMatrix, XMMatrixIdentity());
	DirectX::XMStoreFloat4x4(&ProjMatrix, XMMatrixOrthographicLH(1280, 720, 0.f, 1.f));

	m_pEffect->SetView(DirectX::XMLoadFloat4x4(&ViewMatrix));
	m_pEffect->SetProjection(DirectX::XMLoadFloat4x4(&ProjMatrix));

	Safe_Release(pPipeLine);

	// 입력 레이아웃 설정
	m_pContext->IASetInputLayout(m_pInputLayout);

	// 기하 도형 셰이더 설정
	m_pContext->GSSetShader(nullptr, nullptr, 0);

	// 출력 전에 셰이더에 반드시 던져야할 리소스(텍스쳐, 행렬, 벡터 등) 적용
	m_pEffect->Apply(m_pContext);

	XMFLOAT4 vColor = m_bIsCollision ? XMFLOAT4(1.f, 0.f, 0.f, 1.f) : XMFLOAT4(0.f, 1.f, 0.f, 1.f);

	// 사각형의 네 선분 정의 (토폴로지 D3D11_PRIMITIVE_TOPOLOGY_LINELIST일 때)
	VertexPositionColor vertices[] =
	{
		// 상단 선분
		{ XMFLOAT3(-0.5f, 0.5f, 0.0f), vColor },    // 왼쪽 상단
		{ XMFLOAT3(0.5f, 0.5f, 0.0f), vColor },     // 오른쪽 상단

		// 우측 선분
		{ XMFLOAT3(0.5f, 0.5f, 0.0f), vColor },     // 오른쪽 상단
		{ XMFLOAT3(0.5f, -0.5f, 0.0f), vColor },    // 오른쪽 하단

		// 하단 선분
		{ XMFLOAT3(0.5f, -0.5f, 0.0f), vColor },    // 오른쪽 하단
		{ XMFLOAT3(-0.5f, -0.5f, 0.0f), vColor },   // 왼쪽 하단

		// 좌측 선분
		{ XMFLOAT3(-0.5f, -0.5f, 0.0f), vColor },   // 왼쪽 하단
		{ XMFLOAT3(-0.5f, 0.5f, 0.0f), vColor }     // 왼쪽 상단
	};


	// 사각형의 네 꼭지점 정의 (토폴로지 D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST일 때)
	//VertexPositionColor vertices[] =
	//{
	//	// 첫 번째 삼각형 (왼쪽 위에서 시계 방향으로)
	//	{ XMFLOAT3(-0.5f, 0.5f, 0.0f), vColor },    // 왼쪽 상단
	//	{ XMFLOAT3(0.5f, 0.5f, 0.0f), vColor },     // 오른쪽 상단
	//	{ XMFLOAT3(-0.5f, -0.5f, 0.0f), vColor },   // 왼쪽 하단

	//	// 두 번째 삼각형 (오른쪽 아래에서 반시계 방향으로)
	//	{ XMFLOAT3(-0.5f, -0.5f, 0.0f), vColor },   // 왼쪽 하단
	//	{ XMFLOAT3(0.5f, 0.5f, 0.0f), vColor },     // 오른쪽 상단
	//	{ XMFLOAT3(0.5f, -0.5f, 0.0f), vColor }     // 오른쪽 하단
	//};

	//VertexPositionColor vertices[] =
	//{
	//	{ XMFLOAT3(-0.5f, 0.5f, 0.0f), vColor }, // 왼쪽 상단
	//	{ XMFLOAT3(0.5f, 0.5f, 0.0f), vColor },  // 오른쪽 상단
	//	{ XMFLOAT3(0.5f, -0.5f, 0.0f), vColor }, // 오른쪽 하단
	//	{ XMFLOAT3(-0.5f, -0.5f, 0.0f), vColor } // 왼쪽 하단
	//};

	// 버텍스 버퍼 생성
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(vertices);
	vertexBufferDesc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	vertexBufferDesc.CPUAccessFlags = 0;

	D3D11_SUBRESOURCE_DATA vertexBufferData;
	ZeroMemory(&vertexBufferData, sizeof(vertexBufferData));
	vertexBufferData.pSysMem = vertices;

	ID3D11Buffer* pVertexBuffer = nullptr;
	HRESULT hr = m_pDevice->CreateBuffer(&vertexBufferDesc, &vertexBufferData, &pVertexBuffer);
	if (FAILED(hr))
	{
		MSG_BOX("CCollider - Render() - BUFFER ERROR");
		return E_FAIL;
	}

	// 프리미티브 유형 및 토폴로지 설정 (라인 리스트로 설정)
	m_pContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_LINELIST);

	// 정점 버퍼 설정
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	m_pContext->IASetVertexBuffers(0, 1, &pVertexBuffer, &stride, &offset);

	// 사각형 외곽선 그리기
	m_pContext->Draw(8, 0); // 4개의 선분을 그리므로 정점 개수는 8입니다.

	// 정점 버퍼 해제
	Safe_Release(pVertexBuffer);

	return S_OK;
}
#endif // _DEBUG


_matrix CCollider::Remove_Rotation(_fmatrix TransformMatrix)
{
	_matrix		Matrix = TransformMatrix;

	Matrix.r[0] = XMVectorSet(1.f, 0.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Matrix.r[0]));
	Matrix.r[1] = XMVectorSet(0.f, 1.f, 0.f, 0.f) * XMVectorGetX(XMVector3Length(Matrix.r[1]));
	Matrix.r[2] = XMVectorSet(0.f, 0.f, 1.f, 0.f) * XMVectorGetX(XMVector3Length(Matrix.r[2]));

	return Matrix;
}

void CCollider::Free()
{
	__super::Free();

#ifdef _DEBUG
	if (false == m_isCloned)
	{
		Safe_Delete(m_pBatch);
		Safe_Delete(m_pEffect);
	}
	Safe_Release(m_pInputLayout);
#endif // _DEBUG
}
