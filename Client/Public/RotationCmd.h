#pragma once
#include "Command.h"

class CRotationCmd : public CCommand
{
public:
    explicit CRotationCmd() {}
    ~CRotationCmd() {}

public:
    virtual void Execute(CGameObject* pObject) override;
};

