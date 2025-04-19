#include "Client_Pch.h"
#include "..\Public\BatState.h"
#include "MonsterHurtState.h"
#include "MonsterStunState.h"
#include "Monster.h"
#include "Player.h"
#include "Bat.h"
#include "PlayerState.h"
#include "Transform.h"
#include "Model.h"
#include "Animation.h"
#include "GameInstance.h"
#include "VIBuffer_Point_Instance.h"
#include "Camera_Player.h"
#include "UI.h"

CBat* CBatState::m_pBat = nullptr;
_uint CBatState::m_iAttackIdx = 0;
_int CBatState::m_iAttackCount = 2;
_int CBatState::m_iChargeCount = 2;


CBatState::CBatState()
{
}

CBatState::~CBatState()
{
}

CMonsterState* CBatState::Transition(CMonster& _monster)
{
	CPlayerState* pPlayerState = m_pPlayer->Get_PlayerState();
	if (_monster.Get_MonsterHit() && m_szName != "Hurt" && pPlayerState->Get_Attack())
	{
		/*if (pPlayerState->Get_Name() == "Plunder")
			_monster.Set_HP(_monster.Get_Hp() - 1);
		else
			_monster.Set_HP(_monster.Get_Hp() - 20);
		if (_monster.Get_Hp() <= 0)
			return new CMonsterStunState;
		else
		{
			return new CMonsterHurtState(HURT_S1);
		}*/
	}
	return nullptr;
}

void CBatState::Entry(CMonster& _player)
{
	m_pGameInstance = CGameInstance::Get_Instance();

	m_pPlayer = static_cast<CPlayer*>(m_pGameInstance->Get_Player());
	m_pTransformCom = static_cast<CTransform*>(_player.Find_Component(TEXT("Com_Transform")));
	m_pPlayerTransformCom = static_cast<CTransform*>(m_pGameInstance->Get_Player()->Find_Component(L"Com_Transform"));
	m_pModelCom = static_cast<CModel*>(_player.Get_Model());
	XMStoreFloat4(&m_vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
	m_pModelCom->Get_CurrentAnim()->ResetEnd();
	m_fLRAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_RIGHT)),
		XMVector3Normalize(m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom))));
	m_fLRAngleDegree = XMConvertToDegrees(m_fLRAngle);
	m_fTurnAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)),
		XMVector3Normalize(m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom))));
	m_pWeaponLTransform = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_WeaponLTransform")));
	m_pWeaponRTransform = static_cast<CTransform*>(m_pPlayer->Get_Component(TEXT("Com_WeaponRTransform")));
	for (auto& elem : _player.Get_AttackDesc())
		elem->bActive = false;
	if (nullptr != m_pPlayer->Get_PlayerCamera())
		m_pCameraTransformCom = static_cast<CTransform*>(m_pPlayer->Get_PlayerCamera()->Get_Transform());
}

void CBatState::Update(CMonster& _monster, _float fTimeDelta)
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
	//È¸Àü Áß
	if (m_bRotStart)
	{
		if(m_pModelCom->Get_CurrentAnimIndex() == TURN_L)
		{
			if (m_pModelCom->Get_CurrentAnim()->GetEnd())
				m_pModelCom->Get_CurrentAnim()->SetEnd(false);
			m_fTurnTime -= fTimeDelta;
			if (m_fTurnTime > 0)
				m_pTransformCom->Turn({ 0,-1,0 }, fTimeDelta*0.8f);
			else
			{
			/*	if (m_fTurnAngle < 0)*/
				
				m_bRotStart = false;
				//m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
			}
		}
		else if (m_pModelCom->Get_CurrentAnimIndex() == TURN_R)
		{
			if (m_pModelCom->Get_CurrentAnim()->GetEnd())
				m_pModelCom->Get_CurrentAnim()->SetEnd(false);
			m_fTurnTime -= fTimeDelta;
			if (m_fTurnTime > 0)
				m_pTransformCom->Turn({ 0,1,0 }, fTimeDelta * 0.8f);
			else
			{
				//if (m_fTurnAngle < 0)
				
				m_bRotStart = false;
				//m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
			}
		}

		//if (m_pModelCom->Get_CurrentAnim()->GetEnd())
		//{
		//	//m_fTurnAngle = XMVectorGetX(XMVector3Dot(XMVector3Normalize(m_pTransformCom->Get_State(CTransform::STATE_LOOK)),
		//	//	XMVector3Normalize(m_pTransformCom->Get_HoriziontalLookAtDir(m_pPlayerTransformCom))));
		//	//if (m_pModelCom->Get_CurrentAnimIndex() == TURN_L)
		//	//	m_pTransformCom->Turn({ 0,-1,0 }, fTimeDelta);
		//	//else if (m_pModelCom->Get_CurrentAnimIndex() == TURN_R)
		//	//	m_pTransformCom->Turn({ 0,1,0 }, fTimeDelta);
		//	m_bRotStart = false;
		//}
	}

	if(m_pBat->Get_Hit())
	{
		m_fHitCoolTime -= fTimeDelta;
		if (m_fHitCoolTime < 0.f)
		{
			m_fHitCoolTime = 0.5f;
			m_pBat->Set_Hit(false);
		}
	}
	
	if (_monster.Get_MonsterHit() && m_pPlayer->Get_PlayerState()->Get_Attack() && !m_pBat->Get_Hit())
	{
			m_pBat->Set_Hit(true);
			//_monster.Set_MonsterHit(false);
			m_pPlayer->Get_PlayerState()->Set_Attack(false);
			m_bHitRim = true;
			_monster.Set_HP(_monster.Get_Hp() - 4);
			if(m_pPlayer->Get_PlayerState()->Get_Name()=="Plague")
				_monster.Set_HP(_monster.Get_Hp() - 4);

			CUI* pUI = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), 6));
			pUI->Set_Hp(_monster.Get_Hp() / 100.f);

			CTransform* pTransform = nullptr;
			if (nullptr != m_pWeaponLTransform)
				pTransform = m_pWeaponLTransform;
			if (nullptr != m_pWeaponRTransform)
				pTransform = m_pWeaponRTransform;

			CVIBuffer_Point_Instance::POINTINSTANCEDESC pDesc;
			wstring wstr = TEXT("../../SaveFiles/Particles/");
			wstr += TEXT("blood");
			wstr += TEXT(".effect");
			ifstream is(wstr, ios::binary);
			is.read((char*)&pDesc, sizeof(CVIBuffer_Point_Instance::POINTINSTANCEDESC));
			is.close();
			_vector vPos = pTransform->Get_State(CTransform::STATE_POSITION);
			_vector vRight = m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_RIGHT);
			_vector vLook = m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_LOOK);
			switch (m_pPlayer->Get_PlayerState()->Get_AttackIndex())
			{
			case 0:
				XMStoreFloat3(&pDesc.fMinDir, (-vRight - XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
				XMStoreFloat3(&pDesc.fMaxDir, (-vRight + XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
				vPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) + m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_HoriziontalLookDir() * 0.5f + XMVectorSet(0, 2.5, 0, 0);
				break;
			case 1:
				XMStoreFloat3(&pDesc.fMinDir, (vRight - XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
				XMStoreFloat3(&pDesc.fMaxDir, (vRight + XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
				vPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) + m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_HoriziontalLookDir() * 0.5f + XMVectorSet(0, 2.2, 0, 0);
				break;
			case 2:
				XMStoreFloat3(&pDesc.fMinDir, (vLook - XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
				XMStoreFloat3(&pDesc.fMaxDir, (vLook + XMVectorSet(0.5f, 0.5f, 0.5f, 0.f)));
				vPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) + m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_HoriziontalLookDir() * 0.5f + XMVectorSet(0, 2.2, 0, 0);
				break;
			default:
				XMStoreFloat3(&pDesc.fMinDir, (-vLook - XMVectorSet(0.3f, 0.3f, 0.3f, 0.f)));
				XMStoreFloat3(&pDesc.fMaxDir, (-vLook + XMVectorSet(0.3f, 0.3f, 0.3f, 0.f)));
				vPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) + m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_HoriziontalLookDir() * 0.5f + XMVectorSet(0, 2, 0, 0);
				break;
			}
			vPos = m_pPlayerTransformCom->Get_State(CTransform::STATE_POSITION) + m_pPlayer->Get_PlayerCamera()->Get_Transform()->Get_HoriziontalLookDir() * 0.5f + XMVectorSet(0, 2, 0, 0);
			XMStoreFloat4(&pDesc.vPivotPos,vPos);
 			CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
				TEXT("Prototype_GameObject_Effect_Particle"), &pDesc);

			_monster.UpdateRimStrength({ 0.5,0,0,1 }, 1.f, fTimeDelta * 10.f);

			wstring soundName = L"BloodExplosion_0";
			soundName += to_wstring(rand() % 2 + 1);
			soundName += L".wav";

			m_pGameInstance->StopSound(SOUND_ENEMY_BOSS);
			m_pGameInstance->Play_Sound((wchar_t*)soundName.c_str(), SOUND_ENEMY_BOSS, 0.7f);
	}

	if (m_bHitRim)
		_monster.UpdateRimStrength({ 0.5,0,0,1 }, 1, fTimeDelta * 8.f);
	else
		_monster.UpdateRimStrength({ 0.5,0,0,1 }, 1.f, -fTimeDelta * 8.f);

	if (_monster.GetRimStrength() > 0.95f)
	{
		m_bHitRim = false;
		
	}

	for (auto& elem : _monster.Get_AttackDesc())
	{
		if (elem->bActive && elem->bHit && m_bAttack)
			m_pPlayer->Get_HitDesc().bHit = true;
	}
}

void CBatState::Exit(CMonster& _monster)
{
	m_pModelCom->Get_CurrentAnim()->ResetEnd();
	m_bControllerHit = false;
	m_bSideColided = false;
	m_bAttack = false;
	for (auto& elem : m_bExecuted)
		elem = false;
}

void CBatState::PhysicsUpdate(CMonster& _player, _float fTimeDelta)
{
	m_fTime += fTimeDelta;

	m_pTransformCom->Go_Dir({ 0.f, -1.f, 0.f }, 1.f, m_fTime * m_fTime * 10.f * fTimeDelta);

	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);

	PxControllerFilters filters;
	filters.mCCTFilterCallback = this;

	_float3 vDisp;
	XMStoreFloat3(&vDisp, vPos - XMLoadFloat3(&m_vPrePos));
	m_CollisionFlag = _player.Get_Controller()->move(PxVec3(vDisp.x, vDisp.y, vDisp.z), 0.0f, fTimeDelta, filters, CGameInstance::Get_Instance()->GetObstacleContext());
	if (m_CollisionFlag == PxControllerCollisionFlag::eCOLLISION_DOWN)
	{
		m_fTime = 0.f;
	}
	else if (m_CollisionFlag == PxControllerCollisionFlag::eCOLLISION_SIDES)
	{
		m_bSideColided = true;
	}

	_vector quat = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
	_matrix mat = XMMatrixRotationQuaternion(quat);
	_float4 fPos = { 0,0,0,1 };
	//XMStoreFloat4(&fPos, XMVector3TransformNormal({ 0,0,m_fAmountZ }, mat));
	
	PxExtendedVec3 vControllerPos = _player.Get_Controller()->getFootPosition() + PxExtendedVec3(fPos.x, m_fAmountY, fPos.z);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&_float4((float)vControllerPos.x, (float)vControllerPos.y, (float)vControllerPos.z, 1.f)));
}

void CBatState::RootPosUpdate(CMonster& _monster, _float fAmount, _float fTimeDelta)
{
	if (!m_bControllerHit && !m_bSideColided)
	{
		_vector vRot = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
		_vector v = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
		_matrix m = XMMatrixRotationQuaternion(v);

		_vector vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
		_float4 vPos;
		XMStoreFloat4(&vPos, vCurPos - XMVector3TransformNormal(XMLoadFloat4(&m_pModelCom->GetRootPos()) - XMLoadFloat4(&m_vPreRootPos), m) * fAmount * 0.016f);
		_float4 vPosition = { vPos.x,m_vPrePos.y,vPos.z,1.f };
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));
	}
}

bool CBatState::filter(const PxController& a, const PxController& b)
{
	return m_bControllerCollide;
}

void CBatState::Free()
{
}

