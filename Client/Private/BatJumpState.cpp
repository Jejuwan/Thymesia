#include "Client_Pch.h"
#include "BatIdleState.h"
#include "BatWalkState.h"
#include "BatJumpState.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Bone.h"
#include "Player.h"
#include "Camera_Player.h"
#include "Bat.h"

CBatJumpState::CBatJumpState()
{
}

CBatJumpState::~CBatJumpState()
{
}

CMonsterState* CBatJumpState::Transition(CMonster& _player)
{
	if (m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd())
		return new CBatIdleState;
	return nullptr;
}

void CBatJumpState::Entry(CMonster& _player)
{
	__super::Entry(_player);
	m_pModelCom->SetUp_Animation(rand() % 2 + JUMPATK, false);
	m_pModelCom->Set_NextAnim(IDLE);
	_player.m_pattern[3] = true;
	m_pTransformCom->LookAtDir(m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom));
}

void CBatJumpState::Update(CMonster& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();


	//_float fTime = 80.f;
	//if (m_pBat->GetPhaseTurn())
	//	fTime = 50.f;

		switch (m_pModelCom->Get_CurrentAnimIndex())
		{
		case JUMPATK:

			if (m_fTrackPos > 80.f && m_fTrackPos < 110.f)
			{
				if (!m_bExecuted[0])
				{
					m_bExecuted[0] = true;
					m_bAttack = true;
					EFFECTDESC desc;
					desc.texName = TEXT("Smoke4");
					_float3 vPos;
					XMStoreFloat3(&vPos,m_pTransformCom->Get_State(CTransform::STATE_POSITION));
					desc.vScale = { 15.f,15.f,15.f };
					desc.vPos = { vPos.x,vPos.y,vPos.z,1 };
					desc.iRow = 12;
					CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
						TEXT("Prototype_GameObject_Effect_Texture"), &desc);

					if (m_bControllerHit)
					{
						m_pPlayer->Get_HitDesc().bHit = true;
						m_pPlayer->Get_HitDesc().eType = HIT_TYPE::FALLDOWN;
					}
				}
			}

			if (m_fTrackPos > 0.f && !m_bExecuted[1])
			{
				m_bExecuted[1] = true;
				m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
				m_pGameInstance->Play_Sound(L"BossBat_JumpSmash_Chest.wav", SOUND_ENEMY_BOSS, 1.f);
			}
			break;
		case JUMPATK_INPLACE:
			if (m_fTrackPos > 170.f && m_fTrackPos < 200.f)
			{
				if (!m_bExecuted[0])
				{
					m_bExecuted[0] = true;
					m_bAttack = true;
					EFFECTDESC desc;
					desc.texName = TEXT("Smoke4");
					_float3 vPos;
					XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
					desc.vScale = { 15.f,15.f,15.f };
					desc.vPos = { vPos.x,vPos.y,vPos.z,1 };
					desc.iRow = 12;
					CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
						TEXT("Prototype_GameObject_Effect_Texture"), &desc);

					if (m_bControllerHit)
					{
						m_pPlayer->Get_HitDesc().bHit = true;
						m_pPlayer->Get_HitDesc().eType = HIT_TYPE::FALLDOWN;
					}
				}
			}

			if (m_fTrackPos > 0.f && !m_bExecuted[1])
			{
				m_bExecuted[1] = true;
				m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
				m_pGameInstance->Play_Sound(L"BossBat_JumpSmash_Chest.wav", SOUND_ENEMY_BOSS, 1.f);
			}
			break;
		}

	m_vPreRootPos = m_pModelCom->GetRootPos();
	__super::Update(_player, fTimeDelta);
	__super::RootPosUpdate(_player,0.2f, fTimeDelta);
	__super::PhysicsUpdate(_player, fTimeDelta);
}

void CBatJumpState::Exit(CMonster& _player)
{
	__super::Exit(_player);
	m_bControllerCollide = true;
	m_pPlayer->Get_HitDesc().eType = HIT_TYPE::DEFAULT;
}

void CBatJumpState::Free()
{
}
