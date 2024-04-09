#pragma once
#include "SpriteObject.h"

class CPlayer : public CSpriteObject
{
public:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CPlayer() {}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	/* Prototype */
	static CPlayer* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpriteObject* Clone(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr);
	virtual void Free() override;

};

