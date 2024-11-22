#pragma once

#include "Tool_Defines.h"
#include "Camera.h"

BEGIN(Tool)

class CCamera_Dynamic final : public CCamera
{
public:
	typedef struct tagCameraDynamicDesc
	{
		CCamera::CAMERADESC		tCameraDesc;
	}CAMERA_DYNAMIC_DESC;

protected:
	CCamera_Dynamic(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera_Dynamic(const CCamera_Dynamic& rhs);
	virtual ~CCamera_Dynamic() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;
private:
	CAMERA_DYNAMIC_DESC			m_tCameraDynamicDesc;

public:
	static CCamera_Dynamic* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg = nullptr) const override;
	virtual void Free() override;
};

END