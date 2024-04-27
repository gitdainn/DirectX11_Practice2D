#pragma once

#include "GameObject.h"
#include "Transform.h"

/* 클라이언트에서 제작한 카메라의 부모 */

BEGIN(Engine)

class ENGINE_DLL CCamera abstract : public CGameObject
{
public:
	typedef struct CameraDesc
	{
		_float4		vEye, vAt, vAxisY;

		_float		fFovy, fAspect, fNear, fFar;

		CTransform::TRANSFORM_DESC		TransformDesc;
	}CAMERADESC;

protected:
	CCamera(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CCamera(const CCamera& rhs);
	virtual ~CCamera() = default;

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

protected:
	class CTransform*			m_pTransform = { nullptr };
	class CPipeLine*			m_pPipeLine = { nullptr };
	CAMERADESC					m_CameraDesc;

public:
	virtual CGameObject* Clone(const tSpriteInfo& SpriteInfo, void* pArg = nullptr) override;
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END