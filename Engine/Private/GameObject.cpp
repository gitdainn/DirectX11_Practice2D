#include "..\Public\GameObject.h"
#include "Component_Manager.h"

CGameObject::CGameObject(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

/** @qurious - 자식 객체가 삭제될 때 최상위 부모의 소멸자까지 호출되는가
*/

/** @qurious - m_pTransformCom과 m_pRendererCom은 각 객체마다 사본 만들어서 사용하는데
* 그렇다면 Safe_Release로 원본 삭제? 아니면 Safe_AddRef를 해줘야함? => 내 생각은 AddRef 생략 */

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);
}

HRESULT CGameObject::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CGameObject::Initialize(void * pArg)
{
	return S_OK;
}

_uint CGameObject::Tick(_double TimeDelta)
{
	return _uint();
}

_uint CGameObject::LateTick(_double TimeDelta)
{
	return _uint();
}

HRESULT CGameObject::Render()
{
	return S_OK;
}


HRESULT CGameObject::Add_Components(_uint iLevelIndex, const _tchar * pPrototypeTag, const _tchar * pComponentTag, CComponent** ppOut, void * pArg)
{
	if (nullptr != Find_Component(pComponentTag))
		return E_FAIL;

	CComponent_Manager*		pComponent_Manager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponent_Manager);

	/* 원형객체를 복사하여 사본 컴포넌트를 만든다. */
	CComponent*		pComponent = pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	Safe_Release(pComponent_Manager);

	/* 부모의 맵컨테이너에 복제한 컴포넌트를 추가한다. */
	m_Components.emplace(pComponentTag, pComponent);

	/* 자식에 변수에게도 공유시켜주었다. */
	*ppOut = pComponent;

	Safe_AddRef(pComponent);
	
	return S_OK;
}

CComponent * CGameObject::Find_Component(const _tchar * pComponentTag)
{
	auto	iter = find_if(m_Components.begin(), m_Components.end(), CTag_Finder(pComponentTag));

	if (iter == m_Components.end())
		return nullptr;

	return iter->second;
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
		Safe_Release(Pair.second);

	m_Components.clear();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
