#pragma once
#include "C:\Users\juwan\Documents\Github\Thymesia\Reference\Headers\GameObject.h"

BEGIN(Engine)
class CShader;
class CTexture;
class CRenderer;
class CTransform;
class CVIBuffer_Rect;
END

BEGIN(Client)
class CUIComponent :
    public CGameObject
{
private:
	CUIComponent(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	CUIComponent(const CUIComponent& rhs);
	virtual ~CUIComponent() = default;

public:
	virtual HRESULT Initialize_Prototype();
	virtual HRESULT Initialize(void* pArg);
	virtual void Tick(_float fTimeDelta);
	virtual void Late_Tick(_float fTimeDelta);
	virtual HRESULT Render();
private:
	HRESULT Add_Components();
	HRESULT Bind_ShaderResources();

protected:
	CShader* m_pShaderCom = { nullptr };
	CTexture* m_pTextureCom = { nullptr };
	CRenderer* m_pRendererCom = { nullptr };
	CTransform* m_pTransformCom = { nullptr };
	CVIBuffer_Rect* m_pVIBufferCom = { nullptr };

public:
	static CUIComponent* Create(ID3D11Device* pDevice, ID3D11DeviceContext* pContext);
	virtual CGameObject* Clone(void* pArg)	override;
	virtual void Free() override;
};
END

