#pragma once

#include "Client_Defines.h"
#include "Camera.h"

BEGIN(Client)

class CCamera_Dynamic final : public CCamera
{
public:
	struct CAMERA_LIMITRANGE
	{
		_float2	vCenterPos = { 0.f, 0.f};
		_float2	vRadius = { 0.f, 0.f };
	};

	typedef struct tagCameraDynamicDesc
	{
		CCamera::CAMERADESC		tCameraDesc;
		CAMERA_LIMITRANGE		tCameraLimitRange;
	}CAMERA_DYNAMIC_DESC;

protected:
	CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Dynamic(const CCamera_Dynamic& rhs);
	virtual ~CCamera_Dynamic() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual HRESULT Late_Initialize(void* pArg) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	void	Follow_Owner();
	void	LimitCameraToArea();

private:
	CGameObject*				m_pOwner = { nullptr };
	CAMERA_DYNAMIC_DESC			m_tCameraDynamicDesc;

public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) const override;
	virtual void Free() override;
};

END