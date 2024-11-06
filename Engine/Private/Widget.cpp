#include "Widget.h"
#include "GameObject.h"

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

	_vector vOwnerPos = pOwnerTransform->Get_State(CTransform::STATE_POSITION);

	for (CGameObject* pObject : m_WidgetList)
	{
		if (nullptr == pObject)
			continue;

		CTransform* pTransform = pObject->Get_TransformCom();
		if (nullptr == pTransform)
			return _uint();

		_vector vPos = { 0.f, -40.f, 0.f, 0.f };
		pTransform->Set_State(CTransform::STATE_POSITION, vOwnerPos + vPos);
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
