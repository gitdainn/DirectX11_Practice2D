#pragma once

#include "GameObject.h"

BEGIN(Engine)

class ENGINE_DLL CBlendObject abstract : public CGameObject
{
protected:
	CBlendObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CBlendObject(const CBlendObject& rhs);
	virtual ~CBlendObject() = default;

public:
	_float Get_ViewZ() const {
		return m_fViewZ;
	}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(void* pArg) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

protected:
	_float			m_fViewZ = { 0.0f };

protected:
	void Compute_ViewZ(_fvector vPosition);

public:
	virtual CGameObject* Clone(const tSpriteInfo& SpriteInfo, void* pArg = nullptr) override;
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};
END