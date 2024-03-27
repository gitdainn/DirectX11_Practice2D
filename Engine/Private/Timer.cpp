#include "Timer.h"

CTimer::CTimer()
	: m_TimeDelta { 0.0 }
{
	ZeroMemory(&m_tFrameTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_tFixTime, sizeof(LARGE_INTEGER));
	ZeroMemory(&m_tLastTime, sizeof(LARGE_INTEGER));

	/* 1초에 cpu가 갱신할 수 있는 시간. */
	ZeroMemory(&m_CpuTick, sizeof(LARGE_INTEGER));
}

HRESULT CTimer::Ready_Timer(void)
{
	QueryPerformanceCounter(&m_tFrameTime);
	QueryPerformanceCounter(&m_tLastTime);
	QueryPerformanceCounter(&m_tFixTime);
	QueryPerformanceFrequency(&m_CpuTick);
	
	return S_OK;
}

void Engine::CTimer::Update_Timer(void)
{
	QueryPerformanceCounter(&m_tFrameTime);

	if (m_tFrameTime.QuadPart - m_tFixTime.QuadPart >= m_CpuTick.QuadPart)
	{
		QueryPerformanceFrequency(&m_CpuTick);
		m_tFixTime = m_tFrameTime;
	}

	/* 각 프레임간 호출되는 시간차.  */
	/* 각 프레임간 호출되는 시간차. / 1초에 카운팅할 수 있는 최대 시간.*/
	/* == 프레임간 호출되는 시간차를 1초를 기준으로 한 비율로 표현한 것. */
	m_TimeDelta = ((m_tFrameTime.QuadPart) - (m_tLastTime.QuadPart)) / _double(m_CpuTick.QuadPart);

	m_tLastTime = m_tFrameTime;
}

CTimer * CTimer::Create(void)
{
	CTimer *	pInstance = new CTimer;

	if (FAILED(pInstance->Ready_Timer()))
		Safe_Release(pInstance);

	return pInstance;
}

void CTimer::Free(void)
{

}
