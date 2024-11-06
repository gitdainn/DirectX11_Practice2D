#pragma once

#include "Client_Defines.h"
#include "UI.h"

BEGIN(Client)

class CSkillUI : public CUI
{
private:
	CSkillUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSkillUI(const CSkillUI& rhs);
	virtual ~CSkillUI() = default;

public:
	virtual HRESULT Initialize_Prototype() override; /* 원형객체의 초기화작업 */
	virtual HRESULT Initialize(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) override; /* 사본객체의 초기화작업 */
	virtual HRESULT Initialize(void* pArg = nullptr) override;
	virtual _uint Tick(_double TimeDelta) override;
	virtual _uint LateTick(_double TimeDelta) override;
	virtual HRESULT Render() override;

public:
	void	Add_Child(CUI* pChild)
	{
		if (nullptr == pChild)
			return;

		m_ChildList.emplace_back(pChild);
	}

private:
	virtual HRESULT Add_Components(void* pArg = nullptr) override;
	virtual HRESULT SetUp_ShaderResources() override; /* 셰이더 전역변수에 값을 던진다. */
	virtual void Add_Animation() override {};

private:
	void	onEvent()
	{
		if (m_ChildList.empty())
			return;

		for (CUI* pUI : m_ChildList)
		{
			if (nullptr == pUI)
				continue;

			CTransform* pChildTransform = pUI->Get_TransformCom();
			if (nullptr == pChildTransform)
				continue;

			pChildTransform->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}
	}

public:
	virtual void OnCollisionEnter(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override {};
	virtual void OnCollisionStay(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override {};
	virtual void OnCollisionExit(CCollider* pTargetCollider, CGameObject* pTarget, const _tchar* pTargetLayer) override {};

private:
	list<CUI*>			m_ChildList;

public:
	/* 사본 객체를 생성한다. */
	static  CSkillUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CSpriteObject* Clone(const SPRITE_INFO& InSpriteInfo, void* pArg = nullptr) const;
	virtual CSpriteObject* Clone(void* pArg = nullptr) const;
	virtual void Free() override;
};

END