#include "Client_Pch.h"
#include "..\Public\PlayerPlunderState.h"
#include "PlayerIdleState.h"
#include "Player.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Transform.h"
#include "Camera_Player.h"
#include "Monster.h"

CPlayerPlunderState::CPlayerPlunderState()
{
}

CPlayerPlunderState::~CPlayerPlunderState()
{
}

CPlayerState* CPlayerPlunderState::Transition(CPlayer& _player)
{
	CPlayerState* pState = __super::Transition(_player);
	if (nullptr != pState)
		return pState;

	if (_player.Get_Model()->Get_CurrentAnim()->GetInterpolateEnd())
	{
		return new CPlayerIdleState();
	}
	return nullptr;
}

void CPlayerPlunderState::Entry(CPlayer& _player)
{
	__super::Entry(_player);

	m_szName = "Plunder";
	m_pModelCom->SetUp_Animation(PLUNDER, false);
	m_pModelCom->Set_NextAnim(IDLE);
	_player.Set_Weapon(CPlayer::NONE);
}

void CPlayerPlunderState::Update(CPlayer& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

	if (m_fTrackPos > 12.f && m_fTrackPos < 20.f)
		m_bAttack = true;
	else if (m_fTrackPos > 50.f && m_fTrackPos < 60.f)
		m_bAttack = true;
	else 
		m_bAttack = false;

	if (m_fTrackPos > 12.f && !m_bExecuted[0])
	{
		m_bExecuted[0] = true;
		m_pGameInstance->StopSound(SOUND_PLAYER_ATTACK);
		m_pGameInstance->Play_Sound(L"Blood01_VS_TakeExcution-001.wav", SOUND_PLAYER_ATTACK, 0.6f);
	}

	if (m_fTrackPos > 50.f && !m_bExecuted[1])
	{
		m_bExecuted[1] = true;
		m_pGameInstance->StopSound(SOUND_PLAYER_ATTACK);
		m_pGameInstance->Play_Sound(L"Blood01_VS_TakeExcution-002.wav", SOUND_PLAYER_ATTACK, 0.6f);
	}

	_player.Set_ControllerHit(false);
	__super::Update(_player, fTimeDelta);
	__super::RootPosUpdate(_player, 0.5f, fTimeDelta);
	__super::PhysicsUpdate(_player, fTimeDelta);
}

void CPlayerPlunderState::Exit(CPlayer& _player)
{
	__super::Exit(_player);
	m_bAttack = false;
	_player.Set_Weapon(CPlayer::DEFAULT);
}

void CPlayerPlunderState::Free()
{
}
