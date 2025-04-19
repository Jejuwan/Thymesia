#include "Client_Pch.h"
#include "PlayerAvoidState.h"
#include "PlayerAttackState.h"
#include "PlayerWalkState.h"
#include "PlayerIdleState.h"
#include "Player.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Transform.h"
#include "Camera_Player.h"

CPlayerAvoidState::CPlayerAvoidState(ANIM_TYPE type)
{
	m_eType = type;
}

CPlayerAvoidState::~CPlayerAvoidState()
{
	
}

CPlayerState* CPlayerAvoidState::Transition(CPlayer& _player)
{
	CPlayerState* pState = __super::Transition(_player);
	if (nullptr != pState)
		return pState;

	if (m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd())
	{
		if (m_pModelCom->Get_NextAnimIndex() == ATK1)
			return new CPlayerAttackState();
		else if (m_pModelCom->Get_NextAnimIndex() == WALK_F ||
			m_pModelCom->Get_NextAnimIndex() == WALK_L ||
			m_pModelCom->Get_NextAnimIndex() == WALK_R ||
			m_pModelCom->Get_NextAnimIndex() == WALK_B)
			return new CPlayerWalkState();

		return new CPlayerIdleState();
	}

	return nullptr;
}

void CPlayerAvoidState::Entry(CPlayer& _player)
{
	__super::Entry(_player);

	m_szName = "Avoid";
	m_pModelCom->SetUp_Animation(m_eType, false);
	if(m_eType == AVOID_L)
		m_pTransformCom->Rotation({ 0,1,0 }, XMConvertToRadians(22.5f));
	else if (m_eType == AVOID_R)
		m_pTransformCom->Rotation({ 0,1,0 }, XMConvertToRadians(-22.5f));
	m_pModelCom->Set_NextAnim(IDLE);

	m_pGameInstance->StopSound(SOUND_PLAYER_WALK);
	m_pGameInstance->Play_Sound(L"MoveWhooshSmall_01.wav", SOUND_PLAYER_WALK, 0.3f);
}

void CPlayerAvoidState::Update(CPlayer& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

	if (m_fTrackPos > m_fDuration - 30.f && !m_bInterpolate)
	{
		if (m_pGameInstance->Keyboard_Down(VK_LBUTTON))
		{
			m_bInterpolate = true;
			m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
			m_pModelCom->Set_NextAnim(ATK1);
		}
		else if (m_pGameInstance->Keyboard_Pressing('W'))
		{
			m_bInterpolate = true;
			m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
			m_pModelCom->Set_NextAnim(WALK_F);
		}
		else if (m_pGameInstance->Keyboard_Pressing('A'))
		{
			m_bInterpolate = true;
			m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
			m_pModelCom->Set_NextAnim(WALK_L);
		}
		else if (m_pGameInstance->Keyboard_Pressing('S'))
		{
			m_bInterpolate = true;
			m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
			m_pModelCom->Set_NextAnim(WALK_B);
		}
		else if (m_pGameInstance->Keyboard_Pressing('D'))
		{
			m_bInterpolate = true;
			m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
			m_pModelCom->Set_NextAnim(WALK_R);
		}
	}

	__super::Update(_player, fTimeDelta);
	__super::RootPosUpdate(_player, 0.5f, fTimeDelta);
	__super::PhysicsUpdate(_player, fTimeDelta);
}

void CPlayerAvoidState::Exit(CPlayer& _player)
{
	__super::Exit(_player);
	_player.Get_HitDesc().bHit = false;
}

void CPlayerAvoidState::Free()
{
}
