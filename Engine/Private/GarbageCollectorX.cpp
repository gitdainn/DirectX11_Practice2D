#pragma once
#include "GarbageCollector.h"

USING(Engine)

#pragma region SINGLETON
template<typename T>
//CGarbageCollectorX<T>* CGarbageCollectorX<T>::m_pInstance = nullptr;
//
//template<typename T>
//CGarbageCollectorX<T>* CGarbageCollectorX<T>::GetInstance(void)
//{
//	if (nullptr == m_pInstance)
//	{
//		m_pInstance = new CGarbageCollectorX<T>;
//	}
//	return m_pInstance;
//}
//
//template<typename T>
//unsigned long DestroyInstance(void)
//{
//	unsigned long dwRefCnt = 0;
//	if (nullptr != m_pInstance)
//	{
//		dwRefCnt = m_pInstance->Release();
//		if (0 == dwRefCnt)
//			m_pInstance = nullptr;
//	}
//	return dwRefCnt;
//}
#pragma endregion



template<typename T>
HRESULT CGarbageCollectorX<T>::Initialize()
{
	return S_OK;
}

template<typename T>
void CGarbageCollectorX<T>::Add_Garbage(const T& Garbage)
{
	m_GarbageList.push_back(Garbage);
}

template<typename T>
void CGarbageCollectorX<T>::Add_CurLevelGarbage(const T& Garbage)
{
	m_CurLevelGarbageList.emplace_back(Garbage);
}

template<typename T>
void CGarbageCollectorX<T>::Clear_CurLevelGarbage()
{
	for (T& pChar : m_CurLevelGarbageList)
		Safe_Delete_Array(pChar);
	m_CurLevelGarbageList.clear();
}

// @note - 인스턴스화를 허용할 타입 (원래 템플릿은 구현부도 헤더에 작성해야하는데, 허용 타입 제한할 때는 따로 분리하고 아래에 적어줌 
// Safe_Delete_Array이기 떄문에 기본 자료형의 배열만 가능함. (CGameObject를 상속받는 객체는 Safe_Release로 해줘야하기 때문)
// @note - Safe_Delete_Array는 포인터만 가능하기에 배열 삭제 용이면 짜피 ! 배열은 주소 넘기는 거나 다름없으니까 포인터로 선언해주기
template class CGarbageCollectorX<int*>;
template class CGarbageCollectorX<float*>;
template class CGarbageCollectorX<double*>;
template class CGarbageCollectorX<bool*>;
template class CGarbageCollectorX<_uint*>;
template class CGarbageCollectorX<char*>;
template class CGarbageCollectorX<_tchar*>;
