#include "..\Public\Base.h"

CBase::CBase(void)
{	
	
}

unsigned long CBase::AddRef(void)
{
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
	else
	{
		return m_dwRefCnt--;
	}
	

		

	return m_dwRefCnt--;
}

inline void CBase::Free(void)
{
}
