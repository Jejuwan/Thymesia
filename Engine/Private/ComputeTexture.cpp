#include "Engine_Pch.h"
#include "..\Public\ComputeTexture.h"
#include "Shader.h"
#include "GameInstance.h"

CComputeTexture::CComputeTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext)
	: CComponent(pDevice, pContext)
{
}

CComputeTexture::CComputeTexture(const CComputeTexture& rhs)
	: CComponent(rhs)
	, m_Textures(rhs.m_Textures)
{
	for (auto& pSRV : m_Textures)
		Safe_AddRef(pSRV);
}

HRESULT CComputeTexture::Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures)
{

	CGameInstance* pGameInstance = CGameInstance::Get_Instance();

	m_Textures.reserve(iNumTextures);

	for (size_t i = 0; i < iNumTextures; i++)
	{
		pGameInstance->Get_Device()->CreateBuffer(&m_BufferDesc, &m_InitializeData, &m_Buffer);

		ID3D11UnorderedAccessView* uav;
		D3D11_UNORDERED_ACCESS_VIEW_DESC uavDesc;
		ZeroMemory(&uavDesc, sizeof(uavDesc));
		uavDesc.Format = DXGI_FORMAT_R16G16B16A16_FLOAT;
		uavDesc.ViewDimension = D3D11_UAV_DIMENSION_TEXTURE2D;
		uavDesc.Texture2D.MipSlice = 0;
		pGameInstance->Get_Device()->CreateUnorderedAccessView(
			m_Buffer, &uavDesc, &uav);

		m_Textures.push_back(uav);
	}

	return S_OK;
}

HRESULT CComputeTexture::Initialize(void* pArg)
{
	return S_OK;
}

HRESULT CComputeTexture::Bind_ShaderResourceView(CShader* pShader, const char* pConstantName, _uint iTextureIndex)
{
	if (iTextureIndex >= m_Textures.size())
		return E_FAIL;

	return pShader->Bind_UnorderedAccessView(pConstantName, m_Textures[iTextureIndex]);
}

//HRESULT CComputeTexture::Bind_ShaderResourceViews(CShader* pShader, const char* pConstantName)
//{
//	return pShader->Bind_ShaderResourceViews(pConstantName, &m_Textures.front(), (Engine::_uint)m_Textures.size());
//}

CComputeTexture* CComputeTexture::Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures)
{
	CComputeTexture* pInstance = new CComputeTexture(pDevice, pContext);

	if (FAILED(pInstance->Initialize_Prototype(pTextureFilePath, iNumTextures)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Created : CTexture");
	}

	return pInstance;
}

CComponent* CComputeTexture::Clone(void* pArg)
{
	CComputeTexture* pInstance = new CComputeTexture(*this);

	if (FAILED(pInstance->Initialize(pArg)))
	{
		Safe_Release(pInstance);
		MSG_BOX("Failed to Cloned : CTexture");
	}

	return pInstance;
}


void CComputeTexture::Free()
{
	__super::Free();

	for (auto& pSRV : m_Textures)
		Safe_Release(pSRV);

	m_Textures.clear();
}
