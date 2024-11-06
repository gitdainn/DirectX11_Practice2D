#pragma once
#include "Widget.h"

/** 툴에서 만든 것을 Load하지 않고, 클라에서 체력바를 바로 만들기 위한 CWidget 컴포넌트 클래스입니다. */
class CHealthBarWidget : public CWidget
{
public:
	explicit CHealthBarWidget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CHealthBarWidget(const CHealthBarWidget& rhs);
	~CHealthBarWidget() = default;

public:
	HRESULT Initialize_Prototype();
	HRESULT Initialize(void* pArg) override;

private:
	/* 몬스터 생성 시 추가해야 하는 체력바입니다. */
	HRESULT    Attach_HealthBar(CGameObject* pObject);

public:
	static CHealthBarWidget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CComponent* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

