#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Command.h"

class CInputHandler : public CBase
{
    DECLARE_SINGLETON(CInputHandler)

private:
    explicit CInputHandler();
    ~CInputHandler();

private:
    enum class CONTROL_KEY { DOWN, LEFT, RIGHT, MENU, DASH, INTERACTION, ATTACK, JUMP, SKILL1, SKILL2, ESSENCE, ROTATION };

public:
    CCommand* Key_Input() const;
    
private:
    /** @qurious - map 정의 시 const 포인터는 가능하나 const int 같이 타입 상수는 삭제됐다고 안된다고 함 */
    unordered_map<CONTROL_KEY, CCommand*> m_CommandKeyMap;

public:
    virtual void Free() override;
};

