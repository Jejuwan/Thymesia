#include "Client_Pch.h"
#include "..\Public\Player.h"
#include "Client_Struct.h"
#include "GameInstance.h"
#include "PlayerState.h"
#include "PlayerIdleState.h"
#include "Camera_Player.h"
#include "Bone.h"
#include "Layer.h"
#include "Monster.h"
#include "Animation.h"

CPlayer::CPlayer(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
	ZeroMemory(m_pWeaponLModelCom, sizeof m_pWeaponLModelCom);
	ZeroMemory(m_pWeaponRModelCom, sizeof m_pWeaponRModelCom);
}

CPlayer::CPlayer(const CPlayer & rhs)
	: CGameObject(rhs)
{
}

HRESULT CPlayer::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg))) 
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
	
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	PxCapsuleControllerDesc cDesc;
	cDesc.material = pGameInstance->GetMaterial();
	cDesc.position = { 0.f,0.f,0.f };
	cDesc.height = 2.0;
	cDesc.radius = 0.5;
	cDesc.slopeLimit = 0.0f;
	cDesc.contactOffset = 0.1f;
	cDesc.stepOffset = 0.02f;
	cDesc.reportCallback = this;

	m_pController = CGameInstance::Get_Instance()->CreateController(&cDesc);

	m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMLoadFloat4(&_float4(5.f,5.f,5.f,1.f)));

	SPAWNDESC* pSpawnDesc = (SPAWNDESC*)pArg;
	if (pSpawnDesc)
	{
		m_pSpawnDesc.m_eCurLevel = pSpawnDesc->m_eCurLevel;

		switch (pSpawnDesc->m_eCurLevel)
		{
		case LEVEL_BOSS_BAT:
			m_pController->setPosition({ 0.f,2.f,55.f });
			m_pTransformCom->Rotation({ 0,1,0 }, XMConvertToRadians(180.f));
			break;
		default:
			m_pController->setPosition({ -2.f,2.f,-2.f });
			break;
		}
	}

	m_CurState = new CPlayerIdleState();
	m_CurState->Entry(*this);

	m_pAttackShape = pGameInstance->GetPhysics()->createShape(
		PxCapsuleGeometry((PxReal)1.5,(PxReal)1.5), *pGameInstance->GetMaterial());

	m_pAttackShape->setFlag(PxShapeFlag::eSIMULATION_SHAPE, false);
	m_pAttackShape->setFlag(PxShapeFlag::eTRIGGER_SHAPE, true);

	PxTransform localTm(0, 0, 0);
	m_pAttackActor = PxCreateStatic(*pGameInstance->GetPhysics(), localTm,*m_pAttackShape);

	pGameInstance->GetScene()->addActor(*m_pAttackActor);

	pGameInstance->Set_Player(this);

	m_pModelCom->Set_Animation_Speed(2.f);

	for(int i=1;i<9;i++)
		m_pModelCom->Get_Anim(i)->Set_Speed(1.f);
	for (int i = 14; i < 15; i++)
		m_pModelCom->Get_Anim(i)->Set_Speed(1.f);
	for (int i = 32; i < 37; i++)
		m_pModelCom->Get_Anim(i)->Set_Speed(1.f);

	for (int i = 25; i < 32; i++)
		m_pModelCom->Get_Anim(i)->Set_Speed(3.f);
	m_iWeaponR_Boneidx = m_pModelCom->Get_BoneIndex("weapon_r");

	m_HitDesc.bHit = false;
	m_HitDesc.eType = HIT_TYPE::DEFAULT;

	return S_OK;
}

void CPlayer::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
	_vector vPos = m_pTransformCom->Get_State(CTransform::STATE_POSITION);
	
	//까마귀 공격등으로 무기가 없을 때
	if(NONE == m_eWeaponType)
	{
		_float3 vWeaponPos;
		XMStoreFloat3(&vWeaponPos, m_pTransformCom->Get_State(CTransform::STATE_POSITION)/* +
			m_pTransformCom->Get_State(CTransform::STATE_LOOK) * 0.5f */+ XMVectorSet(0, 2.f, 0, 0));

		_float4 vWeaponQuat;
		XMStoreFloat4(&vWeaponQuat, XMQuaternionRotationMatrix(
			XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix()) * 
			XMMatrixRotationY(XMConvertToRadians(90.f))));

		m_pAttackActor->setGlobalPose(PxTransform(PxVec3(vWeaponPos.x, vWeaponPos.y, vWeaponPos.z),
			PxQuat(vWeaponQuat.x, vWeaponQuat.y, vWeaponQuat.z, vWeaponQuat.w)));
	}
	//무기 있을 때
	else 
	{
		CTransform* pTransform = nullptr;

		if (m_eWeaponType == AXE || m_eWeaponType == KNIFE)
			pTransform = m_pWeaponLTransformCom;
		else
			pTransform = m_pWeaponRTransformCom;

		_float3 vWeaponPos;
		XMStoreFloat3(&vWeaponPos, pTransform->Get_State(CTransform::STATE_POSITION));

		_float4 vWeaponQuat;
		XMStoreFloat4(&vWeaponQuat, XMQuaternionRotationMatrix(XMLoadFloat4x4(&pTransform->Get_WorldMatrix())));

		m_pAttackActor->setGlobalPose(PxTransform(PxVec3(vWeaponPos.x, vWeaponPos.y, vWeaponPos.z),
			PxQuat(vWeaponQuat.x, vWeaponQuat.y, vWeaponQuat.z, vWeaponQuat.w)));
	}

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if (pGameInstance->Keyboard_Down(VK_MBUTTON))
	{
		CGameObject* pLockonMonster = nullptr;
		_float fMinDist = 10.f;

		CLayer* pMonLayer = pGameInstance->Get_Layer(LEVEL_GAMEPLAY, TEXT("Layer_Monster"));
		if(nullptr!= pMonLayer)
		{
			int size = pMonLayer->size();
			for (int i = 0; i < size; i++)
			{
				CMonster* pMonster = static_cast<CMonster*>(pGameInstance->Get_GameObject(LEVEL_GAMEPLAY, TEXT("Layer_Monster"), i));
				if (!pMonster->Get_Dead())
				{
					CTransform* pTransform = static_cast<CTransform*>(pMonster->Find_Component(L"Com_Transform"));
					_float fDist = XMVectorGetX(XMVector3Length(m_pTransformCom->Get_State(CTransform::STATE_POSITION)
						- pTransform->Get_State(CTransform::STATE_POSITION)));
					if (fDist < fMinDist)
					{
						fMinDist = fDist;
						pLockonMonster = pMonster;
					}
				}
			}
		}

		if (nullptr != pLockonMonster)
		{
			LookLockOn(static_cast<CTransform*>(pLockonMonster->Find_Component(L"Com_Transform")));
		}
		else
			LookLockOn(nullptr);
	}

	CPlayerState* pState = m_CurState->Transition(*this);
	if (nullptr != pState)
	{
		m_CurState->Exit(*this);
		Safe_Release(m_CurState);
		m_CurState = pState;
		m_CurState->Entry(*this);
	}
	m_CurState->Update(*this, fTimeDelta);

	XMStoreFloat4(&pGameInstance->Get_LightDesc(1)->vPosition,m_pTransformCom->Get_State(CTransform::STATE_POSITION));

	m_pShadowTransformCom->Set_WorldMatrix(m_pTransformCom->Get_WorldMatrix());
	m_pShadowTransformCom->Set_State(CTransform::STATE_POSITION, m_pShadowTransformCom->Get_State(CTransform::STATE_POSITION) + m_pShadowTransformCom->Get_State(CTransform::STATE_LOOK) * 4.f);
	m_pShadowTransformCom->Rotation({ 0,1,0 }, XMConvertToRadians(180.f));
	m_pShadowTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_RIGHT), XMConvertToRadians(-30.f));
}

void CPlayer::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CPlayer::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;
	
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

	//Get_NumMaterials 유효한 머테리얼 수 => 유효한 메시 수와 같음
	for (_uint i = 0; i < iNumMeshes; i++)
	{
		if (FAILED(m_pModelCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_ShaderResourceView(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_fRimStrength", &m_fRimStrength, sizeof(_float))))
			return E_FAIL;

		if (FAILED(m_pShaderCom->Bind_RawValue("g_RimColor", &m_fRimColor, sizeof(_float4))))
			return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pModelCom->Render(i);
	}

	if (m_pWeaponLModelCom[m_eWeaponType] != nullptr)
	{
		if (FAILED(Bind_WeaponL_ShaderResources()))
			return E_FAIL;

		iNumMeshes = m_pWeaponLModelCom[m_eWeaponType]->Get_NumMeshes();

		//Get_NumMaterials 유효한 머테리얼 수 => 유효한 메시 수와 같음
		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pWeaponLModelCom[m_eWeaponType]->Bind_ShaderResourceView(m_pNonAnimShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pNonAnimShaderCom->Begin(2);

			m_pWeaponLModelCom[m_eWeaponType]->Render(i);
		}
	}

	if (m_pWeaponRModelCom[m_eWeaponType] != nullptr)
	{
		if (FAILED(Bind_WeaponR_ShaderResources()))
			return E_FAIL;

		iNumMeshes = m_pWeaponRModelCom[m_eWeaponType]->Get_NumMeshes();

		//Get_NumMaterials 유효한 머테리얼 수 => 유효한 메시 수와 같음
		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pWeaponRModelCom[m_eWeaponType]->Bind_ShaderResourceView(m_pNonAnimShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pNonAnimShaderCom->Begin(2);

			m_pWeaponRModelCom[m_eWeaponType]->Render(i);
		}
	}

	return S_OK;
}

HRESULT CPlayer::Render_Shadow()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;

	for (_uint i = 0; i < m_pModelCom->Get_NumMaterials(); i++)
	{
		//if (FAILED(m_pModelCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
		//	return E_FAIL;

		//if (FAILED(m_pModelCom->Bind_ShaderResourceView(m_pShaderCom, "g_NormalTexture", i, aiTextureType_NORMALS)))
		//	return E_FAIL;

		if (FAILED(m_pModelCom->Bind_BoneMatrices(m_pShaderCom, "g_BoneMatrices", i)))
			return E_FAIL;


		m_pShaderCom->Begin(1);

		m_pModelCom->Render(i);
	}

	if (FAILED(pGameInstance->Bind_ShaderResource_RTV(m_pShaderCom, TEXT("Target_ShadowDepth"), "g_ShadowTexture")))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;

	/* Com_Transform */
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_WeaponLTransform"), (CComponent**)&m_pWeaponLTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_WeaponRTransform"), (CComponent**)&m_pWeaponRTransformCom, &TransformDesc)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_ShadowTransform"), (CComponent**)&m_pShadowTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_NonAnimShader"), (CComponent**)&m_pNonAnimShaderCom)))
		return E_FAIL;

	/* Com_Model */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Corvus_Anim"),
		TEXT("Com_Model"), (CComponent**)&m_pModelCom)))
		return E_FAIL;

	for(int i=0;i<WEAPON_END;i++)
	{
		m_pWeaponLModelCom[i] = nullptr;
		m_pWeaponRModelCom[i] = nullptr;
	}

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Corvus_Dagger"),
		TEXT("Com_WeaponLModel"), (CComponent**)&m_pWeaponLModelCom[DEFAULT])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Corvus_Saber"),
		TEXT("Com_WeaponRModel"), (CComponent**)&m_pWeaponRModelCom[DEFAULT])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Villager_M_Axe"),
		TEXT("Com_WeaponLAxe"), (CComponent**)&m_pWeaponLModelCom[AXE])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Villager_F_Knife"),
		TEXT("Com_WeaponLKnife"), (CComponent**)&m_pWeaponLModelCom[KNIFE])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Joker_Hammer"),
		TEXT("Com_WeaponRHammer"), (CComponent**)&m_pWeaponRModelCom[HAMMER])))
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Gardener_Scythe"),
		TEXT("Com_WeaponRScythe"), (CComponent**)&m_pWeaponRModelCom[SCYTHE])))
		return E_FAIL;



	return S_OK;
}

HRESULT CPlayer::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;


	_float4x4 shadowViewMat;
	XMStoreFloat4x4(&shadowViewMat, m_pShadowTransformCom->Get_WorldMatrix_Inverse());

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ShadowViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_LIGHTVIEW))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Bind_WeaponL_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_matrix worldMat = XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix());
	_float4x4 pivotMat = m_pModelCom->GetPivotMatrix();

	_float4x4 weaponMat;
	XMStoreFloat4x4(&weaponMat, m_pModelCom->GetBones()[103]->Get_CombinedTransformationMatrix());
	_matrix finalMat = XMMatrixScaling(100, 100, 100) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMLoadFloat4x4(&weaponMat) * XMLoadFloat4x4(&pivotMat) * worldMat;

	_float4x4 transformMat;
	XMStoreFloat4x4(&transformMat, finalMat);
	m_pWeaponLTransformCom->Set_WorldMatrix(transformMat);

	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pWeaponLTransformCom->Bind_ShaderResource(m_pNonAnimShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pNonAnimShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pNonAnimShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pNonAnimShaderCom->Bind_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pNonAnimShaderCom->Bind_RawValue("g_fRimStrength", &m_fRimStrength, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pNonAnimShaderCom->Bind_RawValue("g_RimColor", &m_fRimColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayer::Bind_WeaponR_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_matrix worldMat = XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix());
	_float4x4 pivotMat = m_pModelCom->GetPivotMatrix();

	_float4x4 weaponMat;
	XMStoreFloat4x4(&weaponMat, m_pModelCom->GetBones()[49]->Get_CombinedTransformationMatrix());
	_matrix finalMat = XMMatrixScaling(100, 100, 100) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMLoadFloat4x4(&weaponMat) * XMLoadFloat4x4(&pivotMat) * worldMat;

	_float4x4 transformMat;
	XMStoreFloat4x4(&transformMat, finalMat);
	m_pWeaponRTransformCom->Set_WorldMatrix(transformMat);

	const LIGHTDESC* pLightDesc = pGameInstance->Get_LightDesc(0);
	if (nullptr == pLightDesc)
		return E_FAIL;

	if (FAILED(m_pWeaponRTransformCom->Bind_ShaderResource(m_pNonAnimShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pNonAnimShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pNonAnimShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	if (FAILED(m_pNonAnimShaderCom->Bind_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pNonAnimShaderCom->Bind_RawValue("g_fRimStrength", &m_fRimStrength, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pNonAnimShaderCom->Bind_RawValue("g_RimColor", &m_fRimColor, sizeof(_float4))))
		return E_FAIL;

	return S_OK;
}

CPlayer * CPlayer::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CPlayer*	pInstance = new CPlayer(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CPlayer");
	}

	return pInstance;
}

CGameObject * CPlayer::Clone(void * pArg)
{
	CPlayer*	pInstance = new CPlayer(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CPlayer");
	}

	return pInstance;
}

void CPlayer::Free()
{
	__super::Free();

	if (nullptr != m_CurState)
		Safe_Release(m_CurState);

	//m_pController->release();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pWeaponLTransformCom);
	Safe_Release(m_pWeaponRTransformCom);
	Safe_Release(m_pShadowTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNonAnimShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);

	for (int i = 0; i < WEAPON_END; i++)
	{
		if (nullptr != m_pWeaponLModelCom)
			Safe_Release(m_pWeaponLModelCom[i]);
		if (nullptr != m_pWeaponRModelCom)
			Safe_Release(m_pWeaponRModelCom[i]);
	}
}

void CPlayer::LookLockOn(CTransform* _monTrans)
{
	m_pCameraPlayer->Lockon(_monTrans);
}

void CPlayer::onControllerHit(const PxControllersHit& hit)
{
	if (hit.controller != nullptr)
	{
		m_bControllerHit = true;
	}
	
}
