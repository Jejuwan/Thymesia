#include "Client_Pch.h"
#include "PlayerIdleState.h"
#include "PlayerAttackState.h"
#include "PlayerWalkState.h"
#include "PlayerParryState.h"
#include "PlayerRavenState.h"
#include "PlayerPlagueState.h"
#include "PlayerExecuteState.h"
#include "Player.h"
#include "Monster.h"
#include "MonsterState.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"

CPlayerIdleState::CPlayerIdleState()
{
}

CPlayerIdleState::~CPlayerIdleState()
{
}

CPlayerState* CPlayerIdleState::Transition(CPlayer& _player)
{
	CPlayerState* pState = __super::Transition(_player);
	if (nullptr != pState)
		return pState;

	if (m_pGameInstance->Keyboard_Pressing('W') ||
		m_pGameInstance->Keyboard_Pressing('A') ||
		m_pGameInstance->Keyboard_Pressing('S') ||
		m_pGameInstance->Keyboard_Pressing('D'))
		return new CPlayerWalkState();

	if (m_pGameInstance->Mouse_Down(CInput_Device::DIMK_LBUTTON))
		return new CPlayerAttackState();

	if (m_pGameInstance->Mouse_Down(CInput_Device::DIMK_RBUTTON))
		return new CPlayerRavenState();

	if (m_pGameInstance->Keyboard_Down('Q'))
		return new CPlayerParryState(PARRY1);

	if (m_pGameInstance->Keyboard_Down('2') && -1!= _player.Get_StealType())
	{
		switch (_player.Get_StealType())
		{
		case CMonster::VILLAGER_M:
			return new CPlayerPlagueState(AXE1);
			break;
		case CMonster::VILLAGER_F:
			return new CPlayerPlagueState(KNIFE);
			break;
		case CMonster::GARDENER:
			return new CPlayerPlagueState(SCYTHE1);
			break;
		case CMonster::JOKER:
			return new CPlayerPlagueState(HAMMER1);
			break;
		}
	}

	if (m_pGameInstance->Keyboard_Down('3'))
	{
		_player.Set_StealType(0);
	}
	if (m_pGameInstance->Keyboard_Down('4'))
	{
		_player.Set_StealType(1);
	}
	if (m_pGameInstance->Keyboard_Down('5'))
	{
		_player.Set_StealType(2);
	}
	if (m_pGameInstance->Keyboard_Down('6'))
	{
		_player.Set_StealType(3);
	}

	return nullptr;
}

void CPlayerIdleState::Entry(CPlayer& _player)
{
	__super::Entry(_player);
	m_pModelCom->SetUp_Animation(IDLE, true);
}

void CPlayerIdleState::Update(CPlayer& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos,m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	__super::Update(_player,fTimeDelta);
	__super::PhysicsUpdate(_player, fTimeDelta);
}

void CPlayerIdleState::Exit(CPlayer& _player)
{
	__super::Exit(_player);
}

void CPlayerIdleState::Free()
{
}
