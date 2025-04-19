#include "Client_Pch.h"
#include "MonsterIdleState.h"
#include "MonsterWalkState.h"
#include "MonsterAttackState.h"
#include "Monster.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"

CMonsterIdleState::CMonsterIdleState()
{
}

CMonsterIdleState::~CMonsterIdleState()
{
}

CMonsterState* CMonsterIdleState::Transition(CMonster& _monster)
{
	CMonsterState* pState = __super::Transition(_monster);
	if (nullptr != pState)
		return pState;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_float fDist = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION)
		- m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION)));

	if (fDist < 4.f && !m_bRotStart)
		return new CMonsterAttackState;

	if (fDist < 10.f && !m_bRotStart)
		return new CMonsterWalkState;

	return nullptr;
}

void CMonsterIdleState::Entry(CMonster& _player)
{
	__super::Entry(_player);
	m_pModelCom->SetUp_Animation(IDLE, true);
	m_bRotStart = true;
	m_vCurDir = m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom);
}

void CMonsterIdleState::Update(CMonster& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	__super::Update(_player, fTimeDelta);
	__super::PhysicsUpdate(_player, fTimeDelta);
}

void CMonsterIdleState::Exit(CMonster& _player)
{
	__super::Exit(_player);
}

void CMonsterIdleState::Free()
{
}
