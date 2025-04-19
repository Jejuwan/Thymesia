#include "Client_Pch.h"
#include "PlayerHurtState.h"
#include "PlayerAvoidState.h"
#include "PlayerAttackState.h"
#include "PlayerIdleState.h"
#include "Player.h"
#include "Monster.h"
#include "MonsterState.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Transform.h"
#include "Camera_Player.h"
#include "UI.h"

CPlayerHurtState::CPlayerHurtState()
{
}

CPlayerHurtState::~CPlayerHurtState()
{
}

CPlayerState* CPlayerHurtState::Transition(CPlayer& _player)
{
	CPlayerState* pState = __super::Transition(_player);
	if (nullptr != pState)
		return pState;

	if (m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd())
		return new CPlayerIdleState();
	return nullptr;
}

void CPlayerHurtState::Entry(CPlayer& _player)
{
	__super::Entry(_player);

	_player.Get_HitDesc().bHit = false;
	CMonster* pMonster = _player.Get_TargetMonster();

	pMonster->Get_MonsterState()->Set_Attack(false);

	m_szName = "Hurt";
	switch (_player.Get_HitDesc().eType)
	{
	case HIT_TYPE::DEFAULT:
		m_pModelCom->SetUp_Animation(HURT_S, false);
		break;
	case HIT_TYPE::STRONG:
		m_pModelCom->SetUp_Animation(HURT_L, false);
		break;
	case HIT_TYPE::FALLDOWN:
		m_pModelCom->SetUp_Animation(FALLDOWN, false);
		break;
	case HIT_TYPE::HEADACHE:
		m_pModelCom->SetUp_Animation(HEADACHE_START, false);
		break;
	}
	m_pModelCom->Set_NextAnim(IDLE);
	m_pTransformCom->LookAtDir(m_pCameraTransformCom->Get_HoriziontalLookDir());

	_player.Set_Hp(_player.Get_Hp() - 5);
	CUI* pUI = static_cast<CUI*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), 3));
	pUI->Set_Hp(_player.Get_Hp() / 100.f);

	if(_player.Get_HitDesc().eType != HIT_TYPE::HEADACHE)
	{
		CVIBuffer_Point_Instance::POINTINSTANCEDESC pDesc;
		wstring wstr = TEXT("../../SaveFiles/Particles/");
		wstr += TEXT("blood");
		wstr += TEXT(".effect");
		ifstream is(wstr, ios::binary);
		is.read((char*)&pDesc, sizeof(CVIBuffer_Point_Instance::POINTINSTANCEDESC));
		is.close();
		pDesc.bNum300 = true;

		_vector vLook = m_pCameraTransformCom->Get_State(CTransform::STATE_LOOK);
		XMStoreFloat4(&pDesc.vPivotPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION) - vLook * 0.5f);
		XMStoreFloat3(&pDesc.fMinDir, (-vLook - XMVectorSet(0.3f, 0.3f, 0.3f, 0.f)));
		XMStoreFloat3(&pDesc.fMaxDir, (-vLook + XMVectorSet(0.3f, 0.3f, 0.3f, 0.f)));

		CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
			TEXT("Prototype_GameObject_Effect_Particle"), &pDesc);
	}

	wstring soundName = L"BodyStab_0";
	soundName += to_wstring(2);
	soundName += L".wav";
	m_pGameInstance->StopSound(SOUND_PLAYER_HIT);
	m_pGameInstance->Play_Sound((wchar_t*)soundName.c_str(), SOUND_PLAYER_HIT, 0.7f);
}

void CPlayerHurtState::Update(CPlayer& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

	if (m_bPause)
		m_fPauseTime += fTimeDelta;

	if (m_fPauseTime > 2.f)
	{
		m_bPause = false;
	}

	if(!m_bPause)
	{
		__super::Update(_player, fTimeDelta);
		if (!m_bSideColided)
		{
			_vector vRot = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_vector v = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
			_matrix m = XMMatrixRotationQuaternion(v);

			_float4 vPos;
			XMStoreFloat4(&vPos, XMLoadFloat4(&m_vInitPos) - XMVector3TransformNormal(XMLoadFloat4(&m_pModelCom->GetRootPos()) * 1.f, m) * 0.0166f);
			_float4 vPosition = { vPos.x,m_vPrePos.y,vPos.z,1.f };
			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));
		}
		__super::PhysicsUpdate(_player, fTimeDelta);
	}

	if (m_fTrackPos > m_fDuration - 1.f && !m_bPause)
	{
		switch (m_pModelCom->Get_CurrentAnimIndex())
		{
		case FALLDOWN:
			m_bPause = true;
			XMStoreFloat4(&m_vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
			m_pModelCom->Get_CurrentAnim()->ResetEnd();
			m_pModelCom->SetUp_Animation(FALLDOWN_END,false);
			break;
		case HEADACHE_START:
			m_pModelCom->Get_CurrentAnim()->ResetEnd();
			m_pModelCom->SetUp_Animation(HEADACHE_LOOP, true);
			break;
		}
	}

	if (m_pModelCom->Get_CurrentAnimIndex() == HEADACHE_LOOP)
	{
		m_fPauseTime += fTimeDelta;
		if (m_fPauseTime > 2.f)
		{
			m_pModelCom->Get_CurrentAnim()->ResetEnd();
			m_pModelCom->SetUp_Animation(HEADACHE_END, false);
		}
	}
}

void CPlayerHurtState::Exit(CPlayer& _player)
{
	__super::Exit(_player);
	_player.Get_HitDesc().bHit = false;
}

void CPlayerHurtState::Free()
{
}
