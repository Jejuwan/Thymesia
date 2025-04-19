#include "Client_Pch.h"
#include "PlayerRavenState.h"
#include "PlayerPlunderState.h"
#include "PlayerIdleState.h"
#include "PlayerAvoidState.h"
#include "Player.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Transform.h"
#include "Camera_Player.h"
#include "Effect.h"
#include "Layer.h"

CPlayerRavenState::CPlayerRavenState()
{
}

CPlayerRavenState::~CPlayerRavenState()
{
}

CPlayerState* CPlayerRavenState::Transition(CPlayer& _player)
{
	CPlayerState* pState = __super::Transition(_player);
	if (nullptr != pState)
		return pState;

	if (m_fTrackPos >= 30.f && m_bHoldButton)
		return new CPlayerPlunderState();

	if (_player.Get_Model()->Get_CurrentAnim()->GetInterpolateEnd())
		return new CPlayerIdleState();

	if (m_fTrackPos > m_fDuration - 100.f && m_pGameInstance->Keyboard_Down(VK_SPACE))
	{
		if (m_pGameInstance->Keyboard_Pressing('W'))
		{
			return new CPlayerAvoidState(AVOID_F);
		}
		if (m_pGameInstance->Keyboard_Pressing('A'))
		{
			return new CPlayerAvoidState(AVOID_L);
		}
		if (m_pGameInstance->Keyboard_Pressing('S'))
		{
			return new CPlayerAvoidState(AVOID_B);
		}
		if (m_pGameInstance->Keyboard_Pressing('D'))
		{
			return new CPlayerAvoidState(AVOID_R);
		}
	}

	return nullptr;
}

void CPlayerRavenState::Entry(CPlayer& _player)
{
	__super::Entry(_player);

	m_szName = "Plague";

	m_pModelCom->SetUp_Animation(CROW1, false);
	m_pModelCom->Set_NextAnim(IDLE);

	_vector vLook = _player.Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_LOOK);
	_float3 modified;
	XMStoreFloat3(&modified, vLook);
	modified.y = 0.f;
	m_pTransformCom->LookAtDir(XMLoadFloat3(&modified));
	m_bHoldButton = true;
	_player.Set_Weapon(CPlayer::NONE);
	m_iAttackIndex = 4;
}

void CPlayerRavenState::Update(CPlayer& _player, _float fTimeDelta)
{

	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();
	
		EFFECTDESC desc;
		desc.meshPath = TEXT("SMFX_WeaponTrailMesh_03");
		desc.vScale = { 1.f,3.f,1.f };
		_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_HoriziontalLookDir() * 1.5f;
		desc.vPos = vPos + XMVectorSet(0, 0.3, 0, 0);
		desc.iRow = 1;
		desc.fTickTime = 1.f;
		desc.iShaderNum = 2;
		switch (m_pModelCom->Get_CurrentAnimIndex())
		{
		case CROW1:
			if (m_fTrackPos > 50.f && m_fTrackPos < 60.f)
			{
				_player.UpdateRimStrength({ 0,0.2,0,1 }, 1.f, -fTimeDelta * 10.f);
			}
			else if (m_fTrackPos > 40.f && m_fTrackPos < 50.f)
			{
				_player.UpdateRimStrength({ 0,0.2,0,1 }, 1.f, fTimeDelta * 10.f);
				if(!m_bExecuted[0])
				{
					m_bExecuted[0] = true;
					for (int i = 0; i < 3; i++)
					{
						_player.UpdateRimStrength({ 0,0.5,0,1 }, 1.f, fTimeDelta * 10.f);
						CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
							TEXT("Prototype_GameObject_Effect_Texture"), &desc);
						CLayer* pLayer = m_pGameInstance->Get_Layer(LEVEL_BOSS_BAT, TEXT("Layer_Effect"));
						CEffect* pEffect = static_cast<CEffect*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"), pLayer->size() - 1));
						CTransform* pTransform = static_cast<CTransform*>(pEffect->Find_Component(TEXT("Com_Transform")));
						pTransform->LookAtDir(m_pTransformCom->Get_HoriziontalLookDir());
						pTransform->Rotation(m_pTransformCom->Get_HoriziontalLookDir(), XMConvertToRadians(45.f));
						desc.vPos += XMVectorSet(0, 0.4, 0, 0);
					}
				}
			}

			if (m_fTrackPos > 40.f && m_fTrackPos < 50.f)
				m_bAttack = true;
			else
				m_bAttack = false;

			if (m_fTrackPos > 30.f && !m_bExecuted[8])
			{
				m_bExecuted[8] = true;
				m_pGameInstance->StopSound(SOUND_PLAYER_ATTACK);
				m_pGameInstance->Play_Sound(L"PlayerSkillLaunch_03.wav", SOUND_PLAYER_ATTACK, 1.f);
			}
			break;
		case CROW2:
			if (m_fTrackPos > 42.f && m_fTrackPos < 52.f)
			{
				_player.UpdateRimStrength({ 0,0.2,0,1 }, 1.f, -fTimeDelta * 8.f);
			}
			else if (m_fTrackPos > 32.f && m_fTrackPos < 42.f)
			{
				_player.UpdateRimStrength({ 0,0.2,0,1 }, 1.f, fTimeDelta * 8.f);
				if(!m_bExecuted[1])
				{
					m_bExecuted[1] = true;
					for (int i = 0; i < 3; i++)
					{
						CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
							TEXT("Prototype_GameObject_Effect_Texture"), &desc);
						CLayer* pLayer = m_pGameInstance->Get_Layer(LEVEL_BOSS_BAT, TEXT("Layer_Effect"));
						CEffect* pEffect = static_cast<CEffect*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"), pLayer->size() - 1));
						CTransform* pTransform = static_cast<CTransform*>(pEffect->Find_Component(TEXT("Com_Transform")));
						pTransform->LookAtDir(m_pTransformCom->Get_HoriziontalLookDir());
						pTransform->Rotation(m_pTransformCom->Get_HoriziontalLookDir(), XMConvertToRadians(135.f));
						desc.vPos += XMVectorSet(0, 0.4, 0, 0);
					}
				}
			}

			if (m_fTrackPos > 32.f && m_fTrackPos < 42.f)
				m_bAttack = true;
			else
				m_bAttack = false;

			if (m_fTrackPos > 22.f && !m_bExecuted[9])
			{
				m_bExecuted[9] = true;
				m_pGameInstance->StopSound(SOUND_PLAYER_ATTACK);
				m_pGameInstance->Play_Sound(L"PlayerSkillLaunch_03.wav", SOUND_PLAYER_ATTACK, 1.f);
			}
			break;
		}

	if (!m_bInterpolate)
	{
		if (m_fTrackPos > 10.f && !m_pGameInstance->Keyboard_Pressing(VK_RBUTTON))
		{
			m_bHoldButton = false;
		}

		if (m_fTrackPos > m_fDuration - 25.f)
		{
			m_pModelCom->Set_NextAnim(IDLE);
			m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
		}

		if (m_pGameInstance->Mouse_Down(CInput_Device::DIMK_RBUTTON)
			&& m_pModelCom->Get_CurrentAnimIndex() == CROW1)
		{
			if (m_fTrackPos > m_fDuration - 100.f && m_fTrackPos < m_fDuration - 40.f)
			{
				m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
				m_bInterpolate = true;
				m_pModelCom->Set_NextAnim(CROW2);
				//m_bAttackChange = true;
			}
		}
	}

	_float4 vPreRootPos = m_pModelCom->GetRootPos();

	if (m_bAnimChange)
	{
		vPreRootPos = { 0,0,0,1 };
		m_bAnimChange = false;
	}

	__super::Update(_player, fTimeDelta);

	if(_player.Get_PlayerCamera()->GetLockonTransform()==nullptr)
		__super::RootPosUpdate(_player, 1.f, fTimeDelta);
	else
	{
		if (!_player.Get_ControllerHit())
		{
			_vector vRot = m_pTransformCom->Get_State(CTransform::STATE_LOOK);
			_vector v = XMQuaternionRotationMatrix(XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()));
			_matrix m = XMMatrixRotationQuaternion(v);

			_vector vCurPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
			_float4 vPos;
			XMStoreFloat4(&vPos, vCurPos - XMVector3TransformNormal(XMLoadFloat4(&m_pModelCom->GetRootPos()) - XMLoadFloat4(&vPreRootPos), m) * 1.f * 0.016f);
			_float4 vPosition = { vPos.x,m_vPrePos.y,vPos.z,1.f };

			m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&vPosition));
		}

		m_pTransformCom->LookAtDir(m_pTransformCom->Get_HoriziontalLookAtDir(_player.Get_PlayerCamera()->GetLockonTransform()));
	}
	__super::PhysicsUpdate(_player, fTimeDelta);

	if (m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd() && m_bInterpolate)
	{
		XMStoreFloat4(&m_vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pModelCom->Get_CurrentAnim()->ResetEnd();
		m_pModelCom->SetUp_Animation(m_pModelCom->Get_NextAnimIndex(), false);
		m_bInterpolate = false;
		_player.Set_ControllerHit(false);
		m_bAnimChange = true;
	}
}

void CPlayerRavenState::Exit(CPlayer& _player)
{
	__super::Exit(_player);
	m_bAttack = false;
	_player.Set_Weapon(CPlayer::DEFAULT);
}

void CPlayerRavenState::Free()
{
}
