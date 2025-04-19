#include "Client_Pch.h"
#include "..\Public\MonsterState.h"
#include "MonsterHurtState.h"
#include "MonsterStunState.h"
#include "Monster.h"
#include "Player.h"
#include "PlayerState.h"
#include "Transform.h"
#include "Model.h"
#include "Animation.h"
#include "GameInstance.h"
#include "UI.h"
#include "Camera_Player.h"

CMonsterState::CMonsterState()
{
}

CMonsterState::~CMonsterState()
{
}

CMonsterState* CMonsterState::Transition(CMonster& _monster)
{
	CPlayerState* pPlayerState = m_pPlayer->Get_PlayerState();
	if (_monster.Get_MonsterHit() && m_szName != "Hurt" && pPlayerState->Get_Attack())
	{
		if (pPlayerState->Get_Name() == "Plunder")
		{
			_monster.Set_HP(_monster.Get_Hp() - 1);
		}
		else
		{
			if(_monster.Get_MonsterType()!=CMonster::JOKER)
			{
				_monster.Set_HP(_monster.Get_Hp() - 10);
				if (pPlayerState->Get_Name() == "Plague")
					_monster.Set_HP(_monster.Get_Hp() - 10);
			}
			else
			{
				_monster.Set_HP(_monster.Get_Hp() - 5);
				if (pPlayerState->Get_Name() == "Plague")
					_monster.Set_HP(_monster.Get_Hp() - 5);
			}
		}
		if (_monster.Get_Hp() <= 0)
			return new CMonsterStunState;
		else
		{
			return new CMonsterHurtState(HURT_S1);
		}
	}
    return nullptr;
}

void CMonsterState::Entry(CMonster& _player)
{
	m_pGameInstance = CGameInstance::Get_Instance();

	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Player());
	m_pTransformCom = static_cast<CTransform*>(_player.Find_Component(TEXT("Com_Transform")));
	m_pPlayerTransformCom = static_cast<CTransform*>(m_pGameInstance->Get_Player()->Find_Component(L"Com_Transform"));
	m_pModelCom = static_cast<CModel*>(_player.Get_Model());
	XMStoreFloat4(&m_vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pModelCom->Get_CurrentAnim()->ResetEnd();
	for (auto& elem : _player.Get_AttackDesc())
		elem->bActive = false;
}

void CMonsterState::Update(CMonster& _monster, _float fTimeDelta)
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

	m_vPreDir = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
	//회전 중
	if (m_bRotStart)
	{
		m_RotTime += fTimeDelta;
		_vector v = XMVectorLerp(m_vPreDir, m_vCurDir, m_RotTime*0.2f);
		m_pTransformCom->LookAtDir(v);
		if (m_RotTime > 1.f)
			m_bRotStart = false;
	}
	//회전 끝
	else
	{
		m_RotTime = 0.f;
	}

	if (m_bHitRim)
		_monster.UpdateRimStrength({ 1,0,0,1 }, 1, fTimeDelta * 5.f);
	else
		_monster.UpdateRimStrength({ 1,0,0,1 }, 1.f, -fTimeDelta * 5.f);

	if (_monster.GetRimStrength() > 0.95f)
	{
		m_bHitRim = false;
	}

	for (auto& elem : _monster.Get_AttackDesc())
	{
		if (elem->bActive && elem->bHit && m_bAttack)
			m_pPlayer->Get_HitDesc().bHit = true;
	}

	if(-1!=_monster.Get_HpBarIdx())
	{
		CUI* pUI = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), _monster.Get_HpBarIdx()));
		CUI* pUI2 = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), _monster.Get_HpBarIdx() - 1));
		CUI* pUI3 = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), _monster.Get_HpBarIdx() + 1));
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0, 2, 0, 0);
		CTransform* pUITransform = static_cast<CTransform*>(pUI->Get_Component(L"Com_Transform"));
		CTransform* pUITransform2 = static_cast<CTransform*>(pUI2->Get_Component(L"Com_Transform"));
		if (pUI->Get_Desc()->bVisible)
		{
			XMStoreFloat3(&pUI->Get_Desc()->vPos, vPos);
			pUI->Set_Hp(_monster.Get_Hp()/100.f);
		}
		if (pUI3->Get_Desc()->bVisible)
		{
			XMStoreFloat3(&pUI3->Get_Desc()->vPos, vPos);
			if (pUI3->Get_Alpha() < 0.05f)
				m_bStunBarShine = true;
			else if (pUI3->Get_Alpha() > 0.95f)
				m_bStunBarShine = false;

			if (!m_bStunBarShine)
				pUI3->Set_Alpha(-fTimeDelta * 3.f);
			else
				pUI3->Set_Alpha(fTimeDelta * 3.f);
		}
		if (pUI2->Get_Desc()->bVisible)
		{
			vPos += m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_LOOK) * 0.001f /*+ XMVectorSet(0, 0.02f, 0, 0)*/;
			XMStoreFloat3(&pUI2->Get_Desc()->vPos, vPos);
		}
	}
}

void CMonsterState::Exit(CMonster& _monster)
{
	m_pModelCom->Get_CurrentAnim()->ResetEnd();
	m_bControllerHit = false;
	m_bSideColided = false;
	m_bInitHit = false;
	m_bAttack = false;
	for (auto& elem : m_bExecuted)
		elem = false;
}

void CMonsterState::PhysicsUpdate(CMonster& _player, _float fTimeDelta)
{
	m_fTime += fTimeDelta;

	m_pTransformCom->Go_Dir({ 0.f, -1.f, 0.f }, 1.f, m_fTime * m_fTime * 10.f * fTimeDelta);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	_float3 vDisp;
	XMStoreFloat3(&vDisp, vPos - XMLoadFloat3(&m_vPrePos));
	m_CollisionFlag = _player.Get_Controller()->move(PxVec3(vDisp.x, vDisp.y, vDisp.z), 0.0f, fTimeDelta, PxControllerFilters(), CGameInstance::Get_Instance()->GetObstacleContext());
	if (m_CollisionFlag == PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		m_fTime = 0.f;
	}
	else if (m_CollisionFlag == PxControllerCollisionFlag::eCOLLISION_SIDES)
	{
		m_bSideColided = true;
	}

	_float fAmount = 0.f;
	if (_player.Get_MonsterType() == CMonster::GARDENER || _player.Get_MonsterType() == CMonster::JOKER)
		fAmount = 0.f;
	else
		fAmount = 0.f;
	PxExtendedVec3 vControllerPos = _player.Get_Controller()->getFootPosition() + PxExtendedVec3(0, fAmount, 0);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&_float4((float)vControllerPos.x, (float)vControllerPos.y, (float)vControllerPos.z, 1.f)));
}

void CMonsterState::RootPosUpdate(CMonster& _monster, _float fAmount, _float fTimeDelta)
{
	if (!m_bControllerHit && !m_bSideColided)
	{
		_vector vRot = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector v = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
		_matrix m = XMMatrixRotationQuaternion(v);

		_float4 vPos;
		XMStoreFloat4(&vPos, XMLoadFloat4(&m_vInitPos) - XMVector3TransformNormal(XMLoadFloat4(&m_pModelCom->GetRootPos()), m) * fAmount * 0.016f);
		_float4 vPosition = { vPos.x,m_vPrePos.y,vPos.z,1.f };
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));
	}
}

void CMonsterState::Free()
{
}

