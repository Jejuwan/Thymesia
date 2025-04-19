#include "Client_Pch.h"
#include "BatIdleState.h"
#include "BatAttackState.h"
#include "Monster.h"
#include "Bat.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"

CBatAttackState::CBatAttackState()
{
}

CBatAttackState::~CBatAttackState()
{
}

CMonsterState* CBatAttackState::Transition(CMonster& _monster)
{
	if (m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd())
	{
		m_iAttackCount--;
		return new CBatIdleState();
	}
	return nullptr;
}

void CBatAttackState::Entry(CMonster& _player)
{
	__super::Entry(_player);
	m_szName = "Attack";
	m_eType = (ANIM_TYPE)(m_iAttackCount + ATTACK_L);
	m_pModelCom->SetUp_Animation(m_eType, false);
	m_pModelCom->Set_NextAnim(IDLE);
	_player.m_pattern[0] = true;
}

void CBatAttackState::Update(CMonster& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

	switch (m_pModelCom->Get_CurrentAnimIndex())
	{
	case ATTACK_L:
		_player.Get_AttackDesc()[0]->bActive = true;
		if (m_fTrackPos > 190.f && m_fTrackPos < 192.f)
		{
			if(!m_bExecuted[0])
			{
				m_bExecuted[0] = true;
				m_bAttack = true;
				EFFECTDESC desc;
				desc.texName = TEXT("Smoke2");
				PxTransform pTransform = m_pBat->Get_AttackDesc()[0]->pActor->getGlobalPose();
				desc.vScale = { 5.f,5.f,5.f };
				desc.vPos = { pTransform.p.x,pTransform.p.y,pTransform.p.z,1 };
				desc.iRow = 8;
				CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
					TEXT("Prototype_GameObject_Effect_Texture"), &desc);
				
				m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
				m_pGameInstance->Play_Sound(L"BossBat_Attack_01_2a.wav", SOUND_ENEMY_BOSS, 0.4f);
			}
		}
		else
			m_bAttack = false;
		break;
	case ATTACK_R:
		_player.Get_AttackDesc()[1]->bActive = true;
		if (m_fTrackPos >95.f && m_fTrackPos < 97.f)
		{
			if (!m_bExecuted[1])
			{
				m_bExecuted[1] = true;
				m_bAttack = true;
				EFFECTDESC desc;
				desc.texName = TEXT("Smoke2");
				PxTransform pTransform = m_pBat->Get_AttackDesc()[1]->pActor->getGlobalPose();
				desc.vScale = { 5.f,5.f,5.f };
				desc.vPos = { pTransform.p.x,pTransform.p.y,pTransform.p.z,1 };
				desc.iRow = 8;
				CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
					TEXT("Prototype_GameObject_Effect_Texture"), &desc);
			
				m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
				m_pGameInstance->Play_Sound(L"BossBat_Attack_01_2a.wav", SOUND_ENEMY_BOSS, 0.4f);
			}
		}
		else
			m_bAttack = false;
		break;
	case ATTACK_R_HARD:
		_player.Get_AttackDesc()[1]->bActive = true;
		if (m_fTrackPos > 110.f && m_fTrackPos < 112.f)
		{
			if (!m_bExecuted[2])
			{
				m_bExecuted[2] = true;
				m_bAttack = true;
				EFFECTDESC desc;
				desc.texName = TEXT("Smoke2");
				PxTransform pTransform = m_pBat->Get_AttackDesc()[1]->pActor->getGlobalPose();
				desc.vScale = { 5.f,5.f,5.f };
				desc.vPos = { pTransform.p.x,pTransform.p.y,pTransform.p.z,1 };
				desc.iRow = 8;
				CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
					TEXT("Prototype_GameObject_Effect_Texture"), &desc);

				m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
				m_pGameInstance->Play_Sound(L"BossBat_Attack_01_2a.wav", SOUND_ENEMY_BOSS, 0.4f);
			}
		}
		else
			m_bAttack = false;
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

	//m_vPreRootPos = m_pModelCom->GetRootPos();

	__super::Update(_player, fTimeDelta);
	//__super::RootPosUpdate(_player, 0.0f, fTimeDelta);
	__super::PhysicsUpdate(_player, fTimeDelta);

	if(!m_pModelCom->Get_CurrentAnim()->GetEnd() &&  m_fTrackPos < 50.f)
		m_pTransformCom->LookAtDir(m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom));
}

void CBatAttackState::Exit(CMonster& _player)
{
	__super::Exit(_player);
	m_bAttack = false;
}

void CBatAttackState::Free()
{
}
