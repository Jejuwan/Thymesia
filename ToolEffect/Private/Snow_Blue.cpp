#include "ToolModelEffect_Pch.h"
#include "..\Public\Snow_Blue.h"
#include "GameInstance.h"

CSnow_Blue::CSnow_Blue(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
	: CGameObject(pDevice, pContext)
{

}

CSnow_Blue::CSnow_Blue(const CSnow_Blue & rhs)
	: CGameObject(rhs)
{

}

HRESULT CSnow_Blue::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;
	
	return S_OK;
}

HRESULT CSnow_Blue::Initialize(void * pArg)
{
	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;
		
	return S_OK;
}

void CSnow_Blue::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);	

	m_pVIBufferCom->Tick(fTimeDelta);	
}

void CSnow_Blue::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_NONLIGHT, this);
}

HRESULT CSnow_Blue::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	if (FAILED(Bind_ShaderResources()))
		return E_FAIL;	

	m_pShaderCom->Begin(0);

	m_pVIBufferCom->Render();	

	return S_OK;
}

HRESULT CSnow_Blue::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Shader_VtxRectInstance"), 
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom))) 
		return E_FAIL;

	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_Texture_Snow"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	CVIBuffer_Rect_Instance::RECTINSTANCEDESC		RectInstanceDesc;
	RectInstanceDesc.vPivotPos = _float4(0.f, 30.f, 0.f, 1.f);
	RectInstanceDesc.vRange = _float3(20.f, 5.f, 20.f);
	//RectInstanceDesc.fMinSpeed = 1.0f;
	//RectInstanceDesc.fMaxSpeed = 5.0f;

	if (FAILED(__super::Add_Component(LEVEL_GAMEPLAY, TEXT("Prototype_Component_VIBuffer_Rect_Instance"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom, &RectInstanceDesc)))
		return E_FAIL;

	return S_OK;
}

HRESULT CSnow_Blue::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	CGameInstance*		pGameInstance = CGameInstance::Get_Instance();
	Safe_AddRef(pGameInstance);

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW))))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ))))
		return E_FAIL;	
	
	Safe_Release(pGameInstance);

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	return S_OK;
}

CSnow_Blue * CSnow_Blue::Create(ID3D11Device * pDevice, ID3D11DeviceContext * pContext)
{
	CSnow_Blue*	pInstance = new CSnow_Blue(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CSnow_Blue");
	}

	return pInstance;
}

CGameObject * CSnow_Blue::Clone(void * pArg)
{
	CSnow_Blue*	pInstance = new CSnow_Blue(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CSnow_Blue");
	}

	return pInstance;
}

void CSnow_Blue::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pVIBufferCom);
}
