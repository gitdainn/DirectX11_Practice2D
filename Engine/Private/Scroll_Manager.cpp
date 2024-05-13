#include "Scroll_Manager.h"

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
	if (0.f <= m_fScrollX)	// 맨 왼쪽에 도달했을 때
		m_fScrollX = 0.f;

	if (0.f <= m_fScrollY) // 맨 위쪽에 도달했을 때
		m_fScrollY = 0.f;

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
	m_fScrollX = 0.f;
	m_fScrollY = 0.f;
}

void CScroll_Manager::Free()
{
	__super::Free();
}

