#include "Client_Pch.h"
#include "MonsterAttackState.h"
#include "MonsterIdleState.h"
#include "Monster.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Player.h"

CMonsterAttackState::CMonsterAttackState()
{
}

CMonsterAttackState::~CMonsterAttackState()
{
}

CMonsterState* CMonsterAttackState::Transition(CMonster& _monster)
{
	CMonsterState* pState = __super::Transition(_monster);
	if (nullptr != pState)
		return pState;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd())
	{
		return new CMonsterIdleState();
	}

	return nullptr;
}

void CMonsterAttackState::Entry(CMonster& _player)
{
	__super::Entry(_player);

	m_szName = "Attack";
	m_eType = (ANIM_TYPE)((rand()%3 + 2));
	m_pModelCom->SetUp_Animation(m_eType, false);
	m_pModelCom->Set_NextAnim(IDLE);
	_player.Get_AttackDesc()[0]->bActive = true; 
}

void CMonsterAttackState::Update(CMonster& _player, _float fTimeDelta)
{

	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

	switch(_player.Get_MonsterType())
	{
	case CMonster::VILLAGER_M:
	{
		_float fVolume = 0.7f;
		switch (m_pModelCom->Get_CurrentAnimIndex())
		{
		case ATK1:
			if (m_fTrackPos > 110.f && m_fTrackPos < 112.f)
			{
				m_bAttack = true;
		
			}
			else
				m_bAttack = false;

			if (m_fTrackPos > 70.f && !m_bExecuted[0])
			{
				m_bExecuted[0] = true;
				if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1))
					m_pGameInstance->Play_Sound(L"WhooshAxe_01.wav", SOUND_ENEMY1, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2))
					m_pGameInstance->Play_Sound(L"WhooshAxe_01.wav", SOUND_ENEMY2, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3))
					m_pGameInstance->Play_Sound(L"WhooshAxe_01.wav", SOUND_ENEMY3, fVolume);

				if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboA02-002.wav", SOUND_ENEMY1_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboA02-002.wav", SOUND_ENEMY2_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboA02-002.wav", SOUND_ENEMY3_VOICE, fVolume);
			}
			break;
		case ATK2:
			if (m_fTrackPos > 85.f && m_fTrackPos <87.f )
			{
				m_bAttack = true;
	
			}
			else
				m_bAttack = false;

			if (m_fTrackPos > 60.f && !m_bExecuted[0])
			{
				m_bExecuted[0] = true;
				if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1))
					m_pGameInstance->Play_Sound(L"WhooshAxe_01.wav", SOUND_ENEMY1, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2))
					m_pGameInstance->Play_Sound(L"WhooshAxe_01.wav", SOUND_ENEMY2, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3))
					m_pGameInstance->Play_Sound(L"WhooshAxe_01.wav", SOUND_ENEMY3, fVolume);
			
				if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboA03.wav", SOUND_ENEMY1_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboA03.wav", SOUND_ENEMY2_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboA03.wav", SOUND_ENEMY3_VOICE, fVolume);
			}
			break;
		case ATK3:
			if (m_fTrackPos >55.f && m_fTrackPos < 62.f)
			{
				m_bAttack = true;
			}
			else
				m_bAttack = false;

			if (m_fTrackPos > 50.f && !m_bExecuted[0])
			{
				m_bExecuted[0] = true;
				if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1))
					m_pGameInstance->Play_Sound(L"WhooshAxe_02.wav", SOUND_ENEMY1, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2))
					m_pGameInstance->Play_Sound(L"WhooshAxe_02.wav", SOUND_ENEMY2, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3))
					m_pGameInstance->Play_Sound(L"WhooshAxe_02.wav", SOUND_ENEMY3, fVolume);

				if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboA02-002.wav", SOUND_ENEMY1_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboA02-002.wav", SOUND_ENEMY2_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboA02-002.wav", SOUND_ENEMY3_VOICE, fVolume);
			}
			break;
		}
	}
		break;
	case CMonster::VILLAGER_F:
	{
		_float fVolume = 0.7f;
		switch (m_pModelCom->Get_CurrentAnimIndex())
		{
		case ATK1:
			if (m_fTrackPos > 10.f && !m_bExecuted[0])
			{
				m_bAttack = true;
			
					m_bExecuted[0] = true;
					if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1))
						m_pGameInstance->Play_Sound(L"Villager_WhooshKnife_01.wav", SOUND_ENEMY1, fVolume);
					else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2))
						m_pGameInstance->Play_Sound(L"Villager_WhooshKnife_01.wav", SOUND_ENEMY2, fVolume);
					else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3))
						m_pGameInstance->Play_Sound(L"Villager_WhooshKnife_01.wav", SOUND_ENEMY3, fVolume);
				
					if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
						m_pGameInstance->Play_Sound(L"VillagerF_Voice_Attack_01.wav", SOUND_ENEMY1_VOICE, fVolume);
					else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
						m_pGameInstance->Play_Sound(L"VillagerF_Voice_Attack_01.wav", SOUND_ENEMY2_VOICE, fVolume);
					else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
						m_pGameInstance->Play_Sound(L"VillagerF_Voice_Attack_01.wav", SOUND_ENEMY3_VOICE, fVolume);
			}

			break;
		case ATK2:
			if (m_fTrackPos > 20.f && !m_bExecuted[1])
			{
				m_bAttack = true;

					m_bExecuted[1] = true;
					if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1))
						m_pGameInstance->Play_Sound(L"Villager_WhooshKnife_01.wav", SOUND_ENEMY1, fVolume);
					else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2))
						m_pGameInstance->Play_Sound(L"Villager_WhooshKnife_01.wav", SOUND_ENEMY2, fVolume);
					else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3))
						m_pGameInstance->Play_Sound(L"Villager_WhooshKnife_01.wav", SOUND_ENEMY3, fVolume);
			
					if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
						m_pGameInstance->Play_Sound(L"VillagerF_Voice_Attack_02.wav", SOUND_ENEMY1_VOICE, fVolume);
					else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
						m_pGameInstance->Play_Sound(L"VillagerF_Voice_Attack_02.wav", SOUND_ENEMY2_VOICE, fVolume);
					else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
						m_pGameInstance->Play_Sound(L"VillagerF_Voice_Attack_02.wav", SOUND_ENEMY3_VOICE, fVolume);
			}

			break;
		case ATK3:
			if (m_fTrackPos > 70.f && !m_bExecuted[2])
			{
				m_bAttack = true;

					m_bExecuted[2] = true;
					if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1))
						m_pGameInstance->Play_Sound(L"Villager_WhooshKnife_01.wav", SOUND_ENEMY1, fVolume);
					else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2))
						m_pGameInstance->Play_Sound(L"Villager_WhooshKnife_01.wav", SOUND_ENEMY2, fVolume);
					else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3))
						m_pGameInstance->Play_Sound(L"Villager_WhooshKnife_01.wav", SOUND_ENEMY3, fVolume);
				
					if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
						m_pGameInstance->Play_Sound(L"VillagerF_Voice_Attack_02.wav", SOUND_ENEMY1_VOICE, fVolume);
					else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
						m_pGameInstance->Play_Sound(L"VillagerF_Voice_Attack_02.wav", SOUND_ENEMY2_VOICE, fVolume);
					else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
						m_pGameInstance->Play_Sound(L"VillagerF_Voice_Attack_02.wav", SOUND_ENEMY3_VOICE, fVolume);
			}

			if (m_fTrackPos > 110.f)
				m_bAttack = false;
			break;
		}
	}
		break;
	case CMonster::JOKER:
	{
		_float fVolume = 0.7f;
		switch (m_pModelCom->Get_CurrentAnimIndex())
		{
		case ATK1:
			/*if (m_fTrackPos > 20.f && m_fTrackPos < 25.f)
			{
				m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_ENEMY_ATTACK, 1.f);
			}*/
			if (m_fTrackPos > 50.f && m_fTrackPos < 90.f && !m_bExecuted[0])
			{
				m_bExecuted[0] = true;
				m_bAttack = true;
				m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_ENEMY_ATTACK, 1.f);

				if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
					m_pGameInstance->Play_Sound(L"Joker_Vocal_Swing_01.wav", SOUND_ENEMY1_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
					m_pGameInstance->Play_Sound(L"Joker_Vocal_Swing_01.wav", SOUND_ENEMY2_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
					m_pGameInstance->Play_Sound(L"Joker_Vocal_Swing_01.wav", SOUND_ENEMY3_VOICE, fVolume);
			}
			else
				m_bAttack = false;
			break;
		case ATK2:
			if (m_fTrackPos > 50.f && m_fTrackPos < 55.f)
			{
				m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_ENEMY1, 1.f);
			}
			if (m_fTrackPos > 80.f && m_fTrackPos < 90.f && !m_bExecuted[0])
			{
				m_bExecuted[0] = true;
				m_bAttack = true;
				EFFECTDESC desc;
				desc.texName = TEXT("Smoke4");
				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_HoriziontalLookDir() * 1.f;
				desc.vScale = { 3.f,3.f,3.f };
				desc.vPos = vPos;
				desc.iRow = 12;
				CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
					TEXT("Prototype_GameObject_Effect_Texture"), &desc);

				m_pGameInstance->Play_Sound(L"WoodHeavtHit04.wav", SOUND_ENEMY_ATTACK, 1.f);

				if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
					m_pGameInstance->Play_Sound(L"Joker_Vocal_Swing_02.wav", SOUND_ENEMY1_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
					m_pGameInstance->Play_Sound(L"Joker_Vocal_Swing_02.wav", SOUND_ENEMY2_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
					m_pGameInstance->Play_Sound(L"Joker_Vocal_Swing_02.wav", SOUND_ENEMY3_VOICE, fVolume);
			}
			else
				m_bAttack = false;
			break;
		case ATK3:
			if (m_fTrackPos > 60.f && m_fTrackPos < 90.f && !m_bExecuted[0])
			{
				m_bExecuted[0] = true;
				m_bAttack = true;
				m_bExecuted[0] = true;
				m_bAttack = true;
				EFFECTDESC desc;
				desc.texName = TEXT("Smoke4");
				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_HoriziontalLookDir() * 1.f;
				desc.vScale = { 3.f,3.f,3.f };
				desc.vPos = vPos;
				desc.iRow = 12;
				CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
					TEXT("Prototype_GameObject_Effect_Texture"), &desc);
				m_pGameInstance->Play_Sound(L"WoodHeavtHit04.wav", SOUND_ENEMY_ATTACK, 1.f);

				if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
					m_pGameInstance->Play_Sound(L"Joker_Vocal_Swing_03.wav", SOUND_ENEMY1_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
					m_pGameInstance->Play_Sound(L"Joker_Vocal_Swing_03.wav", SOUND_ENEMY2_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
					m_pGameInstance->Play_Sound(L"Joker_Vocal_Swing_03.wav", SOUND_ENEMY3_VOICE, fVolume);
			}
			else
				m_bAttack = false;
			break;
		}
	}
		break;
	case CMonster::GARDENER:
	{
		_float fVolume = 1.f;
		switch (m_pModelCom->Get_CurrentAnimIndex())
		{
		case ATK1:
			if (m_fTrackPos > 40.f && m_fTrackPos < 80.f && !m_bExecuted[0])
			{
				m_bExecuted[0] = true;
				m_bAttack = true;
				if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1))
					m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_ENEMY1, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2))
					m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_ENEMY2, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3))
					m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_ENEMY3, fVolume);

				if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboA02-002.wav", SOUND_ENEMY1_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboA02-002.wav", SOUND_ENEMY2_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboA02-002.wav", SOUND_ENEMY3_VOICE, fVolume);
			}
			break;
		case ATK2:
			if (m_fTrackPos > 40.f && m_fTrackPos < 80.f && !m_bExecuted[1])
			{
				m_bExecuted[1] = true;
				m_bAttack = true;
				if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1))
					m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_ENEMY1, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2))
					m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_ENEMY2, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3))
					m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_ENEMY3, fVolume);

				if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboA03.wav", SOUND_ENEMY1_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboA03.wav", SOUND_ENEMY2_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboA03.wav", SOUND_ENEMY3_VOICE, fVolume);
			}
			break;
		case ATK3:
			if (m_fTrackPos > 60.f && m_fTrackPos < 100.f && !m_bExecuted[2])
			{
				m_bExecuted[2] = true;
				m_bAttack = true;
				if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1))
					m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_ENEMY1, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2))
					m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_ENEMY2, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3))
					m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_ENEMY3, fVolume);

				if (!m_pGameInstance->IsPlaying(SOUND_ENEMY1_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboB01.wav", SOUND_ENEMY1_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY2_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboB01.wav", SOUND_ENEMY2_VOICE, fVolume);
				else if (!m_pGameInstance->IsPlaying(SOUND_ENEMY3_VOICE))
					m_pGameInstance->Play_Sound(L"NM_V_Halberds_ComboB01.wav", SOUND_ENEMY3_VOICE, fVolume);
			}
		}
	}
		break;
	}

	if (!m_bInterpolate)
	{
		if (m_fTrackPos > m_fDuration - 20.f)
		{
			m_pModelCom->Set_NextAnim(IDLE);
			m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
		}
	}

	_float4 vPreRootPos = m_pModelCom->GetRootPos();

	__super::Update(_player, fTimeDelta);

	if (!m_bControllerHit && !m_bSideColided)
	{
		_vector vRot = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector v = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
		_matrix m = XMMatrixRotationQuaternion(v);

		_vector vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float4 vPos;
		XMStoreFloat4(&vPos, vCurPos - XMVector3TransformNormal(XMLoadFloat4(&m_pModelCom->GetRootPos()) - XMLoadFloat4(&vPreRootPos), m) * 1.f * 0.016f);
		_float4 vPosition = { vPos.x,m_vPrePos.y,vPos.z,1.f };
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));
	}

	__super::PhysicsUpdate(_player, fTimeDelta);

	m_pTransformCom->LookAtDir(m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom));

}

void CMonsterAttackState::Exit(CMonster& _player)
{
	__super::Exit(_player);
	m_bAttack = false;
}

void CMonsterAttackState::Free()
{
}
