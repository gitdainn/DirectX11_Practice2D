#pragma once
#include "Base.h"
#include "Player.h"

/** @qurious - 싱글톤에 final 붙이면 안된다? */
class CPlayerInfo : public CBase
{
    DECLARE_SINGLETON(CPlayerInfo)

public:
    explicit CPlayerInfo();
    ~CPlayerInfo();

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

    void Set_EquippedSkul(CPlayer* Skul)
    {
        m_pEquippedSkul = Skul;
        m_pEquippedSkul->Set_IsEquipped(true);
    }

    void Set_HoldingSkul(CPlayer* Skul)
    {
        m_pHoldingSkul = Skul;
    }

private:
    CPlayer* m_pEquippedSkul;
    CPlayer* m_pHoldingSkul;

public:
    void Free() override;
};

