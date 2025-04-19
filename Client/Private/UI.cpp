#include "Client_Pch.h"
#include "..\Public\UI.h"
#include "GameInstance.h"

CUI::CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CBlendObject(pDevice, pContext)
{
}

CUI::CUI(const CUI& rhs)
	: CBlendObject(rhs)
{
}

HRESULT CUI::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI::Initialize(void* pArg)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	UIDESC* desc = (UIDESC*)pArg;
	if (nullptr != desc)
	{
		m_UIDesc.texName = desc->texName; 
		m_UIDesc.bVisible = desc->bVisible;
		m_UIDesc.bLookAt = desc->bLookAt;
		m_UIDesc.eType = desc->eType;
		m_UIDesc.vPos = desc->vPos;
		m_UIDesc.vScale = desc->vScale;
		m_UIDesc.fAngle = desc->fAngle;
	}

	m_fX = m_UIDesc.vPos.x;
	m_fY = m_UIDesc.vPos.y;
	m_fSizeX = m_UIDesc.vScale.x;
	m_fSizeY = m_UIDesc.vScale.y;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	m_pTransformCom->Scaled(_float3(m_fSizeX, m_fSizeY, 1.f));
	m_pTransformCom->Rotation({ 0,0,1 }, XMConvertToRadians(m_UIDesc.fAngle));

	if(m_UIDesc.eType == ORTHOGRAPHIC)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX, m_fY, 0.f, 1.f));
		XMStoreFloat4x4(&m_ViewMatrix, XMMatrixIdentity());
		XMStoreFloat4x4(&m_ProjMatrix, XMMatrixOrthographicLH(g_iWinSizeX, g_iWinSizeY, 0.f, 1.f));
	}
	else
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX, m_fY, m_UIDesc.vPos.z, 1.f));
		m_ViewMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
		m_ProjMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
	}

	m_fAlpha = 1.f;

	return S_OK;
}

void CUI::Tick(_float fTimeDelta)
{
	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	__super::Tick(fTimeDelta);
	if (m_UIDesc.eType == ORTHOGRAPHIC)
	{
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_fX - g_iWinSizeX * 0.5f, -m_fY + g_iWinSizeY * 0.5f, m_UIDesc.vPos.z, 1.f));
	}
	else if(m_UIDesc.bVisible)
	{
		if(m_UIDesc.bLookAt)
		{
			m_pTransformCom->LookAt(XMLoadFloat4(&pGameInstance->Get_CamPosition()));
			m_pTransformCom->Rotation(m_pTransformCom->Get_State(CTransform::STATE_UP), XMConvertToRadians(180.f));
		}
		
		m_ViewMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_VIEW);
		m_ProjMatrix = pGameInstance->Get_Transform_Float4x4(CPipeLine::D3DTS_PROJ);
		m_pTransformCom->Set_State(CTransform::STATE_POSITION, XMVectorSet(m_UIDesc.vPos.x, m_UIDesc.vPos.y, m_UIDesc.vPos.z, 1.f));
	}
}

void CUI::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);

	if (m_UIDesc.eType == ORTHOGRAPHIC)
		Compute_CamDistance(m_pTransformCom);

	m_pRendererCom->Add_RenderGroup(CRenderer::RENDER_BLEND, this);
}

HRESULT CUI::Render()
{
	if(m_UIDesc.bVisible)
	{
		if (FAILED(__super::Render()))
			return E_FAIL;

		if (FAILED(Bind_ShaderResources()))
			return E_FAIL;

		m_pShaderCom->Begin(0);

		m_pVIBufferCom->Render();
	}

	return S_OK;
}

HRESULT CUI::Add_Components()
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
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Shader_VtxPosTex"),
		TEXT("Com_Shader"), (CComponent**)&m_pShaderCom)))
		return E_FAIL;

	wstring wstr = TEXT("Prototype_Component_Texture_");
	wstr += m_UIDesc.texName;
	if (FAILED(__super::Add_Component(LEVEL_STATIC, wstr,
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;

	/* Com_VIBuffer */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_VIBuffer_Rect"),
		TEXT("Com_VIBuffer"), (CComponent**)&m_pVIBufferCom)))
		return E_FAIL;

	return S_OK;
}

HRESULT CUI::Bind_ShaderResources()
{
	if (FAILED(m_pTransformCom->Bind_ShaderResource(m_pShaderCom, "g_WorldMatrix")))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_Matrix("g_ViewMatrix", &m_ViewMatrix)))
		return E_FAIL;
	if (FAILED(m_pShaderCom->Bind_Matrix("g_ProjMatrix", &m_ProjMatrix)))
		return E_FAIL;

	if (FAILED(m_pTextureCom->Bind_ShaderResourceView(m_pShaderCom, "g_DiffuseTexture", 0)))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fAlpha", &m_fAlpha, sizeof(_float))))
		return E_FAIL;

	if (FAILED(m_pShaderCom->Bind_RawValue("g_fHp", &m_fHpRatio, sizeof(_float))))
		return E_FAIL;

	return S_OK;
}

CUI* CUI::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CUI* pInstance = new CUI(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CUI");
	}

	return pInstance;
}

CGameObject* CUI::Clone(void* pArg)
{
	CUI* pInstance = new CUI(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CUI");
	}

	return pInstance;
}

void CUI::Free()
{
	__super::Free();

	Safe_Release(m_pTransformCom);
	Safe_Release(m_pShaderCom);
	Safe_Release(m_pTextureCom);
	Safe_Release(m_pRendererCom);
	Safe_Release(m_pVIBufferCom);
}
