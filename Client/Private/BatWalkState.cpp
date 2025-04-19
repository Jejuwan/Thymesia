#include "Client_Pch.h"
#include "MonsterIdleState.h"
#include "BatWalkState.h"
#include "BatAttackState.h"
#include "BatBiteState.h"
#include "BatChargeState.h"
#include "BatScreamState.h"
#include "BatSonicBoomState.h"
#include "BatStunState.h"
#include "BatJumpState.h"
#include "BatIdleState.h"
#include "BatDeadState.h"
#include "Monster.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"

CBatWalkState::CBatWalkState()
{
}

CBatWalkState::~CBatWalkState()
{
}

CMonsterState* CBatWalkState::Transition(CMonster& _player)
{
	switch (m_iAttackIdx)
	{
	case 0:
		if (m_pTransformCom->Distance(m_pPlayerTransformCom) < 10.f)
			return new CBatAttackState;
		break;
	case 1:
		if (m_pTransformCom->Distance(m_pPlayerTransformCom) < 15.f)
			return new CBatBiteState;
		break;
	case 2:
		if (m_pTransformCom->Distance(m_pPlayerTransformCom) < 40.f)
			return new CBatChargeState;
		break;	
	case 3:
		if (m_pTransformCom->Distance(m_pPlayerTransformCom) < 15.f)
			return new CBatJumpState;
		break;
	case 4:
		if (m_pTransformCom->Distance(m_pPlayerTransformCom) < 15.f)
			return new CBatSonicBoomState;
		break;
	}
	return nullptr;
}

void CBatWalkState::Entry(CMonster& _player)
{
	__super::Entry(_player);
	m_pModelCom->SetUp_Animation(WALK, true);
}

void CBatWalkState::Update(CMonster& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));


	//m_vPreDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	//m_vCurDir = m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom);

	//_vector v = XMVectorLerp(m_vPreDir, m_vCurDir, 0.1f);

	m_pTransformCom->LookAtDir(m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom));
	m_pTransformCom->Chase(vPlayerPos, .7f, fTimeDelta);


	__super::Update(_player, fTimeDelta);
	__super::PhysicsUpdate(_player, fTimeDelta);

	if (m_fStepSoundTime < 0.f)
	{
		m_pGameInstance->Play_Sound(L"FootStep_Heavy_01.wav", SOUND_ENEMY1_WALK, 0.3f);
		m_fStepSoundTime = 0.1f;
	}

	if (!m_pGameInstance->IsPlaying(SOUND_PLAYER_WALK))
	{
		m_fStepSoundTime -= fTimeDelta;
	}
}

void CBatWalkState::Exit(CMonster& _player)
{
	__super::Exit(_player);
}

void CBatWalkState::Free()
{
}
