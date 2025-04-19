#pragma once

#include "Component.h"

BEGIN(Engine)

class ENGINE_DLL CComputeTexture final : public CComponent
{
private:
	CComputeTexture(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CComputeTexture(const CComputeTexture& rhs);
	virtual ~CComputeTexture() = default;

public:
	virtual HRESULT Initialize_Prototype(const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	virtual HRESULT Initialize(void* pArg) override;

public:
	//텍스쳐들중 index 정해서 그것만 던짐
	HRESULT Bind_ShaderResourceView(class CShader* pShader, const char* pConstantName, _uint iTextureIndex = 0);
	//모든 텍스쳐 던짐
	//HRESULT Bind_ShaderResourceViews(class CShader* pShader, const char* pConstantName);


private:
	vector<ID3D11UnorderedAccessView*>			m_Textures;
	ID3D11Buffer* m_Buffer;
	D3D11_BUFFER_DESC			m_BufferDesc;
	D3D11_SUBRESOURCE_DATA		m_InitializeData;
public:
	static CComputeTexture* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, const _tchar* pTextureFilePath, _uint iNumTextures = 1);
	virtual CComponent* Clone(void* pArg) override;
	virtual void Free() override;
};

END