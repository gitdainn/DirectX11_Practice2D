#include "Widget.h"
#include "GameObject.h"
#include "PipeLine.h"

CWidget::CWidget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CWidget::CWidget(const CWidget& rhs)
	: CComponent(rhs)
{
}

HRESULT CWidget::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
	{
		return E_FAIL;
	}

	return S_OK;
}

HRESULT CWidget::Initialize(void* pArg)
{
	if (FAILED(__super::Initialize(pArg)))
	{
		return E_FAIL;
	}

	if (nullptr == pArg)
	{
		MSG_BOX("CWidget - Initialize() - Widget 이름을 적어주세요.");
		return E_FAIL;
	}

	WIDGET_ARGUMENT tWidgetArg;
	memcpy(&tWidgetArg, pArg, sizeof(WIDGET_ARGUMENT));

	m_pOwner = tWidgetArg.pOwner;

	// pFilePath로 LoadFile 하기

	return S_OK;
}

_uint CWidget::Tick_BindToOwner(_double TimeDelta)
{
	/** 직교 투영되는 위젯 UI는 스크린 상의 좌표로 고정이 됩니다.
	즉, 원근 투영되는 몬스터를 따라가게 하고 싶으면 월드 상의 몬스터 좌표를 스크린 상의 좌표로 변환하여 셋팅해야 합니다. */

	if (m_WidgetList.empty())
		return _uint();

	if (nullptr == m_pOwner || m_pOwner->Get_IsDead())
	{
		Notify_OwnerDead();
		return _uint();
	}

	CTransform* pOwnerTransform = m_pOwner->Get_TransformCom();
	if (nullptr == pOwnerTransform)
		return _uint();

	CPipeLine* pPipeLine = CPipeLine::GetInstance();
	if (nullptr == pPipeLine)
		return _uint();

	//_float4x4 OwnerWorldMatrix = pOwnerTransform->Get_WorldMatrixFloat();
	//OwnerWorldMatrix._42 -= 40.f; // y 위치 보정

	//// 월드 -> 뷰스
	//_matrix ViewMatrix = pPipeLine->Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE::D3DTS_VIEW);
	//_matrix MultipleMatrix = XMMatrixMultiply(XMLoadFloat4x4(&OwnerWorldMatrix), ViewMatrix);

	//// 뷰스 -> 투영
	//_matrix ProjMatrix = pPipeLine->Get_Transform_Matrix(CPipeLine::TRANSFORMSTATE::D3DTS_PROJ);
	//MultipleMatrix = XMMatrixMultiply(MultipleMatrix, ProjMatrix);

	//// 투영 -> 뷰포트
	//_float fViewportX = ((XMVectorGetX(MultipleMatrix.r[3]) + 1.f) * (1280.f * 0.5f));
	//_float fViewportY = ((XMVectorGetY(MultipleMatrix.r[3]) - 1.f) * -(720.f * 0.5f));

	//// 뷰포트 -> 윈도우 좌표로 보정하기
	//_vector vOwnerPosInWindow = XMVectorSet(fViewportX - 1280.f * 0.5f
	//						, -fViewportY + 720.f * 0.5f
	//						, 0.f, 1.f);

	_vector vOwnerPos = m_pOwner->Get_TransformCom()->Get_State(CTransform::STATE_POSITION);
	vOwnerPos = XMVectorSetY(vOwnerPos, XMVectorGetY(vOwnerPos) - 40.f); // y 위치 보정

	// World -> View Space
	vOwnerPos = XMVector3TransformCoord(vOwnerPos, pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_VIEW)); 
	// View Space -> Projection
	vOwnerPos = XMVector3TransformCoord(vOwnerPos, pPipeLine->Get_Transform_Matrix(CPipeLine::D3DTS_PROJ)); 

	// Projection ->View port
	_float4 vViewPortPos = { 0.f, 0.f, 0.f, 1.f }; // Projection ->View port
	vViewPortPos.x = (XMVectorGetX(vOwnerPos) + 1.f) * (1280 * 0.5f);
	vViewPortPos.y = (XMVectorGetY(vOwnerPos) - 1.f) * -(720 * 0.5f);

	// 뷰포트 상 좌표와 스크린 상 좌표 위치 보정
	vOwnerPos = XMVectorSet(vViewPortPos.x - 1280 * 0.5f
		, -vViewPortPos.y + 720 * 0.5f
		, 0.f, 1.f); 

	for (CGameObject* pObject : m_WidgetList)
	{
		if (nullptr == pObject)
			continue;

		CTransform* pTransform = pObject->Get_TransformCom();
		if (nullptr == pTransform)
			return _uint();

		pTransform->Set_State(CTransform::STATE_POSITION, vOwnerPos);
	}

	return _uint();
}

HRESULT CWidget::Add_Widget(CGameObject* pObject)
{
	if (nullptr == pObject)
		return E_FAIL;

	m_WidgetList.emplace_back(pObject);

	return S_OK;
}

CGameObject* CWidget::Get_WidgetByName(const _tchar* pObjName)
{
	if (nullptr == pObjName || m_WidgetList.empty())
		return nullptr;

	CGameObject* pTargetObject = { nullptr };
	for (CGameObject* pObj : m_WidgetList)
	{
		if (nullptr == pObj)
			continue;

		if (!lstrcmp(pObjName, pObj->Get_NameTag()))
		{
			pTargetObject = pObj;
			break;
		}
	}

	return pTargetObject;
}

void CWidget::Notify_OwnerDead()
{
	if (m_WidgetList.empty())
		return;

	for (CGameObject* pObject : m_WidgetList)
	{
		if (nullptr == pObject)
			continue;

		pObject->Set_Dead();
	}
}

CWidget* CWidget::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CWidget* pInstance = new CWidget(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		MSG_BOX("Failed to Created : CWidget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

CComponent* CWidget::Clone(void* pArg)
{
	CWidget* pInstance = new CWidget(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		MSG_BOX("Failed to Cloned : CWidget");
		Safe_Release(pInstance);
	}

	return pInstance;
}

void CWidget::Free()
{
	m_WidgetList.clear();
	__super::Free();
}
