#pragma once
#include "Base.h"

BEGIN(Engine)

class CGarbageCollectorBase abstract : public CBase
{
public:
    CGarbageCollectorBase() : CBase() {}
    ~CGarbageCollectorBase() {}

public:
    virtual void Free() = 0;

};

END
