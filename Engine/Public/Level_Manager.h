#pragma once

#include "Base.h"

/* 현재 클라이언트에서 화면에 보여줘야할 레벨을 보관하고 구동한다.(Tick를 호출한다) */

BEGIN(Engine)

class CLevel_Manager final : public CBase
{
	DECLARE_SINGLETON(CLevel_Manager)
private:
	CLevel_Manager();
	virtual ~CLevel_Manager() = default;

public:
	/* SetUp_Level */
	HRESULT Open_Level(_uint iLevelIndex, class CLevel* pNewLevel);
	void Tick_Level(_double TimeDelta);

private:
	class CLevel*				m_pCurrentLevel = { nullptr };
	_uint						m_iLevelIndex = { 0 };

public:
	virtual void Free() override;
};

END