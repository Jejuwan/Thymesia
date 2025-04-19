#include "Client_Pch.h"
#include "PlayerState.h"
#include "PlayerHurtState.h"
#include "PlayerExecuteState.h"
#include "Player.h"
#include "Camera_Player.h"
#include "Transform.h"
#include "Model.h"
#include "Animation.h"
#include "GameInstance.h"

CPlayerState::CPlayerState()
{
}

CPlayerState::~CPlayerState()
{
}

CPlayerState* CPlayerState::Transition(CPlayer& _player)
{
	if (m_bBatExecute)
		return new CPlayerExecuteState;

	if (_player.Get_HitDesc().bHit)
	{
		return new CPlayerHurtState();
	}	

	return nullptr;
}

void CPlayerState::Entry(CPlayer& _player)
{
	m_pGameInstance = CGameInstance::Get_Instance();
	m_pTransformCom = static_cast<CTransform*>(_player.Find_Component(TEXT("Com_Transform")));
	if(nullptr!=_player.Get_PlayerCamera())
		m_pCameraTransformCom = static_cast<CTransform*>(_player.Get_PlayerCamera()->Get_Transform());
	m_pModelCom = static_cast<CModel*>(_player.Get_Model());
	XMStoreFloat4(&m_vInitPos,m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pModelCom->Get_CurrentAnim()->ResetEnd();

}

void CPlayerState::Update(CPlayer& _player, _float fTimeDelta)
{
	if (m_pModelCom->Get_CurrentAnim()->GetLoop())
	{
		m_pModelCom->Play_Animation(fTimeDelta);
	}
	else
	{
		if (m_pModelCom->Get_CurrentAnim()->GetEnd())
		{
			m_pModelCom->Interpolate_Animation(fTimeDelta);
		}
		else
			m_pModelCom->Play_Animation(fTimeDelta);
	}

	if (m_bRimChange)
		_player.UpdateRimStrength({ 0,.5,0,1 }, 1.f, fTimeDelta *4.f);
	else
		_player.UpdateRimStrength({ 0,.5,0,1 }, 1.f, -fTimeDelta * 4.f);

	if (_player.GetRimStrength() > 0.95f)
		m_bRimChange = false;
}

void CPlayerState::Exit(CPlayer& _player)
{
	_player.Set_ControllerHit(false);
	m_bSideColided = false;
	for (auto& elem : m_bExecuted)
		elem = false;
}

void CPlayerState::PhysicsUpdate(CPlayer& _player, _float fTimeDelta)
{
	m_fTime += fTimeDelta;

	m_pTransformCom->Go_Dir({ 0.f, -1.f, 0.f }, 1.f, m_fTime * m_fTime * 10.f * fTimeDelta);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 vDisp;
	XMStoreFloat3(&vDisp, vPos - XMLoadFloat3(&m_vPrePos));
	m_CollisionFlag = _player.Get_Controller()->move(PxVec3(vDisp.x, vDisp.y, vDisp.z), 0.0f, fTimeDelta, PxControllerFilters(),CGameInstance::Get_Instance()->GetObstacleContext());
	if (m_CollisionFlag == PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		m_fTime = 0.f;
	}
	else if (m_CollisionFlag == PxControllerCollisionFlag::eCOLLISION_SIDES)
	{
		m_bSideColided = true;
	}

	PxExtendedVec3 vControllerPos = _player.Get_Controller()->getFootPosition() + PxExtendedVec3(0,0.f,0);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&_float4((float)vControllerPos.x, (float)vControllerPos.y, (float)vControllerPos.z, 1.f)));
}

void CPlayerState::RootPosUpdate(CPlayer& _player,_float fAmount, _float fTimeDelta)
{
	if (!_player.Get_ControllerHit()/* && !m_bSideColided*/)
	{
		_vector vRot = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector v = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
		_matrix m = XMMatrixRotationQuaternion(v);

		_float4 vPos;
		XMStoreFloat4(&vPos, XMLoadFloat4(&m_vInitPos) - XMVector3TransformNormal(XMLoadFloat4(&m_pModelCom->GetRootPos())* fAmount, m) * 0.0166f);
		_float4 vPosition = { vPos.x,m_vPrePos.y,vPos.z,1.f };
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));
	}
}

void CPlayerState::Free()
{
}