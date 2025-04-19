#include "Client_Pch.h"
#include "PlayerAttackState.h"
#include "PlayerRavenState.h"
#include "PlayerWalkState.h"
#include "PlayerIdleState.h"
#include "PlayerAvoidState.h"
#include "PlayerParryState.h"
#include "PlayerPlagueState.h"
#include "Player.h"
#include "Monster.h"
#include "Camera_Player.h"
#include "GameInstance.h"
#include "Animation.h"

CPlayerWalkState::CPlayerWalkState()
{
}

CPlayerWalkState::~CPlayerWalkState()
{
}

CPlayerState* CPlayerWalkState::Transition(CPlayer& _player)
{
	CPlayerState* pState = __super::Transition(_player);
	if (nullptr != pState)
		return pState;

	if(m_pGameInstance->Keyboard_Down(VK_SPACE))
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
	
	if (m_bEnd && _player.Get_Model()->Get_CurrentAnim()->GetInterpolateEnd())
	{
		m_pModelCom->Get_CurrentAnim()->ResetEnd();
		return new CPlayerIdleState();
	}

	if (m_pGameInstance->Mouse_Down(CInput_Device::DIMK_LBUTTON))
		return new CPlayerAttackState();

	if (m_pGameInstance->Mouse_Down(CInput_Device::DIMK_RBUTTON))
		return new CPlayerRavenState();

	if (m_pGameInstance->Keyboard_Down('Q'))
		return new CPlayerParryState(PARRY1);

	if (m_pGameInstance->Keyboard_Down('2') && -1 != _player.Get_StealType())
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
    return nullptr;
}

void CPlayerWalkState::Entry(CPlayer& _player)
{
	__super::Entry(_player);
	m_pModelCom->SetInterpolateTime(0.1f);
}

void CPlayerWalkState::Update(CPlayer& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	if (!m_bEnd && !m_bDirChange)
	{
		_vector vLook = m_pCameraTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector vRight = XMVector3Cross(XMVectorSet(0, 1, 0, 0), vLook);

		if (m_pGameInstance->Keyboard_Pressing('W'))
		{
			m_vDir = vLook;
			m_pModelCom->Set_NextAnim(WALK_F);
			m_iCurDir = WALK_F;
			if (m_pGameInstance->Keyboard_Pressing('A'))
			{
				m_vDir = vLook - vRight;
				m_pModelCom->Set_NextAnim(WALK_FL);
				m_iCurDir = WALK_FL;
			}
			else if (m_pGameInstance->Keyboard_Pressing('D'))
			{
				m_vDir = vLook + vRight;
				m_pModelCom->Set_NextAnim(WALK_FR);
				m_iCurDir = WALK_FR;
			}
		}
		else if (m_pGameInstance->Keyboard_Pressing('S'))
		{
			m_vDir = -vLook;
			m_pModelCom->Set_NextAnim(WALK_B);
			m_iCurDir = WALK_B;
			if (m_pGameInstance->Keyboard_Pressing('A'))
			{
				m_vDir = -vLook - vRight;
				m_pModelCom->Set_NextAnim(WALK_BL);
				m_iCurDir = WALK_BL;
			}
			else if (m_pGameInstance->Keyboard_Pressing('D'))
			{
				m_vDir = -vLook + vRight;
				m_pModelCom->Set_NextAnim(WALK_BR);
				m_iCurDir = WALK_BR;
			}
		}
		else if (m_pGameInstance->Keyboard_Pressing('A'))
		{
			m_vDir = -vRight;
			m_pModelCom->Set_NextAnim(WALK_L);
			m_iCurDir = WALK_L;
		}
		else if (m_pGameInstance->Keyboard_Pressing('D'))
		{
			m_vDir = vRight;
			m_pModelCom->Set_NextAnim(WALK_R);
			m_iCurDir = WALK_R;
		}

		if (nullptr == _player.Get_PlayerCamera()->GetLockonTransform())
		{
			m_pModelCom->Set_NextAnim(WALK_F);
			m_pModelCom->SetUp_Animation(m_pModelCom->Get_NextAnimIndex(), true);
		}
		//카메라 락온시
		else
		{
			//방향이 바뀌었을 때
			if (m_iPrevIndex != -1 && m_iPrevIndex != m_pModelCom->Get_NextAnimIndex())
			{
				m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
				m_bDirChange = true;
				if ((m_pModelCom->Get_CurrentAnimIndex() == WALK_L && m_pModelCom->Get_NextAnimIndex() == WALK_R) ||
					(m_pModelCom->Get_CurrentAnimIndex() == WALK_R && m_pModelCom->Get_NextAnimIndex() == WALK_L))
					m_pModelCom->SetInterpolateTime(0.2f);
			}
			else
				m_pModelCom->SetUp_Animation(m_pModelCom->Get_NextAnimIndex(), true);
		}

		m_vPreDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);

		_float3 modified;
		XMStoreFloat3(&modified, m_vDir);
		modified.y = 0;
		m_vModifiedDir = XMLoadFloat3(&modified);
		if (nullptr == _player.Get_PlayerCamera()->GetLockonTransform())
		{
			m_vCurDir = m_vModifiedDir;
			if (m_iPrevDir != m_iCurDir)
			{
				m_bRotStart = true;
				m_RotTime = 0.f;

				if ((m_iPrevDir == WALK_L && m_iCurDir == WALK_R) ||
					(m_iPrevDir == WALK_R && m_iCurDir == WALK_L))
				{
					_float3 modified2;
					XMStoreFloat3(&modified2, vLook);
					modified2.y = 0;
					m_vMidDir = XMLoadFloat3(&modified2);
					m_bSpin = true;
				}
				else if ((m_iPrevDir == WALK_F && m_iCurDir == WALK_B) ||
					(m_iPrevDir == WALK_B && m_iCurDir == WALK_F))
				{
					_float3 modified2;
					XMStoreFloat3(&modified2, vRight);
					modified2.y = 0;
					m_vMidDir = XMLoadFloat3(&modified2);
					m_bSpin = true;
				}
				else
					m_bSpin = false;
			}

			//회전 중
			if (m_bRotStart)
			{
				m_RotTime += fTimeDelta * 2.f;

				if (m_bSpin)
				{
					if (m_RotTime < 0.5f)
					{
						_vector v = XMVectorLerp(m_vPreDir, m_vMidDir, m_RotTime);
						m_pTransformCom->LookAtDir(v);

					}
					else
					{
						_vector v = XMVectorLerp(m_vPreDir, m_vCurDir, m_RotTime);
						m_pTransformCom->LookAtDir(v);
					}
				}
				else
				{
					_vector v = XMVectorLerp(m_vPreDir, m_vCurDir, m_RotTime);
					m_pTransformCom->LookAtDir(v);
				}

				if (m_RotTime > 1.f)
				{
					m_RotTime = 0.f;
					m_bRotStart = false;
					m_bSpin = false;
				}
				m_RotEndTime = 0.f;
			}
			//회전 끝
			else
			{
				if (m_RotEndTime < 1.f)
					m_RotEndTime += fTimeDelta * 2.f;
				m_vCurDir = m_vModifiedDir;
				m_RotTime = 0.f;
				_vector v = XMVectorLerp(m_vPreDir, m_vCurDir, m_RotEndTime);
				m_pTransformCom->LookAtDir(v);
			}
		}
		else
		{
			_vector vCamLook = m_pCameraTransformCom->Get_State(CTransform::STATE_LOOK);
			_float3 fCamLook;
			XMStoreFloat3(&fCamLook, vCamLook);
			fCamLook.y = 0.f;
			m_pTransformCom->LookAtDir(XMLoadFloat3(&fCamLook));
		}

		if (!m_pGameInstance->Keyboard_Pressing('W') &&
			!m_pGameInstance->Keyboard_Pressing('A') &&
			!m_pGameInstance->Keyboard_Pressing('S') &&
			!m_pGameInstance->Keyboard_Pressing('D'))
		{
			m_bEnd = true;
			m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
			m_pModelCom->Set_NextAnim(IDLE);
		}
	}

	_float3 dir;
	XMStoreFloat3(&dir, m_vModifiedDir);
	m_pTransformCom->Go_Dir(dir, 0.6f, fTimeDelta);

	__super::Update(_player, fTimeDelta);
	__super::PhysicsUpdate(_player, fTimeDelta);

	if (m_bLockSpin)
	{
		m_LockSpinTime += fTimeDelta;
		if (m_LockSpinTime < 0.5f)
		{
			m_pModelCom->Set_NextAnim(WALK_F);
		}
		else
		{
			m_pModelCom->Set_NextAnim(m_iLockSpinIndex);
		}

		if (m_LockSpinTime > 1.f)
		{
			m_LockSpinTime = 0.f;
			m_bLockSpin = false;
		}
	}
	//방향 전환 애니메이션이 끝났을 때
	if (m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd() && m_bDirChange && !m_bLockSpin)
	{
		if ((m_pModelCom->Get_CurrentAnimIndex() == WALK_L && m_pModelCom->Get_NextAnimIndex() == WALK_R) ||
			(m_pModelCom->Get_CurrentAnimIndex() == WALK_R && m_pModelCom->Get_NextAnimIndex() == WALK_L))
			m_pModelCom->SetInterpolateTime(0.1f);
		m_pModelCom->Get_CurrentAnim()->ResetEnd();
		m_pModelCom->SetUp_Animation(m_pModelCom->Get_NextAnimIndex(), true);
		m_bDirChange = false;
	}
	m_iPrevIndex = m_pModelCom->Get_CurrentAnimIndex();
	m_iPrevDir = m_iCurDir;

	if(m_fStepSoundTime <0.f)
	{
		m_pGameInstance->Play_Sound(L"FootStep_Magician_01.wav", SOUND_PLAYER_WALK, 1.f);
		m_fStepSoundTime = 0.1f;
	}

	if (!m_pGameInstance->IsPlaying(SOUND_PLAYER_WALK))
	{
		m_fStepSoundTime -= fTimeDelta;
	}
}

void CPlayerWalkState::Exit(CPlayer& _player)
{
	__super::Exit(_player);
	m_pModelCom->SetInterpolateTime(0.2f);
}

void CPlayerWalkState::Free()
{
}
