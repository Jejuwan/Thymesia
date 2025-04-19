#include "Client_Pch.h"
#include "BatIdleState.h"
#include "BatWalkState.h"
#include "BatScreamState.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Bone.h"
#include "Player.h"
#include "Camera_Player.h"
#include "Bat.h"

CBatScreamState::CBatScreamState()
{
}

CBatScreamState::~CBatScreamState()
{
}

CMonsterState* CBatScreamState::Transition(CMonster& _player)
{
	if (m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd())
		return new CBatIdleState;
	return nullptr;
}

void CBatScreamState::Entry(CMonster& _player)
{
	__super::Entry(_player);
	m_pModelCom->SetUp_Animation(FLYSCREAM, false);
	m_pModelCom->Set_NextAnim(IDLE);
	m_pBat->SetPhaseTurn(true);
}

void CBatScreamState::Update(CMonster& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

	if (m_fTrackPos > 270.f && m_fTrackPos < 300.f)
	{
		if(!m_bExecuted[0])
		{
			m_bExecuted[0] = true;
			m_pPlayer->Get_HitDesc().bHit = true;
			m_pPlayer->Get_HitDesc().eType = HIT_TYPE::HEADACHE;

		}
	}

	if (m_fTrackPos > 240.f && m_fTrackPos < 300.f && !m_bExecuted[1])
	{
		m_bExecuted[1] = true;
		m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
		m_pGameInstance->Play_Sound(L"BossBat_Yell.wav", SOUND_ENEMY_BOSS, 1.f);
	}

	if (m_fTrackPos > 40.f &&!m_bExecuted[2])
	{
		m_bExecuted[2] = true;
		m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
		m_pGameInstance->Play_Sound(L"BossBat_Death-007.wav", SOUND_ENEMY_BOSS, 1.f);
	}
	m_vPreRootPos = m_pModelCom->GetRootPos();

	__super::Update(_player, fTimeDelta);
	__super::RootPosUpdate(_player, 1.f, fTimeDelta);
	__super::PhysicsUpdate(_player, fTimeDelta);


}

void CBatScreamState::Exit(CMonster& _player)
{
	__super::Exit(_player);

	m_pModelCom->Set_Animation_Speed(6.f);
	m_pModelCom->Get_Anim(11)->Set_Speed(3.f);
	m_pModelCom->Get_Anim(14)->Set_Speed(2.f);
	m_pModelCom->Get_Anim(15)->Set_Speed(3.f);
	m_pModelCom->Get_Anim(10)->Set_Speed(8.f);
}

void CBatScreamState::Free()
{
}
