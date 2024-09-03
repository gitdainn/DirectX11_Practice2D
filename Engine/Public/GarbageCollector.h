#pragma once
#include "Engine_Defines.h"
#include "Base.h"

BEGIN(Engine)

class CGarbageCollector final : public CBase
{
	DECLARE_SINGLETON(CGarbageCollector)
private:
	CGarbageCollector();
	virtual ~CGarbageCollector() = default;

public:
	HRESULT Initialize();

public:
	// 게임이 끝날 때 삭제될 문자열
	void	Add_Garbage(char* pChar);
	// 게임이 끝날 때 삭제될 문자열
	void	Add_Garbage(_tchar* pTChar);
	void	Add_Garbage(const _tchar* pTChar);

public:
	// 레벨이 바뀔 때 삭제될 문자열
	void	Add_CurLevelGarbage(char* pChar);
	// 레벨이 바뀔 때 삭제될 문자열
	void	Add_CurLevelGarbage(_tchar* pTChar);

public:
	void	Clear_CurLevelGarbage();

private:
	list<char*>		m_CharList;
	list<_tchar*>	m_TCharList;
	list<const _tchar*>	m_ConstTCharList;

private:
	list<char*>		m_CurLevelCharList;
	list<_tchar*>	m_CurLevelTCharList;

public:
	virtual void Free() override;
};

END