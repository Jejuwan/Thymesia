#pragma once

#include "Client_Defines.h"
#include "BlendObject.h"
#include "Client_Struct.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)

class CUI : public CBlendObject
{
protected:
	CUI(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUI(const CUI& rhs);
	virtual ~CUI() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

protected:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

	UIDESC m_UIDesc;
protected:
	_float							m_fX, m_fY, m_fSizeX, m_fSizeY;
	_float4x4						m_ViewMatrix, m_ProjMatrix;
	_float					m_fHpRatio = 1.f;
	_float					m_fAlpha = 0.f;
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();


public:
	static CUI* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;

	UIDESC* Get_Desc() { return &m_UIDesc; }
	void Set_Pos(_float x, _float y) {
		m_fX = x;
		m_fY = y;
	}
	void Set_Hp(_float f) { m_fHpRatio = f; }
	_float Get_Alpha() { return m_fAlpha; }
	void Set_Alpha(_float f) { m_fAlpha += f; }
	void RealSet_Alpha(_float f) { 
		m_fAlpha = f;
		m_fAlpha = min(0.f, max(1, m_fAlpha));
	}
};

END