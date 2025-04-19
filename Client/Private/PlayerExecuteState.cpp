#include "Client_Pch.h"
#include "PlayerAvoidState.h"
#include "PlayerAttackState.h"
#include "PlayerWalkState.h"
#include "PlayerIdleState.h"
#include "Player.h"
#include "Model.h"
#include "Bone.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Transform.h"
#include "Camera_Player.h"
#include "PlayerExecuteState.h"

CPlayerExecuteState::CPlayerExecuteState()
{
}

CPlayerExecuteState::~CPlayerExecuteState()
{
}

CPlayerState* CPlayerExecuteState::Transition(CPlayer& _player)
{
	if (fCameraMoveTime > 1.f)
	{
		return new CPlayerIdleState;
	}
	return nullptr;
}

void CPlayerExecuteState::Entry(CPlayer& _player)
{
	__super::Entry(_player);
	m_pModelCom->SetUp_Animation(BAT_EXECUTE, false);
	m_pModelCom->Set_NextAnim(IDLE);
	_player.Get_PlayerCamera()->Set_Camera_Fixed(true);
}

void CPlayerExecuteState::Update(CPlayer& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

	if (m_pModelCom->Get_CurrentAnimIndex() == BAT_EXECUTE && !m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd())
	{
		m_bIntroAnim = true;
		if (!m_pModelCom->Get_CurrentAnim()->GetEnd()&& m_fTrackPos> 5.f)
		{
			_matrix worldMat = XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix());
			_float4x4 pivotMat = m_pModelCom->GetPivotMatrix();
			_matrix camMat = m_pModelCom->GetBones()[m_pModelCom->Get_BoneIndex("camera")]->Get_CombinedTransformationMatrix();

			_matrix finalMat = XMMatrixScaling(100, 100, 100) * XMMatrixRotationX(XMConvertToRadians(-90.f)) * XMMatrixRotationZ(XMConvertToRadians(-90.f)) * camMat * XMLoadFloat4x4(&pivotMat) * worldMat;
			//_matrix finalMat = XMLoadFloat4x4(&pivotMat) * camMat * worldMat /** XMMatrixRotationY(XMConvertToRadians(-90.f))*/;
			_float4x4 f;
			XMStoreFloat4x4(&f, finalMat);
			_player.Get_PlayerCamera()->Get_Transform()->Set_WorldMatrix(f);
		}


		if (m_fTrackPos > m_fDuration - 20.f && !m_bInterpolate)
		{
			_matrix		RotationMatrix = XMMatrixRotationAxis(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(3.f));
			m_vNextLook = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			XMVector3TransformNormal(m_vNextLook, RotationMatrix);
			m_vPreLook = _player.Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_LOOK);
			m_vPreCamPos = _player.Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_POSITION);
			m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
			m_bInterpolate = true;
		}

		if (m_pModelCom->Get_CurrentAnim()->GetEnd() && !m_bExecuteEnd)
		{
			m_bExecuteEnd = true;
			XMStoreFloat4(&m_vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		}
		if (m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd())
		{
			m_pModelCom->SetUp_Animation(IDLE, true);
		}
	}
	if (m_pModelCom->Get_CurrentAnimIndex() == BAT_EXECUTE && m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd())
	{
		m_bIntroAnim = false;
		fCameraMoveTime += fTimeDelta;
		if (fCameraMoveTime < 1.f)
		{
			_vector vLerpLook = XMVectorLerp(m_vPreLook, m_vNextLook, fCameraMoveTime);
			_player.Get_PlayerCamera()->Get_Transform()->LookAtDir(vLerpLook);

			_vector vCamLook = _player.Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_LOOK);
			_vector vPlayerPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);


			_vector vLerpPos = XMVectorLerp(m_vPreCamPos, vPlayerPos - vCamLook * 4.f + XMVectorSet(0, 2.f, 0, 0), fCameraMoveTime);
			_player.Get_PlayerCamera()->Get_Transform()->Set_State(CTransform::STATE_POSITION, vLerpPos);
		}
	}
	__super::Update(_player, fTimeDelta);
	_vector vRot = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	_vector v = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
	_matrix m = XMMatrixRotationQuaternion(v);

	_float fAmount = 0.15f;
	if (m_pModelCom->Get_CurrentAnimIndex() == BAT_EXECUTE && !m_pModelCom->Get_CurrentAnim()->GetEnd())
	{
		/*if(m_fTrackPos > m_fDuration - 100.f)
			fAmount = 2.f;*/
		_float4 vPos;
		XMStoreFloat4(&vPos, XMLoadFloat4(&m_vInitPos) - XMVector3TransformNormal(XMLoadFloat4(&m_pModelCom->GetRootPos()) * fAmount, m) * 0.0166f);
		_float4 vPosition = { vPos.x,m_vPrePos.y,vPos.z,1.f };
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));
	}
	__super::PhysicsUpdate(_player, fTimeDelta);
}

void CPlayerExecuteState::Exit(CPlayer& _player)
{
	__super::Exit(_player);
	_player.Get_PlayerCamera()->Set_Camera_Fixed(false);
}

void CPlayerExecuteState::Free()
{
}
