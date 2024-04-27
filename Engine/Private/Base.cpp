#include "..\Public\Base.h"
#include "Renderer.h"

CBase::CBase(void)
{	
	
}

unsigned long CBase::AddRef(void)
{
	if (nullptr != dynamic_cast<CRenderer*>(this))
	{
		int i = 0;
	}
	return ++m_dwRefCnt;	
}

unsigned long CBase::Release(void)
{
	if (0 == m_dwRefCnt)
	{
		Free();

		/*삭제한다. */
		delete this;

		return 0;
	}

	return m_dwRefCnt--;
}

inline void CBase::Free(void)
{
}
