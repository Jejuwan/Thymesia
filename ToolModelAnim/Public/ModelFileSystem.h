#pragma once
#include "Base.h"
class CModelFileSystem :
    public CBase
{
    DECLARE_SINGLETON(CModelFileSystem)
public:
    CModelFileSystem();
    ~CModelFileSystem();

    void Initialize();

    void Save_Model(const wstring& strFilePath);
    void Load_Model(const wstring& strFilePath);

    virtual void Free() override;
};

