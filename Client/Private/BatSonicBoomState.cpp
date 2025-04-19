#include "Client_Pch.h"
#include "BatIdleState.h"
#include "BatWalkState.h"
#include "BatScreamState.h"
#include "BatDeadState.h"	
#include "BatSonicBoomState.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Bone.h"
#include "Player.h"
#include "Camera_Player.h"
#include "Bat.h"
#include "Layer.h"

CBatSonicBoomState::CBatSonicBoomState()
{
}

CBatSonicBoomState::~CBatSonicBoomState()
{
}

CMonsterState* CBatSonicBoomState::Transition(CMonster& _player)
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
		if (m_iAttackCount <= 0)
		{
			m_iAttackCount = 2;
			return new CBatIdleState();
		}
	}
	return nullptr;
}

void CBatSonicBoomState::Entry(CMonster& _player)
{
	__super::Entry(_player);
	m_pModelCom->SetUp_Animation(SONICBOOM_BEFORE, false);
	m_pModelCom->Set_NextAnim(SONICBOOM);
	_player.m_pattern[4] = true;
}

void CBatSonicBoomState::Update(CMonster& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

	m_vPreRootPos = m_pModelCom->GetRootPos();

	__super::Update(_player, fTimeDelta);

	if (m_pModelCom->Get_CurrentAnimIndex() == SONICBOOM_BEFORE)
	{
		if (!m_bSideColided)
		{
			_vector vRot = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_vector v = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
			_matrix m = XMMatrixRotationQuaternion(v);

			_vector vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float4 vPos;
			XMStoreFloat4(&vPos, vCurPos - XMVector3TransformNormal(XMLoadFloat4(&m_pModelCom->GetRootPos()) - XMLoadFloat4(&m_vPreRootPos), m) * 1.f * 0.016f);
			_float4 vPosition = { vPos.x,m_vPrePos.y,vPos.z,1.f };
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));
		}

		if (m_fTrackPos > m_fDuration - 10.f && !m_bTurn)
		{
			m_bTurn = true;
			m_pModelCom->Get_CurrentAnim()->ResetEnd();
			m_pModelCom->SetUp_Animation(SONICBOOM, false);
			m_pModelCom->Set_NextAnim(IDLE);
			m_vPreRootPos = { 0,0,0,1 };
			m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
		}
		if (m_fTrackPos > 100.f && !m_bExecuted[0])
		{
			m_bExecuted[0] = true;
			m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
			m_pGameInstance->Play_Sound(L"BossBat_WingAction_01.wav", SOUND_ENEMY_BOSS, 1.f);
		}
	}

	if (m_pModelCom->Get_CurrentAnimIndex() == SONICBOOM)
	{
		if (m_fTrackPos > 200.f && m_fTrackPos < 210.f)
		{
			m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
			m_pGameInstance->Play_Sound(L"BossBat_SonicBoom-007.wav", SOUND_ENEMY_BOSS, 1.f);
		}

		if (m_fTrackPos > 240.f && m_bTurn)
		{
			m_bTurn = false;
			CGameInstance::Get_Instance()->Add_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), TEXT("Prototype_GameObject_B_Bat_Boomball"));
			CLayer* pLayer = m_pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
			CTransform* pTransform = static_cast<CTransform*>(m_pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), pLayer->size() - 1)->Get_Component(TEXT("Com_Transform")));
			pTransform->Scaled({10.f, 10.f, 10.f});
			pTransform->LookAtDir(m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom));
			pTransform->Set_State(CTransform::STATE_POSITION, m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0,1,0,0));
		}

		if (m_pModelCom->Get_CurrentAnim()->GetEnd() && m_iAttackCount > 0)
		{
			m_pModelCom->Get_CurrentAnim()->ResetEnd();
			m_bTurn = true;
			m_iAttackCount--;
		}

	}

	__super::PhysicsUpdate(_player, fTimeDelta);
	m_pTransformCom->LookAtDir(m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom));
}

void CBatSonicBoomState::Exit(CMonster& _player)
{
	__super::Exit(_player);
}

void CBatSonicBoomState::Free()
{
}
