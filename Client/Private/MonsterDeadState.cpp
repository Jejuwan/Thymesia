#include "Client_Pch.h"
#include "MonsterDeadState.h"
#include "MonsterHurtState.h"
#include "MonsterIdleState.h"
#include "Monster.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Player.h"
#include "PlayerState.h"
#include "PlayerAttackState.h"
#include "UI.h"
#include "Camera_Player.h"

CMonsterDeadState::CMonsterDeadState()
{
}

CMonsterDeadState::~CMonsterDeadState()
{
}

CMonsterState* CMonsterDeadState::Transition(CMonster& _player)
{
    return nullptr;
}

void CMonsterDeadState::Entry(CMonster& _monster)
{
	__super::Entry(_monster);

	m_pModelCom->SetUp_Animation(DIE, false);
	m_pPlayer->LookLockOn(nullptr);
    _monster.Entry_Dead();
    CUI* pUI = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), _monster.Get_HpBarIdx()));
    CUI* pUI2 = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), _monster.Get_HpBarIdx() - 1));

    pUI->Get_Desc()->bVisible = false;
    pUI->Set_Hp(1.f);
    pUI2->Get_Desc()->bVisible = false;

	m_bHitRim = true;

	CVIBuffer_Point_Instance::POINTINSTANCEDESC pDesc;
	wstring wstr = TEXT("../../SaveFiles/Particles/");
	wstr += TEXT("blood");
	wstr += TEXT(".effect");
	ifstream is(wstr, ios::binary);
	is.read((char*)&pDesc, sizeof(CVIBuffer_Point_Instance::POINTINSTANCEDESC));
	is.close();
	pDesc.bNum300 = true;
	_vector vPos;
	_vector vRight = m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
	_vector vLook = m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_LOOK);
	switch (m_pPlayer->Get_PlayerState()->Get_AttackIndex())
	{
	case 0:
		XMStoreFloat3(&pDesc.fMinDir, (-vRight - XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
		XMStoreFloat3(&pDesc.fMaxDir, (-vRight + XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
		vPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) + m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_HoriziontalLookDir() * 0.5f + XMVectorSet(0, 2.5, 0, 0);
		break;
	case 1:
		XMStoreFloat3(&pDesc.fMinDir, (vRight - XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
		XMStoreFloat3(&pDesc.fMaxDir, (vRight + XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
		vPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) + m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_HoriziontalLookDir() * 0.5f + XMVectorSet(0, 2.2, 0, 0);
		break;
	case 2:
		XMStoreFloat3(&pDesc.fMinDir, (vLook - XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
		XMStoreFloat3(&pDesc.fMaxDir, (vLook + XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
		vPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) + m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_HoriziontalLookDir() * 0.5f + XMVectorSet(0, 2.2, 0, 0);
		break;
	default:
		XMStoreFloat3(&pDesc.fMinDir, (-vLook - XMVectorSet(0.3f, 0.3f, 0.3f, 0.f)));
		XMStoreFloat3(&pDesc.fMaxDir, (-vLook + XMVectorSet(0.3f, 0.3f, 0.3f, 0.f)));
		vPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) + m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_HoriziontalLookDir() * 0.5f + XMVectorSet(0, 2, 0, 0);
		break;
	}
	vPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) + m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_HoriziontalLookDir() * 0.5f + XMVectorSet(0, 2, 0, 0);
	XMStoreFloat4(&pDesc.vPivotPos, vPos);
	CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_Effect_Particle"), &pDesc);

	wstring soundName = L"BodyStab_0";
	soundName += to_wstring(rand() % 3 + 1);
	soundName += L".wav";

	_float fVolume = 0.7f;
	switch (_monster.Get_HpBarIdx())
	{
	case 5:
		m_pGameInstance->StopSound(SOUND_ENEMY1);
		m_pGameInstance->Play_Sound((wchar_t*)soundName.c_str(), SOUND_ENEMY1, 1.f);

		switch (_monster.Get_MonsterType())
		{
		case CMonster::VILLAGER_M:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		case CMonster::VILLAGER_F:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"VillagerF_Voice_Die_01.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"VillagerF_Voice_Die_01.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"VillagerF_Voice_Die_01.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		case CMonster::JOKER:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"Joker_Vocal_Death_01.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"Joker_Vocal_Death_01.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"Joker_Vocal_Death_01.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		case CMonster::GARDENER:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		}
		break;
	case 8:
		m_pGameInstance->StopSound(SOUND_ENEMY2);
		m_pGameInstance->Play_Sound((wchar_t*)soundName.c_str(), SOUND_ENEMY2, 1.f);
		switch (_monster.Get_MonsterType())
		{
		case CMonster::VILLAGER_M:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		case CMonster::VILLAGER_F:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"VillagerF_Voice_Die_01.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"VillagerF_Voice_Die_01.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"VillagerF_Voice_Die_01.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		case CMonster::JOKER:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"Joker_Vocal_Death_01.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"Joker_Vocal_Death_01.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"Joker_Vocal_Death_01.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		case CMonster::GARDENER:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		}
		break;
	case 11:
		m_pGameInstance->StopSound(SOUND_ENEMY3);
		m_pGameInstance->Play_Sound((wchar_t*)soundName.c_str(), SOUND_ENEMY3, 1.f);
		switch (_monster.Get_MonsterType())
		{
		case CMonster::VILLAGER_M:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		case CMonster::VILLAGER_F:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"VillagerF_Voice_Die_01.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"VillagerF_Voice_Die_01.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"VillagerF_Voice_Die_01.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		case CMonster::JOKER:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"Joker_Vocal_Death_01.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"Joker_Vocal_Death_01.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"Joker_Vocal_Death_01.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		case CMonster::GARDENER:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_LieDie.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		}
		break;
	}


}

void CMonsterDeadState::Update(CMonster& _monster, _float fTimeDelta)
{
    XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
    m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();
    m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();

    if(!m_pModelCom->Get_CurrentAnim()->GetEnd())
    {
        __super::Update(_monster, fTimeDelta);
        if(m_fTrackPos < 15.f)
        {
            _float fSpeed = 0;
            switch (_monster.Get_MonsterType())
            {
            case CMonster::GARDENER:
                fSpeed = 0.4f;
                break;
            case CMonster::JOKER:
                fSpeed = 0.0f;
                break;
            case CMonster::VILLAGER_M:
                fSpeed =0.25f;
                break;
            case CMonster::VILLAGER_F:
                fSpeed = 0.00f;
                break;
            }
      
            m_pTransformCom->Go_Dir({ 0,-1,0 }, fSpeed, fTimeDelta);
        }
    }
}

void CMonsterDeadState::Exit(CMonster& _monster)
{
    __super::Exit(_monster);
}

void CMonsterDeadState::Free()
{
}
