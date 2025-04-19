#include "Client_Pch.h"
#include "BatIntroState.h"
#include "BatIdleState.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Bone.h"
#include "Player.h"
#include "Camera_Player.h"
#include "Bat.h"
#include "UI.h"

CBatIntroState::CBatIntroState()
{
}

CBatIntroState::~CBatIntroState()
{
}

CMonsterState* CBatIntroState::Transition(CMonster& _player)
{
	if (fCameraMoveTime > 1.f)
		return new CBatIdleState;
	return nullptr;
}

void CBatIntroState::Entry(CMonster& _player)
{
	__super::Entry(_player);
	m_pModelCom->SetUp_Animation(INTRO, false);
	m_pModelCom->Set_NextAnim(IDLE);

	//m_pPlayer->Set_TargetMonster(m_pBat);
	m_pGameInstance->StopSound(SOUND_BGM0);
}

void CBatIntroState::Update(CMonster& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

	if (m_pGameInstance->Keyboard_Down(VK_LEFT))
	{
		m_pPlayer->Get_PlayerCamera()->Set_Camera_Fixed(true);
		m_pModelCom->Get_Anim(0)->Set_Speed(4.f);
		m_fAmountY = 2.f;
		m_bIntroAnim = true;
	}

	if (m_fTrackPos > 250 && !m_bExecuted[2])
	{
		m_bExecuted[2] = true;
		m_pGameInstance->Play_Sound(L"Bat_Growl_02.wav", SOUND_ENEMY_BOSS, 0.5f);
	}

	if (m_fTrackPos > 750.f && !m_bExecuted[0])
	{
		m_bExecuted[0] = true;
		m_pGameInstance->Play_Sound(L"Blood_Bat_-_Intro.wav", SOUND_BGM0, 1.f);
	}

	if (m_fTrackPos > m_fDuration - 1060.f && !m_bExecuted[3])
	{
		m_bExecuted[3] = true;
		m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
		m_pGameInstance->Play_Sound(L"BossBat_Wing_Flap.wav", SOUND_ENEMY_BOSS, 1.f);
	}

	if (m_fTrackPos > m_fDuration - 870.f && !m_bExecuted[4])
	{
		m_bExecuted[4] = true;
		m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
		m_pGameInstance->Play_Sound(L"BossBat_JumpSmash_Chest.wav", SOUND_ENEMY_BOSS, 0.5f);

	}

	if (m_fTrackPos > m_fDuration - 780.f && !m_bExecuted[5])
	{
		m_bExecuted[5] = true;

		EFFECTDESC desc;
		desc.texName = TEXT("Smoke4");
		_float3 vPos;
		XMStoreFloat3(&vPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		desc.vScale = { 25.f,25.f,25.f };
		desc.vPos = { vPos.x,vPos.y,vPos.z,1 };
		desc.iRow = 12;
		CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
			TEXT("Prototype_GameObject_Effect_Texture"), &desc);
	}

	if (m_fTrackPos > m_fDuration - 370.f && !m_bExecuted[1])
	{
		m_bExecuted[1] = true;
		m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
		m_pGameInstance->Play_Sound(L"BossBat_SP01_01_Yell.wav", SOUND_ENEMY_BOSS, 1.f);
	}

	if(m_pModelCom->Get_CurrentAnimIndex()==INTRO)
	{
		if(!m_pModelCom->Get_CurrentAnim()->GetEnd() && m_bIntroAnim)
		{
			_matrix worldMat = XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix());
			_float4x4 pivotMat = m_pModelCom->GetPivotMatrix();
			_matrix camMat = m_pModelCom->GetBones()[m_pModelCom->Get_BoneIndex("camera")]->Get_CombinedTransformationMatrix();

			_matrix finalMat = XMMatrixScaling(100, 100, 100) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(-90.f)) * camMat * XMLoadFloat4x4(&pivotMat) * worldMat;
			//_matrix finalMat = XMLoadFloat4x4(&pivotMat) * camMat * worldMat /** XMMatrixRotationY(XMConvertToRadians(-90.f))*/;
			_float4x4 f;
			XMStoreFloat4x4(&f, finalMat);
			f._42 += 2.f;
			m_pPlayer->Get_PlayerCamera()->Get_Transform()->Set_WorldMatrix(f);
		}


		if (m_fTrackPos > m_fDuration - 20.f && !m_bInterpolate)
		{
			_matrix		RotationMatrix = XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT) , XMConvertToRadians(3.f));
			m_vNextLook = m_pPlayerTransformCom->Get_State(CTransform::STATE_LOOK);
			XMVector3TransformNormal(m_vNextLook, RotationMatrix);
			m_vPreLook = m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_LOOK);
			m_vPreCamPos = m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
			m_bInterpolate = true;
		}

		if (m_pModelCom->Get_CurrentAnim()->GetEnd() && !m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd())
		{
			m_fAmountY -= fTimeDelta * 10.f;
		}

		if (m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd())
		{
			m_pModelCom->SetUp_Animation(IDLE, true);
		}
	}
	else if (m_pModelCom->Get_CurrentAnimIndex() == IDLE)
	{
		fCameraMoveTime += fTimeDelta;
		if (fCameraMoveTime < 1.f)
		{
			_vector vLerpLook = XMVectorLerp(m_vPreLook, m_vNextLook, fCameraMoveTime);
			m_pPlayer->Get_PlayerCamera()->Get_Transform()->LookAtDir(vLerpLook);

			_vector vCamLook = m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_LOOK);
			_vector vPlayerPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION);

	
			_vector vLerpPos = XMVectorLerp(m_vPreCamPos, vPlayerPos - vCamLook * 4.f + XMVectorSet(0, 2.f, 0, 0), fCameraMoveTime);
			m_pPlayer->Get_PlayerCamera()->Get_Transform()->Set_State(CTransform::STATE_POSITION, vLerpPos);
		}
	}


	__super::Update(_player, fTimeDelta);
	__super::PhysicsUpdate(_player, fTimeDelta);
}

void CBatIntroState::Exit(CMonster& _player)
{
	__super::Exit(_player);
	m_pPlayer->Get_PlayerCamera()->Set_Camera_Fixed(false);
	m_pGameInstance->StopSound(SOUND_BGM0);
	m_pGameInstance->Play_Sound(L"Blood_Bat_-_loop.wav", SOUND_BGM0, 0.6f);
	for(int i=4;i<=7;i++)
	{
		CUI* pUI = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), i));
		pUI->Get_Desc()->bVisible = true;
	}
}

void CBatIntroState::Free()
{
}
