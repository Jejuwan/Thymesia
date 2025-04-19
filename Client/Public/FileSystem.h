#pragma once
#include "Base.h"
class CFileSystem :
    public CBase
{
    DECLARE_SINGLETON(CFileSystem)
public:
    CFileSystem();
    ~CFileSystem();

    void Save_Map(const wstring& strFilePath);
    void Load_Map(const wstring& strFilePath);
    void Load_Game(const wstring& strFilePath);
    virtual void Free() override;
};

