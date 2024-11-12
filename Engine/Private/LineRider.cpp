#include "LineRider.h"
#include "Line_Manager.h"

CLineRider::CLineRider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
	ZeroMemory(&m_tClosestLandingLine, sizeof(LINE_INFO));
}

CLineRider::CLineRider(const CLineRider& rhs)
	: CComponent(rhs)
	, m_pLine_Manager(CLine_Manager::GetInstance())
{
	ZeroMemory(&m_tClosestLandingLine, sizeof(LINE_INFO));
	Safe_AddRef(m_pLine_Manager);
}

HRESULT CLineRider::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CLineRider::Initialize(void* pArg)
{
	if (nullptr == pArg)
	{
		MSG_BOX("CLineRider - Initialize() - NULL");
		return E_FAIL;
	}

	_float2 vPosition = { 0.f, 0.f };
	memcpy(&vPosition, pArg, sizeof(_float2));

	// 가장 가까운 선 받아오기
	if (FAILED(m_pLine_Manager->Get_ClosestLineToRide(vPosition, m_tClosestLandingLine)))
	{
		MSG_BOX("CLineRider - Initialize() - 탈 수 있는 선이 없습니다.");
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CLineRider::Collision_Line(_vector vPosition, _float& fOutLandingY)
{
	// 현재 가장 가까운 땅의 양끝을 벗어났다면 재검색합니다.
	if (m_tClosestLandingLine.tLeftVertex.position.x > XMVectorGetX(vPosition)
		|| m_tClosestLandingLine.tRightVertex.position.x < XMVectorGetX(vPosition))
	{
		// 착지할 수 있는 땅이 없습니다.
		_float2 fPositon = _float2(XMVectorGetX(vPosition), XMVectorGetY(vPosition));
		if (FAILED(m_pLine_Manager->Get_ClosestLineToRide(fPositon, m_tClosestLandingLine)))
		{
			ZeroMemory(&m_tClosestLandingLine, sizeof(LINE_INFO));
			return E_FAIL;
		}
		return E_FAIL;
	}

	// 두 점(라인) 사이의 점 중에 객체의 x 좌표에 해당하는 점의 y 구하기
	const _float3 fLeftX = m_tClosestLandingLine.tLeftVertex.position;
	const _float3 fRightX = m_tClosestLandingLine.tRightVertex.position;
	
	fOutLandingY = m_pLine_Manager->EquationOfLine(fLeftX, fRightX, XMVectorGetX(vPosition));

	return S_OK;
}

_bool CLineRider::CheckLineLanding(const _vector vPosition, _float& fOutLandingY)
{
	const _float fLimitY = { -1000.f };

	const _float3 fLeftX = m_tClosestLandingLine.tLeftVertex.position;
	const _float3 fRightX = m_tClosestLandingLine.tRightVertex.position;

	const _float fLineY = m_pLine_Manager->EquationOfLine(fLeftX, fRightX, XMVectorGetX(vPosition));

	// 이전 틱에서 가장 가까웠던 선보다 플레이어 위치가 낮아지면 해당 선으로 착지합니다.
	if (fLineY > XMVectorGetY(vPosition))
	{
		fOutLandingY = fLineY;
		return true;
	}
	else if (fLimitY > XMVectorGetY(vPosition))
	{
		MSG_BOX("CLineRider - CheckLineLanding - 무한 추락 상태입니다. 사망 처리 해주세요.");
	}
	else
	{
		_float2 fPositon = _float2(XMVectorGetX(vPosition), XMVectorGetY(vPosition));
		// 점프하는동안 탈 수 있는 선이 없을 수 있습니다. 없는 경우 선의 y좌표를 한계치로 설정합니다.
		if (FAILED(m_pLine_Manager->Get_ClosestLineToRide(fPositon, m_tClosestLandingLine)))
		{
			m_tClosestLandingLine.tLeftVertex.position = _float3(0.f, fLimitY, 1.f);
			m_tClosestLandingLine.tRightVertex.position = _float3(0.f, fLimitY, 1.f);
		}
	}

	return false;
}

CLineRider* CLineRider::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CLineRider* pInstance = new CLineRider(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CLineRider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CLineRider::Clone(void* pArg)
{
	CLineRider* pInstance = new CLineRider(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Created : CLineRider");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CLineRider::Free()
{
	__super::Free();

	Safe_Release(m_pLine_Manager);

}
