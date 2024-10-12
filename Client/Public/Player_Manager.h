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
        if (nullptr == m_pSubSkul)
            return;
        CPlayer* pTemp = m_pMainSkul;
        m_pMainSkul = m_pSubSkul;
        m_pSubSkul = pTemp;

        m_pMainSkul->Get_TransformCom()->Set_State(CTransform::STATE_POSITION,
            m_pSubSkul->Get_TransformCom()->Get_State(CTransform::STATE_POSITION));

        m_pMainSkul->Set_IsEquipped(true);
        m_pSubSkul->Set_IsEquipped(false);
    };

    void    Acquire_Skul(CPlayer* pPlayer);
    void    Drop_Skul();
    void    Swap_Skul();

public: // Setter //
    void Set_MainSkul(CPlayer* Skul)
    {
        m_pMainSkul = Skul;
        m_pMainSkul->Set_IsEquipped(true);
    }

    void Set_SubSkul(CPlayer* Skul)
    {
        m_bIsMaxSkulEquipped = true;
        m_pSubSkul = Skul;
    }

public: // Getter //
    CPlayer* Get_MainSkul()
    {
        return m_pMainSkul;
    }
    CPlayer* Get_SubSkul()
    {
        return m_pSubSkul;
    }

    _bool   Get_IsMaxSkulEqquipedl()
    {
        return m_bIsMaxSkulEquipped;
    }

private:
    CPlayer* m_pMainSkul;
    CPlayer* m_pSubSkul;

private:
    _bool   m_bIsAcquireSkul;
    _bool   m_bIsMaxSkulEquipped;

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

