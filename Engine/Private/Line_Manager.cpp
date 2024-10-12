#include "Line_Manager.h"
#include "PipeLine.h"
#include <queue>

IMPLEMENT_SINGLETON(CLine_Manager)

bool	operator==(XMFLOAT3 A, const XMFLOAT3 B)
{
	return A.x == B.x && A.y == B.y && A.z == B.z;
}

CLine_Manager::CLine_Manager()
{
	ZeroMemory(&m_tClosestLandingLine, sizeof(LINE_INFO));
}

HRESULT CLine_Manager::Initialize(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, void* pArg)
{
#ifdef _DEBUG
	m_pDevice = pDevice;
	m_pContext = pContext;

	m_pBatch = new PrimitiveBatch<DirectX::VertexPositionColor>(pContext);
	if (nullptr == m_pBatch)
		return E_FAIL;

	m_pEffect = new BasicEffect(pDevice);
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

_uint CLine_Manager::Tick(_double TimeDelta)
{
	return _uint();
}

_uint CLine_Manager::LateTick(_double TimeDelta)
{
	return _uint();
}

#ifdef _DEBUG
HRESULT CLine_Manager::Render()
{
	if (m_LineList.empty())
		return S_OK;

	//m_pEffect->SetWorld(DirectX::XMLoadFloat4x4(&m_WorldMatrix));
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
	const _uint iVertexNum = (_uint)m_LineList.size() * 2;
	VertexPositionColor* vertices = new VertexPositionColor[iVertexNum];

	list<tLine>::iterator iter = m_LineList.begin();
	int i = { -1 };
	for (iter; iter != m_LineList.end(); ++iter)
	{
		vertices[++i] = (*iter).tLeftVertex;
		vertices[++i] = (*iter).tRightVertex;
	}

	// 버텍스 버퍼 생성
	D3D11_BUFFER_DESC vertexBufferDesc;
	ZeroMemory(&vertexBufferDesc, sizeof(vertexBufferDesc));
	vertexBufferDesc.Usage = D3D11_USAGE_DEFAULT;
	vertexBufferDesc.ByteWidth = sizeof(VertexPositionColor) * iVertexNum;
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
	m_pContext->Draw(iVertexNum, 0); // 4개의 선분을 그리므로 정점 개수는 8입니다.

	// 정점 버퍼 해제
	Safe_Release(pVertexBuffer);
	Safe_Delete_Array(vertices);

	return S_OK;
}
#endif

bool CLine_Manager::HasPassableLine(const _float2& vInObjectPosition, _float& fOutLandingY)
{
	if (m_LineList.empty())
		return false;

	// greater: 내림차순 정렬
	// priority_queue<_float>로 사용 시 기본 정렬인 less로 적용됨
	const _float fImpossibleLanding = { -1000.f };
	_float fClosestLandingLineY = fImpossibleLanding;
	for (const tLine& tLine : m_LineList)
	{
		// 객체의 x 범위 안에 있는지 체크
		_float3 fLeftPos = tLine.tLeftVertex.position;
		_float3 fRightPos = tLine.tRightVertex.position;
		if (fLeftPos.x > vInObjectPosition.x
			|| fRightPos.x < vInObjectPosition.x)
		{
			continue;
		}

		// 평면의 방정식으로 두 점(라인) 사이의 점 중 x 좌표에 해당하는 점의 y 구하기
		_float fLandingY = EquationOfLine(fLeftPos, fRightPos, vInObjectPosition.x);

		// 객체보다 높이있는 선은 착지 범위에서 제외
		if (fLandingY > vInObjectPosition.y)
			continue;

		if (fClosestLandingLineY < fLandingY)
		{
			fClosestLandingLineY = fLandingY;
			m_tClosestLandingLine = tLine;
		}
		// @qurious - 왜 emplace보다 insert가 안전한지. 무엇이 더 좋은지 알아볼 것
	}

	// 탈 수 있는 라인이 없다면 종료
	if (fClosestLandingLineY == fImpossibleLanding)
		return false;
	
	fOutLandingY = fClosestLandingLineY;

	return true;
}

bool CLine_Manager::IsCurrentLineOccupied(const _float2& vObjectPosition, _float& fOutLandingY)
{
	// 매번 검사하면 오르막길을 못 오르기 때문에 한 번만 검사하고, 무조건 내가 타고 있는 라인의 y로 고정
	if (_float3(0.f, 0.f, 0.f) == m_tClosestLandingLine.tLeftVertex.position
		&& _float3(0.f, 0.f, 0.f) == m_tClosestLandingLine.tRightVertex.position)
	{
		_float fLandingY = { 0 };
		if (FAILED(HasPassableLine(vObjectPosition, fLandingY)))
			return false;
	}

	if (m_tClosestLandingLine.tLeftVertex.position.x > vObjectPosition.x
		|| m_tClosestLandingLine.tRightVertex.position.x < vObjectPosition.x)
	{
		return false;
	}

	// 두 점(라인) 사이의 점 중에 객체의 x 좌표에 해당하는 점의 y 구하기
	_float3 fA = m_tClosestLandingLine.tLeftVertex.position;
	_float3 fB = m_tClosestLandingLine.tRightVertex.position;

	fOutLandingY = EquationOfLine(fA, fB, vObjectPosition.x);
	return true;
}

const _float CLine_Manager::Get_Slope(const _float3 fA, const _float3 fB)
{
	/* 기울기 구하는 법
	* m = (y2 - y1) / (x2 - x1)
	*/
	return (fB.y - fA.y) / (fB.x - fA.x);
}

const _float CLine_Manager::Get_InterceptY(const _float fSlope, const _float3 fB)
{
	/* 직선의 방정식
	* 점 - 기울기 형태
	y - y1 = m * (x - x1)
	기울기-절편 형태 (y절편: x가 0일 때 y의 값)
	y = ax + b (a: 기울기, b는 y절편)
	*/

	_float fInterceptY;
	// 기울기 3, 한 점 (4, 6)일 때,
	// fA.y - 6 = 3 * (fA.x - 4);
	// fA.y = 3fA.x - 12 + 6;
	// fA.y는 y 절편이므로 fA.x는 0이기에
	// y절편 = 0 - 6 = -6
	fInterceptY = (fSlope * 0) + (fSlope * -1 * fB.x) + fB.y;
	return fInterceptY;
}

const _float CLine_Manager::EquationOfLine(const _float fSlope, const _float fInterceptY, const _float fX)
{
	/** 직선의 방정식
	y = ax + b (a: 기울기, b는 y절편)
	*/

	return fSlope * fX + fInterceptY;
}

const _float CLine_Manager::EquationOfLine(const _float3 fA, const _float3 fB, const _float fObjectX)
{
	const _float fSlope = Get_Slope(fA, fB);
	const _float fInterceptY = Get_InterceptY(fSlope, fB);

	return fSlope * fObjectX + fInterceptY;
}

void CLine_Manager::Free()
{
	__super::Free();

#ifdef _DEBUG
	Safe_Delete(m_pBatch);
	Safe_Delete(m_pEffect);
	Safe_Release(m_pInputLayout);
#endif // _DEBUG

	Safe_Delete(m_pDestVertex);
	Safe_Delete(m_pSourVertex);
	m_LineList.clear();
}
