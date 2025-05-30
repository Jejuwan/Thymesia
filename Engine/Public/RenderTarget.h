#pragma once

#include "Base.h"

/* 렌더타겟 하나를 의미하는 클래스 */

BEGIN(Engine)

class CRenderTarget final : public CBase
{
private:
	CRenderTarget(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual ~CRenderTarget() = default;
public:
	enum TYPE { RENDER_TARGET, DEPTH_STENCIL };
public:
	ID3D11RenderTargetView* Get_RTV() const {
		return m_pRTV;
	}
	ID3D11DepthStencilView* Get_DSV() const {
		return m_pDSV;
	}
	TYPE Get_Type() const {
		return m_eType;
	}
public:
	HRESULT Initialize(_uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vColor, TYPE eType);
	HRESULT Clear();
	HRESULT ClearDSV();
	HRESULT Bind_ShaderResource(class CShader* pShader, const char* pConstantName);

#ifdef _DEBUG
public:
	HRESULT Ready_Debug(_float fX, _float fY, _float fSizeX, _float fSizeY);
	HRESULT Debug_Render(class CShader* pShader, class CVIBuffer_Rect* pVIBuffer);
#endif
private:
	TYPE								m_eType;
private:	
	ID3D11Device*						m_pDevice = { nullptr };
	ID3D11DeviceContext*				m_pContext = { nullptr };
private:
	ID3D11Texture2D*					m_pTexture2D = { nullptr };
	ID3D11RenderTargetView*				m_pRTV = { nullptr };
	ID3D11DepthStencilView*				m_pDSV = { nullptr };
	ID3D11ShaderResourceView*			m_pSRV = { nullptr };
private:
	_float4								m_vClearColor = _float4(0.f, 0.f, 0.f, 0.f);

#ifdef _DEBUG
private:
	_float4x4							m_WorldMatrix;
#endif

public:
	static CRenderTarget* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext, _uint iSizeX, _uint iSizeY, DXGI_FORMAT ePixelFormat, const _float4& vColor,TYPE eType);
	virtual void Free() override;
};

END