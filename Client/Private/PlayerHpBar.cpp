#include "Client_Pch.h"
#include "..\Public\PlayerHpBar.h"

CPlayerHpBar::CPlayerHpBar(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	:CUI(pDevice,pContext)
{
}

CPlayerHpBar::CPlayerHpBar(const CPlayerHpBar& rhs)
	:CUI(rhs)
{
}

HRESULT CPlayerHpBar::Initialize_Prototype()
{
	if (FAILED(__super::Initialize_Prototype()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerHpBar::Initialize(void* pArg)
{
	m_fSizeX = g_iWinSizeX;
	m_fSizeY = g_iWinSizeY * 0.02f;
	m_fX = g_iWinSizeX * 0.9f;
	m_fY = g_iWinSizeY * 0.9f;

	if (FAILED(__super::Initialize(pArg)))
		return E_FAIL;

	if (FAILED(Add_Components()))
		return E_FAIL;

	return S_OK;
}

void CPlayerHpBar::Tick(_float fTimeDelta)
{
	__super::Tick(fTimeDelta);
}

void CPlayerHpBar::Late_Tick(_float fTimeDelta)
{
	__super::Late_Tick(fTimeDelta);
}

HRESULT CPlayerHpBar::Render()
{
	if (FAILED(__super::Render()))
		return E_FAIL;

	return S_OK;
}

HRESULT CPlayerHpBar::Add_Components()
{
	/* Com_Texture */
	if (FAILED(__super::Add_Component(LEVEL_STATIC, TEXT("Prototype_Component_Texture_UI_TexUI_HPBar_2BG"),
		TEXT("Com_Texture"), (CComponent**)&m_pTextureCom)))
		return E_FAIL;
}

HRESULT CPlayerHpBar::Bind_ShaderResources()
{
	return S_OK; 

	  
}

CUI* CPlayerHpBar::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	CPlayerHpBar* pInstance = new CPlayerHpBar(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype()))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CPlayerHpBar");
	}

	return pInstance;
}

CGameObject* CPlayerHpBar::Clone(void* pArg)
{
	CPlayerHpBar* pInstance = new CPlayerHpBar(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CPlayerHpBar");
	}

	return pInstance;
}

void CPlayerHpBar::Free()
{
	__super::Free();

}
