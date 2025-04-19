#include "Client_Pch.h"
#include "MonsterIdleState.h"
#include "MonsterWalkState.h"
#include "MonsterAttackState.h"
#include "Monster.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"

CMonsterWalkState::CMonsterWalkState()
{
}

CMonsterWalkState::~CMonsterWalkState()
{
}

CMonsterState* CMonsterWalkState::Transition(CMonster& _monster)
{
	CMonsterState* pState = __super::Transition(_monster);
	if (nullptr != pState)
		return pState;

	if (m_pTransformCom->Distance(m_pPlayerTransformCom) > 10.f)
		return new CMonsterIdleState;

	if (m_pTransformCom->Distance(m_pPlayerTransformCom) < 4.f)
		return new CMonsterAttackState;

	return nullptr;
}

void CMonsterWalkState::Entry(CMonster& _player)
{
	__super::Entry(_player);
	m_pModelCom->SetUp_Animation(WALK, true);
}

void CMonsterWalkState::Update(CMonster& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));


	m_vPreDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);
	m_vCurDir = m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom);

	_vector v = XMVectorLerp(m_vPreDir, m_vCurDir, 0.1f);

	m_pTransformCom->LookAtDir(v);
	m_pTransformCom->Chase(vPlayerPos, 0.2f, fTimeDelta);


	__super::Update(_player, fTimeDelta);
	__super::PhysicsUpdate(_player, fTimeDelta);

	if (m_fStepSoundTime < 0.f)
	{
		m_pGameInstance->Play_Sound(L"FootStep_Gardener_01.wav", SOUND_ENEMY1_WALK, 0.3f);

		m_fStepSoundTime = 0.1f;
	}

	if (!m_pGameInstance->IsPlaying(SOUND_PLAYER_WALK))
	{
		m_fStepSoundTime -= fTimeDelta;
	}
}

void CMonsterWalkState::Exit(CMonster& _player)
{
	__super::Exit(_player);
}

void CMonsterWalkState::Free()
{
}
