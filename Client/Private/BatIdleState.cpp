#include "Client_Pch.h"
#include "BatIdleState.h"
#include "BatWalkState.h"
#include "BatBiteState.h"
#include "BatAttackState.h"
#include "BatChargeState.h"
#include "BatScreamState.h"
#include "BatJumpState.h"
#include "BatSonicBoomState.h"
#include "BatDeadState.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Bone.h"
#include "Player.h"
#include "Camera_Player.h"
#include "Bat.h"

CBatIdleState::CBatIdleState()
{
}

CBatIdleState::~CBatIdleState()
{
}

CMonsterState* CBatIdleState::Transition(CMonster& _player)
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

	_bool bPossible = false;
	for (auto& elem : _player.m_pattern)
	{
		if (!elem)
			bPossible = true;
	}

	if (!bPossible)
	{
		for (auto& elem : _player.m_pattern)
		{
			elem = false;
		}
	}

	if (!m_bRotStart && bPossible)
	{
		if(m_iAttackIdx!=0 || (m_iAttackCount < 0))
		{
			m_iAttackCount = 2;
			m_iAttackIdx = rand() % 5;
			while (_player.m_pattern[m_iAttackIdx])
			{
				m_iAttackIdx = rand() % 5;
			}
		}

		if (m_pBat->GetPhaseTurn())
		{
			m_iAttackIdx = 2;
			if (m_iChargeCount < 0)
			{
				m_pBat->SetPhaseTurn(false);
			}
		}

		switch (m_iAttackIdx)
		{
		case 0:
			if (m_pTransformCom->Distance(m_pPlayerTransformCom) < 10.f)
				return new CBatAttackState;
			else
				return new CBatWalkState;
			break;
		case 1:
			if (m_pTransformCom->Distance(m_pPlayerTransformCom) < 15.f)
				return new CBatBiteState;
			else
				return new CBatWalkState;
			break;
		case 2:
			if (m_pTransformCom->Distance(m_pPlayerTransformCom) < 40.f)
				return new CBatChargeState;
			else
				return new CBatWalkState;
			break;
		case 3:
			if (m_pTransformCom->Distance(m_pPlayerTransformCom) < 15.f)
				return new CBatJumpState;
			else
				return new CBatWalkState;
			break;
		case 4:
			if (m_pTransformCom->Distance(m_pPlayerTransformCom) < 15.f)
				return new CBatSonicBoomState;
			else
				return new CBatWalkState;
			break;
		}
	}

	return nullptr;
}

void CBatIdleState::Entry(CMonster& _player)
{
		__super::Entry(_player);


		/*if (m_fTurnAngle > 0.9f)
			m_fTurnAngle = 0.9f;*/

			if (m_fLRAngle < 0.f)
				m_pModelCom->SetUp_Animation(TURN_L, false);
			else
				m_pModelCom->SetUp_Animation(TURN_R, false);
			m_bRotStart = true;
			if (m_fLRAngle < 0) // 哭率
			{
				if (m_fTurnAngle > 0) //菊率
					m_fTurnTime = 1 - abs(m_fTurnAngle);
				else //第率
					m_fTurnTime = 1 + abs(m_fTurnAngle);
			}
			else // 坷弗率
			{
				if (m_fTurnAngle > 0) //菊率
					m_fTurnTime = 1 - abs(m_fTurnAngle);
				else //第率
					m_fTurnTime = 1 + abs(m_fTurnAngle);
			}
			m_fTurnTime *= 0.5f;
		
		
}

void CBatIdleState::Update(CMonster& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_vCurDir = m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom);
	
		__super::Update(_player, fTimeDelta);
		__super::PhysicsUpdate(_player, fTimeDelta);


}

void CBatIdleState::Exit(CMonster& _player)
{
	__super::Exit(_player);
}

void CBatIdleState::Free()
{
}
