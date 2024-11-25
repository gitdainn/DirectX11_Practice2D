#pragma once
#include "Base.h"
#include "Player.h"

/** @qurious - 싱글톤에 final 붙이면 안된다? */
class CStage_Manager : public CBase
{
    DECLARE_SINGLETON(CStage_Manager)

public:
    explicit CStage_Manager();
    ~CStage_Manager();

public:
    _uint   Tick(_double TimeDelta);
    /** 스테이지 입장 시 호출해주세요. */
    void    Enter(const ROOM_TYPE& eRoomType, const LEVEL& eLevel);
    /** 클리어 조건 달성 시 호출해주세요. 보상을 제공합니다. */
    void Give_ClearReward();

public: // inline //
    /** 적 생성 시 호출해주세요. */
    void    IncreaseEnemyCount() { ++m_iEnemyCount; }
    /* 적 사망 시 호출해주세요. */
    void    DecreaseEnemyCount() { if (0 == m_iEnemyCount) return; --m_iEnemyCount; }

public: // Setter //

public: // Getter //
    const _bool     Get_IsClear() const { return 0 == m_iEnemyCount; } // 적이 0이면 Clear
    const ROOM_TYPE& Get_RoomType() const { return m_eRoomType; }

#ifdef _DEBUG
    const _uint     Get_EnemyCount() const { return m_iEnemyCount; } // 적이 0이면 Clear
#endif

private:
    void    Give_Skul();
    void    Give_Essence();
    void    Give_Item();
    void    Give_Coin();

private:
    void    Initialize_Member();
    HRESULT Activate_Door();

private:
    list<CGameObject*>  m_RewardList;

private:
    ROOM_TYPE   m_eRoomType = ROOM_TYPE::ROOM_END;
    LEVEL       m_eLevel = LEVEL::LEVEL_END;

    _bool   m_bHasGivenReward = { false };
    _uint   m_iEnemyCount = { 0 };


public:
    void Free() override;
};

