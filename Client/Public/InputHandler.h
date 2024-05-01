#pragma once
#include "Client_Defines.h"
#include "Base.h"
#include "Command.h"
#include "GameObject.h"

class CInputHandler : public CBase
{
    DECLARE_SINGLETON(CInputHandler)

private:
    explicit CInputHandler();
    ~CInputHandler();

public:
    CCommand* Key_Input(CGameObject** OutObject) const;
    
private:
    /** @qurious - map 정의 시 const 포인터는 가능하나 const int 같이 타입 상수는 삭제됐다고 안된다고 함 */
    /** @note - 템플릿을 컨테이너 원소로 보관하는 경우 CCommand<T>는 안되고, 들어갈 자료형을 명시해야지만 가능함 */
    unordered_map<CONTROL_KEY, CCommand*> m_CommandKeyMap;

public:
    virtual void Free() override;
};

