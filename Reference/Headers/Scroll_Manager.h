#pragma once

#include "Base.h"

BEGIN(Engine)

class CScroll_Manager : public CBase
{
	DECLARE_SINGLETON(CScroll_Manager)

private:
	CScroll_Manager();
	~CScroll_Manager();

public:
	const _float&		Get_ScrollX(void) const { return m_fScrollX; }
	const _float&		Get_ScrollY(void) const { return m_fScrollY; }

	void		Set_ScrollX(const _float _fX) { m_fScrollX += _fX; }
	void		Set_ScrollY(const _float _fY) { m_fScrollY += _fY; }

	void		Scroll_Lock();
	void		Scroll_Clear();
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
	float					m_fScrollX = 0.f;
	float					m_fScrollY = 0.f;

public:
	virtual void Free() override;
};

END