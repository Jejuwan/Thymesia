#include "Client_Pch.h"
#include "PlayerPlagueState.h"
#include "PlayerIdleState.h"
#include "Player.h"
#include "Model.h"
#include "GameInstance.h"
#include "Animation.h"
#include "Transform.h"
#include "Camera_Player.h"
#include "Effect.h"
#include "Layer.h"

CPlayerPlagueState::CPlayerPlagueState(ANIM_TYPE type)
{
	m_eType = type;
}

CPlayerPlagueState::~CPlayerPlagueState()
{
}

CPlayerState* CPlayerPlagueState::Transition(CPlayer& _player)
{
	CPlayerState* pState = __super::Transition(_player);
	if (nullptr != pState)
		return pState;

	if (_player.Get_Model()->Get_CurrentAnim()->GetInterpolateEnd())
		return new CPlayerIdleState();


	return nullptr;
}

void CPlayerPlagueState::Entry(CPlayer& _player)
{
	__super::Entry(_player);

	m_pModelCom->SetUp_Animation(m_eType, false);
	m_pModelCom->Set_NextAnim(IDLE);

	m_szName = "Plague";
	_vector vLook = _player.Get_PlayerCamera()->Get_Transform()->Get_State(CTransform::STATE_LOOK);
	_float3 modified;
	XMStoreFloat3(&modified, vLook);
	modified.y = 0.f;
	m_pTransformCom->LookAtDir(XMLoadFloat3(&modified));

	switch (m_eType)
	{
	case AXE1:
		_player.Set_Weapon(CPlayer::AXE);
		break;
	case KNIFE:
		_player.Set_Weapon(CPlayer::KNIFE);
		break;
	case HAMMER1:
		_player.Set_Weapon(CPlayer::HAMMER);
		break;
	case SCYTHE1:
		_player.Set_Weapon(CPlayer::SCYTHE);
		break;
	}

}

void CPlayerPlagueState::Update(CPlayer& _player, _float fTimeDelta)
{
	XMStoreFloat3(&m_vPrePos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_fTrackPos = m_pModelCom->Get_CurrentAnim()->GetTrackPosition();
	m_fDuration = m_pModelCom->Get_CurrentAnim()->GetDuration();

	switch (m_pModelCom->Get_CurrentAnimIndex())
	{
	case AXE1:
		if (m_fTrackPos > 60.f && m_fTrackPos < 70.f && !m_bExecuted[0])
		{
			m_bExecuted[0] = true;
			EFFECTDESC desc;
			desc.texName = TEXT("Smoke4");
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_HoriziontalLookDir() * 1.f;
			desc.vScale = { 3.f,3.f,3.f };
			desc.vPos = vPos;
			desc.iRow = 12;
			desc.iShaderNum = 3;
			CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
				TEXT("Prototype_GameObject_Effect_Texture"), &desc);

			m_pGameInstance->StopSound(SOUND_PLAYER_ATTACK);
			m_pGameInstance->Play_Sound(L"WhooshAxe_02.wav", SOUND_PLAYER_ATTACK, 0.8f);
			m_bRimChange = true;

		}
		if (m_fTrackPos > 70.f && m_fTrackPos < 80.f)
		{
			m_bAttack = true;
			if (!m_bExecuted[1])
			{
				m_bExecuted[1] = true;
			}
		}
		else 
			m_bAttack = false;

		if (m_fTrackPos > 0.f && !m_bExecuted[7])
		{
			m_bExecuted[7] = true;
			m_pGameInstance->StopSound(SOUND_PLAYER_ATTACK);
			m_pGameInstance->Play_Sound(L"PlayerSkillLaunch_03.wav", SOUND_PLAYER_ATTACK, 1.f);
		}
		break;
	case AXE2://¾È¾¸
		if (m_fTrackPos > 60.f && m_fTrackPos < 80.f)
		{
			//m_bAttack = true;
		}
		break;
	case KNIFE:
		if (m_fTrackPos > 60.f && m_fTrackPos < 80.f && !m_bExecuted[0])
		{
			m_bExecuted[0] = true;
			EFFECTDESC desc;
			desc.texName = TEXT("Smoke2");
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_HoriziontalLookDir() * 1.f + XMVectorSet(0, 1, 0, 0);
			desc.vScale = { 3.f,3.f,3.f };
			desc.vPos = vPos;
			desc.iRow = 8;
			desc.iShaderNum = 3;
			CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
				TEXT("Prototype_GameObject_Effect_Texture"), &desc);

			m_pGameInstance->StopSound(SOUND_PLAYER_ATTACK);
			m_pGameInstance->Play_Sound(L"Villager_WhooshKnife_01.wav", SOUND_PLAYER_ATTACK, 0.8f);
			m_bRimChange = true;
		}
		if (m_fTrackPos > 60.f && m_fTrackPos < 80.f)
		{
			m_bAttack = true;
			if (!m_bExecuted[1])
			{
				m_bExecuted[1] = true;
			}
		}
		else
			m_bAttack = false;

		if (m_fTrackPos > 0.f && !m_bExecuted[8])
		{
			m_bExecuted[8] = true;
			m_pGameInstance->StopSound(SOUND_PLAYER_ATTACK);
			m_pGameInstance->Play_Sound(L"PlayerSkillLaunch_03.wav", SOUND_PLAYER_ATTACK, 1.f);
		}
		break;
	case HAMMER1:
		if (m_fTrackPos > 105.f && m_fTrackPos < 115.f && !m_bExecuted[0])
		{
			m_bExecuted[0] = true;
			EFFECTDESC desc;
			desc.texName = TEXT("Smoke4");
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_HoriziontalLookDir() * 1.f;
			desc.vScale = { 3.f,3.f,3.f };
			desc.vPos = vPos;
			desc.iRow = 12;
			desc.iShaderNum = 3;
			CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
				TEXT("Prototype_GameObject_Effect_Texture"), &desc);

			m_bRimChange = true;
		}
		if (m_fTrackPos > 50.f && m_fTrackPos < 60.f)
		{
			m_bAttack = true;
			m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_PLAYER_TALK, 1.f);
		}

		if (m_fTrackPos > 100.f && m_fTrackPos < 110.f)
		{
			m_bAttack = true;
			m_pGameInstance->Play_Sound(L"WoodHeavtHit04.wav", SOUND_PLAYER_ATTACK, 1.f);
		}
		else
			m_bAttack = false;
		break;
	case HAMMER2:
		if (m_fTrackPos > 60.f && m_fTrackPos < 80.f)
		{
			if(!m_bExecuted[0])
			{
				m_bExecuted[0] = true;
				m_bRimChange = true;
			}
			m_bAttack = true;
			m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_PLAYER_ATTACK, 1.f);
		}
		else if (m_fTrackPos > 120.f && m_fTrackPos < 130.f)
		{
			if (!m_bExecuted[1])
			{
				m_bExecuted[1] = true;
				m_bRimChange = true;
			}
			m_bAttack = true;
			m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_PLAYER_ATTACK, 1.f);
		}
		else if (m_fTrackPos > 170.f && m_fTrackPos < 190.f)
		{
			m_bAttack = true;
			m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_PLAYER_ATTACK, 1.f);
		}
		else if (m_fTrackPos > 220.f && m_fTrackPos < 260.f)
		{
			m_bAttack = true;
			if(!m_bExecuted[2])
			{
				m_bExecuted[2] = true;
				m_pGameInstance->Play_Sound(L"WoodHeavtHit04.wav", SOUND_PLAYER_ATTACK, 1.f);
				m_bRimChange = true;
			}
		}
		else
			m_bAttack = false;

		if (m_fTrackPos > 230.f && m_fTrackPos < 240.f && !m_bExecuted[1])
		{
			m_bExecuted[1] = true;
			EFFECTDESC desc;
			desc.texName = TEXT("Smoke4");
			_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_HoriziontalLookDir() * 1.5f;
			desc.vScale = { 3.f,3.f,3.f };
			desc.vPos = vPos;
			desc.iRow = 12;
			desc.iShaderNum = 3;
			CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
				TEXT("Prototype_GameObject_Effect_Texture"), &desc);

			m_bRimChange = true;
		}
		break;
	case SCYTHE1:
		if (m_fTrackPos > 80.f && m_fTrackPos < 90.f)
		{
			m_bAttack = true;

			if(!m_bExecuted[0])
			{
				m_bExecuted[0] = true;
				EFFECTDESC desc;
				desc.meshPath = TEXT("SMFX_WeaponTrailMesh_03");
				desc.vScale = { 2.f,10.f,2.f };
				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_HoriziontalLookDir() * 1.f;
				desc.vPos = vPos + XMVectorSet(0, 1, 0, 0);
				desc.iRow = 1;
				desc.fTickTime = 1.f;
				desc.iShaderNum = 2;

				CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
					TEXT("Prototype_GameObject_Effect_Texture"), &desc);
				CLayer* pLayer = m_pGameInstance->Get_Layer(LEVEL_BOSS_BAT, TEXT("Layer_Effect"));
				CEffect* pEffect = static_cast<CEffect*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"), pLayer->size() - 1));
				CTransform* pTransform = static_cast<CTransform*>(pEffect->Find_Component(TEXT("Com_Transform")));
				pTransform->LookAtDir(m_pTransformCom->Get_HoriziontalLookDir());
				pTransform->Rotation(m_pTransformCom->Get_HoriziontalLookDir(), XMConvertToRadians(45.f));
				pTransform->Rotation({ 0,1,0,0 }, XMConvertToRadians(-30.f));
				m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_PLAYER_ATTACK, 1.f);
				m_bRimChange = true;
			}
		}
		else
			m_bAttack = false;
		break;
	case SCYTHE2:
		if (m_fTrackPos > 40.f && m_fTrackPos < 60.f)
		{
			m_bAttack = true;

			if(!m_bExecuted[1])
			{
				m_bExecuted[1] = true;
				EFFECTDESC desc;
				desc.meshPath = TEXT("SMFX_WeaponTrailMesh_03");
				desc.vScale = { 2.f,10.f,2.f };
				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) + m_pTransformCom->Get_HoriziontalLookDir() * 2.f;
				desc.vPos = vPos + XMVectorSet(0, 1, 0, 0);
				desc.iRow = 1;
				desc.fTickTime = 1.f;
				desc.iShaderNum = 2;

				CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
					TEXT("Prototype_GameObject_Effect_Texture"), &desc);
				CLayer* pLayer = m_pGameInstance->Get_Layer(LEVEL_BOSS_BAT, TEXT("Layer_Effect"));
				CEffect* pEffect = static_cast<CEffect*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"), pLayer->size() - 1));
				CTransform* pTransform = static_cast<CTransform*>(pEffect->Find_Component(TEXT("Com_Transform")));
				pTransform->LookAtDir(m_pTransformCom->Get_HoriziontalLookDir());
				pTransform->Rotation(m_pTransformCom->Get_HoriziontalLookDir(), XMConvertToRadians(-45.f));
				//pTransform->Rotation({ 1,0,0,0 }, XMConvertToRadians(30.f));
				m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_PLAYER_ATTACK, 1.f);

				m_bRimChange = true;
			}
		}
		//else if (m_fTrackPos > 80.f && m_fTrackPos < 100.f)
		//{
		//	m_bAttack = true;
		//}
		else if (m_fTrackPos > 115.f && m_fTrackPos < 130.f)
		{
			m_bAttack = true;
		
			if (!m_bExecuted[2])
			{
				m_bExecuted[2] = true;
				EFFECTDESC desc;
				desc.meshPath = TEXT("SMFX_WeaponTrailMesh_03");
				desc.vScale = { 2.f,10.f,2.f };
				_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION) - m_pTransformCom->Get_HoriziontalLookDir() * 1.f;
				desc.vPos = vPos + XMVectorSet(0, 1, 0, 0);
				desc.iRow = 1;
				desc.fTickTime = 1.f;
				desc.iShaderNum = 2;

				CGameInstance::Get_Instance()->Add_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"),
					TEXT("Prototype_GameObject_Effect_Texture"), &desc);
				CLayer* pLayer = m_pGameInstance->Get_Layer(LEVEL_BOSS_BAT, TEXT("Layer_Effect"));
				CEffect* pEffect = static_cast<CEffect*>(m_pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_Effect"), pLayer->size() - 1));
				CTransform* pTransform = static_cast<CTransform*>(pEffect->Find_Component(TEXT("Com_Transform")));
				pTransform->LookAtDir(m_pTransformCom->Get_HoriziontalLookDir());
				pTransform->Rotation(m_pTransformCom->Get_HoriziontalLookDir(), XMConvertToRadians(45.f));
				pTransform->Rotation({ 0,1,0,0 }, XMConvertToRadians(-30.f));
				m_pGameInstance->Play_Sound(L"WhooshHeavySword_01.wav", SOUND_PLAYER_ATTACK, 1.f);

				m_bRimChange = true;
			}
		}
		else
			m_bAttack = false;
		break;
	}

	if (m_fTrackPos > m_fDuration - 50.f)
	{
	}

	if (!m_bInterpolate)
	{
		if (m_fTrackPos > m_fDuration - 25.f)
		{
			m_pModelCom->Set_NextAnim(IDLE);
			m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
		}

		if (m_pGameInstance->Keyboard_Down('2'))
		{
			switch(m_pModelCom->Get_CurrentAnimIndex())
			{
			case AXE1:
				if (m_fTrackPos > m_fDuration - 100.f && m_fTrackPos < m_fDuration - 40.f)
				{
					m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
					m_bInterpolate = true;
					m_pModelCom->Set_NextAnim(AXE2);
					m_bAttackChange = true;
				}
				break;
			case HAMMER1:
				if (m_fTrackPos > m_fDuration - 150.f && m_fTrackPos < m_fDuration - 40.f)
				{
					m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
					m_bInterpolate = true;
					m_pModelCom->Set_NextAnim(HAMMER2);
					m_bAttackChange = true;
				}
				break;
			case SCYTHE1:
				if (m_fTrackPos > m_fDuration - 100.f && m_fTrackPos < m_fDuration - 40.f)
				{
					m_pModelCom->Get_CurrentAnim()->SetEndForInterpolate();
					m_bInterpolate = true;
					m_pModelCom->Set_NextAnim(SCYTHE2);
					m_bAttackChange = true;
				}
				break;
			}
		}
	}

	__super::Update(_player, fTimeDelta);
	__super::RootPosUpdate(_player, 1.f, fTimeDelta);
	__super::PhysicsUpdate(_player, fTimeDelta);

	//CTransform* mon = _player.Get_PlayerCamera()->GetLockonTransform();
	//if (nullptr != mon)
	//{
	//	m_pTransformCom->LookAtDir(m_pTransformCom->Get_HoriziontalDir(mon));
	//}

	if (m_pModelCom->Get_CurrentAnim()->GetInterpolateEnd() && m_bInterpolate)
	{
		XMStoreFloat4(&m_vInitPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION));
		m_pModelCom->Get_CurrentAnim()->ResetEnd();
		m_pModelCom->SetUp_Animation(m_pModelCom->Get_NextAnimIndex(), false);
		m_bInterpolate = false;
		_player.Set_ControllerHit(false);
	}
}

void CPlayerPlagueState::Exit(CPlayer& _player)
{
	__super::Exit(_player);
	_player.Set_Weapon(CPlayer::DEFAULT);
}

void CPlayerPlagueState::Free()
{
}
