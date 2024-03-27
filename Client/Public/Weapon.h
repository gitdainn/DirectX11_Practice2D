#pragma once

#include "Client_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CVIBuffer_Terrain;
class CTransform;
class CRenderer;
class CCollider;
class CTexture;
class CShader;
class CModel;
class CBone;
END

BEGIN(Client)

class CWeapon final : public CGameObject
{
public:
	enum COLLIDERTYPE { COLL_AABB, COLL_OBB, COLL_SPHERE, COLL_END };

public:
	typedef struct WeaponDesc
	{
		CBone*		pBonePtr = { nullptr };
		CTransform*	pParent = { nullptr };
		_float4x4	PivotMatrix;

	}WEAPONDESC;
private:
	CWeapon(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CWeapon(const CWeapon& rhs);
	virtual ~CWeapon() = default;

public:
	virtual HRESULT Initialize_Prototype() override; /* 원형객체의 초기화작업 */
	virtual HRESULT Initialize(void* pArg) override; /* 사본객체의 초기화작업 */
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

private:
	CTransform*			m_pTransformCom = { nullptr }; /* 부모기준의 상태표현. */
	CRenderer*			m_pRendererCom = { nullptr };	
	CShader*			m_pShaderCom = { nullptr };
	CModel*				m_pModelCom = { nullptr };
	CCollider*			m_pColliderCom[COLL_END] = { nullptr };

	WEAPONDESC			m_WeaponDesc;

	_float4x4			m_FinalWorldMatrix; /* 원점기준 (내 월드 * 부모월드) */



private:
	HRESULT Add_Components();
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 던진다. */
	_matrix Remove_Scale(_fmatrix Matrix);

public:	
	static CWeapon* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);	
	virtual CGameObject* Clone(void* pArg = nullptr) override;
	virtual void Free() override;
};

END