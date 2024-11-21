#pragma once

#include "Base.h"

BEGIN(Engine)

class CGameObject;

class CScroll_Manager : public CBase
{
	DECLARE_SINGLETON(CScroll_Manager)

private:
	CScroll_Manager();
	~CScroll_Manager();

public:
	const _float		Get_ScrollX(void) const { return m_fScroll.x; }
	const _float		Get_ScrollY(void) const { return m_fScroll.y; }

	void		Set_Scroll(const _float2 fScroll) { m_fScroll.x += fScroll.x; m_fScroll.y += fScroll.y;  Notify(fScroll); }

	void		Scroll_Lock();
	void		Scroll_Clear();

	void		Add_ScrollListener(CGameObject* pObject);

public:
	static CScroll_Manager* Get_Instance(void)
	{
		if (nullptr == m_pInstance)
			m_pInstance = new CScroll_Manager;

		return m_pInstance;
	}
	void		Destory_Instance(void)
	{
		if (m_pInstance)
		{
			delete m_pInstance;
			m_pInstance = nullptr;
		}
	}

private:
	/** Set_Scroll 시 호출되어 스크롤이 적용될 오브젝트들의 위치를 갱신합니다. */
	void		Notify(const _float2 fScroll);

private:
	list<CGameObject*>		m_ScrollList; // <객체, 스크롤속도>

private:
	_float2					m_fScroll = { 0.f, 0.f };

public:
	virtual void Free() override;
};

END