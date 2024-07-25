#include "..\Public\GameObject.h"
#include "Component_Manager.h"

int CGameObject::g_iObjectID = { 0 };

CGameObject::CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: m_pDevice(pDevice)
	, m_pContext(pContext)
	, m_bIsDead(false), m_bIsRender(true)
	, m_eRenderGroup(CRenderer::RENDERGROUP::RENDER_PRIORITY)
	, m_ID(0)
{
	Safe_AddRef(m_pDevice);
	Safe_AddRef(m_pContext);

	XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
	XMStoreFloat4x4(&m_ProjMatrix, XMMatrixIdentity());
}

/** @note - 자식 객체가 삭제될 때 최상위 부모의 소멸자까지 호출되는가
* => 자식 객체 소멸자 호출 -> 부모 소멸자 호출 -> virtual 시 자식 소멸자 한 번 더 호출?
*/

/** @note - m_pTransformCom과 m_pRendererCom은 각 객체마다 Add_Components로 사본 만들어서 사용함.
* 이때, Add_Components에서 Add_Ref를 해주고, Free()에서 m_Components에 담긴 모든 것들 Relese 해주기 때문에 delete 정상 다 됨!
*/

CGameObject::CGameObject(const CGameObject & rhs)
	: m_pDevice(rhs.m_pDevice)
	, m_pContext(rhs.m_pContext)
	, m_bIsDead(rhs.m_bIsDead), m_bIsRender(rhs.m_bIsRender)
	, m_eRenderGroup(rhs.m_eRenderGroup)
	, m_ViewMatrix(rhs.m_ViewMatrix), m_ProjMatrix(rhs.m_ProjMatrix)
	, m_ID(rhs.m_ID)
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
	if (m_bIsDead)
		return OBJ_DEAD;

	return _uint();
}

_uint CGameObject::LateTick(_double TimeDelta)
{
	if (m_bIsRender)
	{
		//@qurious. enum class가 아니라 일반 enum이면 engine의 열거체를 멤버로 선언 시 사용불가임.
		m_pRendererCom->Add_RenderGroup(m_eRenderGroup, this);
	}

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

HRESULT CGameObject::Change_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg)
{
	if (nullptr == Find_Component(pComponentTag))
	{
		MSG_BOX("CGameObject - Change_Component() - You Have to Add_Component()");
		return E_FAIL;
	}

	CComponent_Manager* pComponent_Manager = CComponent_Manager::GetInstance();
	Safe_AddRef(pComponent_Manager);

	/* 원형객체를 복사하여 사본 컴포넌트를 만든다. */
	CComponent* pComponent = pComponent_Manager->Clone_Component(iLevelIndex, pPrototypeTag, pArg);
	if (nullptr == pComponent)
		return E_FAIL;

	Safe_Release(pComponent_Manager);

	/* 부모의 맵컨테이너에 복제한 컴포넌트를 추가한다. */
	m_Components.erase(pComponentTag);
	m_Components.emplace(pComponentTag, pComponent);

	Safe_Release(*ppOut);
	Safe_Release(*ppOut);
	*ppOut = pComponent;

	Safe_AddRef(pComponent);

	return S_OK;
}

void CGameObject::Free()
{
	for (auto& Pair : m_Components)
	{
		Safe_Release(Pair.second);
	}
	m_Components.clear();

	Safe_Release(m_pDevice);
	Safe_Release(m_pContext);
}
