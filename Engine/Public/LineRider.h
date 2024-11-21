#pragma once
#include "Component.h"

BEGIN(Engine)

class CLine_Manager;

class ENGINE_DLL CLineRider final : public CComponent
{
private:
	CLineRider(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CLineRider(const CLineRider& rhs);
	virtual ~CLineRider() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	/** @param - 선을 타는 대상의 위치 정보(XMVECTOR 타입)를 넣어주세요. */
	virtual HRESULT Initialize(void* pArg) override;

public:
	HRESULT	Collision_Line(_vector vPosition, _float& fOutLandingY);
	const LINE_INFO* Get_LandingLine() { return m_pClosestLandingLine; }

	/** 라인에 착지하는 시점을 알려줍니다. 
	* @warning - 점프 시 추락 시점부터 사용해주세요. 점프 직후부터 사용하면 내리막길에서는 바로 땅에 붙어버립니다. */
	_bool	CheckLineLanding(const _vector vPosition, _float& fOutLandingY);

private:
	CLine_Manager* m_pLine_Manager = { nullptr };

private:
	// 포인터로 가르키도록 하여 Line_Manager의 Scroll로 인해 변동되는 라인값을 실시간 받게하도록 함.
	LINE_INFO* m_pClosestLandingLine = { nullptr };

public:
	static CLineRider* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END