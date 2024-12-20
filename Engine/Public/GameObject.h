#pragma once

#include "Base.h"
#include "Transform.h"
#include "Renderer.h"
#include "VIBuffer_Rect.h"

BEGIN(Engine)

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	class CComponent* Get_Component(const _tchar* pComponentTag) {
		return Find_Component(pComponentTag);
	}

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_double TimeDelta);
	virtual _uint LateTick(_double TimeDelta);
	virtual HRESULT Render();

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

protected:
	CTransform* m_pTransformCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };

protected:
	/* 해시테이블 */
	unordered_map<const _tchar*, class CComponent*>			m_Components;

protected:
	HRESULT Add_Components(_uint iLevelIndex, const _tchar* pPrototypeTag, 
		const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);
	class CComponent* Find_Component(const _tchar* pComponentTag);

public:			
	virtual CGameObject* Clone(const tSpriteInfo& SpriteInfo, void* pArg = nullptr) = 0;
	virtual CGameObject* Clone(void* pArg = nullptr) = 0;
	virtual void Free() override;
};

END