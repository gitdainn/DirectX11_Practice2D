#pragma once

#include "Engine_Defines.h"

/* ���۷���ī��Ʈ�� �����Ѵ�. */
/* ���۷���ī��Ʈ : ���� ���� */

BEGIN(Engine)

class ENGINE_DLL CBase abstract
{
protected:
	explicit CBase(void);
	virtual ~CBase(void) = default;

public:
	/* ���۷��� ī��Ʈ�� ������Ų��. */
	unsigned long	AddRef(void);

	/* ���۷��� ī��Ʈ�� ���ҽ�Ų��. or �����Ѵ�. */
	/* ���ҽ�Ű�� ������ ���� �����Ѵ�. */
	unsigned long	Release(void);

private:
	unsigned long			m_dwRefCnt = {0};

public:
	inline virtual void	Free(void) = 0;

};

END