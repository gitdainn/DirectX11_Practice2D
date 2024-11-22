#include "Scroll_Manager.h"
#include "Line_Manager.h"
#include "GameObject.h"

USING(Engine)

IMPLEMENT_SINGLETON(CScroll_Manager)

CScroll_Manager::CScroll_Manager()
{
}

CScroll_Manager::~CScroll_Manager()
{
}

void CScroll_Manager::Scroll_Lock()
{
	if (0.f <= m_fScroll.x)	// 맨 왼쪽에 도달했을 때
		m_fScroll.x = 0.f;

	if (0.f <= m_fScroll.y) // 맨 위쪽에 도달했을 때
		m_fScroll.y = 0.f;

	//switch (_eScene)
	//{ // y만 전부 30씩 더 추가해줌 ( 그래야 가생이가 완벽히 안 보이더라 이유는 모르겠음 )
	//case SC_TOWN:
	//	if (WINCX - 5712 >= (int)m_fScrollX)	// 맨 왼쪽에 도달했을 때
	//		m_fScrollX = WINCX - 5712;

	//	if (WINCY - 1182 >= (int)m_fScrollY) // 맨 위쪽에 도달했을 때
	//		m_fScrollY = WINCY - 1182
	//	break;
	//}

}

void CScroll_Manager::Scroll_Clear()
{
	m_fScroll = { 0.f, 0.f };
}

void CScroll_Manager::Add_ScrollListener(CGameObject* pObject)
{
	if (nullptr == pObject)
		return;

	m_ScrollList.emplace_back(pObject);
	return;
}

void CScroll_Manager::Notify(const _float2 fScroll)
{
	// 1. 변화량만큼 객체들 위치 갱신
	for (CGameObject* pObject : m_ScrollList)
	{
		if (nullptr == pObject)
			continue;

		CTransform* pTransformCom = pObject->Get_TransformCom();
		if (nullptr == pTransformCom)
			return;

		_vector vPosition = pTransformCom->Get_State(CTransform::STATE_POSITION);

		_float fScrollSpeed = { 1.f }; // 이건 객체마다 달라야해서 임시코드
		vPosition = XMVectorSetX(vPosition, XMVectorGetX(vPosition) + fScroll.x * fScrollSpeed);
		vPosition = XMVectorSetY(vPosition, XMVectorGetY(vPosition) + fScroll.y * fScrollSpeed);

		pTransformCom->Set_State(CTransform::STATE_POSITION, vPosition);

		//pObject->ScrollNotify(fScroll); 
	}

	// 2. 라인도 스크롤 적용
	CLine_Manager* pLineManager = CLine_Manager::GetInstance();
	if (nullptr == pLineManager)
		return;
	pLineManager->Scroll_Line(Get_ScrollX(), Get_ScrollY());
}

void CScroll_Manager::Free()
{
	__super::Free();
}

