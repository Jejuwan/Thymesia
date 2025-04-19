#pragma once
#include "Engine_Pch.h"
#include "Base.h"

BEGIN(Engine)

class CSound_Manager final :
    public CBase
{
    DECLARE_SINGLETON(CSound_Manager)

private:
    CSound_Manager();
    ~CSound_Manager();

public:
    HRESULT Initialize();
    void Tick(_float fTimeDelta);
    void Free() override;

public:
    void PlaySound(TCHAR* pSoundKey, CHANNELID eID, float fVolume);
    void PlayBGM(TCHAR* pSoundKey, CHANNELID eID, float fVolume);
    void StopBGM(CHANNELID eID);
    void StopSound(CHANNELID eID);
    void StopAll();
    void SetChannelVolume(CHANNELID eID, float fVolume);
    _bool SetBGM(void* pArg);
    _bool IsPlaying(CHANNELID eID);
    _bool PlayBGM_Event(void* pArg);
    _bool StopBGM_Event(void* pArg);
    _bool Set_BGM_Mute(void* pArg);

private:
    void LoadSoundFile();
    void LoadSoundFile_Loop();

private:

    // 사운드 리소스 정보를 갖는 객체 
    map<TCHAR*, Sound*> m_mapSound;

    // FMOD_CHANNEL : 재생하고 있는 사운드를 관리할 객체 
    Channel* m_pChannelArr[MAXCHANNEL];

    // 사운드 ,채널 객체 및 장치를 관리하는 객체 
    FMOD::System* m_pSystem = { nullptr };


    _float m_fFadeTime;

    _float m_fBGMVolumes[8] = { -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f, -1.f };

    _bool m_bPlayerDead;

};

END