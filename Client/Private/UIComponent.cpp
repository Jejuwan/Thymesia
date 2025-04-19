#include "Client_Pch.h"
#include "..\Public\UIComponent.h"

CUIComponent::CUIComponent(const CUIComponent& rhs)
{
}

HRESULT CUIComponent::Initialize_Prototype()
{
	return E_NOTIMPL;
}

HRESULT CUIComponent::Initialize(void* pArg)
{
	return E_NOTIMPL;
}

void CUIComponent::Tick(_float fTimeDelta)
{
}

void CUIComponent::Late_Tick(_float fTimeDelta)
{
}

HRESULT CUIComponent::Render()
{
	return E_NOTIMPL;
}

HRESULT CUIComponent::Add_Components()
{
	return E_NOTIMPL;
}

HRESULT CUIComponent::Bind_ShaderResources()
{
	return E_NOTIMPL;
}

CUIComponent* CUIComponent::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
{
	return nullptr;
}

CGameObject* CUIComponent::Clone(void* pArg)
{
	return nullptr;
}

void CUIComponent::Free()
{
}
