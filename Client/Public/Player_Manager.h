#pragma once
#include "Base.h"
#include "Player.h"

/** @qurious - 싱글톤에 final 붙이면 안된다? */
class CPlayer_Manager : public CBase
{
    DECLARE_SINGLETON(CPlayer_Manager)

public:
    explicit CPlayer_Manager();
    ~CPlayer_Manager();

public:
    void Rotation_Skul()
    {
        if (nullptr == m_pHoldingSkul)
            return;
        CPlayer* pTemp = m_pEquippedSkul;
        m_pEquippedSkul = m_pHoldingSkul;
        m_pHoldingSkul = pTemp;

        m_pEquippedSkul->Get_TransformCom()->Set_State(CTransform::STATE_POSITION,
            m_pHoldingSkul->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

        m_pEquippedSkul->Set_IsEquipped(true);
        m_pHoldingSkul->Set_IsEquipped(false);
    };

public: // Setter //
    void Set_EquippedSkul(CPlayer* Skul)
    {
        m_pEquippedSkul = Skul;
        m_pEquippedSkul->Set_IsEquipped(true);
    }

    void Set_HoldingSkul(CPlayer* Skul)
    {
        m_pHoldingSkul = Skul;
    }

public: // Getter //
    CPlayer* Get_EquippedSkul()
    {
        return m_pEquippedSkul;
    }

private:
    CPlayer* m_pEquippedSkul;
    CPlayer* m_pHoldingSkul;

private:
    _int m_iHp;
    _int m_iMagicAttack;
    _int m_iPhysicalAttack;
    _int m_iMagicDefense;
    _int m_iPhysicalDefense;
    _float m_fMovementSpeed;
    _double m_InvulnerabilityDuration;

    _int iMagicAttackIncrease;
    _int iPhysicalAttackIncrease;
    _int iMagicDefenseIncrease;
    _int iPhysicalDefenseIncrease;

public:
    void Free() override;
};

