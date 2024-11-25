#pragma once
#include "Base.h"
#include "Player.h"

class CUI_Handler;

/** @qurious - 싱글톤에 final 붙이면 안된다? */
class CPlayer_Manager : public CBase
{
    DECLARE_SINGLETON(CPlayer_Manager)

public:
    explicit CPlayer_Manager();
    ~CPlayer_Manager();

private:
    struct BASE_STATS
    {
        _int iMagicAttack = { 60 };
        _int iHp = { 100 };
        _int iDefense = { 1 };
        _int iPhysicalAttack = { 10 };
        _int iMaxJumpCount = { 2 };

        _float fMovementSpeed = { 10 };
        _float fAttackSpeed = { 10 };
        _float fReduceCoolDownSpeed = { 0 };
        _float fCriticalHit;

        _int iMagicAttackIncrease = { 0 };
        _int iPhysicalAttackIncrease = { 0 };
        _int iDefenseIncrease = { 0 };

        _bool	bIsInvulnerable = { false };
        _double InvulnerabilityDuration = { 0.0 };
    };

public:
    void    Swap_Skul();

    void    Acquire_Skul(CPlayer* pPlayer);
    void    Drop_Skul();

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

    _bool   Get_IsMaxSkulEqquiped()
    {
        return m_bIsMaxSkulEquipped;
    }

public: // State //
    void    Set_Damaged(const _int iDamaged);

private:
    CPlayer* m_pMainSkul = { nullptr };
    CPlayer* m_pSubSkul = { nullptr };
    CUI_Handler* m_pUI_Handler = { nullptr };

private:
    _bool   m_bIsAcquireSkul = { false };
    _bool   m_bIsMaxSkulEquipped = { false };

private:
    BASE_STATS  m_tBaseStats;

public:
    void Free() override;
};

