#include "Client_Pch.h"
#include "BatDeadState.h"
#include "BatIdleState.h"
#include "PlayerExecuteState.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Bone.h"
#include "Player.h"
#include "PlayerState.h"
#include "Camera_Player.h"
#include "Bat.h"

CBatDeadState::CBatDeadState()
{
}

CBatDeadState::~CBatDeadState()
{
}

CMonsterState* CBatDeadState::Transition(CMonster& _player)
{
	return nullptr;
}

void CBatDeadState::Entry(CMonster& _player)
{
	__super::Entry(_player);
	m_pModelCom->SetUp_Animation(DIE, false);
	m_pPlayer->Get_PlayerState()->Set_BatExecute(true);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_HoriziontalLookDir() * 5.f + m_pTransformCom->Get_State(CTransform::STATE_RIGHT) *0.4f;
	m_pPlayerTransformCom->Set_State(CTransform::STATE_POSITION, vPos);
	m_pPlayerTransformCom->LookAtDir(m_pPlayerTransformCom->Get_HoriziontalLookAtDir(m_pTransformCom));

	m_pGameInstance->Play_Sound(L"BossBat_StunStart.wav", SOUND_ENEMY1_WALK, 1.f);
}

void CBatDeadState::Update(CMonster& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

	if (m_fTrackPos > m_fDuration - 130.f && !m_bExecuted[0])
	{
		m_bExecuted[0] = true;
		CVIBuffer_Point_Instance::POINTINSTANCEDESC pDesc;
		wstring wstr = TEXT("../../SaveFiles/Particles/");
		wstr += TEXT("bloodbat");
		wstr += TEXT(".effect");
		ifstream is(wstr, ios::binary);
		is.read((char*)&pDesc, sizeof(CVIBuffer_Point_Instance::POINTINSTANCEDESC));
		is.close();

		XMStoreFloat4(&pDesc.vPivotPos, { m_pBat->Get_AttackDesc()[2]->pActor->getGlobalPose().p.x,  m_pBat->Get_AttackDesc()[2]->pActor->getGlobalPose().p.y,  m_pBat->Get_AttackDesc()[2]->pActor->getGlobalPose().p.z, 1});

		CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
			TEXT("Prototype_GameObject_Effect_Particle"), &pDesc);

		EFFECTDESC desc;
		desc.texName = TEXT("Smoke4");
		_float3 vPos;
		XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_State(CTransform::STATE_LOOK)*3.f);
		desc.vScale = { 15.f,15.f,15.f };
		desc.vPos = { vPos.x,vPos.y,vPos.z,1 };
		desc.iRow = 12;
		CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
			TEXT("Prototype_GameObject_Effect_Texture"), &desc);

		m_pGameInstance->Play_Sound(L"BossBat_Death-001.wav", SOUND_ENEMY1, 0.7f);
	}

	if (m_fTrackPos > m_fDuration - 130.f && !m_bExecuted[1])
	{
		m_bExecuted[1] = true;
		m_pGameInstance->Play_Sound(L"BossBat_Death-007.wav", SOUND_ENEMY2, 0.7f);
	}

	if (m_fTrackPos > m_fDuration - 80.f && !m_bExecuted[2])
	{
		m_bExecuted[2] = true;
		m_pGameInstance->Play_Sound(L"BossBat_Death-009.wav", SOUND_ENEMY3, 1.0f);
	}

	if (m_fTrackPos > 80.f && !m_bExecuted[3])
	{
		m_bExecuted[3] = true;
		m_pGameInstance->Play_Sound(L"Foot_Hit_01.wav", SOUND_ENEMY2_WALK, 0.7f);
	}

	if (m_fTrackPos >120.f && !m_bExecuted[4])
	{
		m_bExecuted[4] = true;
		m_pGameInstance->Play_Sound(L"Foot_Hit_02.wav", SOUND_ENEMY3_WALK, 0.7f);
	}



	if(m_fTrackPos <m_fDuration - 5.f)
	{
		__super::Update(_player, fTimeDelta);
		//__super::PhysicsUpdate(_player, fTimeDelta);
	}
}

void CBatDeadState::Exit(CMonster& _player)
{
	__super::Exit(_player);
}

void CBatDeadState::Free()
{
}
