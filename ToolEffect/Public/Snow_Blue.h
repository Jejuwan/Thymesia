#pragma once

#include "EffectTool_Defines.h"
#include "GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect_Instance;
END

BEGIN(ToolEffect)

class CSnow_Blue final : public CGameObject
{
private:
	CSnow_Blue(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CSnow_Blue(const CSnow_Blue& rhs);
	virtual ~CSnow_Blue() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();

private:	
	CShader*						m_pShaderCom = { nullptr };	
	CTexture*						m_pTextureCom = { nullptr };
	CRenderer*						m_pRendererCom = { nullptr };
	CTransform*						m_pTransformCom = { nullptr };
	CVIBuffer_Rect_Instance*		m_pVIBufferCom = { nullptr };
	

private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

public:
	static CSnow_Blue* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};

END