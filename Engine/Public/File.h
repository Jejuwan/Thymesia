#pragma once
#include "Base.h"

BEGIN(Engine)
class ENGINE_DLL CFile :
    public CBase
{
    DECLARE_SINGLETON(CFile)
public:
    CFile();
    ~CFile() = default;
public:
    void Initialize();
    virtual void Free() override;

    void SetPointer(streampos p)
    {
        g_Pointer = (int)p;
    }
    int GetPointer()
    {
        return g_Pointer;
    }

};
END

