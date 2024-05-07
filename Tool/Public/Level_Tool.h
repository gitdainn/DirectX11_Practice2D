#pragma once

#include "Tool_Defines.h"
#include "Level.h"

BEGIN(Tool)

class CLevel_Tool final : public CLevel
{
private:
	CLevel_Tool(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CLevel_Tool() = default;

public:
	virtual HRESULT Initialize() override;
	virtual void Tick(_double TimeDelta) override;

private: /* 이 레벨이 사용하기위한 사본 객체들을 그룹별로 생성한다. */
	HRESULT Ready_Layer_GameObject();

public:
	static CLevel_Tool* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual void Free() override;

};

END