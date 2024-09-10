#include "Line.h"
#include "PipeLine.h"

CLine::CLine(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
    : CGameObject(pDevice, pContext)
{
	ZeroMemory(&m_DestVertex, sizeof(VertexPositionColor));
	ZeroMemory(&m_SourVertex, sizeof(VertexPositionColor));
}

CLine::CLine(const CLine& rhs)
	: CGameObject(rhs)
	//, m_WorldMatrix(rhs.m_WorldMatrix)
#ifdef _DEBUG
	, m_pBatch(rhs.m_pBatch)
	, m_pEffect(rhs.m_pEffect)
	, m_pInputLayout(rhs.m_pInputLayout)
#endif // _DEBUG
{
#ifdef _DEBUG
	Safe_AddRef(m_pInputLayout);
#endif _DEBUG

	ZeroMemory(&m_DestVertex, sizeof(VertexPositionColor));
	ZeroMemory(&m_SourVertex, sizeof(VertexPositionColor));
}

HRESULT CLine::Initialize_Prototype()
{
#ifdef _DEBUG
	m_pBatch = new PrimitiveBatch<DirectX::VertexPositionColor>(m_pContext);
	if (nullptr == m_pBatch)
		return E_FAIL;

	m_pEffect = new BasicEffect(m_pDevice);
	if (nullptr == m_pEffect)
		return E_FAIL;

	m_pEffect->SetVertexColorEnabled(true);

	const void* pShaderByteCode = { nullptr };
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

HRESULT CLine::Initialize(const VertexPositionColor& vDestVertex, const VertexPositionColor& vSourVertex, void* pArg)
{
	return S_OK;
}

HRESULT CLine::Render()
{
	//m_pEffect->SetWorld(DirectX::XMLoadFloat4x4(&WorldMatrix));
	m_pEffect->SetWorld(XMMatrixIdentity());

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

	XMFLOAT4 vColor = XMFLOAT4(0.f, 1.f, 0.f, 1.f);

	// 사각형의 네 선분 정의 (토폴로지 D3D11_PRIMITIVE_TOPOLOGY_LINELIST일 때)
	VertexPositionColor vertices[] =
	{
		m_DestVertex, m_SourVertex
	};

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
	m_pContext->Draw(2, 0); // 4개의 선분을 그리므로 정점 개수는 8입니다.

	// 정점 버퍼 해제
	Safe_Release(pVertexBuffer);

	return S_OK;
}

CGameObject* CLine::Clone(const tSpriteInfo& SpriteInfo, void* pArg) const
{
    MSG_BOX("CLine - Clone() - NOT Use this Clone");
    return nullptr;
}

CGameObject* CLine::Clone(void* pArg) const
{
	CLine* pInstance = new CLine(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned CInstallObject");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLine::Free()
{
    __super::Free();
}
