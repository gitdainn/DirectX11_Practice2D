#pragma once

#include "Client_Defines.h"
#include "Level.h"

/* 다음레벨을 준비하기위한 CLoader를 생성한다. */
/* 로더의 역활이 끝나면 실제 할당하기위한 다음레벨로 넘긴다. */
/* 로딩레벨에서 보여줘야할 객체를 생성하고 갱신, 렌더를 수행한다. */
BEGIN(Client)

class CLevel_Loading final : public CLevel
{
private:
	CLevel_Loading(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Loading() = default;

public:
	virtual HRESULT Initialize(LEVEL eNextLevelID);
	virtual void Tick(_double TimeDelta) override;

private:
	LEVEL			m_eNextLevelID = { LEVEL_END };
	class CLoader*	m_pLoader = { nullptr };

private:
	HRESULT Ready_Layer_BackGround();
	HRESULT Ready_Layer_UI();

public:
	static CLevel_Loading* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, LEVEL eNextLevelID);
	virtual void Free() override;

};

END