#include "Client_Pch.h"
#include "BatIdleState.h"
#include "BatWalkState.h"
#include "BatChargeState.h"
#include "BatScreamState.h"
#include "BatDeadState.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Bone.h"
#include "Player.h"
#include "Camera_Player.h"
#include "Bat.h"

CBatChargeState::CBatChargeState()
{
}

CBatChargeState::~CBatChargeState()
{
}

CMonsterState* CBatChargeState::Transition(CMonster& _player)
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

	if (m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd())
	{
		m_iChargeCount--;
		return new CBatIdleState;
	}
	return nullptr;
}

void CBatChargeState::Entry(CMonster& _player)
{
	__super::Entry(_player);
	m_pModelCom->SetUp_Animation(CHARGE, false);
	m_pModelCom->Set_NextAnim(IDLE);
	_player.m_pattern[2] = true;

}

void CBatChargeState::Update(CMonster& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

	if (m_bControllerHit )
	{
		m_bControllerCollide = false;
		m_bControllerHit = false;
		m_bSideColided = false;
		if (m_fTrackPos > 50.f && m_fTrackPos < m_fDuration - 50.f)
		{
			m_pPlayer->Get_HitDesc().bHit = true;
			m_pPlayer->Get_HitDesc().eType = HIT_TYPE::FALLDOWN;
		}
	}

	m_vPreRootPos = m_pModelCom->GetRootPos();

	__super::Update(_player, fTimeDelta);
	__super::RootPosUpdate(_player, 1.f, fTimeDelta);
	__super::PhysicsUpdate(_player, fTimeDelta);


	_float fTime = 150.f;
	if (m_pBat->GetPhaseTurn())
		fTime = 120.f;

	if(m_fTrackPos > fTime)
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

			m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
			m_pGameInstance->Play_Sound(L"BossBat_Attack_01_2a.wav", SOUND_ENEMY1_WALK, 0.4f);
		}
	}
	if (m_fTrackPos > fTime + 50.f)
	{
		if (!m_bExecuted[1])
		{
			m_bExecuted[1] = true;
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

			//m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
			//m_pGameInstance->Play_Sound(L"BossBat_Attack_01_2a.wav", SOUND_ENEMY2_WALK, 0.4f);
		}
	}
	if (m_fTrackPos > fTime + 100.f)
	{
		if (!m_bExecuted[2])
		{
			m_bExecuted[2] = true;
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

			m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
			m_pGameInstance->Play_Sound(L"BossBat_Attack_01_2a.wav", SOUND_ENEMY3_WALK, 0.4f);
		}
	}

	if (m_fTrackPos > fTime + 150.f)
	{
		if (!m_bExecuted[3])
		{
			m_bExecuted[3] = true;
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

			//m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
			//m_pGameInstance->Play_Sound(L"BossBat_Attack_01_3a-002.wav", SOUND_ENEMY3, 0.4f);
		}
	}


	if (!m_pModelCom->Get_CurrentAnim()->GetEnd() && m_fTrackPos < 100.f)
		m_pTransformCom->LookAtDir(m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom));

	if (m_fTrackPos > m_fDuration - 20.f && !m_bInterpolate)
	{
		m_bInterpolate = true;
		m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
	}
}

void CBatChargeState::Exit(CMonster& _player)
{
	__super::Exit(_player);
	m_bControllerCollide = true;
	m_pPlayer->Get_HitDesc().eType = HIT_TYPE::DEFAULT;
}

void CBatChargeState::Free()
{
}
