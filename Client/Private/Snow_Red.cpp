#include "Client_Pch.h"
#include "..\Public\Snow_Red.h"
#include "GameInstance.h"

CSnow_Red::CSnow_Red(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{
}

CSnow_Red::CSnow_Red(const CSnow_Red & rhs)
	: CGameObject(rhs)
{
}

HRESULT CSnow_Red::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	

	return S_OK;
}

HRESULT CSnow_Red::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	memcpy(&m_InitializeData, pArg, sizeof m_InitializeData);

	if (FAILED(Add_Components()))
		return E_FAIL;


	return S_OK;
}

void CSnow_Red::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);	

	m_pVIBufferCom->Tick(fTimeDelta);	

	if (Get_PointInstanceDesc()->fLifeTime > Get_PointInstanceDesc()->fLifeMaxTime)
		m_bDead = true;

	if (Get_PointInstanceDesc()->fLifeTime > Get_PointInstanceDesc()->fLifeMaxTime - Get_PointInstanceDesc()->fDisappearTime)
		m_InitializeData.vColor.w -= 1/Get_PointInstanceDesc()->fDisappearTime * fTimeDelta;
}

void CSnow_Red::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CSnow_Red::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;	

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();	

	return S_OK;
}

HRESULT CSnow_Red::Add_Components()
{
	/* Com_Renderer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Renderer"),
		TEXT("Com_Renderer"), (CComponent**)&m_pRendererCom)))
		return E_FAIL;
	
	/* Com_Transform */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Transform"),
		TEXT("Com_Transform"), (CComponent**)&m_pTransformCom)))
		return E_FAIL;

	/* Com_Shader */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPointInstance"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) 
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_Snow"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if(m_InitializeData.bNum300)
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance_300"),
			TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &m_InitializeData)))
			return E_FAIL;
	}
	else
	{
		if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Point_Instance"),
			TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &m_InitializeData)))
			return E_FAIL;
	}

	return S_OK;
}

HRESULT CSnow_Red::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;	

	if (FAILED(m_pShaderCom->Bind_RawValue("g_vCamPosition", &pGameInstance->Get_CamPosition(), sizeof(_float4))))
		return E_FAIL;
	
	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Color", &m_InitializeData.vColor, sizeof(_float4))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_Size", &m_InitializeData.vSize, sizeof(_float2))))
		return E_FAIL;

	return S_OK;
}

CSnow_Red * CSnow_Red::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSnow_Red*	pInstance = new CSnow_Red(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CSnow_Red");
	}

	return pInstance;
}

CGameObject * CSnow_Red::Clone(void * pArg)
{
	CSnow_Red*	pInstance = new CSnow_Red(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CSnow_Red");
	}

	return pInstance;
}

void CSnow_Red::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
