#include "Engine_Pch.h"
#include "..\Public\SoundManager.h"
#include "GameInstance.h"

IMPLEMENT_SINGLETON(CSound_Manager)

CSound_Manager::CSound_Manager()
{
}

CSound_Manager::~CSound_Manager()
{
	Free();
}

HRESULT CSound_Manager::Initialize()
{
	FMOD_RESULT result;

	result = FMOD::System_Create(&m_pSystem);      // Create the main system object.
	if (result != FMOD_OK)
	{
		MSG_BOX("FMOD::System_Create Fail");
		return E_FAIL;
	}

	result = m_pSystem->init(64, FMOD_INIT_NORMAL, 0);
	if (result != FMOD_OK)
	{
		MSG_BOX("FMOD::System_Init Fail");
		return E_FAIL;
	}
	LoadSoundFile();
	LoadSoundFile_Loop();

	m_fFadeTime = 5.f;
	m_bPlayerDead = false;

	m_pChannelArr[SOUND_BGM0]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[SOUND_BGM1]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[SOUND_BGM2]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[SOUND_BGM3]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[SOUND_BGM0_1]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[SOUND_BGM1_1]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[SOUND_BGM2_1]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[SOUND_BGM3_1]->setMode(FMOD_LOOP_NORMAL);
	m_pChannelArr[SOUND_OVER]->setMode(FMOD_LOOP_NORMAL);

	/*result = Debug_Initialize(FMOD_DEBUG_LEVEL_ERROR, FMOD_DEBUG_MODE_TTY);
	if (result != FMOD_OK)
	{
		MSG_BOX("»ç¿îµå µð¹ö±× ÃÊ±âÈ­ ½ÇÆÐ");
		return E_FAIL;
	}*/

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);
	/*pGameInstance->Subscribe(EVENT_SET_BGM, std::bind(&CSound_Manager::SetBGM, this, std::placeholders::_1));
	pGameInstance->Subscribe(EVENT_PLAY_BGM, std::bind(&CSound_Manager::PlayBGM_Event, this, std::placeholders::_1));
	pGameInstance->Subscribe(EVENT_STOP_BGM, std::bind(&CSound_Manager::StopBGM_Event, this, std::placeholders::_1));
	pGameInstance->Subscribe(EVENT_SET_BGM_MUTE, std::bind(&CSound_Manager::Set_BGM_Mute, this, std::placeholders::_1));*/
	Safe_Release(pGameInstance);

	return S_OK;
}

void CSound_Manager::Tick(_float fTimeDelta)
{
	//for (auto& iterator_Info = m_FadeInfo.begin(); iterator_Info != m_FadeInfo.end(); )
	//{
	//	//누적시간체크
	//	iterator_Info->fAccTime += fTimeDelta;

	//	//만약 페이드지정시간을 넘은 경우
	//	if (m_fFadeTime <= iterator_Info->fAccTime)
	//	{
	//		//만약 이전 채널에서 재생중이었다면
	//		if (-1.0f < (*iterator_Info).OldVolume)
	//		{
	//			//이전 채널에서 재생중지.
	//			m_pChannelArr[(*iterator_Info).OldChannel]->stop();
	//		}
	//		//현재 구조체를 제거
	//		iterator_Info = m_FadeInfo.erase(iterator_Info);
	//		if (0 == m_FadeInfo.size())
	//		{
	//			m_pSystem->update();
	//			return;
	//		}
	//	}

	//	//만약 페이드지정시간을 넘지 않은 경우
	//	else {
	//		if (false == m_bPlayerDead)
	//		{
	//			//이전 채널이 재생중이지 않고, 현재 채널만 재생중인 경우에
	//			if (0.0f > (*iterator_Info).OldVolume && 0.0f <= (*iterator_Info).CurrentVolume)
	//			{
	//				//현재 채널 볼륨만 조정
	//				m_pChannelArr[(*iterator_Info).CurrentChannel]->setVolume(iterator_Info->fAccTime * (*iterator_Info).CurrentVolume / m_fFadeTime);
	//			}
	//			//현재 채널은 재생중이지 않고, 이전 채널만 재생중인 경우에
	//			else if (0.0f > (*iterator_Info).CurrentVolume && 0.0f <= (*iterator_Info).OldVolume)
	//			{
	//				//이전 채널 볼륨만 조정
	//				m_pChannelArr[(*iterator_Info).OldChannel]->setVolume((*iterator_Info).OldVolume * ((1.0f - iterator_Info->fAccTime) / m_fFadeTime));
	//			}
	//			//현재, 이전 채널 모두 재생시
	//			else if (0.0f <= (*iterator_Info).CurrentVolume && 0.0f <= (*iterator_Info).OldVolume)
	//			{
	//				//현재 채널 볼륨 조정
	//				m_pChannelArr[(*iterator_Info).CurrentChannel]->setVolume(iterator_Info->fAccTime * (*iterator_Info).CurrentVolume / m_fFadeTime);
	//				//이전 채널 볼륨 조정
	//				m_pChannelArr[(*iterator_Info).OldChannel]->setVolume((*iterator_Info).OldVolume * ((1.0f - iterator_Info->fAccTime) / m_fFadeTime));
	//			}
	//		}

	//		iterator_Info++;
	//	}
	//}
	//m_pSystem->update();
	//return;
}

void CSound_Manager::Free()
{
	StopAll();
	for (auto& Mypair : m_mapSound)
	{
		delete[] Mypair.first;
		//FMOD_Sound_Release(Mypair.second);
		Mypair.second->release();
	}
	m_mapSound.clear();

	FMOD_RESULT result;
	result = m_pSystem->release();
	if (result != FMOD_OK)
	{
		return;
	}

	//m_FadeInfo.clear();

	/*result = m_pSystem->System::close();
	if (result != FMOD_OK)
	{
		MSG_BOX("»ç¿îµå½Ã½ºÅÛ ´Ý±â ½ÇÆÐ");
		return;
	}*/
}

void CSound_Manager::PlaySound(TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	map<TCHAR*, Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(),
		[&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	_bool bPlay = FALSE;

	if (m_pChannelArr[eID]->isPlaying(&bPlay))
	{
		m_pSystem->System::playSound(iter->second, nullptr, FALSE, &m_pChannelArr[eID]);
	}

	m_pChannelArr[eID]->setVolume(fVolume);

	m_pSystem->update();//FMOD_System_Update(m_pSystem);
}

void CSound_Manager::PlayBGM(TCHAR* pSoundKey, CHANNELID eID, float fVolume)
{
	map<TCHAR*, Sound*>::iterator iter;

	// iter = find_if(m_mapSound.begin(), m_mapSound.end(), CTag_Finder(pSoundKey));
	iter = find_if(m_mapSound.begin(), m_mapSound.end(), [&](auto& iter)->bool
		{
			return !lstrcmp(pSoundKey, iter.first);
		});

	if (iter == m_mapSound.end())
		return;

	//FADEINFO FadeInfo;

	//Â¦²áÀÎ Ã¤³Î
	CHANNELID PairID;

	switch (eID)
	{
	case SOUND_BGM0:
		PairID = SOUND_BGM0_1;
		break;
	case SOUND_BGM0_1:
		PairID = SOUND_BGM0;
		break;
	case SOUND_BGM1:
		PairID = SOUND_BGM1_1;
		break;
	case SOUND_BGM1_1:
		PairID = SOUND_BGM1;
		break;
	case SOUND_BGM2:
		PairID = SOUND_BGM2_1;
		break;
	case SOUND_BGM2_1:
		PairID = SOUND_BGM2;
		break;
	case SOUND_BGM3:
		PairID = SOUND_BGM3_1;
		break;
	case SOUND_BGM3_1:
		PairID = SOUND_BGM3;
		break;
	}



	m_pSystem->System::playSound(iter->second, nullptr, FALSE, &m_pChannelArr[eID]);
	//m_pChannelArr[eID]->setMode(FMOD_LOOP_NORMAL);
	//m_pChannelArr[eID]->setVolume(0.0f);
	m_pSystem->update();
}

void CSound_Manager::StopBGM(CHANNELID eID)
{
	/*_bool bIsPlay = false;
	m_pChannelArr[eID]->isPlaying(&bIsPlay);
	if (false == bIsPlay)
		return;
	FADEINFO FadeInfo;
	FadeInfo.CurrentChannel = MAXCHANNEL;
	FadeInfo.CurrentVolume = -1.f;
	FadeInfo.OldChannel = eID;
	_float fOldVolume = 0.0f;
	m_pChannelArr[eID]->getVolume(&fOldVolume);
	FadeInfo.OldVolume = fOldVolume;
	FadeInfo.fAccTime = 0.0f;
	m_FadeInfo.push_back(FadeInfo);*/
}

void CSound_Manager::StopSound(CHANNELID eID)
{
	m_pChannelArr[eID]->stop();

}

void CSound_Manager::StopAll()
{
	for (int i = 0; i < MAXCHANNEL; ++i)
		m_pChannelArr[i]->stop();
}

void CSound_Manager::SetChannelVolume(CHANNELID eID, float fVolume)
{

	m_pChannelArr[eID]->setVolume(fVolume);
	m_pSystem->update();
}

_bool CSound_Manager::SetBGM(void* pArg)
{
	_uint iIndex = *(_uint*)pArg;
	switch (iIndex)
	{
		//¹Ù³ª³ª, ÄÝ¶ó
	case 0:
		StopSound(SOUND_BGM1);
		StopSound(SOUND_BGM1_1);
		StopSound(SOUND_BGM2);
		StopSound(SOUND_BGM2_1);
		StopSound(SOUND_BGM3);
		StopSound(SOUND_BGM3_1);
		PlayBGM(L"Music_MysteriousRevelationsShort.wav", SOUND_BGM0, 0.3f);

		//SOUND_BGM0, SOUND_BGM1
	case 1:
		PlayBGM(L"Ambience_Alley.wav", SOUND_BGM1, 0.2f);
		break;

		//¹®µû°í µé¾î°¡ÀÚ¸¶ÀÚ
		//SOUND_BGM0
	case 2:
		StopBGM(SOUND_BGM1);
		break;

		//ÁöÆ÷¶óÀÌÅÍ ¶ß´Â ºÎºÐ
		//SOUND_BGM0, SOUND_BGM1, SOUND_BGM2
	case 3:
		PlayBGM(L"Ambience_Industrial.wav", SOUND_BGM1, 0.3f);
		PlayBGM(L"Ambience_StrongWind.wav", SOUND_BGM2, 0.2f);
		break;

		//SOUND_BGM0, SOUND_BGM1, SOUND_BGM2, SOUND_BGM3
	case 4:
		PlayBGM(L"Ambience_FireLoop.wav", SOUND_BGM3, 0.2f);
		break;

		//SOUND_BGM0, SOUND_BGM1, SOUND_BGM2, SOUND_BGM3
	case 5:
		StopBGM(SOUND_BGM3);
		break;

		//SOUND_BGM0, SOUND_BGM1, SOUND_BGM2_1, SOUND_BGM3_1
	case 6:
		/*StopSound(SOUND_BGM2);
		StopSound(SOUND_BGM3);*/
		PlayBGM(L"Ambience_BubblingFountain.wav", SOUND_BGM2_1, 0.2f);
		PlayBGM(L"Ambience_LightBuzz.wav", SOUND_BGM3_1, 0.2f);
		break;

		//SOUND_BGM0_1,
	case 7:
		//StopSound(SOUND_BGM0);
		StopBGM(SOUND_BGM1);
		StopBGM(SOUND_BGM2_1);
		StopBGM(SOUND_BGM3_1);
		PlayBGM(L"Music_NightclubReveal.wav", SOUND_BGM0_1, 0.2f);
		break;

		//SOUND_BGM0
	case 8:
		//StopSound(SOUND_BGM0);
		StopSound(SOUND_BGM1);
		StopSound(SOUND_BGM1_1);
		StopSound(SOUND_BGM2);
		StopSound(SOUND_BGM2_1);
		StopSound(SOUND_BGM3);
		StopSound(SOUND_BGM3_1);
		PlayBGM(L"Ambience_OldTimeyMusic6.wav", SOUND_BGM0, 0.2f);
		break;

		//SOUND_BGM0, SOUND_BGM1
	case 9:
		PlayBGM(L"MALONE_Mad.wav", SOUND_BGM1, 0.6f);
		break;

		//SOUND_BGM0, SOUND_BGM1_1
	case 10:
		StopSound(SOUND_BGM0);
		PlaySound(L"Music_JusticeHitsHard.wav", SOUND_BGM1_1, 0.2f);
		break;

	case 11:
		StopSound(SOUND_BGM1);
		StopSound(SOUND_BGM1_1);
		StopSound(SOUND_BGM2);
		StopSound(SOUND_BGM2_1);
		StopSound(SOUND_BGM3);
		StopSound(SOUND_BGM3_1);
		PlaySound(L"Music_ActionA.wav", SOUND_BGM0, 0.2f);
		break;
	}
	return true;
}

_bool CSound_Manager::IsPlaying(CHANNELID eID)
{
	_bool bPlaying;
	m_pChannelArr[eID]->isPlaying(&bPlaying);
	return bPlaying;
}

_bool CSound_Manager::PlayBGM_Event(void* pArg)
{
	/*BGMINFO* pBGMInfo = (BGMINFO*)pArg;
	PlayBGM(pBGMInfo->pSoundKey, (CHANNELID)pBGMInfo->ChannelId, pBGMInfo->fVolume);
	return true;*/
	return true;
}

_bool CSound_Manager::StopBGM_Event(void* pArg)
{
	StopBGM((CHANNELID)(*(_uint*)pArg));
	return true;
}

_bool CSound_Manager::Set_BGM_Mute(void* pArg)
{
	//_bool bMute = *(_bool*)pArg;
	//_bool bIsPlay = false;
	//if (true == bMute)
	//{
	//	m_bPlayerDead = true;
	//	//일단 모두 기록 후
	//	for (int i = 0; i < 8; i++)
	//	{
	//		m_pChannelArr[i + 1]->isPlaying(&bIsPlay);
	//		if (true == bIsPlay)
	//		{
	//			_float fVolume = 0.0f;
	//			m_pChannelArr[i + 1]->getVolume(&fVolume);
	//			m_fBGMVolumes[i] = fVolume;
	//		}
	//	}
	//	//이전 채널 값은 모두 기록하지 않는다.
	//	for (auto& FadeInfo : m_FadeInfo)
	//	{
	//		if (0.0f <= FadeInfo.CurrentVolume)
	//		{
	//			m_fBGMVolumes[FadeInfo.CurrentChannel - 1] = FadeInfo.CurrentVolume;
	//		}
	//		if (0.0f <= FadeInfo.OldVolume)
	//		{
	//			m_fBGMVolumes[FadeInfo.OldChannel - 1] = -1.0f;
	//		}
	//	}
	//	//그리고 모든 BGM채널 소리를 0으로 만든다.
	//	for (int j = 0; j < 8; j++)
	//	{
	//		m_pChannelArr[j + 1]->setVolume(0.0f);
	//		m_pSystem->update();
	//	}
	//	PlaySound(L"Music_GameOver.wav", SOUND_OVER, 0.3f);
	//	m_pSystem->update();

	//}

	//else
	//{
	//	m_bPlayerDead = false;
	//	for (int i = 0; i < 8; i++)
	//	{
	//		if (-1.f < m_fBGMVolumes[i])
	//		{
	//			m_pChannelArr[i + 1]->setVolume(m_fBGMVolumes[i]);
	//			m_pSystem->update();
	//		}
	//		m_fBGMVolumes[i] = -1.f;
	//	}
	//	StopSound(SOUND_OVER);
	//	m_pSystem->update();

	//}


	return true;
}

void CSound_Manager::LoadSoundFile()
{
	
	_finddata_t fd;

	
	intptr_t handle = _findfirst("../../Sound/Default/*.wav", &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[128] = "../../Sound/Default/";
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../ Sound/Success.wav"
		strcat_s(szFullPath, fd.name);

		Sound* pSound = nullptr;

		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_DEFAULT, 0, &pSound);//FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		
		iResult = _findnext(handle, &fd);
	}

	m_pSystem->update();

	_findclose(handle);
}

void CSound_Manager::LoadSoundFile_Loop()
{
	_finddata_t fd;

	intptr_t handle = _findfirst("../../Sound/Loop/*.wav", &fd);

	if (handle == -1)
		return;

	int iResult = 0;

	char szCurPath[128] = "../../Sound/Loop/";
	char szFullPath[128] = "";

	while (iResult != -1)
	{
		strcpy_s(szFullPath, szCurPath);

		// "../ Sound/Success.wav"
		strcat_s(szFullPath, fd.name);

		Sound* pSound = nullptr;

		FMOD_RESULT eRes = m_pSystem->createSound(szFullPath, FMOD_LOOP_NORMAL, 0, &pSound);//FMOD_System_CreateSound(m_pSystem, szFullPath, FMOD_DEFAULT, 0, &pSound);

		if (eRes == FMOD_OK)
		{
			int iLength = strlen(fd.name) + 1;

			TCHAR* pSoundKey = new TCHAR[iLength];
			ZeroMemory(pSoundKey, sizeof(TCHAR) * iLength);

			
			MultiByteToWideChar(CP_ACP, 0, fd.name, iLength, pSoundKey, iLength);

			m_mapSound.emplace(pSoundKey, pSound);
		}
		
		iResult = _findnext(handle, &fd);
	}

	m_pSystem->update();

	_findclose(handle);
}
