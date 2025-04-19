#include "Client_Pch.h"
#include "BatIdleState.h"
#include "BatWalkState.h"
#include "BatStunState.h"
#include "BatDeadState.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Bone.h"
#include "Player.h"
#include "PlayerState.h"
#include "Camera_Player.h"
#include "Bat.h"
#include "Layer.h"

CBatStunState::CBatStunState()
{
}

CBatStunState::~CBatStunState()
{
}

CMonsterState* CBatStunState::Transition(CMonster& _player)
{
	if (_player .Get_MonsterHit() && m_pPlayer->Get_PlayerState()->Get_Attack())
	{
		m_pModelCom->Get_CurrentAnim()->ResetEnd();
		return new CBatDeadState;
	}
	return nullptr;
}

void CBatStunState::Entry(CMonster& _player)
{
	__super::Entry(_player);

	m_szName = "Stun";
	m_pModelCom->SetUp_Animation(STUNSTART, false);
}

void CBatStunState::Update(CMonster& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

	if(m_fTrackPos < m_fDuration - 5.f)
	{
		__super::Update(_player, fTimeDelta);
		__super::PhysicsUpdate(_player, fTimeDelta);
	}
}

void CBatStunState::Exit(CMonster& _player)
{
	__super::Exit(_player);
}

void CBatStunState::Free()
{
}
