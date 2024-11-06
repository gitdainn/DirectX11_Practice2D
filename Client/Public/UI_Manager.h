#pragma once
#include "Base.h"
#include "UI.h"
#include "Player_Manager.h"

/** @qurious - 싱글톤에 final 붙이면 안된다? */
class CUI_Manager : public CBase
{
    DECLARE_SINGLETON(CUI_Manager)

public:
    explicit CUI_Manager();
    ~CUI_Manager();

public:

public:
    void Free() override;
};

