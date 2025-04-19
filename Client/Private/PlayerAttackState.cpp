#include "Client_Pch.h"
#include "PlayerAttackState.h"
#include "PlayerIdleState.h"
#include "PlayerAvoidState.h"
#include "Player.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Transform.h"
#include "Camera_Player.h"

CPlayerAttackState::CPlayerAttackState()
{
}

CPlayerAttackState::~CPlayerAttackState()
{
}

CPlayerState* CPlayerAttackState::Transition(CPlayer& _player)
{
	CPlayerState* pState = __super::Transition(_player);
	if (nullptr != pState)
		return pState;

	if (_player.Get_Model()->Get_CurrentAnim()->GetInterpolateEnd())
		return new CPlayerIdleState();

	if (m_fTrackPos > m_fDuration - 60.f && m_pGameInstance->Keyboard_Down(VK_SPACE))
	{
		if (m_pGameInstance->Keyboard_Pressing('W'))
		{
			return new CPlayerAvoidState(AVOID_F);
		}
		if (m_pGameInstance->Keyboard_Pressing('A'))
		{
			return new CPlayerAvoidState(AVOID_L);
		}
		if (m_pGameInstance->Keyboard_Pressing('S'))
		{
			return new CPlayerAvoidState(AVOID_B);
		}
		if (m_pGameInstance->Keyboard_Pressing('D'))
		{
			return new CPlayerAvoidState(AVOID_R);
		}
	}
	return nullptr;
}

void CPlayerAttackState::Entry(CPlayer& _player)
{
	__super::Entry(_player);

	m_szName = "Attack";
	m_pModelCom->SetUp_Animation(ATK1, false);
	m_pModelCom->Set_NextAnim(IDLE);

	m_pGameInstance->StopSound(SOUND_EFFECT);
	m_pGameInstance->Play_Sound(L"Attack_Light_01.wav", SOUND_EFFECT, 1.f);
}

void CPlayerAttackState::Update(CPlayer& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

	switch (m_pModelCom->Get_CurrentAnimIndex())
	{
	case ATK1:
		if (m_fTrackPos > 10.f && m_fTrackPos < 20.f)
		{
			m_bAttack = true;
			m_iAttackIndex = 0;
		}
		else
			m_bAttack = false;
		break;
	case ATK2:
		if (m_fTrackPos > 5.f && m_fTrackPos < 15.f)
		{
			m_bAttack = true;
			m_iAttackIndex = 1;
			if(!m_bExecuted[0])
			{
				m_bExecuted[0] = true;
				m_pGameInstance->StopSound(SOUND_EFFECT);
				m_pGameInstance->Play_Sound(L"Attack_Light_01.wav", SOUND_EFFECT, 1.f);
			}
		}
		else
			m_bAttack = false;
		break;
	case ATK3:
		if (m_fTrackPos > 20.f && m_fTrackPos < 30.f)
		{
			m_bAttack = true;
			m_iAttackIndex = 2;
			if (!m_bExecuted[1])
			{
				m_bExecuted[1] = true;
				m_pGameInstance->StopSound(SOUND_EFFECT);
				m_pGameInstance->Play_Sound(L"Attack_Light_01.wav", SOUND_EFFECT, 1.f);
			}
		}
		else
			m_bAttack = false;
		break;
	}
	if(!m_bInterpolate)
	{
		if (m_fTrackPos > m_fDuration - 25.f)
		{
			m_pModelCom->Set_NextAnim(IDLE);
			m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
		}

		if (m_pGameInstance->Mouse_Down(CInput_Device::DIMK_LBUTTON))
		{
			switch (m_pModelCom->Get_CurrentAnimIndex())
			{
			case ATK1:
				if (m_fTrackPos > m_fDuration - 60.f && m_fTrackPos < m_fDuration - 30.f)
				{
					m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
					m_bInterpolate = true;
					m_pModelCom->Set_NextAnim(ATK2);
				}
				break;
			case ATK2:
				if (m_fTrackPos > m_fDuration - 80.f && m_fTrackPos < m_fDuration - 50.f)
				{
					m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
					m_bInterpolate = true;
					m_pModelCom->Set_NextAnim(ATK3);
				}
				break;
			}
		}	
	}

	__super::Update(_player, fTimeDelta);
	__super::RootPosUpdate(_player, 1.f, fTimeDelta);
	__super::PhysicsUpdate(_player, fTimeDelta);

	/*CTransform* mon = _player.Get_PlayerCamera()->GetLockonTransform();
	if (nullptr != mon)
	{
		m_pTransformCom->LookAtDir(m_pTransformCom->Get_HoriziontalDir(mon));
	}*/

	if (m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd() && m_bInterpolate)
	{
		XMStoreFloat4(&m_vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pModelCom->Get_CurrentAnim()->ResetEnd();
		m_pModelCom->SetUp_Animation(m_pModelCom->Get_NextAnimIndex(), false);
		m_bInterpolate = false;
		_player.Set_ControllerHit(false);
		m_bAttackChange = true;
	}
}

void CPlayerAttackState::Exit(CPlayer& _player)
{
	__super::Exit(_player);
}

void CPlayerAttackState::Free()
{
}
