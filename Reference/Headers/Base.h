#pragma once

#include "Engine_Defines.h"

/* 레퍼런스카운트를 관리한다. */
/* 레퍼런스카운트 : 참조 개수 */

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	explicit CBase(void);
	virtual ~CBase(void) = default;

public:
	/* 레퍼런스 카운트를 증가시킨다. */
	unsigned long	AddRef(void);

	/* 레퍼런스 카운트를 감소시킨다. or 삭제한다. */
	/* 감소시키기 이전의 값을 리턴한다. */
	unsigned long	Release(void);

private:
	unsigned long			m_dwRefCnt = {0};

public:
	inline virtual void	Free(void) = 0;

};

END