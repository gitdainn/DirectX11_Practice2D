#pragma once

#include "Client_Defines.h"
#include "SpriteObject.h"

BEGIN(Engine)
class CVIBuffer_Rect;
class CRenderer;
class CTexture;
class CShader;
END

BEGIN(Client)

class CUI abstract : public CSpriteObject
{
protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override; /* 원형객체의 초기화작업 */
	virtual HRESULT Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) override; /* 사본객체의 초기화작업 */
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_Owner(CGameObject* pOwner) { m_pOwner = pOwner; }
	void	Set_Offset(const _float2 fOffset) { m_fOffset = fOffset; }

protected:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources() = 0;

public:
	virtual void OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override {};
	virtual void OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override {};
	virtual void OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override {};

private:
	CGameObject* m_pOwner = { nullptr };
	_float2		m_fOffset = { 0.f, 0.f };

public:
	virtual void Free() override;
};

END