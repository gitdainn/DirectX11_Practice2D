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
	virtual HRESULT Initialize(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) override; /* 사본객체의 초기화작업 */
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Set_TextureTag(const _tchar* pTextureTag)
	{
		if (nullptr == pTextureTag)
			return;

		Safe_Delete_Array(m_pTextureTag);
		m_pTextureTag = pTextureTag;
	}

protected:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources() override; /* 셰이더 전역변수에 값을 던진다. */
	virtual void Add_Animation() override {};

public:
	virtual void OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget) override {};
	virtual void OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget) override {};
	virtual void OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget) override {};

private:
	const		_tchar* m_pTextureTag = { nullptr };

public:
	virtual void Free() override;
};

END