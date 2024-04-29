#pragma once
#include "SpriteObject.h"

class CPlayer : public CSpriteObject
{
public:
	explicit CPlayer(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	~CPlayer() {}

public:
	enum class PLAYER_MOTION { IDLE, WALK, DASH, ATK, JUMP_ATK, SKILL1, SKILL2, FALL, MOTION_END };
	union MOTION
	{
		PLAYER_MOTION Right;
		PLAYER_MOTION Left;
	};

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_IsEquipped(const bool bIsEquipped)
	{
		m_bIsEquipped = bIsEquipped;
	}

protected:
	virtual void Add_Animation() = 0;
	virtual void Key_Input(_double TImeDelta) = 0;
	
protected:
	HRESULT Add_Components(void* pArg = nullptr);
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 던진다. */

protected:
	//unordered_map<const CONTROL_KEY, _uint>	m_ControlMap;
	_tchar* m_pTextureTag;
	_uint	m_iUVTextureIndex;
	bool	m_bIsEquipped;

public:
	/* Prototype */
	virtual CSpriteObject* Clone(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) const = 0;
	virtual void Free() override;

};

