#pragma once
#include "SpriteObject.h"
#include "Player_Manager.h"
#include <functional>


class CEnemy abstract : public CSpriteObject
{
public:
	explicit CEnemy(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	explicit CEnemy(const CEnemy& rhs);
	~CEnemy() {}

protected:
	enum ENEMY_STATE
	{
		IDLE, WALK, CHASE, ATK1, ATK2, DAMAGED, ENEMY_END
	};

public:
	virtual HRESULT Initialize_Prototype() override;
	virtual HRESULT Initialize(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) override;
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void			Input_Handler(const ENEMY_STATE eEnemyState);
	void			Execute_Skill(_uint iSkillIndex);

public:
	virtual void OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget) override;
	virtual void OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget) override;
	virtual void OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget) override;

protected:
	virtual void Idle(_double TimeDelta);
	virtual void Walk(_double TimeDelta);
	virtual void Attack(_double TimeDelta);
	virtual void Chase(_double TimeDelta);
	virtual void Damaged(_double TimeDelta);

protected:
	virtual void End_Animation(_uint& iSpriteIndex) override;
	//void	Mapping_SkulData(const _tchar* pObjectID);
	//CSkill* Get_Skill(const _tchar* pObjectID);
	//void Awaken();
	virtual void Add_Animation() = 0;

protected:
	CTransform* Get_PlayerTransformCom() const
	{
		CPlayer_Manager* pPlayer_Manager = CPlayer_Manager::GetInstance();
		if (nullptr == pPlayer_Manager)
			return nullptr;
		Safe_AddRef(pPlayer_Manager);

		CPlayer* pPlayer = pPlayer_Manager->Get_MainSkul();
		if (nullptr == pPlayer)
		{
			Safe_Release(pPlayer_Manager);
			return nullptr;
		}

		Safe_Release(pPlayer_Manager);

		return pPlayer->Get_TransformCom();
	}

	const _float Get_DistanceX(const _vector vCurrentPos, const _vector vDirectionPos) const
	{
		_vector vCurrentPosX = XMVectorSetY(vCurrentPos, 0.f);
		_vector vDirectionPosX = XMVectorSetY(vDirectionPos, 0.f);
		return XMVectorGetX(XMVector3Length(vDirectionPosX - vCurrentPosX));
	}
	void	Landing_Ground();

protected:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	HRESULT SetUp_ShaderResources(); /* 셰이더 전역변수에 값을 던진다. */

protected:
	CCollider* m_pDefaultAtkColliderCom = { nullptr };

protected:
	// 멤버 함수를 담을 땐 첫 번째 인자로 자기자신 넘기기
	// @note - function<반환타입(매개변수)>
	unordered_map<ENEMY_STATE, function<void(CEnemy&, _double)>> m_StateFunctionMap;
	function<void(CEnemy&, _double)> m_StateFunc;

protected:
	pair<_float3, _float3>	m_LineEndPoints;

	_uint	m_iSearchRoundsCount = { 0 };

	_double	m_IdleTimeAcc = { 0.0 };

public:
	/* Prototype */
	virtual CSpriteObject* Clone(const tSpriteInfo& InSpriteInfo, void* pArg = nullptr) const = 0;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const = 0;
	virtual void Free() override;

};

