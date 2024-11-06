#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CRenderer;
class CTexture;
class CShader;
END

BEGIN(Client)

class CHealthBarUI : public CUI
{
protected:
	CHealthBarUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CHealthBarUI(const CHealthBarUI& rhs);
	virtual ~CHealthBarUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override; 
	virtual HRESULT Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) override; 
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_Hp(_int iHp)
	{
		m_fNormalizedHp = _float(iHp) / _float(m_iMaxHp);
	}

protected:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources() override; 

private:
	_int		m_iMaxHp = { 100 };
	_float		m_fNormalizedHp = { 1.f };

public:
	/* 사본 객체를 생성한다. */
	static  CHealthBarUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpriteObject* Clone(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) const;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const;
	virtual void Free() override;
};

END