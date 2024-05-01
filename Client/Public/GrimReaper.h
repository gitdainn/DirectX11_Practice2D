#pragma once
#include "Player.h"

class CGrimReaper final: public CPlayer
{
public:
	explicit CGrimReaper(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CGrimReaper() {}

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	virtual void Add_Animation() override;

private:
	HRESULT Add_Components(void* pArg = nullptr);
	HRESULT SetUp_ShaderResources(); /* ���̴� ���������� ���� ������. */

public:
	/* Prototype */
	static CGrimReaper* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpriteObject* Clone(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) const;
	virtual void Free() override;
};
