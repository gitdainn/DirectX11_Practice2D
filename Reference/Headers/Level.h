#pragma once

#include "Base.h"

/* Ŭ���̾�Ʈ�� ������ �������� �θ�Ŭ������. */
/* �������� �������� ����� �ο��Ѵ�. */
/* �������� ������ �����ϰ����ϴ°�� Ÿ��(�θ�Ÿ��CLevel)�� �������ش�. */
/* �������� Ŭ�� ���ǵ� Ŭ������ ����Լ��� ȣ���� ������������ �Ѵ�.(�Լ� �������̵�) */

BEGIN(Engine)

class ENGINE_DLL CLevel abstract : public CBase
{
protected:
	CLevel(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel() = default;

public:
	virtual HRESULT Initialize();
	virtual void Tick(_double TimeDelta);

protected:
	ID3D11Device*				m_pDevice = { nullptr };
	ID3D11DeviceContext*		m_pContext = { nullptr };

public:
	virtual void Free() override;
};

END