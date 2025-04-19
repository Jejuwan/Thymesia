#include "Client_Pch.h"
#include "..\Public\Monster.h"
#include "GameInstance.h"
#include "MonsterIdleState.h"
#include "Bone.h"
#include "Player.h"

CMonster::CMonster(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CMonster::CMonster(const CMonster & rhs)
	: CGameObject(rhs) 

{
}

HRESULT CMonster::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	if(nullptr!=pArg)
	{
		MODELDESC* md = static_cast<MODELDESC*>(pArg);
		m_pTransformCom->Set_WorldMatrix(md->initWorldMat);
	}

	return S_OK;
}

void CMonster::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CMonster::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONBLEND, this);
}

HRESULT CMonster::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;	

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_uint		iNumMeshes = m_pModelCom->Get_NumMeshes();

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

	if (FAILED(Bind_WeaponR_ShaderResources()))
		return E_FAIL;

	if(nullptr!= m_pWeaponRModelCom)
	{
		iNumMeshes = m_pWeaponRModelCom->Get_NumMeshes();

		for (_uint i = 0; i < iNumMeshes; i++)
		{
			if (FAILED(m_pWeaponRModelCom->Bind_ShaderResourceView(m_pNonAnimShaderCom, "g_DiffuseTexture", i, aiTextureType_DIFFUSE)))
				return E_FAIL;

			m_pNonAnimShaderCom->Begin(0);

			m_pWeaponRModelCom->Render(i);
		}
	}

	return S_OK;
}

HRESULT CMonster::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	
	CTransform::TRANSFORMDESC		TransformDesc;
	TransformDesc.fSpeedPerSec = 10.f;
	TransformDesc.fRotationPerSec = XMConvertToRadians(90.f);
	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom, &TransformDesc)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxAnimMesh"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) 
		return E_FAIL;

	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxMesh"),
		TEXT("Com_NonAnimShader"), (CComponent**)&m_pNonAnimShaderCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

HRESULT CMonster::Bind_WeaponR_ShaderResources()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	_matrix worldMat = XMLoadFloat4x4(&m_pTransformCom->Get_WorldMatrix());
	_float4x4 pivotMat = m_pModelCom->GetPivotMatrix();

	_float4x4 weaponMat;
	XMStoreFloat4x4(&weaponMat, m_pModelCom->GetBones()[m_iWeaponR_BoneIdx]->Get_CombinedTransformationMatrix());
	_matrix finalMat = XMMatrixScaling(100, 100, 100) * XMMatrixRotationY(XMConvertToRadians(180.f)) * XMLoadFloat4x4(&weaponMat) * XMLoadFloat4x4(&pivotMat) * worldMat;

	_float4x4 transformMat;
	XMStoreFloat4x4(&transformMat, finalMat);
	m_pWeaponRTransformCom->Set_WorldMatrix(transformMat);

	if (FAILED(m_pWeaponRTransformCom->Bind_ShaderResource(m_pNonAnimShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pNonAnimShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pNonAnimShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;

	return S_OK;
}

CMonster * CMonster::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CMonster*	pInstance = new CMonster(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CMonster");
	}

	return pInstance;
}

CGameObject * CMonster::Clone(void * pArg)
{
	CMonster*	pInstance = new CMonster(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CMonster");
	}

	return pInstance;
}

void CMonster::Free()
{
	__super::Free();

	if (nullptr != m_CurState)
		Safe_Release(m_CurState);

	Safe_Release(m_pTransformCom);
	if(nullptr != m_pWeaponRTransformCom)
		Safe_Release(m_pWeaponRTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pNonAnimShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pModelCom);
	if (nullptr != m_pWeaponRModelCom)
		Safe_Release(m_pWeaponRModelCom);

	for (auto& elem : m_pAttackDesc)
		Safe_Delete(elem);
	m_pAttackDesc.clear();
}

void CMonster::Entry_Dead()
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	m_bDead = true;

	if(m_pController)
	{
		m_pController->release();
		m_pController = nullptr;
	}
	
}

void CMonster::onControllerHit(const PxControllersHit& hit)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();
	if(hit.other == static_cast<CPlayer*>(pGameInstance->Get_Player())->Get_Controller())
		m_CurState->Set_ControllerHit(true);
	else
		m_CurState->Set_ControllerHit(false);
}
