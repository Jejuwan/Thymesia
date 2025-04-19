#include "Client_Pch.h"
#include "BatIdleState.h"
#include "BatScreamState.h"
#include "BatDeadState.h"
#include "BatWalkState.h"
#include "BatBiteState.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Bone.h"
#include "Player.h"
#include "Camera_Player.h"
#include "Bat.h"

CBatBiteState::CBatBiteState()
{
}

CBatBiteState::~CBatBiteState()
{
}

CMonsterState* CBatBiteState::Transition(CMonster& _player)
{

	if (_player.Get_Hp() < 50 && !_player.m_bScream)
	{
		_player.m_bScream = true;
		return new CBatScreamState;
	}

	if (_player.Get_Hp() < 0 && !_player.m_bDie)
	{
		_player.m_bDie = true;
		return new CBatDeadState;
	}

	if (m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd())
		return new CBatIdleState;
	return nullptr;
}

void CBatBiteState::Entry(CMonster& _player)
{
	__super::Entry(_player);
	m_pModelCom->SetUp_Animation(/*rand() % 2 + */BITE1, false);
	m_pModelCom->Set_NextAnim(IDLE);
	_player.m_pattern[1] = true;
	_player.Get_AttackDesc()[2]->bActive = true;
}

void CBatBiteState::Update(CMonster& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

	
		switch (m_pModelCom->Get_CurrentAnimIndex())
		{
		case BITE1:
			if (m_fTrackPos > 70.f && !m_bExecuted[2])
			{
				m_bExecuted[2] = true;
				m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
				m_pGameInstance->Play_Sound(L"BossBat_Bite_1.wav", SOUND_ENEMY_BOSS, 0.7f);
			}
			if (m_bControllerHit)
			{
				if (m_fTrackPos > 170.f && m_fTrackPos < 180.f)
				{
					if (!m_bExecuted[0])
					{
						m_bExecuted[0] = true;
						m_bAttack = true;
					}
				}
				if (m_fTrackPos > 320.f && m_fTrackPos < 330.f)
				{
					if (!m_bExecuted[1])
					{
						m_bExecuted[1] = true;
						m_bAttack = true;
					}
				}

				if (m_fTrackPos > 380.f)
					m_bAttack = false;
			}
			break;
		case BITE2:
			if (m_fTrackPos > 50.f && !m_bExecuted[1])
			{
				m_bExecuted[1] = true;
				m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
				m_pGameInstance->Play_Sound(L"BossBat_Bite_2.wav", SOUND_ENEMY_BOSS, 0.7f);
			}
			if (m_bControllerHit)
			{
				if (m_fTrackPos > 150.f && m_fTrackPos < 160.f)
				{
					if (!m_bExecuted[0])
					{
						m_bExecuted[0] = true;
						m_bAttack = true;
					}
				}
			}

			if (m_fTrackPos > 200.f)
				m_bAttack = false;
			break;
		}

	m_vPreRootPos = m_pModelCom->GetRootPos();

	__super::Update(_player, fTimeDelta);
	__super::RootPosUpdate(_player, 1.f, fTimeDelta);
	__super::PhysicsUpdate(_player, fTimeDelta);

	if (!m_pModelCom->Get_CurrentAnim()->GetEnd() && m_fTrackPos < m_fDuration - 50.f)
		m_pTransformCom->LookAtDir(m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom));

	if (m_fTrackPos > m_fDuration - 20.f && !m_bInterpolate)
	{
		m_bInterpolate = true;
		m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
	}
}

void CBatBiteState::Exit(CMonster& _player)
{
	__super::Exit(_player);
}

void CBatBiteState::Free()
{
}
