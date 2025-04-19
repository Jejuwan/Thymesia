#include "Client_Pch.h"
#include "..\Public\Camera_Player.h"
#include "GameInstance.h"
#include "Transform.h"
#include "Player.h"
#include "Bone.h"
#include "UI.h"
#include "Client_Struct.h"
#include "Player.h"

CCamera_Player::CCamera_Player(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CCamera(pDevice, pContext)
{
}

CCamera_Player::CCamera_Player(const CCamera_Player& rhs)
	: CCamera(rhs)
{
}

HRESULT CCamera_Player::Initialize_Prototype()
{
	return S_OK;
}

HRESULT CCamera_Player::Initialize(void* pArg)
{
	CAMERAPLAYERDESC* pCameraFreeDesc = (CAMERAPLAYERDESC*)pArg;

	/* 나에게 필요한 정보를 셋한다. */
	if (FAILED(__super::Initialize(&pCameraFreeDesc->CameraDesc)))
		return E_FAIL;

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	static_cast<CPlayer*>(pGameInstance->Get_Player())->Set_PlayerCamera(this);


	return S_OK;
}

void CCamera_Player::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	m_pPlayer = static_cast<CPlayer*>(pGameInstance->Get_Player());
	if (nullptr == m_pPlayer)
		return;

	CTransform* pPlayerTransform = static_cast<CTransform*>(m_pPlayer->Find_Component(L"Com_Transform"));

	_vector vPlayerPos = pPlayerTransform->Get_State(CTransform::STATE_POSITION);

	_long	MouseMove = 0;

	if (!m_bCameraFixed)
	{
		if (nullptr == m_pLockonTransform)
		{

			if (MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_X))
				m_pTransform->Turn(XMVectorSet(0.f, 1.f, 0.f, 0.f), fTimeDelta * MouseMove * 0.3f);

			cout << MouseMove << endl;

			MouseMove = pGameInstance->Get_DIMouseMove(CInput_Device::DIMM_Y);
			if (MouseMove < 15)
				m_pTransform->Turn(m_pTransform->Get_State(CTransform::STATE_RIGHT), fTimeDelta * MouseMove * 0.3f);

			_vector vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
			_float3 modified;
			XMStoreFloat3(&modified, vLook);
			modified.y = clamp(modified.y, -0.7f, 0.f);
			vLook = XMLoadFloat3(&modified);
			m_pTransform->LookAtDir(vLook);

			_int idx = -1;
			switch (m_pPlayer->Get_SpawnDesc().m_eCurLevel)
			{
			case LEVEL_GAMEPLAY:
				idx = 18;
				break;
			case LEVEL_BOSS_BAT:
				idx = 14;
				break;
			default:
				break;
			}
			m_pTransform->Set_State(CTransform::STATE_POSITION, vPlayerPos - vLook * 4.f + XMVectorSet(0, 2.f, 0, 0));
			CUI* pUI = static_cast<CUI*>(pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), idx));
			pUI->Get_Desc()->bVisible = false;
		}
		else
		{
			m_pTransform->LookAt(m_pLockonTransform->Get_State(CTransform::STATE_POSITION));
			m_pTransform->Rotation(m_pTransform->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(3.f));

			_vector vLook = m_pTransform->Get_State(CTransform::STATE_LOOK);
			_float3 modified;
			XMStoreFloat3(&modified, vLook);
			modified.y = clamp(modified.y, -0.7f, 0.f);
			vLook = XMLoadFloat3(&modified);
			m_pTransform->LookAtDir(vLook);

			m_pTransform->Set_State(CTransform::STATE_POSITION, vPlayerPos - vLook * 4.f + XMVectorSet(0, 2.f, 0, 0));

			//CModel* pModel = static_cast<CModel*>(pPlayer->Get_Component(L"Com_Model"));
			//_float4x4 cameraMat = pModel->GetBoneMatrice(pModel->Get_BoneIndex("Camera"));
			//_matrix finalMat = XMLoadFloat4x4(&cameraMat) * XMLoadFloat4x4(&pPlayerTransform->Get_WorldMatrix());
			//_float4x4 fFinalMat;
			//XMStoreFloat4x4(&fFinalMat, finalMat);
			//m_pTransform->Set_WorldMatrix(fFinalMat);

			_int idx = -1;
			switch (m_pPlayer->Get_SpawnDesc().m_eCurLevel)
			{
			case LEVEL_GAMEPLAY:
				idx = 18;
				break;
			case LEVEL_BOSS_BAT:
				idx = 14;
				break;
			default:
				break;
			}
			CUI* pUI = static_cast<CUI*>(pGameInstance->Get_GameObject(LEVEL_BOSS_BAT, TEXT("Layer_UI"), idx));
			_vector vPos = m_pLockonTransform->Get_State(CTransform::STATE_POSITION) + XMVectorSet(0,1,0,0);
			vPos -= m_pTransform->Get_State(CTransform::STATE_LOOK);
			pUI->Get_Desc()->bVisible = false;
			XMStoreFloat3(&pUI->Get_Desc()->vPos,vPos);
		}
	}

	__super::Set_Transform();
}

void CCamera_Player::Late_Tick(_float fTimeDelta)
{
}

CCamera_Player* CCamera_Player::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CCamera_Player* pInstance = new CCamera_Player(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CCamera_Player");
	}

	return pInstance;
}

CGameObject* CCamera_Player::Clone(void* pArg)
{
	CCamera_Player* pInstance = new CCamera_Player(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CCamera_Player");
	}

	return pInstance;
}

void CCamera_Player::Free()
{
	__super::Free();
}

void CCamera_Player::Lockon(CTransform* trans)
{
	if (nullptr == trans)
		m_pLockonTransform = nullptr;
	else if (nullptr == m_pLockonTransform)
		m_pLockonTransform = trans;
	else
	{
		m_pLockonTransform = nullptr;
	}
}
