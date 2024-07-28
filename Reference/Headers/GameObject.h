#pragma once

#include "Base.h"
#include "Transform.h"
#include "Renderer.h"
#include "VIBuffer_Rect.h"

BEGIN(Engine)

class CComponent;
class CCollider;

class ENGINE_DLL CGameObject abstract : public CBase
{
protected:
	CGameObject(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CGameObject(const CGameObject& rhs);
	virtual ~CGameObject() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual _uint Tick(_double TimeDelta);
	virtual _uint LateTick(_double TimeDelta);
	virtual HRESULT Render();

public:
	class CComponent* Get_Component(const _tchar* pComponentTag) {
		return Find_Component(pComponentTag);
	}

	CTransform* Get_TransformCom() const {
		return m_pTransformCom;
	};

	const int& Get_ObjectID() const
	{
		return m_ID;
	}

	const _uint& Get_Order() const
	{
		return m_tSpriteInfo.iOrder;
	}

	unordered_map<const _tchar*, CComponent*>* Get_Components()
	{
		if (m_Components.empty())
		{
			return nullptr;
		}

		return &m_Components;
	}

public:
	void Set_Order(const _uint& iOrder)
	{
		m_tSpriteInfo.iOrder = iOrder;
	}

	void Set_ShaderPass(const _uint iPassIndex)
	{
		m_iShaderPassIndex = iPassIndex;
	}

	void Set_RenderGroup(const CRenderer::RENDERGROUP& eRenderGroup)
	{
		m_eRenderGroup = eRenderGroup;
	}

public:
	HRESULT Add_Components(_uint iLevelIndex, const _tchar* pPrototypeTag,
		const _tchar* pComponentTag, class CComponent** ppOut, void* pArg = nullptr);

protected:
	class CComponent* Find_Component(const _tchar* pComponentTag);
	HRESULT Change_Component(_uint iLevelIndex, const _tchar* pPrototypeTag, const _tchar* pComponentTag, CComponent** ppOut, void* pArg = nullptr);

	// 파일 읽어서 Add_Component하는 함수 //
	// CComponent* 를 넘겨서 null이면 FAIL인거고, 일단 각 오브젝트의 m_Components의 컨테이너에 다 넣는 것! //
	// 그리고 각자 Initialize에서 find 키로 할당해주기1 //

protected:
	ID3D11Device*			m_pDevice = { nullptr };
	ID3D11DeviceContext*	m_pContext = { nullptr };

protected:
	CTransform* m_pTransformCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };

protected:
	/* 해시테이블 */
	unordered_map<const _tchar*, CComponent*>			m_Components;
	SPRITE_INFO m_tSpriteInfo;

protected:
	static int g_iObjectID;
	int		m_ID;
	_bool	m_bIsDead;
	_bool	m_bIsRender;

	CRenderer::RENDERGROUP	m_eRenderGroup;
	_uint	m_iShaderPassIndex = { 0 };
	_uint	m_iTextureIndex = { 0 };
	// m_WorldMatrix는 CTransform에서 사용 중이기에 따로 사용하면 안된다.
	_float4x4	m_ViewMatrix; 
	_float4x4	m_ProjMatrix;

public:			
	virtual CGameObject* Clone(const tSpriteInfo& SpriteInfo, void* pArg = nullptr) const = 0;
	virtual CGameObject* Clone(void* pArg = nullptr) const = 0;
	virtual void Free() override;
};

END