#pragma once
#include "Client_Defines.h"
#include "GameObject.h"

class CCommand abstract
{
public:
    explicit CCommand() {}
    ~CCommand() {}

public:

public:
    virtual void    Execute(CGameObject* pObject = nullptr) = 0;
};

