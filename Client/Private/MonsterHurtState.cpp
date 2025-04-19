#include "Client_Pch.h"
#include "MonsterHurtState.h"
#include "MonsterIdleState.h"
#include "MonsterStunState.h"
#include "Monster.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Player.h"
#include "PlayerState.h"
#include "PlayerAttackState.h"
#include "UI.h"
#include "Camera_Player.h"

CMonsterHurtState::CMonsterHurtState(ANIM_TYPE type)
{
	m_eHurtType = type;
}

CMonsterHurtState::~CMonsterHurtState()
{
}

CMonsterState* CMonsterHurtState::Transition(CMonster& _monster)
{
	CPlayerState* pPlayerState = m_pPlayer->Get_PlayerState();

	if (pPlayerState)
	{
		if (pPlayerState->Get_Name() == "Plunder")
		{
			if (!m_bExecuted[0])
			{
				m_pPlayer->Set_StealType(_monster.Get_MonsterType());
				m_bExecuted[0] = true;
				CUI* pUI0 = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), 13));
				pUI0->Get_Desc()->bVisible = false;
				CUI* pUI1 = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), 14));
				pUI1->Get_Desc()->bVisible = false;
				CUI* pUI2 = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), 16));
				pUI2->Get_Desc()->bVisible = false;
				CUI* pUI3 = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), 15));
				pUI3->Get_Desc()->bVisible = false;
				CUI* pUI4 = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), 17));
				pUI4->Get_Desc()->bVisible = false;
				switch(_monster.Get_MonsterType())
				{
				case CMonster::VILLAGER_M:
					pUI1->Get_Desc()->bVisible = true;
					break;
				case CMonster::VILLAGER_F:
					pUI2->Get_Desc()->bVisible = true;
					break;
				case CMonster::JOKER:
					pUI3->Get_Desc()->bVisible = true;
					break;
				case CMonster::GARDENER:
					pUI4->Get_Desc()->bVisible = true;
					break;
				}
			}
		}
		/*if(pPlayerState->Get_AttackChange() && pPlayerState->Get_Attack() && _monster.Get_MonsterHit())
		{
			if(pPlayerState->Get_Name() == "Plunder")
			{
				m_pPlayer->Set_StealType(_monster.Get_MonsterType());
				_monster.Set_HP(_monster.Get_Hp() - 1);
			}
			else
			{
				_monster.Set_HP(_monster.Get_Hp() - 20);
			}
			if (_monster.Get_Hp() <= 0)
				return new CMonsterStunState;
			else
			{
				pPlayerState->Set_AttackChange(false);
				m_eHurtType == HURT_S1 ? m_eHurtType = HURT_S2 : m_eHurtType = HURT_S1;
				return new CMonsterHurtState(m_eHurtType);
			}
		}*/
	}

	if (m_pModelCom->Get_CurrentAnim()->GetEnd())
		return new CMonsterIdleState();
	return nullptr;
}

void CMonsterHurtState::Entry(CMonster& _monster)
{
	__super::Entry(_monster);

	m_szName = "Hurt";
	m_pModelCom->SetUp_Animation(m_eHurtType, false);
	_vector pLook = m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom);
	m_pTransformCom->LookAtDir(pLook);

	CUI* ptemp1 = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), 5));
	CUI* ptemp2 = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), 8));
	CUI* ptemp3 = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), 11));

	if (!ptemp1->Get_Desc()->bVisible)
	{
		if (-1 == _monster.Get_HpBarIdx())
			_monster.Set_HpBarIdx(5);
	}
	if (!ptemp2->Get_Desc()->bVisible)
	{
		if (-1 == _monster.Get_HpBarIdx())
			_monster.Set_HpBarIdx(8);
	}
	if (!ptemp3->Get_Desc()->bVisible)
	{
		if (-1 == _monster.Get_HpBarIdx())
			_monster.Set_HpBarIdx(11);
	}

	CUI* pUI = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), _monster.Get_HpBarIdx()));
	pUI->Get_Desc()->bVisible = true;
	CUI* pUI2 = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), _monster.Get_HpBarIdx() - 1));
	pUI2->Get_Desc()->bVisible = true;

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
	vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) /*- m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_LOOK)*0.5f*/ +  XMVectorSet(0, 1, 0, 0);
	XMStoreFloat4(&pDesc.vPivotPos, vPos);
	CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
		TEXT("Prototype_GameObject_Effect_Particle"), &pDesc);

	wstring soundName = L"BodyStab_0";
	soundName += to_wstring(rand() % 2 + 1);
	soundName += L".wav";

	_float fVolume = 0.7f;
	switch (_monster.Get_HpBarIdx())
	{
	case 5:
		m_pGameInstance->StopSound(SOUND_ENEMY1);
		m_pGameInstance->Play_Sound((wchar_t*)soundName.c_str(), SOUND_ENEMY1, 0.7f);
		switch (_monster.Get_MonsterType())
		{
		case CMonster::VILLAGER_M:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"VillagerM_Voice_Hurt_01.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"VillagerM_Voice_Hurt_01.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"VillagerM_Voice_Hurt_01.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		case CMonster::VILLAGER_F:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"VillagerF_Voice_Hurt_01.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"VillagerF_Voice_Hurt_01.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"VillagerF_Voice_Hurt_01.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		case CMonster::JOKER:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"Joker_Vocal_Hurt_01.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"Joker_Vocal_Hurt_01.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"Joker_Vocal_Hurt_01.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		case CMonster::GARDENER:
			if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_HurtM.wav", SOUND_ENEMY1_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_HurtM.wav", SOUND_ENEMY2_VOICE, fVolume);
			else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
				m_pGameInstance->Play_Sound(L"NM_V_Halberds_HurtM.wav", SOUND_ENEMY3_VOICE, fVolume);
			break;
		}
		break;
	case 8:
		m_pGameInstance->StopSound(SOUND_ENEMY2);
		m_pGameInstance->Play_Sound((wchar_t*)soundName.c_str(), SOUND_ENEMY2, 0.7f);
		break;
	case 11:
		m_pGameInstance->StopSound(SOUND_ENEMY3);
		m_pGameInstance->Play_Sound((wchar_t*)soundName.c_str(), SOUND_ENEMY3, 0.7f);
		break;
	}
}

void CMonsterHurtState::Update(CMonster& _monster, _float fTimeDelta)
{
  	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();
	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();

	_float amount = 0.f;
	if (m_pPlayer->Get_PlayerState()->Get_Name() == "Plunder")
		amount = 0.f;
	else
		amount = 0.2f;
	__super::Update(_monster, fTimeDelta);
	__super::RootPosUpdate(_monster, amount, fTimeDelta);
	__super::PhysicsUpdate(_monster, fTimeDelta);
}

void CMonsterHurtState::Exit(CMonster& _monster)
{
	__super::Exit(_monster);
}

void CMonsterHurtState::Free()
{
}
