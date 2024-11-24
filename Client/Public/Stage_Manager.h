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
    /** 스테이지 입장 시 호출해주세요. */
    void    Enter(const ROOM_TYPE& eRoomType);

public: // inline //
    /** 적 생성 시 호출해주세요. */
    void    IncreaseEnemyCount() { ++m_iEnemyCount; }
    /* 적 사망 시 호출해주세요. */
    void    DecreaseEnemyCount() { if (0 == m_iEnemyCount) return; --m_iEnemyCount; }

public: // Setter //

public: // Getter //
    const _bool     Get_IsClear() const { return 0 == m_iEnemyCount; } // 적이 0이면 Clear
    const _uint     Get_EnemyCount() const { return m_iEnemyCount; } // 적이 0이면 Clear


private:
    _uint   m_iEnemyCount = { 0 };

public:
    void Free() override;
};

