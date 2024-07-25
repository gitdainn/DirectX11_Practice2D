#pragma once
#include "Base.h"

BEGIN(Engine)

template <typename T>
class ENGINE_DLL CGarbageCollectorX final : public CBase
{
public:
	CGarbageCollectorX()
		: CGarbageCollectorXBase()
	{}
	virtual ~CGarbageCollectorX()
	{
		__super::Free();

		Clear_CurLevelGarbage();

		for (auto& pGarbage : m_GarbageList)
			Safe_Delete_Array(pGarbage);
		m_GarbageList.clear();
	}

public:
	HRESULT Initialize();

public:
	// 게임이 끝날 때 삭제될 메모리
	void	Add_Garbage(const T& Garbage);

public:
	// 레벨이 바뀔 때 삭제될 메모리
	void	Add_CurLevelGarbage(const T& Garbage);

public:
	void	Clear_CurLevelGarbage();

private:
	list<T>			m_GarbageList;
	list<T>			m_CurLevelGarbageList;

public:
	virtual void Free() override {}
};

END